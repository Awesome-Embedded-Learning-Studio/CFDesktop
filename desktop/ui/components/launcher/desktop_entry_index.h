/**
 * @file    desktop/ui/components/launcher/desktop_entry_index.h
 * @brief   Indexes XDG .desktop entries into AppEntry values.
 *
 * DesktopEntryIndex scans freedesktop .desktop files (Type=Application,
 * NoDisplay!=true) and returns AppEntry values with launch_kind=DetachedProcess,
 * so the launcher grid can surface system applications (firefox, etc.) the
 * user already has installed, alongside CFDesktop's own manifest apps.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "../app_entry.h"

#include <QList>
#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  Indexes XDG .desktop entries into AppEntry values.
 *
 * The discovery contract:
 * - A .desktop file's [Desktop Entry] section is parsed.
 * - Only Type=Application with NoDisplay!=true is indexed.
 * - app_id = file basename without ".desktop" (e.g. "firefox.desktop" -> "firefox").
 * - display_name = Name=, icon_path = Icon= (theme name or path, unresolved),
 *   exec_command = Exec= with %-fields stripped (e.g. "firefox %u" -> "firefox").
 *
 * @ingroup components
 */
class DesktopEntryIndex {
  public:
    DesktopEntryIndex() = delete;

    /**
     * @brief   Indexes default XDG application dirs.
     *
     * Scans @c ~/.local/share/applications and @c /usr/share/applications.
     * Missing dirs contribute nothing (no error). Returns DetachedProcess
     * AppEntry values for each valid Application-type .desktop.
     *
     * @return  Indexed AppEntry list; empty if no .desktop files are found.
     *
     * @throws  None.
     *
     * @since   0.20
     * @ingroup components
     */
    static QList<AppEntry> index();

    /**
     * @brief              Indexes .desktop files under @p dir.
     *
     * @param[in] dir       Directory containing *.desktop files.
     *
     * @return             Parsed AppEntry list.
     *
     * @throws             None.
     * @note               Used by index(); exposed for unit tests.
     * @since              0.20
     * @ingroup            components
     */
    static QList<AppEntry> indexFrom(const QString& dir);
};

} // namespace cf::desktop::desktop_component
