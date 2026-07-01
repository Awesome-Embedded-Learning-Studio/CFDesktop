#include "desktop_entry.h"
#include "init/init.h"
#include "ui/CFDesktopEntity.h"
#include "ui/CFDesktopWindowProxy.h"

namespace cf::desktop {

DesktopExitResult boot_desktop() {
    /* Desktop Placing here, for easy access handle... by manual :) */
    // Make A Instance Here
    auto& instance [[maybe_unused]] = CFDesktopEntity::instance();
    /* We might lately pass the entry to each components */
    instance.run_init();

    CFDesktopWindowProxy window_proxy;
    window_proxy.show_desktop();

    /* Ok, Can run off */
    init_session::ReleaseStageInitOldResources();

    // Oh, quit normally
    return DesktopExitResult::NORMAL_QUIT;
}

} // namespace cf::desktop
