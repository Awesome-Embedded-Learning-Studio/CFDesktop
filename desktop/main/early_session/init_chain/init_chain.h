/**
 * @file    init_chain.h
 * @brief   Defines the early initialization chain runner for stage-based boot.
 *
 * Provides the EarlyInitRunner class that manages sequential execution of
 * initialization stages. Critical stages (e.g., logger) execute before the
 * main stage chain.
 *
 * @author  CFDesktop Team
 * @date    2026-03-16
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup early_session
 */
#pragma once
#include "init_stage.h"
#include <functional>
#include <memory>
#include <vector>

namespace cf::desktop::early_stage {

/**
 * @brief  Runner for executing early initialization stages in sequence.
 *
 * Manages the registration and execution of initialization stages.
 * The first stages (critical boot like logger) execute before the main
 * stage chain. Each stage is created, executed, and then discarded.
 *
 * @note   Stage execution is synchronous. Dependency graph management is
 *         not provided; dependent stages must be wrapped as a single stage.
 * @warning None
 * @since  0.13.0
 * @ingroup early_session
 */
class EarlyInitRunner {
  public:
    EarlyInitRunner() = default;
    ~EarlyInitRunner();
    /**
     * @brief  Factory function type for creating early stage instances.
     *
     * Each registered factory creates a new IEarlyStage instance when the
     * chain executes.
     */
    using SessionCreation = std::function<std::unique_ptr<IEarlyStage>(void)>;

    /**
     * @brief  Registers a stage factory to the main execution chain.
     *
     * Adds the factory to the back of the main stage queue. Stages execute
     * in registration order.
     *
     * @param[in] f Factory function that creates the stage instance.
     * @return     None
     * @throws     None
     * @note       Registration is synchronous. Multithreaded execution is
     *             not supported.
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    void register_stage_back(SessionCreation f) { early_stages_.emplace_back(std::move(f)); }

    /**
     * @brief  Registers a critical stage that executes before the main chain.
     *
     * Only critical boot components (e.g., logger) should use this method.
     * These stages run before any main stage to ensure logging and other
     * critical services are available.
     *
     * @param[in] f Factory function that creates the stage instance.
     * @return     None
     * @throws     None
     * @note       Currently only the logger is allowed to register here.
     * @warning    Abuse of this method may result in missing log output.
     * @since      0.13.0
     * @ingroup    early_session
     */
    void register_stage_execute_before(SessionCreation f) {
        first_stages_.emplace_back(std::move(f));
    }
    /**
     * @brief  Executes all registered initialization stages in sequence.
     *
     * Creates and runs each stage instance in order: first stages execute
     * before main stages. Each stage is discarded after execution.
     *
     * @return     None
     * @throws     None
     * @note       Execution order: first stages (critical), then main stages.
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    void execute();

  private:
    /**
     * @brief  Executes first stages without logging enabled.
     *
     * Runs the critical stage chain (e.g., logger initialization) when
     * logging is not yet available.
     */
    void execute_with_no_log_enabled();

    /**
     * @brief  Executes main stages with logging enabled.
     *
     * Runs the standard stage chain after logging is initialized.
     */
    void execute_with_log();

    /**
     * @brief  Terminates the application due to critical boot failure.
     */
    void direct_quit_app();

  private:
    bool boot_finished_{false};
    int stage_at_current{0};
    std::vector<SessionCreation> first_stages_;
    std::vector<SessionCreation> early_stages_;
};
} // namespace cf::desktop::early_stage