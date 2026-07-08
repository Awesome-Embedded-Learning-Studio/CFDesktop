/**
 * @file    launcher_tile.h
 * @brief   Single application tile for the launcher grid.
 *
 * LauncherTile renders one launchable application as a rounded-square glyph
 * tile with the application display name beneath it. It zooms the glyph on
 * hover, plays a self-drawn press ripple, and emits clicked(app_id) on a
 * left-button release inside the tile. It is the grid counterpart of
 * TaskbarIcon (taller, with a label, no running indicator).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "app_entry.h"

#include <QColor>
#include <QFont>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QString>
#include <QWidget>

class QEnterEvent;
class QEvent;
class QMouseEvent;
class QPaintEvent;
class QTimer;
class QVariantAnimation;

namespace cf::desktop::desktop_component {

/**
 * @brief  Where a LauncherTile is used; drives long-press behavior.
 *
 * In Launcher context a long-press emits addToDesktopRequested() (pin to the
 * desktop). In Desktop context a long-press enters drag mode (longPressed /
 * dragMoved / dragEnded) for rearranging. The target is i.MX6ULL resistive
 * touch, so drag is long-press-triggered, not mouse-move-triggered.
 *
 * @ingroup components
 */
enum class TileContext {
    Launcher, ///< Shown in the start-menu popup; long-press pins to desktop.
    Desktop,  ///< Shown on the desktop wallpaper; long-press starts a drag.
};

/**
 * @brief  One application tile shown in the launcher grid.
 *
 * Paints a rounded glyph tile (bearing the application initial) with the
 * display name elided beneath it, zooms the glyph on hover, plays a press
 * ripple, and emits clicked() on release. All colors and typography follow the
 * active Material theme.
 *
 * @ingroup components
 */
class LauncherTile final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the tile for a given application entry.
     *
     * @param[in] entry   The application this tile represents.
     * @param[in] parent  Owning widget (the launcher popup).
     *
     * @throws None
     * @note   Resolves theme colors and starts idle at scale 1.0.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    explicit LauncherTile(AppEntry entry, QWidget* parent = nullptr);

    /**
     * @brief  Destructs the tile.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    ~LauncherTile() override;

    /**
     * @brief  Replaces the application entry backing this tile.
     *
     * @param[in] entry  The new application entry.
     *
     * @throws None
     * @note   Triggers a repaint.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void setEntry(const AppEntry& entry);

    /**
     * @brief  Returns this tile's application identifier.
     *
     * @return The app_id of the backing entry.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
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
     * @since  0.20
     * @ingroup components
     */
    QSize sizeHint() const override;

    /**
     * @brief  Sets whether this tile lives in the launcher or on the desktop.
     *
     * @param[in] context  Launcher (long-press sends to desktop) or Desktop
     *                     (long-press starts a drag).
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    void setContext(TileContext context) noexcept { context_ = context; }

  signals:
    /**
     * @brief  Emitted when the tile is clicked (left-button release inside).
     *
     * @param[in] app_id  The application identifier of this tile.
     *
     * @since 0.20
     * @ingroup components
     */
    void clicked(const QString& app_id);

    /**
     * @brief  Emitted when a desktop tile's long-press fires (drag begins).
     *
     * The DesktopIconLayer creates a floating ghost and tracks dragMoved.
     *
     * @param[in] app_id  The application identifier being dragged.
     *
     * @since 0.20
     * @ingroup components
     */
    void longPressed(const QString& app_id);

    /**
     * @brief  Emitted while a desktop tile is being dragged.
     *
     * @param[in] global_pos  Current pointer position in global screen coords.
     *
     * @since 0.20
     * @ingroup components
     */
    void dragMoved(const QPoint& global_pos);

    /**
     * @brief  Emitted when a desktop tile drag ends (finger lifted).
     *
     * The layer computes the drop cell and applies swap / remove + persist.
     *
     * @param[in] global_pos  Final pointer position in global screen coords.
     *
     * @since 0.20
     * @ingroup components
     */
    void dragEnded(const QPoint& global_pos);

    /**
     * @brief  Emitted when a launcher tile is long-pressed (pin to desktop).
     *
     * @param[in] app_id  The application to add to the desktop.
     *
     * @since 0.20
     * @ingroup components
     */
    void addToDesktopRequested(const QString& app_id);

  protected:
    /**
     * @brief  Paints the glyph tile, overlay, ripple, initial, and label.
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
     * @brief  Starts the glyph zoom-in animation on mouse enter.
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

    /**
     * @brief  Tracks drag movement or cancels an in-progress long-press.
     *
     * @param[in] event  The mouse move event descriptor.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    void mouseMoveEvent(QMouseEvent* event) override;

  private:
    /// @brief Resolves theme colors and typography, then repaints.
    void applyTheme();
    /// @brief Animates the glyph scale toward the resting or hovered value.
    void startHover(bool entering);
    /// @brief Starts an expanding ripple from a center point.
    void startRipple(const QPointF& center);
    /// @brief Creates the hover and ripple animations and wires them.
    void setupAnimations();
    /// @brief Resolves entry_.icon_path into cached_icon_ (null -> letter fallback).
    void refreshIcon();

    /// Long-press / drag state machine (desktop context only).
    enum class DragState {
        Idle,      ///< No interaction.
        Pressed,   ///< Finger down, long-press timer running.
        Cancelled, ///< Moved past threshold before timer; release is a no-op.
        DragReady, ///< Timer fired; next move begins the drag.
        Dragging,  ///< Actively dragging; release commits the drop.
    };

    AppEntry entry_;             ///< Backing application entry.
    QPixmap cached_icon_;        ///< Resolved icon (null -> initial-letter fallback).
    qreal hover_scale_{1.0};     ///< Current glyph scale (1.0 idle, >1 hover).
    qreal ripple_progress_{0.0}; ///< Ripple expansion in [0, 1].
    QPointF ripple_center_;      ///< Ripple origin in local coordinates.
    bool rippling_{false};       ///< Whether a ripple is animating.

    QColor tile_color_;       ///< Glyph tile fill (surface variant).
    QColor foreground_color_; ///< Initial / overlay color (on surface).
    QColor label_color_;      ///< Caption color (on surface variant).

    QFont glyph_font_; ///< Font used for the initial letter.
    QFont label_font_; ///< Font used for the caption beneath the glyph.

    QVariantAnimation* hover_anim_{nullptr};  ///< Zoom-in/out animation.
    QVariantAnimation* ripple_anim_{nullptr}; ///< Ripple expansion animation.

    TileContext context_{TileContext::Launcher}; ///< Launcher vs desktop behavior.
    DragState drag_state_{DragState::Idle};      ///< Long-press / drag state.
    QPointF press_pos_;                          ///< Press origin (local coords).
    QTimer* long_press_timer_{nullptr};          ///< Long-press detector.
};

} // namespace cf::desktop::desktop_component
