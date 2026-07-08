/**
 * @file    ipc_stage.h
 * @brief   IPC server initialization stage for the early boot sequence.
 *
 * Defines the stage that starts the single-instance IPC server so a
 * second shell instance can signal the running one (e.g. to raise it).
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
 * @brief  Starts the single-instance IPC server.
 *
 * Brings up the QLocalServer-based IPC server during early boot so that
 * a second shell instance can ask this one to raise itself. Must run
 * after the logger stage so failures are observable.
 *
 * @note           Non-critical: failure to start does not halt boot.
 * @warning        None
 * @since          0.19.0
 * @ingroup        early_session
 */
class IpcStage : public IEarlyStage {
  public:
    IpcStage() = default;

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
    std::string_view name() const override { return "IPC Server Boot"; }

    /**
     * @brief  Returns the expected position in the boot sequence.
     *
     * @return     Expected boot position (3 = fourth stage, after logger).
     * @throws     None
     * @note       Must run after the logger stage.
     * @warning    None
     * @since      0.19.0
     * @ingroup    early_session
     */
    std::optional<unsigned int> atExpectedStageBootup() const override { return 3; }

    /**
     * @brief  Starts the IPC server.
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
