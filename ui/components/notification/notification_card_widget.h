/**
 * @file    notification_card_widget.h
 * @brief   Single notification card rendered by the banner and center.
 *
 * NotificationCardWidget paints one Notification as a rounded Material card
 * (title, message, app/time line, dismiss hotspot). The banner shows a
 * single card for the newest notification; the notification center stacks
 * one card per active notification. Colors follow the active Material theme.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#pragma once

#include "notification.h"

#include <QColor>
#include <QRect>
#include <QWidget>

namespace cf::desktop::desktop_component {

/**
 * @brief  Self-painted single-notification card.
 *
 * Renders a rounded surface with the notification title, message, and an
 * app/time meta line; a dismiss "x" hotspot sits in the top-right corner.
 * Clicking the hotspot emits dismissRequested with the notification id.
 *
 * @ingroup notification
 */
class NotificationCardWidget : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the card.
     *
     * @param[in]  parent  Owning widget.
     * @throws     None
     * @note       Call setNotification() before first paint to supply data.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    explicit NotificationCardWidget(QWidget* parent = nullptr);

    /**
     * @brief  Sets the notification to render.
     *
     * @param[in]  notification  The notification to display.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void setNotification(const Notification& notification);

    /**
     * @brief  Returns the rendered notification.
     *
     * @return     The current notification.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    Notification notification() const;

    /**
     * @brief  Returns the preferred card size.
     *
     * @return     Size hint (width x height).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    QSize sizeHint() const override;

  signals:
    /**
     * @brief  Emitted when the dismiss hotspot is clicked.
     *
     * @param[in]  id  The id of the notification to dismiss.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void dismissRequested(const QString& id);

  protected:
    /**
     * @brief  Paints the rounded card, text, and dismiss hotspot.
     *
     * @param[in]  event  The paint event descriptor.
     * @throws     None
     * @note       Theme colors resolve in applyTheme().
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Records a press on the dismiss hotspot.
     *
     * @param[in]  event  The mouse event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Activates the dismiss hotspot when the press releases on it.
     *
     * @param[in]  event  The mouse event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    /// @brief Resolves theme colors and repaints.
    void applyTheme();

    /// @brief Currently rendered notification.
    Notification notif_;

    /// @brief Dismiss hotspot geometry (top-right), in widget coords.
    QRect close_rect_;

    /// @brief True while a press is active inside the dismiss hotspot.
    bool pressed_on_close_{false};

    /// @brief Card surface fill (surfaceContainer).
    QColor surface_color_;
    /// @brief Title text color (onSurface).
    QColor title_color_;
    /// @brief Message text color (onSurfaceVariant).
    QColor message_color_;
    /// @brief App/time meta line color (outline).
    QColor meta_color_;
    /// @brief Dismiss glyph color (onSurfaceVariant).
    QColor close_color_;
};

} // namespace cf::desktop::desktop_component
