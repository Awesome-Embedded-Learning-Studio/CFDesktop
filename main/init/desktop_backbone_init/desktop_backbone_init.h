/**
 * @file desktop/main/init/desktop_backbone_init/desktop_backbone_init.h
 * @brief Initialization stage for desktop backbone setup.
 *
 * @author CFDesktop Team
 * @ingroup init_session
 */

#pragma once
#include "init_stage.h"
#include "init_stage_name.h"
namespace cf::desktop::init_session {

/**
 * @brief Initialization stage that sets up the desktop backbone structure.
 *
 * @ingroup init_session
 */
class DesktopBackboneSetupStage : public IInitStage {
  public:
    /**
     * @brief Returns the stage name identifier.
     *
     * @return Compile-time constant stage name.
     */
    std::string_view name() const noexcept override { return DESKTOP_BACKBONE_SETUP; }
    std::vector<aex::WeakPtr<IInitStage>> request_before_actions_init() const override;

    /**
     * @brief Executes the backbone setup initialization logic.
     *
     * @return StageResult indicating success or failure.
     */
    StageResult run_session() override;
};

} // namespace cf::desktop::init_session
