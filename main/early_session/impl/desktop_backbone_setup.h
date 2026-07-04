/**
 * @file desktop/main/early_session/impl/desktop_backbone_setup.h
 * @brief Early-stage desktop backbone directory setup.
 *
 * @author CFDesktop Team
 * @ingroup early_session
 */

#pragma once
#include "init_chain/init_stage.h"

namespace cf::desktop::early_stage {
/**
 * @brief Sets up desktop backbone directories during early initialization.
 *
 * Ensures that the required desktop directory structure exists before
 * the main initialization chain begins.
 *
 * @ingroup early_session
 */
class DesktopBackboneEarlySetup : public IEarlyStage {
  public:
    DesktopBackboneEarlySetup() = default;
    ~DesktopBackboneEarlySetup() = default;

    /**
     * @brief Returns the human-readable name of this early stage.
     *
     * @return Stage name identifier.
     */
    std::string_view name() const override { return "Desktop Backbone Setup"; }

    /**
     * @brief Executes the backbone directory setup.
     *
     * @return BootResult indicating success or failure.
     */
    BootResult run_session() override;
    std::optional<unsigned int> atExpectedStageBootup() const override { return {}; }
};
} // namespace cf::desktop::early_stage
