#include "early_pass_stage.h"
#include "early_handle/early_handle.h"
#include "init_settings.h"

namespace cf::desktop::init_session {

EarlyTransferStage::StageResult EarlyTransferStage::run_session() {
    /* Pass the handle */
    InitInfoHandle::instance().passedEarlyBootInfo(early_stage::EarlyHandle::instance());
    return StageResult::ok("Early Transfer OK");
}
} // namespace cf::desktop::init_session