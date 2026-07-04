#include "desktop_backbone_setup.h"
#include "cflog.h"
#include "early_handle/early_handle.h"
#include "file_op.h"

namespace cf::desktop::early_stage {
DesktopBackboneEarlySetup::BootResult DesktopBackboneEarlySetup::run_session() {
    /* Backbones setups */
    // Get the Early Stage
    const auto desktop_root = EarlyHandle::instance().early_settings().get_desktop_root();
    log::infoftag("Desktop Early Backbone Boot", "Get the desktop Early Boot: {}",
                  desktop_root.toStdString());

    if (!base::filesystem::exsited(desktop_root)) {
        log::warningftag("Desktop Early Backbone Boot",
                         "{} indicated as root desktop does not exsited! attempting to create one!",
                         desktop_root.toStdString());
        if (!base::filesystem::create_anyway(desktop_root)) {
            log::errorftag("Desktop Early Backbone Boot",
                           "Failed to create the desktop root, maybe the path is never you want! "
                           "or the permisson denied...");
            return BootResult::CRITICAL_FAILED;
        }
    }

    return BootResult::OK;
}
} // namespace cf::desktop::early_stage
