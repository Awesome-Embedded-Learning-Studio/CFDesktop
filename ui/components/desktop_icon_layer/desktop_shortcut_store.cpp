/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_shortcut_store.cpp
 * @brief   Implementation of DesktopShortcutStore.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "desktop_shortcut_store.h"

#include "cfconfig.hpp"
#include "cflog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <algorithm>

namespace cf::desktop::desktop_component {

namespace {
/// ConfigStore domain for the desktop icon layout.
constexpr const char* kDomain = "desktop_icons";
/// Config group/key (stored as a JSON string under "layout.shortcuts").
constexpr const char* kGroup = "layout";
constexpr const char* kKey = "shortcuts";
/// Default-seed size and column count (kept modest so the first-run desktop
/// is not cluttered; the user customizes from here).
constexpr int kSeedCount = 12;
/// Tag for log lines.
constexpr const char* kLogTag = "DesktopShortcutStore";

/// JSON field names.
constexpr const char* kFieldAppId = "app_id";
constexpr const char* kFieldCol = "col";
constexpr const char* kFieldRow = "row";
} // namespace

QByteArray DesktopShortcutStore::serialize(const QList<DesktopShortcut>& shortcuts) {
    QJsonArray arr;
    for (const auto& s : shortcuts) {
        QJsonObject o;
        o[QLatin1String(kFieldAppId)] = s.app_id;
        o[QLatin1String(kFieldCol)] = s.col;
        o[QLatin1String(kFieldRow)] = s.row;
        arr.append(o);
    }
    return QJsonDocument(arr).toJson(QJsonDocument::Compact);
}

QList<DesktopShortcut> DesktopShortcutStore::deserialize(const QByteArray& data) {
    QList<DesktopShortcut> result;
    if (data.isEmpty()) {
        return result;
    }
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        cf::log::warningftag(kLogTag, "shortcuts JSON malformed: {}",
                             err.errorString().toStdString());
        return result;
    }
    for (const auto& value : doc.array()) {
        const QJsonObject o = value.toObject();
        DesktopShortcut s;
        s.app_id = o.value(QLatin1String(kFieldAppId)).toString();
        s.col = o.value(QLatin1String(kFieldCol)).toInt();
        s.row = o.value(QLatin1String(kFieldRow)).toInt();
        if (!s.app_id.isEmpty()) {
            result.append(s);
        }
    }
    return result;
}

QList<DesktopShortcut> DesktopShortcutStore::load() {
    namespace cfg = cf::config;
    auto domain = cfg::ConfigStore::instance().domain(kDomain);
    const std::string raw =
        domain.query<std::string>(cfg::KeyView{.group = kGroup, .key = kKey}, std::string());
    return deserialize(QByteArray::fromStdString(raw));
}

void DesktopShortcutStore::save(const QList<DesktopShortcut>& shortcuts) {
    namespace cfg = cf::config;
    auto domain = cfg::ConfigStore::instance().domain(kDomain);
    const std::string raw = serialize(shortcuts).toStdString();
    domain.set(cfg::KeyView{.group = kGroup, .key = kKey}, raw, cfg::Layer::User,
               cfg::NotifyPolicy::Immediate);
    cfg::ConfigStore::instance().sync();
}

QList<DesktopShortcut> DesktopShortcutStore::seedFrom(const QList<AppEntry>& apps, int cols) {
    QList<DesktopShortcut> result;
    if (cols < 1) {
        cols = 1;
    }
    const int n = std::min<int>(static_cast<int>(apps.size()), kSeedCount);
    result.reserve(n);
    for (int i = 0; i < n; ++i) {
        DesktopShortcut s;
        s.app_id = apps[i].app_id;
        s.col = i % cols;
        s.row = i / cols;
        result.append(s);
    }
    return result;
}

} // namespace cf::desktop::desktop_component
