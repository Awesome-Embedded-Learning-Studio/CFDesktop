/**
 * @file    ShellLayer.h
 * @brief   QWidget-based shell layer implementation.
 *
 * ShellLayer implements the IShellLayer interface using a QWidget
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

#include "IShellLayer.h"
#include <QWidget>
#include <memory>
namespace cf::desktop {

class IShellLayerStrategy;
class WallPaperLayer;
class WindowManager;

/**
 * @brief  QWidget-based shell layer implementation.
 *
 * Implements both QWidget (for Client / DirectRender modes) and
 * IShellLayer (for backend-agnostic access).
 *
 * @ingroup components
 */
class ShellLayer : public QWidget, public IShellLayer {
    Q_OBJECT
  public:
    explicit ShellLayer(QWidget* parent /* Never nullptr! */);
    ~ShellLayer() override;

    /**
     * @brief  Sets the strategy that controls shell layer behavior.
     *
     * Delegates to the QWidget-based strategy setter.
     *
     * @param[in]  strategy  The strategy instance to activate.
     */
    void setStrategy(std::unique_ptr<IShellLayerStrategy> strategy) override;

    /**
     * @brief  Returns the current geometry of the shell layer.
     *
     * @return Shell layer rectangle in device pixels.
     */
    QRect geometry() const override;

  private slots:

    /**
     * @brief  Handles available geometry changes from the display backend.
     *
     * @param[in]  rect  The new available geometry.
     */
    void onAvailableGeometryChanged(const QRect& rect);

  private:
    /// Active strategy instance. Ownership: this layer.
    std::unique_ptr<IShellLayerStrategy> strategy_;
};

} // namespace cf::desktop
