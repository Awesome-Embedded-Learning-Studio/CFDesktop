#include "CFDesktopEntity.h"
#include "CFDesktop.h"
#include "CFDesktopWindowProxy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "IDesktopPropertyStrategy.h"
#include "aex/weak_ptr/weak_ptr.h"
#include "cfconfig.hpp"
#include "cfipc/ipc_server.h"
#include "cflog.h"
#include "components/DisplayServerBackendFactory.h"
#include "components/IDisplayServerBackend.h"
#include "components/PanelManager.h"
#include "components/WindowManager.h"
#include "components/builtin_apps/about_panel.h"
#include "components/builtin_apps/builtin_panel_registry.h"
#include "components/control_center/control_center.h"
#include "components/desktop_icon_layer/desktop_icon_layer.h"
#include "components/desktop_icon_layer/desktop_shortcut_store.h"
#include "components/home_page/home_page.h"
#include "components/home_page/page_stack_widget.h"
#include "components/launcher/app_discoverer.h"
#include "components/launcher/app_launch_service.h"
#include "components/launcher/app_launcher.h"
#include "components/launcher/desktop_entry_index.h"
#include "components/notification/notification_banner.h"
#include "components/notification/notification_center_panel.h"
#include "components/notification/notification_service.h"
#include "components/statusbar/status_bar.h"
#include "components/taskbar/centered_taskbar.h"
#include "components/window_placement/floating_policy.h"
#include "components/window_placement/window_placement_policy.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include "platform/display_backend_helper.h"
#include "platform/shell_layer_helper.h"
#include "qt_format.h"
#include "system/hardware_tier/hardware_tier.h"
#include <QCoreApplication>
#include <QFile>
#include <QGuiApplication>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>
#include <functional>
#include <memory>

namespace cf::desktop {

namespace {
/// Loads the app list by merging builtin in-process panels with auto-
/// discovered standalone manifests, resolving Auto launch_kind against the
/// hardware tier. Falls back to <bin>/../apps.json then defaultApps() when
/// nothing is discovered. A board can drop app manifests or apps.json to
/// customize the launcher/taskbar without a recompile.
QList<desktop_component::AppEntry> loadAppsConfig(bool prefer_inprocess) {
    // Upsert by app_id: builtin panels first (in-process implementations),
    // then discovered manifests (icon/exec/display_name metadata) which may
    // override a builtin entry when a standalone app exists for the same id.
    QList<desktop_component::AppEntry> result;
    const auto upsert = [&](desktop_component::AppEntry entry) {
        for (auto& existing : result) {
            if (existing.app_id == entry.app_id) {
                existing = entry;
                return;
            }
        }
        result.append(entry);
    };

    // 1. Builtin in-process panels (always surfaced).
    for (auto* panel : desktop_component::BuiltinPanelRegistry::instance().all()) {
        desktop_component::AppEntry e;
        e.app_id = panel->appId();
        e.display_name = panel->displayName();
        e.launch_kind = desktop_component::LaunchKind::BuiltinPanel;
        upsert(e);
    }

    // 2. Auto-discovered standalone app manifests.
    auto discovered = desktop_component::AppDiscoverer::discover();
    for (auto& entry : discovered) {
        if (entry.launch_kind == desktop_component::LaunchKind::Auto) {
            // Resolve Auto: prefer in-process only when a builtin implementation
            // exists; otherwise detach (never silently fall back to builtin).
            const bool has_builtin =
                desktop_component::BuiltinPanelRegistry::instance().contains(entry.app_id);
            if (prefer_inprocess && has_builtin) {
                entry.launch_kind = desktop_component::LaunchKind::BuiltinPanel;
                entry.exec_command.clear();
            } else {
                if (prefer_inprocess && !has_builtin) {
                    cf::log::infoftag("CFDesktopEntity",
                                      "App '{}' auto->detached (no builtin impl)",
                                      entry.app_id.toStdString());
                }
                entry.launch_kind = desktop_component::LaunchKind::DetachedProcess;
            }
        }
        upsert(entry);
    }

    // 3. XDG .desktop entries (system applications: firefox, etc.).
    for (auto& entry : desktop_component::DesktopEntryIndex::index()) {
        upsert(entry);
    }

    // 4. Legacy fallback when nothing is discovered: <bin>/../apps.json,
    // then defaultApps() placeholder entries.
    if (discovered.isEmpty()) {
        const QString path =
            QCoreApplication::applicationDirPath() + QStringLiteral("/../apps.json");
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            for (const auto& d : desktop_component::defaultApps()) {
                upsert(d);
            }
            return result;
        }
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        const auto arr = doc.object().value(QStringLiteral("apps")).toArray();
        bool added_any = false;
        for (const auto& value : arr) {
            const auto o = value.toObject();
            desktop_component::AppEntry entry;
            entry.app_id = o.value(QStringLiteral("app_id")).toString();
            entry.display_name = o.value(QStringLiteral("display_name")).toString();
            entry.icon_path = o.value(QStringLiteral("icon_path")).toString();
            entry.exec_command = o.value(QStringLiteral("exec_command")).toString();
            if (!entry.app_id.isEmpty() && !entry.exec_command.isEmpty()) {
                upsert(entry);
                added_any = true;
            }
        }
        if (!added_any) {
            for (const auto& d : desktop_component::defaultApps()) {
                upsert(d);
            }
        }
    }

    return result;
}
} // namespace

std::unique_ptr<CFDesktopEntity> CFDesktopEntity::global_instance_;

CFDesktopEntity& CFDesktopEntity::instance() {
    if (!global_instance_) {
        global_instance_.reset(new CFDesktopEntity);
    }
    return *global_instance_;
}

CFDesktopEntity::CFDesktopEntity()
    : QObject(nullptr), desktop_entity_(new CFDesktop(this)),
      platform_factory_(std::make_unique<platform_strategy::PlatformFactory>()) {
    // Register the platform display backend creator into the factory
    auto api = platform::native_display();
    if (api.creator_func) {
        DisplayServerBackendFactory::instance().register_creator(std::move(api.creator_func),
                                                                 std::move(api.release_func));
    }
    log::tracef("Desktop Entity is created");

    // Single-instance raise: a second shell signals via IPC to bring us to front.
    QObject::connect(&cf::ipc::IPCServer::instance(), &cf::ipc::IPCServer::raiseRequested, this,
                     [this]() {
                         if (desktop_entity_ != nullptr) {
                             desktop_entity_->showNormal();
                             desktop_entity_->raise();
                             desktop_entity_->activateWindow();
                         }
                     });
}

CFDesktopEntity::~CFDesktopEntity() {
    log::tracef("Desktop Entity is released, and never available again!");
}

void CFDesktopEntity::release() {
    global_instance_.reset();
}

CFDesktopEntity::RunsSetupResult CFDesktopEntity::run_init(RunsSetupMethod m) {
    // setup the window show proxy
    using StrategyType = platform_strategy::IDesktopPropertyStrategy::StrategyType;
    CFDesktopWindowProxy proxy;
    log::trace("Start Entity Init");

    auto display_policy_ =
        platform_factory_->create_unique<platform_strategy::IDesktopDisplaySizeStrategy>(
            StrategyType::DisplaySizePolicy);

    proxy.set_window_display_strategy(std::move(display_policy_));
    proxy.activate_window_display_strategy();

    // ── Windows: start display server backend for third-party window tracking ──
    display_backend_ = DisplayServerBackendFactory::instance().make_unique();

    if (display_backend_) {
        if (display_backend_->initialize(0, nullptr)) {
            auto wb = display_backend_->windowBackend();
            if (wb) {
                auto* raw = wb.Get();
                QObject::connect(
                    raw, &IWindowBackend::window_came, this, [](aex::WeakPtr<IWindow> win) {
                        if (win) {
                            cf::log::traceftag("CFDesktopEntity", "External window detected: {}",
                                               win->title().toStdString());
                        }
                    });
                QObject::connect(raw, &IWindowBackend::window_gone, this,
                                 [](aex::WeakPtr<IWindow> /*win*/) {
                                     cf::log::traceftag("CFDesktopEntity", "External window gone");
                                 });
            }
        } else {
            log::errorftag("CFDesktopEntity", "Display server backend init failed");
        }
    }

    // ── Create PanelManager and ShellLayer ──
    auto* panel_mgr = new PanelManager(desktop_entity_, desktop_entity_);

    auto shell_api = platform::native_shell_layer();
    auto* shell = shell_api.shell_creator(desktop_entity_);
    auto strategy = shell_api.strategy_creator();
    shell->setStrategy(std::move(strategy));

    // Inject into CFDesktop
    CFDesktop::InitResources res;
    res.panel_manager_ = panel_mgr;
    res.shell_layer_ = shell;
    desktop_entity_->register_desktop_resources(res);

    // ── Desktop icon layer: construct right after the wallpaper shell layer
    // so Qt child-widget z-order stacks it ABOVE the wallpaper and BELOW the
    // status/task bars created later (creation order = stacking order under a
    // shared parent). Data wiring happens later, once the merged app list and
    // launch_app handler exist; only the widget is created here to lock z-order.
    // Not an IShellLayer (wallpaper-strategy specific) and not an IPanel
    // (edge-anchored) — a plain child widget that consumes the central
    // availableGeometry() like the launcher popup does. ──
    auto* icon_layer = new cf::desktop::desktop_component::DesktopIconLayer(desktop_entity_);

    // Desktop pages: HomePage (information-flow home, page 0) and the icon grid
    // (Win11-style, page 1) stacked and flipped with a horizontal swipe. The
    // icon layer is still wired below (its signals stay); as page 1 it no longer
    // overlaps the home page. HomePage and the clocks do not accept mouse press,
    // so it bubbles to PageStackWidget for the horizontal swipe, while
    // CardStackWidget (inside HomePage) keeps vertical card swipe.
    auto* home_page = new cf::desktop::desktop_component::HomePage();
    auto* page_stack = new cf::desktop::desktop_component::PageStackWidget(desktop_entity_);
    page_stack->addWidget(home_page);
    page_stack->addWidget(icon_layer);
    page_stack->setGeometry(panel_mgr->availableGeometry());
    QObject::connect(panel_mgr, &PanelManager::availableGeometryChanged, desktop_entity_,
                     [page_stack, panel_mgr](const QRect&) {
                         page_stack->setGeometry(panel_mgr->availableGeometry());
                     });

    // Connect PanelManager geometry changes to ShellLayer. The wallpaper shell
    // spans the FULL host geometry (not the panel-reduced central rect) so it
    // renders continuously behind the top/bottom bars; each bar composites a
    // frosted copy of the strip directly behind it. The launcher popup still
    // uses PanelManager::availableGeometry() (the central-rect getter), which is
    // independent and unaffected.
    QObject::connect(panel_mgr, &PanelManager::availableGeometryChanged, desktop_entity_,
                     [shell, this](const QRect&) {
                         shell->onAvailableGeometryChanged(desktop_entity_->rect());
                     });

    // Re-constrain every tracked external window into the current screen-space
    // work area. The work area is PanelManager's local central rect translated
    // to screen coordinates (window->geometry() is root-relative), so it stays
    // correct when the desktop is MOVED, not only resized. Shared by the two
    // triggers below: availableGeometryChanged (desktop resized / panels
    // relaid out) and geometryChanged (desktop moved without resizing) — the
    // back-propagation that was missing for the drag/move case.
    auto reconstrain_windows = [panel_mgr, this]() {
        if (!display_backend_) {
            return;
        }
        auto backend = display_backend_->windowBackend();
        if (!backend) {
            return;
        }
        namespace cfg = cf::config;
        const bool enabled = cfg::ConfigStore::instance()
                                 .domain("window_management")
                                 .query<bool>(cfg::KeyView{.group = "window_management",
                                                           .key = "constrain_to_workarea"},
                                              true);
        const QRect work = panel_mgr->availableGeometry().translated(desktop_entity_->pos());
        const cf::desktop::placement::WindowPlacementPolicy policy;
        int moved = 0;
        for (const auto& wptr : backend->windows()) {
            auto* w = wptr.Get();
            if (w == nullptr) {
                continue;
            }
            const QRect before = w->geometry();
            policy.constrain(*w, work, enabled);
            if (w->geometry() != before) {
                ++moved;
            }
        }
        if (moved > 0) {
            cf::log::infoftag("WindowPlacement",
                              "re-constrained {} window(s) into screen work area {}", moved, work);
        }
    };
    QObject::connect(panel_mgr, &PanelManager::availableGeometryChanged, this,
                     [reconstrain_windows](const QRect&) { reconstrain_windows(); });
    QObject::connect(desktop_entity_, &CFDesktop::geometryChanged, this, reconstrain_windows);

    // ── Status bar: top-edge panel (clock + system icons) ──
    auto* status_bar = new cf::desktop::desktop_component::StatusBar(desktop_entity_);
    panel_mgr->registerPanel(status_bar->GetWeak());
    status_bar->show();

    // ── Window manager: track external windows and link them to the taskbar ──
    // app_pid maps a launched app_id to its process id so window tracking can
    // match external windows back to a taskbar entry.
    auto app_pid = std::make_shared<QHash<QString, qint64>>();
    auto* window_mgr = new cf::desktop::WindowManager(desktop_entity_);
    if (display_backend_) {
        auto window_backend = display_backend_->windowBackend();
        if (window_backend) {
            window_mgr->setBackend(window_backend);
        }
    }

    // ── Window placement: give each new window an initial floating geometry ──
    // On each external window appearance, place it via FloatingPolicy: centered
    // in the central work area (between the status bar and the taskbar),
    // shrunk to fit when larger than the work area, and nudged by a small
    // cascade offset per consecutive window so a burst of launches does not
    // stack exactly on top of each other. The WindowPlacementPolicy still runs
    // on work-area changes (see reconstrain_windows above) to pull windows back
    // inside when the desktop is resized. Runtime toggle via config domain
    // "window_management" / key "constrain_to_workarea" (default on), read per
    // appearance so flipping the config needs no restart.
    auto cascade_index = std::make_shared<int>(0);
    if (display_backend_) {
        if (auto window_backend = display_backend_->windowBackend()) {
            QObject::connect(
                window_backend.Get(), &cf::desktop::IWindowBackend::window_came, this,
                [panel_mgr, this, cascade_index](aex::WeakPtr<cf::desktop::IWindow> win) {
                    if (!win) {
                        cf::log::warningftag("WindowPlacement", "window_came with null window");
                        return;
                    }
                    auto* w = win.Get();
                    namespace cfg = cf::config;
                    const bool enabled =
                        cfg::ConfigStore::instance()
                            .domain("window_management")
                            .query<bool>(cfg::KeyView{.group = "window_management",
                                                      .key = "constrain_to_workarea"},
                                         true);
                    if (!enabled) {
                        return;
                    }
                    const QRect work =
                        panel_mgr->availableGeometry().translated(desktop_entity_->pos());
                    const QRect cur = w->geometry();
                    const int idx = (*cascade_index)++;
                    const auto target = cf::desktop::placement::FloatingPolicy::initialGeometry(
                        work, idx, cur.size());
                    if (target != cur) {
                        w->set_geometry(target);
                        cf::log::infoftag("WindowPlacement", "placed '{}' {} -> {} (cascade #{})",
                                          w->title().toStdString(), cur, target, idx);
                    }
                });
        }
    }

    // ── Builtin in-process panels: register before loadAppsConfig so the
    // merged app list can surface them and resolve Auto launch_kind. ──
    auto& builtin_registry = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    auto* about_panel = new cf::desktop::desktop_component::AboutPanel(desktop_entity_);
    builtin_registry.registerPanel(about_panel);

    // Hardware tier decides whether Auto apps run in-process (Low tier) or
    // detached (Mid/High). setDeviceConfigOverride (env/tests) takes precedence.
    bool prefer_inprocess = false;
    if (auto res = cf::assessHardware(); res.has_value()) {
        if (auto caps = cf::getHardwareTierCapabilities(); caps.has_value()) {
            prefer_inprocess = caps->prefer_inprocess_apps;
        }
    }

    // ── Taskbar: bottom-edge panel (centered app icons) ──
    // apps is captured by the click handler to resolve app_id -> entry.
    const QList<cf::desktop::desktop_component::AppEntry> apps = loadAppsConfig(prefer_inprocess);

    // Desktop icon shortcuts: a user-managed layout persisted via ConfigStore.
    // First run (nothing stored) seeds a default set from the merged registry,
    // then every subsequent change (drag swap / drag-off-remove / launcher add)
    // re-persists via the shortcutsChanged signal wired below.
    QList<cf::desktop::desktop_component::DesktopShortcut> desktop_shortcuts =
        cf::desktop::desktop_component::DesktopShortcutStore::load();
    if (desktop_shortcuts.isEmpty()) {
        // Seed across as many columns as the primary screen can hold, so the
        // first-run layout fills the width instead of clustering in the left
        // half. The seed runs before PanelManager relayout delivers the real
        // central rect, so estimate from the screen geometry.
        int seed_cols = 8;
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            seed_cols = std::max(8, (screen->availableGeometry().width() - 48) / 112);
        }
        desktop_shortcuts =
            cf::desktop::desktop_component::DesktopShortcutStore::seedFrom(apps, seed_cols);
        cf::desktop::desktop_component::DesktopShortcutStore::save(desktop_shortcuts);
    }

    auto* taskbar = new cf::desktop::desktop_component::CenteredTaskbar(desktop_entity_);
    taskbar->setApps(apps);
    panel_mgr->registerPanel(taskbar->GetWeak());
    // Shared launch path: resolve app_id -> entry, dispatch by launch_kind.
    // BuiltinPanel entries render in-process (registry lookup); DetachedProcess
    // entries spawn a QProcess. Used by both taskbar click and launcher popup
    // so the running-state indicator lights for either entry point.
    std::function<void(const QString&)> launch_app = [apps, app_pid, panel_mgr,
                                                      window_mgr](const QString& app_id) {
        const cf::desktop::desktop_component::AppEntry* found = nullptr;
        for (const auto& app : apps) {
            if (app.app_id == app_id) {
                found = &app;
                break;
            }
        }
        if (found == nullptr) {
            cf::log::warningftag("CFDesktopEntity", "No entry for app_id '{}'",
                                 app_id.toStdString());
            return;
        }
        if (found->launch_kind == cf::desktop::desktop_component::LaunchKind::BuiltinPanel) {
            auto* panel =
                cf::desktop::desktop_component::BuiltinPanelRegistry::instance().find(app_id);
            if (panel != nullptr) {
                panel->popup(panel_mgr->availableGeometry());
            } else {
                cf::log::warningftag("CFDesktopEntity", "No builtin panel for '{}'",
                                     app_id.toStdString());
            }
            return;
        }
        // DetachedProcess: if this app already has a live tracked window, toggle
        // it (raise / minimize) instead of spawning a second instance. A stale
        // app_pid entry whose window already died falls through to relaunch —
        // findWindowByPid returns nullopt once the window is gone.
        if (app_pid->contains(app_id)) {
            const qint64 pid = (*app_pid)[app_id];
            if (auto win_id = window_mgr->findWindowByPid(pid); win_id.has_value()) {
                using cf::desktop::WindowState;
                const auto info = window_mgr->getWindowInfo(*win_id);
                if (info.state == WindowState::Normal) {
                    window_mgr->minimizeWindow(*win_id);
                } else if (info.state == WindowState::Minimized) {
                    window_mgr->restoreWindow(*win_id);
                    if (auto* w = window_mgr->find_window(*win_id).Get()) {
                        w->raise();
                    }
                } else {
                    // Maximized / other: leave the state alone, just raise.
                    if (auto* w = window_mgr->find_window(*win_id).Get()) {
                        w->raise();
                    }
                }
                return;
            }
        }
        const auto launched =
            cf::desktop::desktop_component::AppLaunchService::launch(found->exec_command);
        if (launched.has_value()) {
            (*app_pid)[app_id] = *launched;
        }
    };
    QObject::connect(taskbar, &cf::desktop::desktop_component::CenteredTaskbar::appClicked, this,
                     launch_app);

    // ── App launcher popup (Start-menu), opened from the taskbar start button ──
    auto* app_launcher = new cf::desktop::desktop_component::AppLauncher(desktop_entity_);
    app_launcher->setApps(apps);
    QObject::connect(app_launcher, &cf::desktop::desktop_component::AppLauncher::appLaunched, this,
                     launch_app);
    // Launcher long-press "send to desktop": append to the desktop icon layer
    // (which finds the first free cell and persists).
    QObject::connect(app_launcher,
                     &cf::desktop::desktop_component::AppLauncher::addToDesktopRequested,
                     icon_layer, &cf::desktop::desktop_component::DesktopIconLayer::addShortcut);

    // ── Desktop icon layer wiring: same merged apps list and same launch_app
    // dispatch as the taskbar and launcher, so a desktop shortcut launch lights
    // the taskbar running indicator too (shared app_pid tracking). Geometry is
    // the PanelManager central rect (between the bars); tiles are not created
    // until a valid geometry arrives, so showing early does not flash tiles at
    // (0,0). The first panel_mgr->relayout() below delivers the real rect. ──
    icon_layer->setShortcuts(desktop_shortcuts, apps);
    // icon_layer is now a page inside PageStackWidget (local coords 0,0), so
    // feed it the page size rather than the desktop-relative available rect.
    QObject::connect(panel_mgr, &PanelManager::availableGeometryChanged, desktop_entity_,
                     [icon_layer](const QRect& avail) {
                         icon_layer->onAvailableGeometryChanged(
                             QRect(0, 0, avail.width(), avail.height()));
                     });
    {
        const QRect seed = panel_mgr->availableGeometry();
        icon_layer->onAvailableGeometryChanged(QRect(0, 0, seed.width(), seed.height()));
    }
    QObject::connect(icon_layer, &cf::desktop::desktop_component::DesktopIconLayer::appClicked,
                     this, launch_app);
    // Persist any layout change (drag swap / drag-off-remove / launcher add).
    QObject::connect(icon_layer,
                     &cf::desktop::desktop_component::DesktopIconLayer::shortcutsChanged, this,
                     [](const QList<cf::desktop::desktop_component::DesktopShortcut>& shortcuts) {
                         cf::desktop::desktop_component::DesktopShortcutStore::save(shortcuts);
                     });
    // icon_layer is shown by PageStackWidget when its page becomes current.

    QObject::connect(taskbar, &cf::desktop::desktop_component::CenteredTaskbar::launcherRequested,
                     this, [app_launcher, panel_mgr]() {
                         // Toggle: clicking Start while the launcher is open dismisses it,
                         // otherwise pop it up. The start button used to only ever call
                         // popup(), so a second click was a no-op while the menu was already
                         // visible.
                         if (app_launcher->isShowing()) {
                             app_launcher->hideLauncher();
                         } else {
                             app_launcher->popup(panel_mgr->availableGeometry());
                         }
                     });
    // ── Control center + notification system ──
    // NotificationService is a process singleton; force its construction.
    auto& notification_svc = cf::desktop::desktop_component::NotificationService::instance();
    auto* control_center = new cf::desktop::desktop_component::ControlCenter(desktop_entity_);
    auto* notif_center =
        new cf::desktop::desktop_component::NotificationCenterPanel(desktop_entity_);
    auto* notif_banner = new cf::desktop::desktop_component::NotificationBanner(desktop_entity_);

    // Status bar entries: click the clock / notification icon to toggle.
    QObject::connect(status_bar, &cf::desktop::desktop_component::StatusBar::timeClicked, this,
                     [control_center, panel_mgr]() {
                         if (control_center->isShowing()) {
                             control_center->hidePanel();
                         } else {
                             control_center->popup(panel_mgr->availableGeometry());
                         }
                     });
    QObject::connect(status_bar, &cf::desktop::desktop_component::StatusBar::notifyIconClicked,
                     this, [notif_center, panel_mgr]() {
                         if (notif_center->isShowing()) {
                             notif_center->hidePanel();
                         } else {
                             notif_center->popup(panel_mgr->availableGeometry());
                         }
                     });

    // Banner shows for every non-suppressed post (DND off).
    QObject::connect(&notification_svc,
                     &cf::desktop::desktop_component::NotificationService::notificationPosted, this,
                     [notif_banner, panel_mgr](
                         const cf::desktop::desktop_component::Notification& n, bool suppressed) {
                         if (!suppressed) {
                             notif_banner->showFor(n, panel_mgr->availableGeometry());
                         }
                     });

    // External apps deliver notifications over IPC; payload -> service.
    QObject::connect(&cf::ipc::IPCServer::instance(), &cf::ipc::IPCServer::notifyReceived, this,
                     [&notification_svc](const QJsonObject& payload) {
                         cf::desktop::desktop_component::Notification n;
                         n.title = payload.value("title").toString();
                         n.message = payload.value("message").toString();
                         n.app_id = payload.value("app_id").toString();
                         notification_svc.post(n);
                     });
    // Outside-click dismissal is deferred (matches AppLauncher, which relies on
    // ESC + the toggle entry). ESC closes either popup.

    taskbar->show();
    panel_mgr->relayout();

    // ── WM ↔ Taskbar: light a running indicator when a window appears/gone ──
    QObject::connect(window_mgr, &cf::desktop::WindowManager::windowAppeared, this,
                     [app_pid, taskbar](qint64 pid) {
                         if (pid == 0) {
                             return;
                         }
                         for (auto it = app_pid->begin(); it != app_pid->end(); ++it) {
                             if (it.value() == pid) {
                                 taskbar->updateRunningState(it.key(), true);
                                 return;
                             }
                         }
                     });
    QObject::connect(window_mgr, &cf::desktop::WindowManager::windowDisappeared, this,
                     [app_pid, taskbar](qint64 pid) {
                         if (pid == 0) {
                             return;
                         }
                         for (auto it = app_pid->begin(); it != app_pid->end(); ++it) {
                             if (it.value() == pid) {
                                 taskbar->updateRunningState(it.key(), false);
                                 return;
                             }
                         }
                     });

    // Show the desktop full-screen
    desktop_entity_->showFullScreen();

    log::trace("Entity Init");
    return RunsSetupResult::OK;
}

aex::WeakPtr<CFDesktop> CFDesktopEntity::desktop_widget() const {
    return desktop_entity_->GetWeak();
}

} // namespace cf::desktop