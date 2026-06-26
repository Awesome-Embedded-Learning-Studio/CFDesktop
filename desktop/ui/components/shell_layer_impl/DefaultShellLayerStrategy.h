/**
 * @file    DefaultShellLayerStrategy.h
 * @brief   Default strategy implementation for shell layer behavior.
 *
 * Provides a minimal, default strategy that activates and deactivates
 * a shell layer and responds to geometry changes.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once
#include "../IShellLayerStrategy.h"

namespace cf::desktop {

/**
 * @brief  Default shell layer strategy implementation.
 *
 * Provides basic activate/deactivate semantics and geometry-change
 * handling for a shell layer.
 *
 * @ingroup components
 */
class DefaultShellLayerStrategy : public IShellLayerStrategy {
  public:
    DefaultShellLayerStrategy();
    ~DefaultShellLayerStrategy();

    /**
     * @brief  Activates the default strategy on the given shell layer.
     *
     * Stores weak references to the layer and window manager for
     * subsequent geometry updates.
     *
     * @param[in] layer  Weak reference to the shell layer.
     * @param[in] wm     Weak reference to the window manager.
     */
    void activate(aex::WeakPtr<IShellLayer> layer, aex::WeakPtr<WindowManager> wm) override;

    /**
     * @brief  Deactivates the strategy and releases held references.
     */
    void deactivate() override;

  private:
    /**
     * @brief  Handles a geometry change notification.
     *
     * @param[in] available  The new available geometry rectangle.
     */
    void onGeometryChanged(const QRect& available) override;

  private:
    aex::WeakPtr<IShellLayer> layer_;
    aex::WeakPtr<WindowManager> window_manager_;
};
} // namespace cf::desktop
