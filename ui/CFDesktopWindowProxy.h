/**
 * @file    CFDesktopWindowProxy.h
 * @brief   Window-based proxy for CFDesktop display management.
 *
 * Provides window display strategy management and desktop show functionality.
 * Extends CFDesktopProxy with window-specific behavior control.
 *
 * @author  CFDesktop Team
 * @date    2026-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui
 */
#pragma once
#include "../export.h"
#include "CFDesktopProxy.h"
#include "platform/DesktopPropertyStrategyFactory.h"
#include "platform/IDesktopDisplaySizeStrategy.h"
#include <memory>

class QWidget;

namespace cf::desktop {

/**
 * @brief  Window-based proxy for desktop display and strategy management.
 *
 * Manages desktop display strategies and provides window show functionality.
 * Extends CFDesktopProxy with window-specific behavior control.
 *
 * @note   Not thread-safe unless externally synchronized.
 *
 * @ingroup desktop_ui
 *
 * @code
 * CFDesktopWindowProxy proxy;
 * proxy.set_window_display_strategy(std::move(strategy));
 * proxy.activate_window_display_strategy();
 * proxy.show_desktop();
 * @endcode
 */
class CF_DESKTOP_EXPORT CFDesktopWindowProxy : public CFDesktopProxy {
  public:
    /**
     * @brief  Default constructor. Creates a window proxy instance.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui
     */
    CFDesktopWindowProxy();

    using StrategyDeleter = platform_strategy::PlatformFactory::StrategyDeleter;
    using DesktopDisplayStrategy =
        std::unique_ptr<platform_strategy::IDesktopDisplaySizeStrategy, StrategyDeleter>;

    /**
     * @brief  Sets the window display strategy for desktop behavior.
     *
     * Configures the strategy that controls window display properties such
     * as fullscreen mode, frameless display, and position behavior.
     *
     * @param[in]     s    Unique pointer to the display strategy. Takes
     *                     ownership of the strategy pointer.
     * @throws              None
     * @note                Caller transfers ownership of the strategy to this
     *                      proxy. Replaces any existing strategy.
     * @warning             None
     * @since               0.1
     * @ingroup             desktop_ui
     */
    void set_window_display_strategy(DesktopDisplayStrategy s);

    /**
     * @brief  Activates the configured window display strategy.
     *
     * Applies the current display strategy to the desktop window. If no
     * strategy has been set, returns false.
     *
     * @return        True if the strategy was successfully activated, false if
     *                no strategy is configured or activation failed.
     * @throws        None
     * @note          Must call set_window_display_strategy() first with a
     *                valid strategy.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui
     */
    bool activate_window_display_strategy();

    /**
     * @brief  Shows the desktop window using the active display strategy.
     *
     * Displays the desktop window according to the configured display
     * strategy. If no strategy is active, uses default window behavior.
     *
     * @return        True if the desktop was shown successfully, false
     *                otherwise.
     * @throws        None
     * @note          May activate the default strategy if none is set.
     * @warning       None
     * @since         0.1
     * @ingroup       desktop_ui
     */
    bool show_desktop();

  private:
    /// @brief Owning pointer to the desktop display strategy. May be null.
    DesktopDisplayStrategy s_;
};

} // namespace cf::desktop
