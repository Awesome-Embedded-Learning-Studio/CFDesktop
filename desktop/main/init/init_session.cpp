#include "cflog.h"
#include "desktop_backbone_init/desktop_backbone_init.h"
#include "desktop_config_init/desktop_config_init.h"
#include "early_gain/early_pass_stage.h"
#include "gui_progress/gui_init_stage.h"
#include "init_session_chain.h"
#include "init_settings.h"
#include <QWidget>
#include <memory>

namespace cf::desktop::init_session {
void RunStageInit() {
    init_session::InitSessionChain initSessionChain;

    initSessionChain.add_stage_back(std::make_unique<GuiLogoBootStage>());
    initSessionChain.add_stage_back(std::make_unique<EarlyTransferStage>());
    initSessionChain.add_stage_back(std::make_unique<DesktopBackboneSetupStage>());
    initSessionChain.add_stage_back(std::make_unique<DesktopConfigInitStage>());
}

void ReleaseStageInitOldResources() {
    QWidget* bootWidget = InitInfoHandle::instance().unlockBootWidget();

    log::trace("Release the Boot Widget For Memory usage");
    delete bootWidget;
}

} // namespace cf::desktop::init_session