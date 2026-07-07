/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_shortcut.h
 * @brief   User-placed desktop shortcut value type.
 *
 * A DesktopShortcut is one entry in the user-managed desktop icon layout: the
 * application to launch (by app_id, resolved against the merged app registry at
 * runtime) plus its grid cell. Only the app_id + position is persisted; the
 * icon/name/launch-kind are looked up live so the shortcut never goes stale
 * when an app is updated or its manifest changes.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  One user-placed desktop shortcut.
 *
 * @ingroup components
 */
struct DesktopShortcut {
    QString app_id; ///< Stable app identifier (key into the merged AppEntry list).
    int col{0};     ///< Grid column (0-based, left-to-right).
    int row{0};     ///< Grid row (0-based, top-to-bottom).
};

} // namespace cf::desktop::desktop_component
