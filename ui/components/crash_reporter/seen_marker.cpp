/**
 * @file    seen_marker.cpp
 * @brief   Implementation of the crash ".seen" marker helpers.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash_reporter
 */

#include "seen_marker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Builds the sibling {@link stem}.seen path for a crash .json.
QString seenPath(const QString& crash_json_path) {
    const QFileInfo fi(crash_json_path);
    return QDir(fi.absolutePath()).filePath(fi.completeBaseName() + ".seen");
}
} // namespace

bool isCrashSeen(const QString& crash_json_path) {
    return QFileInfo::exists(seenPath(crash_json_path));
}

void markCrashSeen(const QString& crash_json_path) {
    QFile f(seenPath(crash_json_path));
    if (f.open(QIODevice::WriteOnly)) {
        f.close();
    }
}

} // namespace cf::desktop::desktop_component
