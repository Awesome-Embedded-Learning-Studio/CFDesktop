/**
 * @file    IShellLayer.h
 * @brief   Platform-agnostic shell layer interface.
 *
 * IShellLayer defines the shell layer contract without depending on
 * QWidget. This allows Wayland compositor backends to implement the
 * shell layer using custom rendering instead of QWidget.
 *
 * The existing QWidget-based ShellLayer (ShellLayer.h) implements this
 * interface for Client and DirectRender modes.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once

#include "IShellLayerStrategy.h"
#include <QImage>
#include <QRect>
#include <memory>

namespace cf::desktop {

/**
 * @brief  Platform-agnostic shell layer interface.
 *
 * Decoupled from QWidget so that the shell layer can be implemented
 * differently per display backend:
 * - Client / DirectRender: QWidget-based ShellLayer
 * - Wayland compositor: Custom rendering via IShellLayer
 *
 * @ingroup components
 */
class IShellLayer {
  public:
    virtual ~IShellLayer() = default;

    /**
     * @brief  Sets the strategy that controls shell layer behavior.
     *
     * @param[in]  strategy  The strategy to activate.
     */
    virtual void setStrategy(std::unique_ptr<IShellLayerStrategy> strategy) = 0;

    /**
     * @brief  Returns the current geometry of the shell layer.
     *
     * @return Shell layer rectangle in device pixels.
     */
    virtual QRect geometry() const = 0;

    /**
     * @brief  Requests a repaint of the shell layer.
     *
     * Implementations should schedule a repaint without blocking.
     * QWidget-based layers call QWidget::update();
     * Wayland compositor layers commit the surface.
     */
    virtual void requestRepaint() = 0;

    /**
     * @brief  Handles changes to the available screen geometry.
     *
     * Called when the available geometry changes (e.g., after panel layout).
     * Implementations should update their geometry and notify the active strategy.
     *
     * @param[in]  available  The new available geometry rectangle.
     */
    virtual void onAvailableGeometryChanged(const QRect& available) = 0;

    /**
     * @brief  Returns the current background image, if any.
     *
     * The default returns a null QImage. Implementations backed by a wallpaper
     * strategy delegate to it. Shell panels use this to composite a frosted
     * copy of the backdrop (e.g. the status bar and taskbar acrylic surfaces).
     *
     * @return Current background image, or a null QImage when none is available.
     */
    virtual QImage currentBackgroundImage() const { return {}; }
};

} // namespace cf::desktop
