#include "CFDesktopEntity.h"
#include "CFDesktop.h"
#include "CFDesktopWindowProxy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "IDesktopPropertyStrategy.h"
#include "aex/weak_ptr/weak_ptr.h"
#include "cflog.h"
#include "components/DisplayServerBackendFactory.h"
#include "components/IDisplayServerBackend.h"
#include "components/PanelManager.h"
#include "components/WindowManager.h"
#include "components/launcher/app_launch_service.h"
#include "components/launcher/app_launcher.h"
#include "components/statusbar/status_bar.h"
#include "components/taskbar/centered_taskbar.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include "platform/display_backend_helper.h"
#include "platform/shell_layer_helper.h"
#include "qt_format.h"
#include <QHash>
#include <functional>
#include <memory>

namespace cf::desktop {

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

    // Connect PanelManager geometry changes to ShellLayer
    QObject::connect(panel_mgr, &PanelManager::availableGeometryChanged, desktop_entity_,
                     [shell](const QRect& r) { shell->onAvailableGeometryChanged(r); });

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

    // ── Taskbar: bottom-edge panel (centered app icons) ──
    // apps is captured by the click handler to resolve app_id -> exec_command.
    const QList<cf::desktop::desktop_component::AppEntry> apps =
        cf::desktop::desktop_component::defaultApps();
    auto* taskbar = new cf::desktop::desktop_component::CenteredTaskbar(desktop_entity_);
    taskbar->setApps(apps);
    panel_mgr->registerPanel(taskbar->GetWeak());
    // Shared launch path: resolve app_id -> exec, launch, capture PID. Used by
    // both the taskbar tile click and the launcher popup so the running-state
    // indicator lights for either entry point.
    std::function<void(const QString&)> launch_app = [apps, app_pid](const QString& app_id) {
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
        [app_launcher, panel_mgr]() { app_launcher->popup(panel_mgr->availableGeometry()); });
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