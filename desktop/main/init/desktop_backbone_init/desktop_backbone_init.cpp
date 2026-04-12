#include "desktop_backbone_init.h"
#include "cfpath/desktop_main_path_resolvers.h"
#include "init_session_chain.h"
#include "init_settings.h"
#include "init_stage_name.h"

namespace cf::desktop::init_session {

std::vector<WeakPtr<IInitStage>> DesktopBackboneSetupStage::request_before_actions_init() const {
    const auto chain_ref = InitSessionChain::GetChainRef();
    if (!chain_ref) {
        return {};
    }
    return {chain_ref->find_stage(
        CONFIG_PASS_STAGE)}; // Must behind the config pass, else we can not search it!
}

DesktopBackboneSetupStage::StageResult DesktopBackboneSetupStage::run_session() {
    const auto& root_position = InitInfoHandle::instance().root_position();
    if (root_position.isEmpty()) {
        return StageResult::critical_failed("Missing Root Desktop Dirent",
                                            "Missing Root Desktop Dirent");
    }
    /* Runs Init */
    path::DesktopMainPathProvider::init(root_position);

    /* And Setup then */
    path::DesktopMainPathProvider::instance().setup();

    return StageResult::ok("Setup the backbone of the desktop!");
}

} // namespace cf::desktop::init_session