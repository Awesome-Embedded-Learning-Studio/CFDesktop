/**
 * @file    desktop/ui/components/launcher/app_discoverer.cpp
 * @brief   Implementation of AppDiscoverer.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "app_discoverer.h"

#include "cflog.h"
#include "cfpath/desktop_main_path_resolvers.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

namespace cf::desktop::desktop_component {

namespace {
/// Tag for AppDiscoverer log lines.
constexpr const char* kLogTag = "AppDiscoverer";
/// Manifest filename inside each <id>/ app directory.
constexpr const char* kManifestName = "app.json";
} // namespace

QList<AppEntry> AppDiscoverer::discover() {
    // Apps deployment target: <active_root>/apps/<id>/{exe, app.json}. This is
    // where installed/built apps land (e.g. via CFDeskit's cmake --install).
    const QString apps_dir = cf::desktop::path::DesktopMainPathProvider::instance().absolutePath(
        cf::desktop::path::DesktopMainPathProvider::PathType::Apps);
    return discoverFrom(apps_dir);
}

QList<AppEntry> AppDiscoverer::discoverFrom(const QString& apps_dir) {
    QList<AppEntry> result;
    if (!QDir(apps_dir).exists()) {
        // Apps are optional — a missing apps directory is normal (nothing
        // deployed yet). Log INFO, not a warning, and return empty.
        log::infoftag(kLogTag, "Apps directory '{}' not found; no apps to discover",
                      apps_dir.toStdString());
        return result;
    }

    const QFileInfoList subdirs = QDir(apps_dir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto& sub : subdirs) {
        const QString app_dir = sub.absoluteFilePath();
        const QString manifest_path =
            app_dir + QLatin1Char('/') + QString::fromLatin1(kManifestName);

        QFile file(manifest_path);
        if (!file.open(QIODevice::ReadOnly)) {
            continue; // No app.json — not an app directory, skip silently.
        }

        const QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
        AppEntry entry;
        entry.app_id = obj.value(QStringLiteral("app_id")).toString();
        entry.display_name = obj.value(QStringLiteral("display_name")).toString();
        const QString icon = obj.value(QStringLiteral("icon")).toString();
        const QString exec = obj.value(QStringLiteral("exec")).toString();

        if (entry.app_id.isEmpty() || exec.isEmpty()) {
            log::warningftag(kLogTag, "Skipping '{}': missing app_id/exec",
                             manifest_path.toStdString());
            continue;
        }

        entry.icon_path = icon.isEmpty() ? QString() : app_dir + QLatin1Char('/') + icon;
        entry.exec_command = app_dir + QLatin1Char('/') + exec;

        // launch_kind defaults to Auto so the hardware tier decides at load
        // time; detached/builtin force a path, unknown values fall back to Auto.
        const QString kind_str = obj.value(QStringLiteral("launch_kind")).toString();
        if (kind_str.isEmpty() || kind_str == QStringLiteral("auto")) {
            entry.launch_kind = LaunchKind::Auto;
        } else if (kind_str == QStringLiteral("detached")) {
            entry.launch_kind = LaunchKind::DetachedProcess;
        } else if (kind_str == QStringLiteral("builtin")) {
            entry.launch_kind = LaunchKind::BuiltinPanel;
        } else {
            log::warningftag(kLogTag, "Unknown launch_kind '{}' in '{}', defaulting to auto",
                             kind_str.toStdString(), manifest_path.toStdString());
            entry.launch_kind = LaunchKind::Auto;
        }

        result.append(entry);
    }

    if (result.isEmpty()) {
        log::infoftag(kLogTag, "No apps discovered under '{}'", apps_dir.toStdString());
    }
    return result;
}

} // namespace cf::desktop::desktop_component
