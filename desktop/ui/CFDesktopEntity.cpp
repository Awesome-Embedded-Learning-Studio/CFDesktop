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
#include "components/launcher/app_launch_service.h"
#include "components/launcher/app_launcher.h"
#include "components/statusbar/status_bar.h"
#include "components/taskbar/centered_taskbar.h"
#include "components/window_placement/window_placement_policy.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include "platform/display_backend_helper.h"
#include "platform/shell_layer_helper.h"
#include "qt_format.h"
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
/// Loads the per-board app list from <bin>/settings/apps.json. Falls back to
/// defaultApps() when the file is missing, unreadable, empty, or has no valid
/// entries -- so a board only needs to drop an apps.json to customize the
/// launcher/taskbar without a recompile.
QList<desktop_component::AppEntry> loadAppsConfig() {
    // Read from the desktop root (<bin>/../apps.json). Kept out of the
    // board-written settings/ dir so it stays owner-writable on the NFS root.
    const QString path =
        QCoreApplication::applicationDirPath() + QStringLiteral("/../apps.json");
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return desktop_component::defaultApps();
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const auto arr = doc.object().value(QStringLiteral("apps")).toArray();
    QList<desktop_component::AppEntry> apps;
    for (const auto& value : arr) {
        const auto o = value.toObject();
        desktop_component::AppEntry entry;
        entry.app_id = o.value(QStringLiteral("app_id")).toString();
        entry.display_name = o.value(QStringLiteral("display_name")).toString();
        entry.icon_path = o.value(QStringLiteral("icon_path")).toString();
        entry.exec_command = o.value(QStringLiteral("exec_command")).toString();
        if (!entry.app_id.isEmpty() && !entry.exec_command.isEmpty()) {
            apps.append(entry);
        }
    }
    return apps.isEmpty() ? desktop_component::defaultApps() : apps;
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

    // ── Taskbar: bottom-edge panel (centered app icons) ──
    // apps is captured by the click handler to resolve app_id -> exec_command.
    // Loaded from settings/apps.json (per-board app list) if present.
    const QList<cf::desktop::desktop_component::AppEntry> apps = loadAppsConfig();
    auto* taskbar = new cf::desktop::desktop_component::CenteredTaskbar(desktop_entity_);
    taskbar->setApps(apps);
    taskbar->setBackdropSource(shell);
    panel_mgr->registerPanel(taskbar->GetWeak());
    // Shared launch path: resolve app_id -> exec, launch, capture PID. Used by
    // both the taskbar tile click and the launcher popup so the running-state
    // indicator lights for either entry point.
    // Builtin in-process apps live as hidden child widgets of the desktop and
    // are shown when their "builtin:*" exec_command is launched (no QProcess,
    // so no framebuffer fight with the desktop on linuxfb).
    auto* about_panel = new cf::desktop::desktop_component::AboutPanel(desktop_entity_);

    std::function<void(const QString&)> launch_app =
        [apps, app_pid, about_panel, panel_mgr](const QString& app_id) {
            QString exec;
            for (const auto& app : apps) {
                if (app.app_id == app_id) {
                    exec = app.exec_command;
                    break;
                }
            }
            if (exec.isEmpty()) {
                cf::log::warningftag("CFDesktopEntity", "No exec for app_id '{}'",
                                     app_id.toStdString());
                return;
            }
            // Builtin apps render in-process. External apps are launched
            // detached (Stage 2 will add hide-desktop + managed-QProcess for
            // GUI apps that need the full framebuffer).
            if (exec.startsWith(QStringLiteral("builtin:"))) {
                const auto id = exec.mid(QStringLiteral("builtin:").size());
                if (id == QStringLiteral("about") && about_panel != nullptr) {
                    about_panel->popup(panel_mgr->availableGeometry());
                } else {
                    cf::log::warningftag("CFDesktopEntity", "Unknown builtin app '{}'",
                                         id.toStdString());
                }
                return;
            }
            const auto launched = cf::desktop::desktop_component::AppLaunchService::launch(exec);
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
    QObject::connect(
        taskbar, &cf::desktop::desktop_component::CenteredTaskbar::launcherRequested, this,
        [app_launcher, panel_mgr]() {
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