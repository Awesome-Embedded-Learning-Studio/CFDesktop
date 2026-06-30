/**
 * @file    embedded_display_size_policy.h
 * @brief   Embedded (linuxfb / EGLFS) display size policy strategy.
 *
 * Provides a minimal display size strategy for embedded targets that render
 * directly to the framebuffer without a window manager. Unlike the WSL X11
 * policy, it does not probe for X11/Wayland and simply requests a frameless
 * fullscreen surface.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#pragma once

#include "../IDesktopDisplaySizeStrategy.h"

class QWidget;

namespace cf::desktop::platform_strategy::embedded {

/**
 * @brief  Display size policy for embedded framebuffer targets.
 *
 * Configures the desktop widget for frameless fullscreen rendering on a
 * single framebuffer output (Qt linuxfb / EGLFS). No window-manager
 * interaction is performed.
 *
 * @ingroup platform_embedded
 */
class EmbeddedDisplaySizePolicy : public IDesktopDisplaySizeStrategy {
  public:
    /**
     * @brief  Constructs the embedded display size policy.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup platform_embedded
     */
    EmbeddedDisplaySizePolicy();

    /**
     * @brief  Destroys the embedded display size policy.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup platform_embedded
     */
    ~EmbeddedDisplaySizePolicy() override;

    /**
     * @brief  Returns the ABI-friendly name of this strategy.
     * @return     Null-terminated static string identifier.
     * @throws     None
     * @note       The returned string must not be freed.
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    const char* name() const noexcept override;

    /**
     * @brief  Applies frameless fullscreen configuration to the widget.
     * @param[in] widget_data  Pointer to the QWidget to configure. May be nullptr.
     * @return                 True when the widget was configured, false on nullptr.
     * @throws                 None
     * @note                   The widget is shown fullscreen by the desktop entity;
     *                         this call only clears window decorations.
     * @warning                Passing nullptr results in an immediate false return.
     * @since                  0.19.0
     * @ingroup                platform_embedded
     */
    bool action(QWidget* widget_data) override;

    /**
     * @brief  Queries the desktop behaviors supported by this strategy.
     * @return     DesktopBehaviors with Fullscreen and Frameless flags set.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    platform_embedded
     */
    DesktopBehaviors query() const override;
};

} // namespace cf::desktop::platform_strategy::embedded
