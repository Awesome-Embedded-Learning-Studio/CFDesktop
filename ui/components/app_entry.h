/**
 * @file    app_entry.h
 * @brief   Application entry data model for the taskbar.
 *
 * AppEntry describes a single launchable application shown as an icon in the
 * CenteredTaskbar. Each entry carries a LaunchKind that decides whether it is
 * brought on screen as a detached process (exec_command) or as an in-process
 * builtin panel (looked up by app_id). defaultApps() returns a small set of
 * placeholder entries (file manager, terminal, settings, browser) used until a
 * real app registry exists.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.2
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include <QList>
#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  How an AppEntry is brought on screen.
 *
 * @ingroup components
 */
enum class LaunchKind {
    Auto,            ///< Hardware tier decides at load time (default for manifests).
    DetachedProcess, ///< Launched as a separate process via QProcess (exec_command).
    BuiltinPanel,    ///< Shown in-process as a builtin panel (looked up by app_id).
};

/**
 * @brief  Describes one launchable application for the taskbar.
 *
 * @ingroup components
 */
struct AppEntry {
    QString app_id;       ///< Stable unique identifier (e.g. "terminal").
    QString display_name; ///< Human-readable label (initial is drawn on the tile).
    QString icon_path;    ///< Icon resource path; empty leaves the tile blank (no fallback).
    QString exec_command; ///< Program path used only when launch_kind == DetachedProcess.
    LaunchKind launch_kind{LaunchKind::DetachedProcess}; ///< How this app is loaded.
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
 * @since   0.19
 * @ingroup components
 */
inline QList<AppEntry> defaultApps() {
    return {
        {.app_id = QStringLiteral("files"),
         .display_name = QStringLiteral("Files"),
         .icon_path = QStringLiteral(":/cfdesktop/taskbar/files.png"),
         .exec_command = QStringLiteral("xdg-open ."),
         .is_running = false},
        {.app_id = QStringLiteral("terminal"),
         .display_name = QStringLiteral("Terminal"),
         .icon_path = QStringLiteral(":/cfdesktop/taskbar/terminal.png"),
         .exec_command = QStringLiteral("xterm"),
         .is_running = false},
        {.app_id = QStringLiteral("settings"),
         .display_name = QStringLiteral("Settings"),
         .icon_path = QStringLiteral(":/cfdesktop/taskbar/settings.png"),
         .exec_command = QStringLiteral("cfdesktop-settings"),
         .is_running = false},
        {.app_id = QStringLiteral("browser"),
         .display_name = QStringLiteral("Browser"),
         .icon_path = QStringLiteral(":/cfdesktop/taskbar/browser.png"),
         .exec_command = QStringLiteral("xdg-open https://example.com"),
         .is_running = false},
    };
}

} // namespace cf::desktop::desktop_component
