#pragma once
#include "aex/weak_ptr/weak_ptr.h"
#include "aex/weak_ptr/weak_ptr_factory.h"
#include <QObject>
#include <QString>
#include <string_view>
#include <vector>

/**
 * @file    desktop/main/init/init_stage.h
 * @brief   Defines the initialization stage interface for staged boot process.
 *
 * Provides the IInitStage interface and StageResult structure for managing
 * sequential initialization operations with dependency tracking and error handling.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup desktop_init
 */

namespace cf::desktop::init_session {

/**
 * @brief  Interface for initialization stages in the boot process.
 *
 * Defines a contract for individual initialization steps that can be chained
 * together with dependency tracking. Each stage reports success, partial failure,
 * or critical failure to control execution flow.
 *
 * @note   Not thread-safe unless externally synchronized.
 * @ingroup desktop_init
 *
 * @code
 * class MyStage : public IInitStage {
 *     StageResult run_session() override { return StageResult::ok(); }
 *     std::string_view name() const noexcept override { return "MyStage"; }
 * };
 * @endcode
 */
class IInitStage {
  public:
    /**
     * @brief  Status codes for stage execution results.
     *
     * Indicates the outcome of an initialization stage execution.
     *
     * @ingroup desktop_init
     */
    enum class StatusCode {
        OK,            ///< Complete success; stage executed fully.
        PartialFailed, ///< Partial failure; stage completed with errors but can continue.
        CriticalFailed ///< Critical failure; stage failed and execution must stop.
    };

    /**
     * @brief  Detailed result of stage execution.
     *
     * Encapsulates the status code, message, and error details for an
     * initialization stage execution.
     *
     * @ingroup desktop_init
     */
    struct StageResult {
        StatusCode code;      ///< Status code indicating execution outcome.
        QString message;      ///< Human-readable result message (success or failure description).
        QString error_detail; ///< Detailed error information; only present on failure.

        /**
         * @brief  Creates a successful result.
         * @param[in] msg Optional success message. Empty if not provided.
         * @return         StageResult with OK status.
         * @throws         None
         * @note           None
         * @warning        None
         * @since          N/A
         * @ingroup        desktop_init
         */
        static StageResult ok(const QString& msg = QString());

        /**
         * @brief  Creates a partial failure result.
         * @param[in] msg    Summary message describing the partial failure.
         * @param[in] detail Detailed error information.
         * @return           StageResult with PartialFailed status.
         * @throws           None
         * @note             None
         * @warning          None
         * @since            N/A
         * @ingroup          desktop_init
         */
        static StageResult partial_failed(const QString& msg, const QString& detail);

        /**
         * @brief  Creates a critical failure result.
         * @param[in] msg    Summary message describing the critical failure.
         * @param[in] detail Detailed error information.
         * @return           StageResult with CriticalFailed status.
         * @throws           None
         * @note             None
         * @warning          None
         * @since            N/A
         * @ingroup          desktop_init
         */
        static StageResult critical_failed(const QString& msg, const QString& detail);

        /**
         * @brief  Checks whether subsequent stages may execute.
         * @return         true if status is not CriticalFailed, false otherwise.
         * @throws         None
         * @note           Non-critical failures allow continuation.
         * @warning        None
         * @since          N/A
         * @ingroup        desktop_init
         */
        bool can_continue() const noexcept { return code != StatusCode::CriticalFailed; }

        /**
         * @brief  Explicit boolean conversion operator.
         * @return         true only if status is OK, false otherwise.
         * @throws         None
         * @note           Partial failures return false; only full success returns true.
         * @warning        None
         * @since          N/A
         * @ingroup        desktop_init
         */
        explicit operator bool() const noexcept { return code == StatusCode::OK; }
    };

  public:
    IInitStage() : weak_ptr_factory(this) {}
    virtual ~IInitStage() = default;

    /**
     * @brief  Gets the unique name identifier for this stage.
     * @return         Name of the stage as a string view.
     * @throws         None
     * @note           Each concrete stage must provide a unique name.
     * @warning        None
     * @since          N/A
     * @ingroup        desktop_init
     */
    virtual std::string_view name() const noexcept = 0;

    /**
     * @brief  Requests stages that must execute before this stage.
     * @return         Vector of weak pointers to prerequisite stages.
     * @throws         None
     * @note           Used to build the dependency graph for stage execution order.
     * @warning        Default implementation returns an empty vector (no dependencies).
     * @since          N/A
     * @ingroup        desktop_init
     */
    virtual std::vector<aex::WeakPtr<IInitStage>> request_before_actions_init() const;

    /**
     * @brief  Executes the initialization logic for this stage.
     * @return         StageResult indicating the execution outcome.
     * @throws         None
     * @note           Must be implemented by each concrete stage.
     * @warning        None
     * @since          N/A
     * @ingroup        desktop_init
     */
    virtual StageResult run_session() = 0;

    /**
     * @brief  Gets a weak pointer to this stage.
     * @return         aex::WeakPtr<IInitStage> pointing to this stage, or invalid if unsupported.
     * @throws         None
     * @note           Default returns an invalid aex::WeakPtr. Concrete stages with a
     *                 aex::WeakPtrFactory should override this method.
     * @warning        None
     * @since          N/A
     * @ingroup        desktop_init
     */
    virtual aex::WeakPtr<IInitStage> get_weak_ptr() const { return nullptr; }

  private:
    aex::WeakPtrFactory<IInitStage> weak_ptr_factory;
};

} // namespace cf::desktop::init_session
