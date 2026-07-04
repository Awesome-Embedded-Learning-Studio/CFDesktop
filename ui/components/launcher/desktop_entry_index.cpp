/**
 * @file    desktop/ui/components/launcher/desktop_entry_index.cpp
 * @brief   Implementation of DesktopEntryIndex.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "desktop_entry_index.h"

#include "cflog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTextStream>

namespace cf::desktop::desktop_component {

namespace {
/// Tag for DesktopEntryIndex log lines.
constexpr const char* kLogTag = "DesktopEntryIndex";
/// The only section parsed in a .desktop file.
constexpr const char* kDesktopSection = "[Desktop Entry]";

/**
 * @brief   Strips freedesktop %-fields from an Exec= value.
 *
 * "firefox %u" -> "firefox"; "code --foo %F" -> "code --foo". Tokens starting
 * with '%' are dropped per the freedesktop Exec field spec.
 *
 * @param[in] exec  The raw Exec= value.
 *
 * @return  The cleaned command string.
 */
QString cleanExec(const QString& exec) {
    QStringList parts = QProcess::splitCommand(exec);
    QStringList kept;
    kept.reserve(parts.size());
    for (const auto& part : parts) {
        if (part.startsWith(QLatin1Char('%'))) {
            continue;
        }
        kept.append(part);
    }
    return kept.join(QLatin1Char(' '));
}

/**
 * @brief          Parses one .desktop file into an AppEntry.
 *
 * @param[in] path  Absolute path to the .desktop file.
 *
 * @return         AppEntry with launch_kind=DetachedProcess, or an empty
 *                 entry (empty app_id) when Type!=Application or NoDisplay=true
 *                 or the file is unreadable.
 */
AppEntry parseDesktopFile(const QString& path) {
    AppEntry entry; // empty by default; launch_kind defaults DetachedProcess.
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return entry;
    }
    entry.app_id = QFileInfo(path).completeBaseName(); // "firefox.desktop" -> "firefox"

    bool in_section = false;
    bool is_application = false;
    bool no_display = false;
    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString raw = in.readLine();
        const QString line = raw.trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
            continue;
        }
        if (line.startsWith(QLatin1Char('[')) && line.endsWith(QLatin1Char(']'))) {
            in_section = (line == QLatin1String(kDesktopSection));
            continue;
        }
        if (!in_section) {
            continue;
        }
        const int eq = line.indexOf(QLatin1Char('='));
        if (eq < 0) {
            continue;
        }
        const QString key = line.left(eq).trimmed();
        const QString value = line.mid(eq + 1).trimmed();
        if (key == QLatin1String("Type")) {
            is_application = (value == QLatin1String("Application"));
        } else if (key == QLatin1String("NoDisplay")) {
            no_display = (value == QLatin1String("true"));
        } else if (key == QLatin1String("Name") && entry.display_name.isEmpty()) {
            entry.display_name = value;
        } else if (key == QLatin1String("Icon")) {
            entry.icon_path = value; // theme name or absolute path; unresolved here.
        } else if (key == QLatin1String("Exec")) {
            entry.exec_command = cleanExec(value);
        }
    }

    if (!is_application || no_display) {
        return AppEntry{}; // signal skip via empty app_id.
    }
    if (entry.display_name.isEmpty()) {
        entry.display_name = entry.app_id; // fall back to basename.
    }
    return entry;
}
} // namespace

QList<AppEntry> DesktopEntryIndex::index() {
    QList<AppEntry> result;
    const QStringList dirs = {
        QDir::homePath() + QStringLiteral("/.local/share/applications"),
        QStringLiteral("/usr/share/applications"),
    };
    for (const auto& dir : dirs) {
        result.append(indexFrom(dir));
    }
    return result;
}

QList<AppEntry> DesktopEntryIndex::indexFrom(const QString& dir) {
    QList<AppEntry> result;
    QDir d(dir);
    if (!d.exists()) {
        return result;
    }
    const QFileInfoList files =
        d.entryInfoList(QStringList() << QStringLiteral("*.desktop"), QDir::Files);
    for (const auto& info : files) {
        AppEntry entry = parseDesktopFile(info.absoluteFilePath());
        if (entry.app_id.isEmpty() || entry.exec_command.isEmpty()) {
            continue; // unreadable, non-Application, or NoDisplay.
        }
        result.append(entry);
    }
    return result;
}

} // namespace cf::desktop::desktop_component
