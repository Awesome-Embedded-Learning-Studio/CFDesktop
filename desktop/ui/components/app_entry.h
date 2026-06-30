/**
 * @file    app_entry.h
 * @brief   Application entry data model for the taskbar.
 *
 * AppEntry describes a single launchable application shown as an icon in the
 * CenteredTaskbar. defaultApps() returns a small set of placeholder entries
 * (file manager, terminal, settings, browser) used until a real app registry
 * exists.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.1
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include <QList>
#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  Describes one launchable application for the taskbar.
 *
 * @ingroup components
 */
struct AppEntry {
    QString app_id;         ///< Stable unique identifier (e.g. "terminal").
    QString display_name;   ///< Human-readable label (initial is drawn on the tile).
    QString icon_path;      ///< Icon resource path; empty leaves the tile blank (no fallback).
    QString exec_command;   ///< Launch command consumed later by QProcess.
    bool is_running{false}; ///< Whether the app currently has a live window.
};

/**
 * @brief  Returns the default placeholder application set.
 *
 * @return A list of sample AppEntry values (files, terminal, settings,
 *         browser). Their exec_command values are placeholders, not yet
 *         wired to real launching.
 *
 * @throws None
 * @note   None
 * @warning None
 * @since  0.19
 * @ingroup components
 */
inline QList<AppEntry> defaultApps() {
    return {
        {QStringLiteral("files"), QStringLiteral("Files"),
         QStringLiteral(":/cfdesktop/taskbar/files.png"), QStringLiteral("xdg-open ."), false},
        {QStringLiteral("terminal"), QStringLiteral("Terminal"),
         QStringLiteral(":/cfdesktop/taskbar/terminal.png"), QStringLiteral("xterm"), false},
        {QStringLiteral("settings"), QStringLiteral("Settings"),
         QStringLiteral(":/cfdesktop/taskbar/settings.png"), QStringLiteral("cfdesktop-settings"),
         false},
        {QStringLiteral("browser"), QStringLiteral("Browser"),
         QStringLiteral(":/cfdesktop/taskbar/browser.png"),
         QStringLiteral("xdg-open https://example.com"), false},
    };
}

} // namespace cf::desktop::desktop_component
