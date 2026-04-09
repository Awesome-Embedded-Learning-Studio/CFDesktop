/**
 * @file    IShellLayerStrategy.h
 * @brief   Strategy interface for shell layer behavior.
 *
 * IShellLayerStrategy defines a pluggable strategy that controls how
 * a shell layer behaves when activated or deactivated, and how it
 * responds to geometry changes.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once
#include "base/weak_ptr/weak_ptr.h"
#include <QColor>
#include <QImage>
#include <QRect>

namespace cf::desktop {
class IShellLayer;
class WindowManager;

/**
 * @brief  Pluggable strategy for shell layer behavior.
 *
 * Concrete strategies define what happens when a shell layer is
 * activated (e.g. wallpaper rendering, panel hosting) and how
 * geometry changes are handled.
 *
 * @ingroup components
 */
class IShellLayerStrategy {
  public:
    virtual ~IShellLayerStrategy() = default;

    /**
     * @brief  Activates the strategy on the given shell layer.
     *
     * @param[in]  layer  Weak reference to the shell layer.
     * @param[in]  wm     Weak reference to the window manager.
     */
    virtual void activate(WeakPtr<IShellLayer> layer, WeakPtr<WindowManager> wm) = 0;

    /**
     * @brief  Deactivates the strategy and releases resources.
     */
    virtual void deactivate() = 0;

    /**
     * @brief  Called when the available geometry changes.
     *
     * @param[in]  available  The new available geometry rectangle.
     */
    virtual void onGeometryChanged(const QRect& available) = 0;

    /**
     * @brief  Returns the current background image, if any.
     *
     * Default implementation returns a null QImage (no image).
     * Strategies that provide a wallpaper image override this.
     *
     * @return Current background image, or null QImage for solid-color fallback.
     */
    virtual QImage currentBackgroundImage() const { return {}; }

    /**
     * @brief  Returns the background fill color.
     *
     * Used as fallback when no image is available, or for letterbox bars.
     *
     * @return Background color.
     */
    virtual QColor backgroundColor() const { return QColor(0x1c, 0x1b, 0x1f); }
};
} // namespace cf::desktop
