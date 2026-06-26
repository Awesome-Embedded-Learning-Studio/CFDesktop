/**
 * @file    desktop/main/init/early_gain/early_pass_stage.h
 * @brief   Defines the early transfer initialization stage.
 *
 * Provides the EarlyTransferStage class which handles early transformation/setup
 * tasks during the initialization sequence.
 *
 * @author  N/A
 * @date    N/A
 * @version 9cf20027
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "init_stage.h"
#include "init_stage_name.h"

namespace cf::desktop::init_session {

/**
 * @brief  Early transfer initialization stage.
 *
 * Handles early transformation/setup operations during the desktop
 * initialization sequence.
 *
 * @ingroup none
 */
class EarlyTransferStage : public IInitStage {
  public:
    /**
     * @brief  Destroys the EarlyTransferStage instance.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ~EarlyTransferStage() override = default;

    /**
     * @brief  Gets the name of this stage.
     * @return The stage name as a string view.
     * @throws None
     * @note   This function is noexcept.
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    std::string_view name() const noexcept override { return CONFIG_PASS_STAGE; }

    /**
     * @brief  Requests stages that should execute before this stage.
     * @return A vector of weak pointers to prerequisite stages.
     * @throws None
     * @note   Currently returns an empty vector (no dependencies).
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    std::vector<aex::WeakPtr<IInitStage>> request_before_actions_init() const override {
        return {};
    }

    /**
     * @brief  Executes the early transfer stage session.
     * @return StageResult containing the execution status and any messages.
     * @throws None
     * @note   Implements the core logic for this initialization stage.
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    StageResult run_session() override;
};

} // namespace cf::desktop::init_session
