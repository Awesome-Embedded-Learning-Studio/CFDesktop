#pragma once

#include "init_chain/init_stage.h"

namespace cf::desktop::early_stage {

/**
 * @file    early_config_stage.h
 * @brief   Early configuration stage for boot sequence.
 *
 * Defines the early configuration stage that loads and applies initial
 * configuration during the boot process.
 *
 * @author  CFDesktop Team
 * @date    2026-03-16
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup early_session
 */

/**
 * @brief  Early configuration stage.
 *
 * Loads and applies initial configuration during the early boot sequence.
 * This stage must run at position 1 (after ConsoleSignalStage) in the initialization order.
 *
 * @note           This stage must execute at position 1 (second stage).
 * @warning        None
 * @since          0.13.0
 * @ingroup        early_session
 */
class EarlyConfigStage : public IEarlyStage {
  public:
    EarlyConfigStage();

    /**
     * @brief  Returns the name of this stage.
     *
     * @return     Stage name as a string view.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    std::string_view name() const override { return "Early Config Boot"; }

    /**
     * @brief  Returns the expected position in the boot sequence.
     *
     * @return     Expected boot position (0 = first stage).
     * @throws     None
     * @note       This stage must run first in the boot sequence.
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    std::optional<unsigned int> atExpectedStageBootup() const override { return 1; }

    /**
     * @brief  Runs the early configuration session.
     *
     * @return     Result of the boot operation.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    BootResult run_session() override;
};

} // namespace cf::desktop::early_stage
