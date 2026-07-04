#include "gui_init_stage.h"
#include "init_session_chain.h"
#include "init_settings.h"
#include "ui/widget/init_session/boot_progress_widget.h"

namespace cf::desktop::init_session {

GuiLogoBootStage::~GuiLogoBootStage() = default;

GuiLogoBootStage::StageResult GuiLogoBootStage::run_session() {
    /* Currently Selected by Simple */
    BootProgressWidget* widget = GetBootWidget(BootstrapStyle::Simple);
    widget->setInitialDisplayStatus();

    auto chain_ref_weak_ = InitSessionChain::GetChainRef();
    if (!chain_ref_weak_) {
        return StageResult::critical_failed("Boot Subsystem Meets Memory Crashing!",
                                            "Internal Chain Broken sucks!");
    }
    InitSessionChain* chain_ptr = chain_ref_weak_.Get();
    widget->setTotalStage(chain_ptr->size());
    BootProgressWidget::connect(chain_ptr, &InitSessionChain::stage_start, widget,
                                &BootProgressWidget::do_stage_start);
    /* Set the boot widget */
    InitInfoHandle::instance().setBootWidget(widget);
    widget->show();

    return StageResult::ok("BootWidgetStage Show OK");
}
} // namespace cf::desktop::init_session
