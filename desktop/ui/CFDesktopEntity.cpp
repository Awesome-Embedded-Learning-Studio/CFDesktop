#include "CFDesktopEntity.h"
#include "CFDesktop.h"
#include "CFDesktopWindowProxy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "IDesktopPropertyStrategy.h"
#include "base/macro/system_judge.h"
#include "base/weak_ptr/weak_ptr.h"
#include "cflog.h"
#include "components/IDisplayServerBackend.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include <memory>

#ifdef CFDESKTOP_OS_WINDOWS
#    include "platform/windows/windows_display_server_backend.h"
#endif

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
    log::tracef("Desktop Entity is created");
}

CFDesktopEntity::~CFDesktopEntity() {
    log::tracef("Dekstop Entuty is released");
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
#ifdef CFDESKTOP_OS_WINDOWS
    display_backend_ = std::make_unique<backend::windows::WindowsDisplayServerBackend>();

    if (display_backend_->initialize(0, nullptr)) {
        auto wb = display_backend_->windowBackend();
        if (wb) {
            auto* raw = wb.Get();
            QObject::connect(raw, &IWindowBackend::window_came, this, [](WeakPtr<IWindow> win) {
                if (win) {
                    cf::log::traceftag("CFDesktopEntity", "External window detected: %s",
                                       win->title().toStdString().c_str());
                }
            });
            QObject::connect(raw, &IWindowBackend::window_gone, this, [](WeakPtr<IWindow> /*win*/) {
                cf::log::traceftag("CFDesktopEntity", "External window gone");
            });
        }
    } else {
        log::errorftag("CFDesktopEntity", "Windows display server backend init failed");
    }
#endif

    log::trace("Entity Init");
    return RunsSetupResult::OK;
}

WeakPtr<CFDesktop> CFDesktopEntity::desktop_widget() const {
    return desktop_entity_->GetWeak();
}

} // namespace cf::desktop