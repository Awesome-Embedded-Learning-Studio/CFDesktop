/**
 * @file    crash_handler_stage.h
 * @brief   Crash handler initialization stage for the early boot sequence.
 *
 * Defines the stage that arms the crash signal handlers and folds any raw
 * crash snapshot left by a previous (crashed) run into a finalized report.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup early_session
 */

#pragma once
#include "init_chain/init_stage.h"

#include <optional>
#include <string_view>

namespace cf::desktop::early_stage {

/**
 * @brief  Arms crash signal capture and finalizes prior crash reports.
 *
 * Installs the async-signal-safe handlers so a SIGSEGV/SIGABRT/... drops a
 * raw @c .pending snapshot, then folds any leftover @c .pending (from a
 * previous run) into a finalized @c .json report using the logger tail as
 * last_logs. Must run after the logger stage so the logger path is known.
 *
 * @note           Non-critical: failure does not halt boot (best-effort).
 * @warning        None
 * @since          0.19.0
 * @ingroup        early_session
 */
class CrashHandlerStage : public IEarlyStage {
  public:
    CrashHandlerStage() = default;

    /**
     * @brief  Returns the name of this stage.
     *
     * @return     Stage name as a string view.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    early_session
     */
    std::string_view name() const override { return "Crash Handler Boot"; }

    /**
     * @brief  Returns the expected position in the boot sequence.
     *
     * @return     Expected boot position (4 = fifth stage, after IPC).
     * @throws     None
     * @note       Must run after the logger and IPC stages.
     * @warning    None
     * @since      0.19.0
     * @ingroup    early_session
     */
    std::optional<unsigned int> atExpectedStageBootup() const override { return 4; }

    /**
     * @brief  Arms handlers and finalizes prior reports.
     *
     * @return     Result of the boot operation.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    early_session
     */
    BootResult run_session() override;
};

} // namespace cf::desktop::early_stage
