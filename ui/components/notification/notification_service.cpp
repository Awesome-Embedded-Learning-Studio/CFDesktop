/**
 * @file    notification_service.cpp
 * @brief   Implementation of NotificationService.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#include "notification_service.h"

#include "cfconfig.hpp"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "cfconfig_notify_policy.h"
#include "cflog.h"

#include <QDateTime>
#include <QUuid>

namespace cf::desktop::desktop_component {

namespace {
/// ConfigStore domain + key for Do-Not-Disturb persistence.
constexpr const char* kDomain = "notification";
constexpr const char* kDndGroup = "dnd";
constexpr const char* kDndKey = "enabled";
/// Tag for log lines.
constexpr const char* kLogTag = "NotificationService";
} // namespace

NotificationService& NotificationService::instance() {
    static NotificationService service;
    return service;
}

NotificationService::NotificationService() : QObject(nullptr) {}

void NotificationService::post(Notification notification) {
    if (notification.id.isEmpty()) {
        notification.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    if (notification.timestamp == 0) {
        notification.timestamp = QDateTime::currentMSecsSinceEpoch();
    }
    list_.prepend(notification);
    emit notificationPosted(notification, isDndEnabled());
}

bool NotificationService::dismiss(const QString& id) {
    for (int i = 0; i < list_.size(); ++i) {
        if (list_[i].id == id) {
            list_.removeAt(i);
            emit notificationDismissed(id);
            return true;
        }
    }
    return false;
}

int NotificationService::clearAll() {
    const int removed = list_.size();
    if (removed == 0) {
        return 0;
    }
    list_.clear();
    emit allCleared();
    return removed;
}

QList<Notification> NotificationService::all() const {
    return list_;
}

int NotificationService::count() const {
    return list_.size();
}

bool NotificationService::isDndEnabled() const {
    try {
        namespace cfg = cf::config;
        return cfg::ConfigStore::instance().domain(kDomain).query<bool>(
            cfg::KeyView{.group = kDndGroup, .key = kDndKey}, false);
    } catch (...) {
        // ConfigStore not initialized (e.g. unit test without a provider) or
        // query failed: treat DND as off so notifications still surface.
        return false;
    }
}

void NotificationService::setDndEnabled(bool on) {
    try {
        namespace cfg = cf::config;
        const bool ok = cfg::ConfigStore::instance().domain(kDomain).set(
            cfg::KeyView{.group = kDndGroup, .key = kDndKey}, on, cfg::Layer::User,
            cfg::NotifyPolicy::Immediate);
        cfg::ConfigStore::instance().sync();
        if (!ok) {
            // The store rejected the write (key locked / layer read-only):
            // surface it rather than silently drop the user's toggle.
            cf::log::warningftag(kLogTag, "ConfigStore rejected DND={} set", on);
        }
    } catch (...) {
        cf::log::warningftag(kLogTag, "failed to persist DND={} (ConfigStore unavailable)", on);
    }
    emit dndChanged(on);
}

} // namespace cf::desktop::desktop_component
