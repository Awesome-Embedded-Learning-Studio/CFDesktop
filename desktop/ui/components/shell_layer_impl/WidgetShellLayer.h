/**
 * @file    WidgetShellLayer.h
 * @brief   QWidget-based shell layer implementation.
 *
 * WidgetShellLayer implements the IShellLayer interface using a QWidget
 * for Client and DirectRender modes. For Wayland compositor mode,
 * a non-QWidget IShellLayer implementation should be used instead.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once

#include "../IShellLayer.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include <QWidget>
#include <memory>
namespace cf::desktop {

class IShellLayerStrategy;
namespace wallpaper {
class WallPaperLayer;
}
class WindowManager;

/**
 * @brief  QWidget-based shell layer implementation.
 *
 * Implements both QWidget (for Client / DirectRender modes) and
 * IShellLayer (for backend-agnostic access).
 *
 * @ingroup components
 */
class WidgetShellLayer : public QWidget, public IShellLayer {
    Q_OBJECT
  public:
    explicit WidgetShellLayer(QWidget* parent /* Never nullptr! */);
    ~WidgetShellLayer() override;

    // -- IShellLayer ----------------------------------------------------------
    /**
     * @brief  Installs a new strategy for this shell layer.
     *
     * Replaces the current strategy. Ownership of the strategy transfers
     * to this layer.
     *
     * @param[in,out] strategy  The new strategy instance. Ownership: caller → this layer.
     */
    void setStrategy(std::unique_ptr<IShellLayerStrategy> strategy) override;

    /**
     * @brief  Returns the geometry rectangle of this shell layer.
     *
     * @return The current geometry as a QRect.
     */
    QRect geometry() const override;

    // -- Weak reference -------------------------------------------------------
    /**
     * @brief  Returns a weak pointer to this shell layer.
     *
     * @return A WeakPtr referencing this instance.
     */
    WeakPtr<WidgetShellLayer> GetWeak() const { return weak_factory_.GetWeakPtr(); }

    // -- IShellLayer ----------------------------------------------------------
    /**
     * @brief  Requests a repaint of this shell layer widget.
     *
     * Calls QWidget::update() to schedule a repaint without blocking.
     */
    void requestRepaint() override { update(); }

  public slots:
    /**
     * @brief  Handles changes to the available screen geometry.
     *
     * Forwards the new geometry to the active strategy.
     *
     * @param[in] rect  The new available geometry rectangle.
     */
    void onAvailableGeometryChanged(const QRect& rect);

  protected:
    /**
     * @brief  Paints the shell layer widget.
     *
     * Delegates rendering to the active strategy if one is installed.
     *
     * @param[in] event  The paint event descriptor.
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /// Active strategy instance. Ownership: this layer.
    std::unique_ptr<IShellLayerStrategy> strategy_;

    /// Weak pointer factory (must be last member).
    mutable cf::WeakPtrFactory<WidgetShellLayer> weak_factory_{this};
};

} // namespace cf::desktop
