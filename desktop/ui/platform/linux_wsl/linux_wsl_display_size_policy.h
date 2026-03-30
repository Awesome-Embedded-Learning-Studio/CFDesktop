/**
 * @file    desktop/ui/platform/linux_wsl/linux_wsl_display_size_policy.h
 * @brief   WSL-specific display size policy strategy for Qt widgets.
 *
 * Provides display size configuration strategies tailored for Windows Subsystem
 * for Linux (WSL) environments, handling both X11 and Wayland backends.
 *
 * @author  CFDesktop Team
 * @date    2026-03-27
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup platform_wsl
 */

#pragma once

#include "IDesktopDisplaySizeStrategy.h"

class QWidget;

namespace cf::desktop::platform_strategy::wsl {

/**
 * @brief  Display size policy strategy for WSL desktop environments.
 *
 * Implements platform-specific display size and window behavior configuration
 * for Windows Subsystem for Linux environments. Handles X11 backend with
 * window decoration, resizing, and maximization settings.
 *
 * @ingroup platform_wsl
 *
 * @code
 * DisplaySizePolicyMaker policy;
 * policy.action(widget);
 * auto behaviors = policy.query();
 * @endcode
 */
class DisplaySizePolicyMaker : public IDesktopDisplaySizeStrategy {
  public:
    /**
     * @brief  Constructs a new WSL display size policy maker.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.13.0
     * @ingroup platform_wsl
     */
    DisplaySizePolicyMaker();

    /**
     * @brief  Destroys the display size policy maker.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.13.0
     * @ingroup platform_wsl
     */
    ~DisplaySizePolicyMaker();

    /**
     * @brief  Returns the name of this display size policy strategy.
     *
     * @return     Null-terminated string identifier for this strategy.
     * @throws     None
     * @note       The returned string is static and must not be freed.
     * @warning    None
     * @since      0.13.0
     * @ingroup    platform_wsl
     */
    const char* name() const noexcept override { return "WSL Desktop Size Policy"; }

    /**
     * @brief  Applies display size policy settings to the given widget.
     *
     * Configures the widget's window flags, state, and size policy according to
     * the detected Linux Qt backend (X11 or Wayland).
     *
     * @param[in] widget_data  Pointer to the QWidget to configure. May be nullptr.
     * @return                 True if the operation succeeded, false on failure or
     *                         if widget_data is nullptr.
     * @throws                 None
     * @note                   Wayland backend is not yet implemented and fails
     *                         silently.
     * @warning                Passing nullptr results in an immediate false return.
     * @since                  0.13.0
     * @ingroup                platform_wsl
     */
    bool action(QWidget* widget_data) override;

    /**
     * @brief  Queries the desktop behaviors supported by this strategy.
     *
     * Returns behavior flags based on the detected Linux Qt backend,
     * indicating which window behaviors are supported.
     *
     * @return     DesktopBehaviors flags indicating supported behaviors.
     * @throws     None
     * @note       For X11 backend, returns AllowResize flag.
     * @warning    None
     * @since      0.13.0
     * @ingroup    platform_wsl
     */
    DesktopBehaviors query() const override;

  private:
    /**
     * @brief  Applies X11-specific display settings to the given widget.
     *
     * Configures window flags for standard decorations, enables window
     * maximization, and sets the size policy to allow user resizing.
     *
     * @param[in] widget_data  Pointer to the QWidget to configure. May be nullptr.
     * @throws                 None
     * @note                   None
     * @warning                Passing nullptr results in immediate return.
     * @since                  0.13.0
     * @ingroup                platform_wsl
     */
    void act_with_x11(QWidget* widget_data);

    /**
     * @brief  Queries X11-specific desktop behaviors.
     *
     * Returns the behavior flags specific to X11 backend on WSL.
     *
     * @return     DesktopBehaviors with AllowResize flag set.
     * @throws     None
     * @note       X11 backend defaults to allowing resize.
     * @warning    None
     * @since      0.13.0
     * @ingroup    platform_wsl
     */
    DesktopBehaviors x11_query() const;
};

} // namespace cf::desktop::platform_strategy::wsl
