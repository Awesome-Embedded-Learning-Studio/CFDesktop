/**
 * @file    notification_banner.h
 * @brief   Transient top-right toast banner for the newest notification.
 *
 * NotificationBanner is a frameless popup that slides in from the top-right
 * (below the status bar) to show the most recent notification as a single
 * card, then auto-hides after a few seconds. ESC and the card's dismiss
 * hotspot hide it early. It subscribes to NotificationService and only shows
 * when a notification is posted with the banner not suppressed (DND off).
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#pragma once

#include "notification.h"

#include <QWidget>

class QKeyEvent;
class QRect;
class QTimer;

namespace qw::components::material {
class CFMaterialFadeAnimation;
class CFMaterialSlideAnimation;
} // namespace qw::components::material

namespace cf::desktop::desktop_component {

class NotificationCardWidget;

/**
 * @brief  Transient toast banner showing the newest notification.
 *
 * Frameless child widget (not Qt::Popup, so it does not fight the window
 * manager on windowless targets), animated with the QuarkWidgets MD3 fade +
 * slide pair. Auto-hides after a short timeout.
 *
 * @ingroup notification
 */
class NotificationBanner : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the banner (hidden until showFor()).
     *
     * @param[in]  parent  Owning widget (the desktop surface).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    explicit NotificationBanner(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the banner.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    ~NotificationBanner() override;

    /**
     * @brief  Positions (top-right of the available area) and shows the banner
     *         for a notification.
     *
     * @param[in]  notification  The notification to display.
     * @param[in]  available     The free screen geometry (excludes docked panels).
     * @throws     None
     * @note       Restarts the auto-hide timer.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void showFor(const Notification& notification, const QRect& available);

    /**
     * @brief  Starts the exit animation; hides when it finishes.
     *
     * @throws     None
     * @note       No-op when already hidden.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void hideBanner();

    /**
     * @brief  Reports whether the banner is currently visible.
     *
     * @return     True when the banner is shown.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    bool isShowing() const noexcept;

  protected:
    /**
     * @brief  Hides the banner on Escape.
     *
     * @param[in]  event  The key event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void keyPressEvent(QKeyEvent* event) override;

  private:
    /// @brief Creates the MD3 enter/exit fade + slide animations.
    void setupAnimations();

    /// @brief Card rendering the current notification.
    NotificationCardWidget* card_{nullptr};
    /// @brief Auto-hide timer (started on showFor()).
    QTimer* auto_hide_timer_{nullptr};

    qw::components::material::CFMaterialFadeAnimation* enter_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* enter_slide_{nullptr};
    qw::components::material::CFMaterialFadeAnimation* exit_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* exit_slide_{nullptr};
};

} // namespace cf::desktop::desktop_component
