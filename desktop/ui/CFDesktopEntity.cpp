#include "CFDesktopEntity.h"
#include "CFDesktop.h"
#include "CFDesktopWindowProxy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "IDesktopPropertyStrategy.h"
#include "aex/weak_ptr/weak_ptr.h"
#include "cfconfig.hpp"
#include "cflog.h"
#include "components/DisplayServerBackendFactory.h"
#include "components/IDisplayServerBackend.h"
#include "components/PanelManager.h"
#include "components/WindowManager.h"
#include "components/builtin_apps/about_panel.h"
#include "components/builtin_apps/builtin_panel_registry.h"
#include "components/builtin_apps/calculator_builtin_panel.h"
#include "components/launcher/app_discoverer.h"
#include "components/launcher/app_launch_service.h"
#include "components/launcher/app_launcher.h"
#include "components/statusbar/status_bar.h"
#include "components/taskbar/centered_taskbar.h"
#include "components/window_placement/window_placement_policy.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include "platform/display_backend_helper.h"
#include "platform/shell_layer_helper.h"
#include "qt_format.h"
#include "system/hardware_tier/hardware_tier.h"
#include <QCoreApplication>
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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

    // 3. Legacy fallback when nothing is discovered: <bin>/../apps.json,
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
    status_bar->setBackdropSource(shell);
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

    // ── Window placement: constrain launched windows into the work area ──
    // On each external window appearance, clamp it inside the central work area
    // (between the status bar and the taskbar) so it neither overlaps a bar nor
    // flies off-screen, mirroring a real desktop WM. The policy is stateless, so
    // a temporary is fine here. Runtime toggle via config domain
    // "window_management" / key "constrain_to_workarea" (default on), read per
    // appearance so flipping the config needs no restart.
    if (display_backend_) {
        if (auto window_backend = display_backend_->windowBackend()) {
            QObject::connect(
                window_backend.Get(), &cf::desktop::IWindowBackend::window_came, this,
                [panel_mgr, this](aex::WeakPtr<cf::desktop::IWindow> win) {
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
                    const QRect work =
                        panel_mgr->availableGeometry().translated(desktop_entity_->pos());
                    const QRect cur = w->geometry();
                    const auto target =
                        cf::desktop::placement::WindowPlacementPolicy{}.computeConstrain(cur, work,
                                                                                         enabled);
                    if (target.has_value()) {
                        w->set_geometry(*target);
                        cf::log::infoftag("WindowPlacement", "constrained '{}' {} -> {}",
                                          w->title().toStdString(), cur, *target);
                    }
                });
        }
    }

    // ── Builtin in-process panels: register before loadAppsConfig so the
    // merged app list can surface them and resolve Auto launch_kind. ──
    auto& builtin_registry = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    auto* about_panel = new cf::desktop::desktop_component::AboutPanel(desktop_entity_);
    builtin_registry.registerPanel(about_panel);
    auto* calc_builtin =
        new cf::desktop::desktop_component::CalculatorBuiltinPanel(desktop_entity_);
    builtin_registry.registerPanel(calc_builtin);

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
    auto* taskbar = new cf::desktop::desktop_component::CenteredTaskbar(desktop_entity_);
    taskbar->setApps(apps);
    taskbar->setBackdropSource(shell);
    panel_mgr->registerPanel(taskbar->GetWeak());
    // Shared launch path: resolve app_id -> entry, dispatch by launch_kind.
    // BuiltinPanel entries render in-process (registry lookup); DetachedProcess
    // entries spawn a QProcess. Used by both taskbar click and launcher popup
    // so the running-state indicator lights for either entry point.
    std::function<void(const QString&)> launch_app = [apps, app_pid,
                                                      panel_mgr](const QString& app_id) {
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