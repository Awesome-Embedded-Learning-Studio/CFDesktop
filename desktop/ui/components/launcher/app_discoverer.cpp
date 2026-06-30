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

#include <QCoreApplication>
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
    const QString apps_dir = QCoreApplication::applicationDirPath() + QStringLiteral("/../apps");
    return discoverFrom(apps_dir);
}

QList<AppEntry> AppDiscoverer::discoverFrom(const QString& apps_dir) {
    QList<AppEntry> result;
    QDir dir(apps_dir);
    if (!dir.exists()) {
        return result;
    }

    const QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
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
        result.append(entry);
    }
    return result;
}

} // namespace cf::desktop::desktop_component
