/**
 * @file    init_stage.h
 * @brief   Defines the interface for early initialization stages.
 *
 * Provides the IEarlyStage interface and related enums for implementing
 * boot-time initialization components with stage validation and failure
 * handling.
 *
 * @author  CFDesktop Team
 * @date    2026-03-16
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup early_session
 */
#pragma once
#include <optional>
#include <string_view>

namespace cf::desktop::early_stage {

/**
 * @brief  Interface for early initialization stage components.
 *
 * Defines the contract for boot-time initialization stages. Each stage
 * provides name identification, expected stage validation, mismatch policy,
 * and execution session control.
 *
 * @note   Not thread-safe unless externally synchronized.
 * @warning None
 * @since  0.13.0
 * @ingroup early_session
 *
 * @code
 * class MyStage : public IEarlyStage {
 *     std::string_view name() const override { return "MyStage"; }
 *     BootResult run_session() override { return BootResult::OK; }
 * };
 * @endcode
 */
class IEarlyStage {
  public:
    /**
     * @brief  Destroys the early stage instance.
     */
    virtual ~IEarlyStage() = default;

    /**
     * @brief  Constructs an early stage with critical flag.
     *
     * @param[in] critical If true, stage failure causes shutdown.
     * @return             None
     * @throws             None
     * @note               None
     * @warning            None
     * @since              0.13.0
     * @ingroup            early_session
     */
    IEarlyStage(bool critical = true) : critical_(critical) {};

    /**
     * @brief  Returns the name of this early stage.
     *
     * Provides a string identifier for logging and debugging.
     *
     * @return     String view containing the stage name.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    virtual std::string_view name() const = 0;

    /**
     * @brief  Validates the current boot stage matches the expected stage.
     *
     * Returns the expected stage number if the current boot stage matches.
     * Otherwise, the mismatch policy determines the response.
     *
     * @return     Expected stage number if valid, empty if mismatch.
     * @throws     None
     * @note       The mismatch policy (mismatch_policy) is invoked when
     *             the current stage does not match the expected stage.
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    virtual std::optional<unsigned int> atExpectedStageBootup() const = 0;

    /**
     * @brief  Policy for handling unexpected boot stage mismatches.
     *
     * Defines the available actions when the current boot stage does not
     * match the expected stage for a component.
     *
     * @since  0.13.0
     * @ingroup early_session
     */
    enum class UnexpectedStageBootSolution {
        IGNORE,            ///< Ignore the mismatch and continue.
        IGNORE_BUT_NOTIFY, ///< Ignore but log a notification.
        SHUTDOWN           ///< Terminate the application.
    };

    /**
     * @brief  Returns the mismatch policy for unexpected stage boot.
     *
     * Retrieves the configured behavior when the boot stage does not match
     * the expected stage.
     *
     * @return     The mismatch policy enum value.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    UnexpectedStageBootSolution mismatch_policy() { return policy; };

    /**
     * @brief  Result of an initialization stage execution.
     *
     * Indicates the outcome of a stage session run. Critical failures
     * may trigger application shutdown.
     *
     * @since  0.13.0
     * @ingroup early_session
     */
    enum class BootResult {
        CRITICAL_FAILED, ///< Critical failure; application may shutdown.
        FAILED,          ///< Non-critical failure; execution continues.
        OK               ///< Stage completed successfully.
    };

    /**
     * @brief  Executes the initialization stage session.
     *
     * Performs the core initialization logic for this stage.
     *
     * @return     Boot result indicating success or failure type.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.13.0
     * @ingroup    early_session
     */
    virtual BootResult run_session() = 0;

  protected:
    /**
     * @brief  Policy for handling stage mismatches.
     *
     * Default is IGNORE_BUT_NOTIFY.
     */
    UnexpectedStageBootSolution policy{UnexpectedStageBootSolution::IGNORE_BUT_NOTIFY};

    /**
     * @brief  Indicates if stage failure is critical.
     *
     * True if failure triggers application shutdown.
     */
    bool critical_;
};
} // namespace cf::desktop::early_stage