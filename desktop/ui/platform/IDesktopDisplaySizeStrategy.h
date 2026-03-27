/**
 * @file    IDesktopDisplaySizeStrategy.h
 * @brief   Defines the interface for desktop display size strategies.
 *
 * Provides abstract methods for querying desktop display behaviors and applying
 * size-related strategies to widgets.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-26
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "IDesktopPropertyStrategy.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include <QFlags>

class QWidget;

namespace cf::desktop::platform_strategy {

/**
 * @brief  Defines behavioral flags for desktop window management.
 *
 * Controls how desktop windows interact with the system window manager,
 * including fullscreen behavior, frame rendering, and z-order positioning.
 *
 * @ingroup none
 */
enum class DesktopBehaviorFlag {
    Fullscreen = 0x1,     ///< Window occupies the entire screen.
    Frameless = 0x2,      ///< Window renders without a title bar or borders.
    StayOnBottom = 0x4,   ///< Window remains below other windows.
    AllowResize = 0x8,    ///< Window allows user-initiated resizing.
    AvoidSystemUI = 0x10, ///< Window avoids system UI elements (taskbar, docks).
};

Q_DECLARE_FLAGS(DesktopBehaviors, DesktopBehaviorFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DesktopBehaviors)

/**
 * @brief  Abstract interface for desktop display size strategies.
 *
 * Defines the contract for platform-specific display size and behavior
 * strategies. Provides weak pointer support for safe ownership tracking.
 *
 * @note   Not thread-safe unless externally synchronized.
 * @ingroup none
 *
 * @code
 * auto strategy = CreateStrategy();
 * auto weak = strategy->GetOne();
 * strategy->action(widget);
 * auto behaviors = strategy->query();
 * @endcode
 */
class CF_DESKTOP_EXPORT IDesktopDisplaySizeStrategy : public IDesktopPropertyStrategy {
  public:
    /**
     * @brief  Constructs a DisplaySizePolicy strategy instance.
     */
    IDesktopDisplaySizeStrategy()
        : IDesktopPropertyStrategy(StrategyType::DisplaySizePolicy), weak_factory_ptr_(this) {}

    /**
     * @brief  Destroys the strategy instance.
     */
    virtual ~IDesktopDisplaySizeStrategy() = default;

    /**
     * @brief  Returns a weak pointer to this instance.
     *
     * Provides non-owning access for safe lifecycle tracking.
     *
     * @return Weak pointer to this strategy instance.
     * @throws None
     * @note   None
     * @warning None
     * @since   0.1
     * @ingroup none
     */
    WeakPtr<IDesktopDisplaySizeStrategy> GetOne() { return weak_factory_ptr_.GetWeakPtr(); }

    /**
     * @brief  Applies the strategy behavior to the specified widget.
     *
     * The default implementation performs no action, allowing the desktop
     * to exhibit native Qt default behaviors.
     *
     * @param[in] widget_data Pointer to the target widget. May be nullptr.
     * @return                True if the action succeeded, false otherwise.
     * @throws                None
     * @note                  The default implementation returns true without
     *                        modifying the widget.
     * @warning               None
     * @since                 0.1
     * @ingroup               none
     */
    virtual bool action(QWidget* widget_data) { return true; }

    /**
     * @brief  Queries the system for desktop behavior flags.
     *
     * Retrieves the current set of desktop behaviors supported or active
     * on the system.
     *
     * @return Desktop behavior flags indicating system capabilities.
     * @throws None
     * @note   None
     * @warning None
     * @since   0.1
     * @ingroup none
     */
    virtual DesktopBehaviors query() const;

  private:
    /**
     * @brief  Factory for creating weak pointers to this instance.
     *
     * Ownership: owner. Manages the weak reference lifecycle.
     */
    WeakPtrFactory<IDesktopDisplaySizeStrategy> weak_factory_ptr_;
};

} // namespace cf::desktop::platform_strategy
