/**
 * @file    start_button.h
 * @brief   Start affordance that requests the application launcher.
 *
 * StartButton is the leading taskbar tile that emits clicked() to open the
 * AppLauncher popup. It reuses the TaskbarIcon visual language (rounded tile,
 * hover zoom, press ripple) but draws a fixed app-grid glyph instead of an
 * application initial, and carries no running indicator.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QColor>
#include <QPointF>
#include <QWidget>

class QEnterEvent;
class QEvent;
class QMouseEvent;
class QPaintEvent;
class QVariantAnimation;

namespace cf::desktop::desktop_component {

/**
 * @brief  Leading taskbar tile that opens the application launcher.
 *
 * Paints a rounded tile with a fixed app-grid glyph, zooms on hover, plays a
 * press ripple, and emits clicked() on a left-button release inside the tile.
 * All colors follow the active Material theme.
 *
 * @ingroup components
 */
class StartButton final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the start button.
     *
     * @param[in] parent  Owning widget (the taskbar).
     *
     * @throws None
     * @note   Resolves theme colors and starts idle at scale 1.0.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    explicit StartButton(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the start button.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    ~StartButton() override;

    /**
     * @brief  Returns the preferred tile size.
     *
     * @return A fixed square size hint matching a taskbar tile.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    QSize sizeHint() const override;

  signals:
    /**
     * @brief  Emitted when the button is clicked (left-button release inside).
     *
     * @since 0.20
     * @ingroup components
     */
    void clicked();

  protected:
    /**
     * @brief  Paints the tile, overlay, ripple, and app-grid glyph.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   Theme values are resolved in applyTheme().
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Starts the zoom-in animation on mouse enter.
     *
     * @param[in] event  The enter event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void enterEvent(QEnterEvent* event) override;

    /**
     * @brief  Reverts the zoom and cancels the ripple on mouse leave.
     *
     * @param[in] event  The leave event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void leaveEvent(QEvent* event) override;

    /**
     * @brief  Begins a ripple at the press point.
     *
     * @param[in] event  The mouse press event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Emits clicked() when released inside the tile.
     *
     * @param[in] event  The mouse release event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();
    /// @brief Animates the hover scale toward the resting or hovered value.
    void startHover(bool entering);
    /// @brief Starts an expanding ripple from a center point.
    void startRipple(const QPointF& center);
    /// @brief Creates the hover and ripple animations and wires them.
    void setupAnimations();

    qreal hover_scale_{1.0};     ///< Current tile scale (1.0 idle, >1 hover).
    qreal ripple_progress_{0.0}; ///< Ripple expansion in [0, 1].
    QPointF ripple_center_;      ///< Ripple origin in local coordinates.
    bool rippling_{false};       ///< Whether a ripple is animating.

    QColor tile_color_;       ///< Tile fill (surface variant).
    QColor foreground_color_; ///< Glyph / overlay color (on surface).

    QVariantAnimation* hover_anim_{nullptr};  ///< Zoom-in/out animation.
    QVariantAnimation* ripple_anim_{nullptr}; ///< Ripple expansion animation.
};

} // namespace cf::desktop::desktop_component
