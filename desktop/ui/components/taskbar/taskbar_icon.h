/**
 * @file    taskbar_icon.h
 * @brief   Single application icon widget for the centered taskbar.
 *
 * TaskbarIcon renders one launchable application as a rounded-square tile
 * bearing the app's initial. It zooms in on hover, plays a self-drawn press
 * ripple, and shows a running-state indicator dot. It emits clicked(app_id)
 * on a left-button release inside the tile.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.1
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include "app_entry.h"

#include <QColor>
#include <QFont>
#include <QPointF>
#include <QString>
#include <QWidget>

class QEnterEvent;
class QEvent;
class QMouseEvent;
class QVariantAnimation;

namespace cf::desktop::desktop_component {

/**
 * @brief  One application tile shown in the centered taskbar.
 *
 * Paints a rounded-square glyph tile that zooms on hover, shows a press
 * ripple, exposes a running indicator, and emits clicked() on release. All
 * colors and typography follow the active Material theme.
 *
 * @ingroup components
 */
class TaskbarIcon final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the icon for a given application entry.
     *
     * @param[in] entry   The application this tile represents.
     * @param[in] parent  Owning widget (the taskbar).
     *
     * @throws None
     * @note   Resolves theme colors and starts idle at scale 1.0.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    explicit TaskbarIcon(AppEntry entry, QWidget* parent = nullptr);

    /**
     * @brief  Destructs the icon.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    ~TaskbarIcon() override;

    /**
     * @brief  Replaces the application entry backing this tile.
     *
     * @param[in] entry  The new application entry.
     *
     * @throws None
     * @note   Triggers a repaint.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void setEntry(const AppEntry& entry);

    /**
     * @brief  Shows or hides the running indicator.
     *
     * @param[in] running  true to show the indicator dot.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void setRunning(bool running);

    /**
     * @brief  Returns this tile's application identifier.
     *
     * @return The app_id of the backing entry.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    const QString& appId() const noexcept { return entry_.app_id; }

    /**
     * @brief  Returns the preferred tile size.
     *
     * @return A fixed square size hint.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    QSize sizeHint() const override;

  signals:
    /**
     * @brief  Emitted when the tile is clicked (left-button release inside).
     *
     * @param[in] app_id  The application identifier of this tile.
     *
     * @since 0.19
     * @ingroup components
     */
    void clicked(const QString& app_id);

  protected:
    /**
     * @brief  Paints the tile, overlay, ripple, initial, and indicator.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   Theme values are resolved in applyTheme().
     * @warning None
     * @since  0.19
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
     * @since  0.19
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
     * @since  0.19
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
     * @since  0.19
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
     * @since  0.19
     * @ingroup components
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    /// @brief Resolves theme colors and typography, then repaints.
    void applyTheme();
    /// @brief Animates the hover scale toward the resting or hovered value.
    void startHover(bool entering);
    /// @brief Starts an expanding ripple from a center point.
    void startRipple(const QPointF& center);
    /// @brief Creates the hover and ripple animations and wires them.
    void setupAnimations();

    AppEntry entry_;             ///< Backing application entry.
    bool running_{false};        ///< Whether the running indicator shows.
    qreal hover_scale_{1.0};     ///< Current tile scale (1.0 idle, >1 hover).
    qreal ripple_progress_{0.0}; ///< Ripple expansion in [0, 1].
    QPointF ripple_center_;      ///< Ripple origin in local coordinates.
    bool rippling_{false};       ///< Whether a ripple is animating.

    QColor tile_color_;       ///< Tile fill (surface variant).
    QColor foreground_color_; ///< Initial text color (on surface).
    QColor indicator_color_;  ///< Running dot color (on surface variant).
    QFont label_font_;        ///< Font used for the initial letter.

    QVariantAnimation* hover_anim_{nullptr};  ///< Zoom-in/out animation.
    QVariantAnimation* ripple_anim_{nullptr}; ///< Ripple expansion animation.
};

} // namespace cf::desktop::desktop_component
