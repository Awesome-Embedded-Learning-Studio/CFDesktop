/**
 * @file    notification_service.h
 * @brief   Process-wide in-memory notification store.
 *
 * NotificationService is the single source of truth for active desktop
 * notifications inside the shell process. Producers post notifications here
 * (the IPCServer "notify" handler, or any in-process caller); consumers
 * (the banner, the notification center, the status bar unread dot) connect
 * to its signals. The model is in-memory only: a restart clears the list.
 * Do-Not-Disturb is read from / written to the ConfigStore
 * "notification.dnd.enabled" key so the control-center switch and the banner
 * suppress decision stay in sync without a cached copy.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#pragma once

#include "notification.h"

#include <QList>
#include <QObject>
#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  Process-wide singleton notification store.
 *
 * Owns the active notification list and emits signals on every change so UI
 * consumers stay in sync without polling. Do-Not-Disturb is backed by the
 * ConfigStore so it survives a restart and is shared with the control center.
 *
 * @note    Thread-affine: intended for use on the main (Qt) thread.
 * @warning None
 * @since   0.19.0
 * @ingroup notification
 */
class NotificationService : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief  Returns the process-wide singleton instance.
     *
     * @return     Reference to the singleton service.
     * @throws     None
     * @note       Created on first access; lives until process exit.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    static NotificationService& instance();

    /**
     * @brief  Posts a notification.
     *
     * Prepends to the active list (newest first), fills id / timestamp when
     * empty, and emits notificationPosted. The banner is suppressed when
     * Do-Not-Disturb is on; the center still records it.
     *
     * @param[in]  notification  The notification to post.
     * @throws     None
     * @note       Generates an id and timestamp when the caller omits them.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void post(Notification notification);

    /**
     * @brief  Removes a notification by id.
     *
     * @param[in]  id  The id of the notification to remove.
     * @return     True when a notification was removed.
     * @throws     None
     * @note       Emits notificationDismissed on a hit.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    bool dismiss(const QString& id);

    /**
     * @brief  Removes every active notification.
     *
     * @return     The number of notifications removed.
     * @throws     None
     * @note       Emits allCleared when the list was non-empty.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    int clearAll();

    /**
     * @brief  Returns all active notifications, newest first.
     *
     * @return     A copy of the active list.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    QList<Notification> all() const;

    /**
     * @brief  Returns the active notification count.
     *
     * @return     The number of active notifications.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    int count() const;

    /**
     * @brief  Reports whether Do-Not-Disturb is on.
     *
     * @return     True when DND is enabled (banners suppressed).
     * @throws     None
     * @note       Reads the ConfigStore "notification.dnd.enabled" key;
     *             returns false when the store is unavailable.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    bool isDndEnabled() const;

    /**
     * @brief  Enables or disables Do-Not-Disturb.
     *
     * @param[in]  on  True to enable DND, false to disable.
     * @throws     None
     * @note       Persists to ConfigStore "notification.dnd.enabled" (User
     *             layer) and emits dndChanged regardless of store outcome.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void setDndEnabled(bool on);

  signals:
    /**
     * @brief  Emitted right after a notification is posted.
     *
     * @param[in]  notification        The posted notification.
     * @param[in]  banner_suppressed   True when DND suppressed the banner.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void notificationPosted(const Notification& notification, bool banner_suppressed);

    /**
     * @brief  Emitted when a notification is removed by id.
     *
     * @param[in]  id  The id of the removed notification.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void notificationDismissed(const QString& id);

    /**
     * @brief  Emitted when all notifications are cleared at once.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void allCleared();

    /**
     * @brief  Emitted when the Do-Not-Disturb state changes.
     *
     * @param[in]  on  The new DND state.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void dndChanged(bool on);

  private:
    /**
     * @brief  Constructs the service.
     *
     * @throws     None
     * @note       Private; access via instance().
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    NotificationService();

    QList<Notification> list_;
};

} // namespace cf::desktop::desktop_component
