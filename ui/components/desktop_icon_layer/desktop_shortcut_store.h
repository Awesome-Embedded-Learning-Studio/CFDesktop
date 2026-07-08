/**
 * @file    desktop/ui/components/desktop_icon_layer/desktop_shortcut_store.h
 * @brief   Persists the user's desktop shortcut layout via ConfigStore.
 *
 * DesktopShortcutStore bridges DesktopShortcut values and the project's 4-layer
 * ConfigStore: the layout is serialized to a compact JSON string and stored at
 * the User layer (domain "desktop_icons") so it survives sessions. On first run
 * (no stored layout) the caller seeds a default set from the merged app
 * registry. Serialization is split out as static functions so the JSON
 * round-trip is unit-testable without touching the real ConfigStore.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "app_entry.h"
#include "desktop_shortcut.h"

#include <QByteArray>
#include <QList>

namespace cf::desktop::desktop_component {

/**
 * @brief  Loads / saves / seeds the desktop shortcut layout.
 *
 * @ingroup components
 */
class DesktopShortcutStore {
  public:
    /**
     * @brief   Loads the persisted shortcut layout.
     *
     * Reads the JSON string at User layer domain "desktop_icons". Returns an
     * empty list when nothing is stored (first run) or the JSON is malformed
     * (a warning is logged, never an exception).
     *
     * @return  The persisted shortcuts; empty on first run or parse failure.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    static QList<DesktopShortcut> load();

    /**
     * @brief          Persists the shortcut layout to the User layer.
     *
     * Serializes @p shortcuts to a compact JSON string and writes it via
     * ConfigStore (Immediate notify) + sync, so the change hits disk.
     *
     * @param[in] shortcuts  The layout to persist.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    static void save(const QList<DesktopShortcut>& shortcuts);

    /**
     * @brief           Builds a default shortcut layout from the app registry.
     *
     * Used on first run: takes the first N apps and lays them out left-to-right
     * then top-to-bottom so the desktop is not empty. The caller persists the
     * result so subsequent runs are user-managed.
     *
     * @param[in] apps  The merged app registry (builtin + manifest + .desktop).
     * @param[in] cols  Column count for the seed layout (fills the width on
     *                  wide screens; the caller derives it from screen size).
     *
     * @return  A seeded shortcut list (about + the first few apps in grid order).
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    static QList<DesktopShortcut> seedFrom(const QList<AppEntry>& apps, int cols);

    /**
     * @brief           Serializes shortcuts to compact JSON bytes.
     *
     * Exposed for unit tests so the round-trip can be verified without the real
     * ConfigStore. Format: `[{"app_id":..,"col":..,"row":..}, ...]`.
     *
     * @param[in] shortcuts  The layout to serialize.
     *
     * @return  Compact JSON bytes.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    static QByteArray serialize(const QList<DesktopShortcut>& shortcuts);

    /**
     * @brief          Parses shortcuts from JSON bytes.
     *
     * Inverse of serialize(). Malformed input yields an empty list (a warning
     * is logged); entries with an empty app_id are skipped.
     *
     * @param[in] data  JSON bytes (compact or pretty).
     *
     * @return  Parsed shortcuts; empty on parse failure or empty input.
     *
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    static QList<DesktopShortcut> deserialize(const QByteArray& data);
};

} // namespace cf::desktop::desktop_component
