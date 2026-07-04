/**
 * @file    console_signal_stage.h
 * @brief   Platform console signal handler stage for early boot sequence.
 *
 * Installs platform-specific console signal handlers (e.g. Ctrl+C on Windows)
 * to ensure graceful Qt shutdown during the early boot process.
 *
 * @ingroup early_session
 */
#pragma once
#include "init_chain/init_stage.h"

namespace cf::desktop::early_stage {

/**
 * @brief  Console signal handler initialization stage.
 *
 * Installs platform-specific handlers for console control signals so that
 * the application shuts down gracefully via the Qt event loop rather than
 * through the default process-termination behavior.
 *
 * @note           Must run as early as possible (before logger and config).
 * @warning        None
 * @since          0.13.0
 * @ingroup        early_session
 */
class ConsoleSignalStage : public IEarlyStage {
  public:
    ConsoleSignalStage() = default;

    /**
     * @brief  Returns the human-readable name of this stage.
     *
     * @return         Static string view identifying the console signal stage.
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.13.0
     * @ingroup        early_session
     */
    std::string_view name() const override { return "Console Signal Handler"; }

    /**
     * @brief  Validates that this stage runs at boot stage zero.
     *
     * Returns the expected boot stage number for the console signal
     * handler, which must execute before any other initialization.
     *
     * @return         Optional containing the expected stage number (0).
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.13.0
     * @ingroup        early_session
     */
    std::optional<unsigned int> atExpectedStageBootup() const override { return 0; }

    /**
     * @brief  Executes the console signal handler installation.
     *
     * Installs platform-specific handlers for console control signals so
     * that the application shuts down gracefully via the Qt event loop.
     *
     * @return         BootResult indicating success or failure of handler
     *                 installation.
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.13.0
     * @ingroup        early_session
     */
    BootResult run_session() override;
};

} // namespace cf::desktop::early_stage
