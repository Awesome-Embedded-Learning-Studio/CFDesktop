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

  private:
    /**
     * @brief  Called when the available geometry changes.
     *
     * @param[in]  available  The new available geometry rectangle.
     */
    virtual void onGeometryChanged(const QRect& available) = 0;
};
} // namespace cf::desktop
