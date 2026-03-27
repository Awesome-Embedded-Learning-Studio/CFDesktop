/**
 * @file    desktop/ui/CFDesktopEntity.h
 * @brief   Defines the CFDesktopEntity class which manages the desktop application
 *          lifecycle and initialization.
 *
 * Provides singleton access to the desktop instance and handles setup methods
 * and results for initialization sequences.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once

#include "../export.h"
#include "base/weak_ptr/weak_ptr.h"
#include <QObject>
#include <memory>

namespace cf::desktop::platform_strategy {
class PlatformFactory;
}

namespace cf::desktop {

class CFDesktop;

/**
 * @brief  Manages the desktop application lifecycle and initialization.
 *
 * CFDesktopEntity serves as the central singleton for desktop management,
 * providing access to the desktop widget and platform factory. Handles
 * synchronous and asynchronous initialization sequences.
 *
 * @note   Not thread-safe unless externally synchronized.
 * @ingroup none
 *
 * @code
 * auto& entity = CFDesktopEntity::instance();
 * auto result = entity.run_init(RunsSetupMethod::ASYNC);
 * @endcode
 */
// Real Handles
class CF_DESKTOP_EXPORT CFDesktopEntity : public QObject {
  public:
    /**
     * @brief  Returns the singleton instance of CFDesktopEntity.
     *
     * @return             Reference to the global CFDesktopEntity instance.
     * @throws             None
     * @note               The instance is created on first access.
     * @warning            Do not call after destruction during shutdown.
     * @since              N/A
     * @ingroup            none
     */
    static CFDesktopEntity& instance();

    /**
     * @brief  Describes the method used for desktop initialization.
     * @ingroup none
     */
    enum class RunsSetupMethod {
        ASYNC, ///< Performs initialization asynchronously.
        SYNC   ///< Performs initialization synchronously.
    };

    /**
     * @brief  Represents the result of a desktop initialization run.
     * @ingroup none
     */
    enum class RunsSetupResult {
        OK,            ///< Initialization completed successfully.
        PartialFailed, ///< Some non-critical components failed to initialize.
        CriticalFailed ///< Critical initialization failed.
    };

    /* If One day, Desktop Run up itself slow, async boots can be fine */
    /* Runs uncritical issue like, plugin service, and so on... */

    /**
     * @brief  Runs the desktop initialization sequence.
     *
     * Executes the setup process using the specified method. Handles
     * initialization of core desktop services and platform components.
     *
     * @param[in] m        Method for running initialization (SYNC by default).
     * @return             Result status indicating the outcome of initialization.
     * @throws             None
     * @note               Async mode allows non-blocking startup for slow services.
     * @warning            Critical failures may prevent desktop functionality.
     * @since              N/A
     * @ingroup            none
     */
    RunsSetupResult run_init(RunsSetupMethod m = RunsSetupMethod::SYNC);

    /**
     * @brief  Gets a weak pointer to the desktop widget.
     *
     * @return             WeakPtr referencing the desktop widget; may be empty.
     * @throws             None
     * @note               Returns observer reference; caller does not own the widget.
     * @warning            None
     * @since              N/A
     * @ingroup            none
     */
    WeakPtr<CFDesktop> desktop_widget() const;

    /**
     * @brief  Destroys the CFDesktopEntity instance.
     *
     * Cleans up platform factory and desktop resources.
     *
     * @throws             None
     * @note               None
     * @warning            None
     * @since              N/A
     * @ingroup            none
     */
    ~CFDesktopEntity();

  protected:
    /**
     * @brief  Constructs the CFDesktopEntity instance.
     *
     * Initializes platform factory and internal desktop state.
     *
     * @throws             None
     * @note               Constructor is protected; access through instance().
     * @warning            None
     * @since              N/A
     * @ingroup            none
     */
    CFDesktopEntity();

  private:
    /// @brief Pointer to the desktop widget instance. Ownership: owner.
    CFDesktop* desktop_entity_{nullptr};

    /// @brief Factory for creating platform-specific components. Ownership: owner.
    std::unique_ptr<platform_strategy::PlatformFactory> platform_factory_;

  private:
    /// @brief Global singleton instance of CFDesktopEntity. Ownership: owner.
    static std::unique_ptr<CFDesktopEntity> global_instance_;
};

} // namespace cf::desktop
