/**
 * @file    desktop/ui/platform/windows/windows_display_size_policy.h
 * @brief   Windows-specific display size policy strategy for Qt widgets.
 *
 * Provides display size configuration strategies tailored for Windows
 * environments, implementing frameless windows, stay-on-bottom behavior,
 * fixed sizing, and system UI avoidance.
 *
 * @author  N/A
 * @date    2026-03-27
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "IDesktopDisplaySizeStrategy.h"

class QWidget;

namespace cf::desktop::platform_strategy::windows {

/**
 * @brief  Display size policy strategy for Windows desktop environments.
 *
 * Implements platform-specific display size and window behavior configuration
 * for Windows environments. Configures windows to be frameless, stay on bottom,
 * non-resizable, and avoid system UI elements.
 *
 * @ingroup none
 *
 * @code
 * WindowsDisplaySizePolicy policy;
 * policy.action(widget);
 * auto behaviors = policy.query();
 * @endcode
 */
class CF_DESKTOP_EXPORT WindowsDisplaySizePolicy : public IDesktopDisplaySizeStrategy {
  public:
    /**
     * @brief  Constructs a new Windows display size policy maker.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup none
     */
    WindowsDisplaySizePolicy();

    /**
     * @brief  Destroys the display size policy maker.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup none
     */
    ~WindowsDisplaySizePolicy();

    /**
     * @brief  Returns the name of this display size policy strategy.
     *
     * @return     Null-terminated string identifier for this strategy.
     * @throws     None
     * @note       The returned string is static and must not be freed.
     * @warning    None
     * @since      0.1
     * @ingroup    none
     */
    const char* name() const noexcept override { return "Windows Desktop Display Size Policy"; }

    /**
     * @brief  Applies display size policy settings to the given widget.
     *
     * Configures the widget's window flags, state, and size policy according to
     * the Windows-specific configuration:
     * - frameless: true
     * - stay_on_bottom: true
     * - allow_resize: false
     * - fullscreen: false
     * - avoid_system_ui: true
     *
     * @param[in] widget_data  Pointer to the QWidget to configure. May be nullptr.
     * @return                 True if the operation succeeded, false on failure or
     *                         if widget_data is nullptr.
     * @throws                 None
     * @note                   Uses Win32 API for system UI avoidance and Z-order.
     * @warning                Passing nullptr results in an immediate false return.
     * @since                  0.1
     * @ingroup                none
     */
    bool action(QWidget* widget_data) override;

    /**
     * @brief  Queries the desktop behaviors supported by this strategy.
     *
     * Returns behavior flags indicating which window behaviors are
     * supported/enabled by this Windows strategy.
     *
     * @return     DesktopBehaviors flags with Frameless, StayOnBottom, and AvoidSystemUI set.
     * @throws     None
     * @note       Returns Frameless | StayOnBottom | AvoidSystemUI.
     * @warning    None
     * @since      0.1
     * @ingroup    none
     */
    DesktopBehaviors query() const override;

  private:
    /**
     * @brief  Applies Windows-specific display settings to the given widget.
     *
     * Configures window flags for frameless behavior, stay-on-bottom Z-order,
     * fixed sizing, and system UI avoidance using both Qt flags and Win32 API.
     *
     * @param[in] widget_data  Pointer to the QWidget to configure. May be nullptr.
     * @throws                 None
     * @note                   None
     * @warning                Passing nullptr results in immediate return.
     * @since                  0.1
     * @ingroup                none
     */
    void applyWindowsNativeBehavior(QWidget* widget_data);
};

} // namespace cf::desktop::platform_strategy::windows
