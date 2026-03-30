#pragma once
#include "init_chain/init_stage.h"

namespace cf::desktop::early_stage {

/**
 * @file    logger_stage.h
 * @brief   Logger initialization stage for early boot sequence.
 *
 * Defines the logger stage that initializes the logging system during the
 * early boot process.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup early_session
 */

/**
 * @brief  Logger initialization stage.
 *
 * Initializes the logging system during the early boot sequence. This stage
 * must run third in the initialization order.
 *
 * @note           This stage must execute at position 2 (third stage).
 * @warning        None
 * @since          N/A
 * @ingroup        early_session
 */
class LoggerStage : public IEarlyStage {
  public:
    LoggerStage() = default;

    /**
     * @brief  Returns the name of this stage.
     *
     * @return     Stage name as a string view.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    early_session
     */
    std::string_view name() const override { return "Logger Boot"; }

    /**
     * @brief  Returns the expected position in the boot sequence.
     *
     * @return     Expected boot position (1 = second stage).
     * @throws     None
     * @note       Must be at second position in boot sequence.
     * @warning    None
     * @since      N/A
     * @ingroup    early_session
     */
    std::optional<unsigned int> atExpectedStageBootup() const override { return 2; }

    /**
     * @brief  Runs the logger initialization session.
     *
     * @return     Result of the boot operation.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    early_session
     */
    BootResult run_session() override;
};

} // namespace cf::desktop::early_stage
