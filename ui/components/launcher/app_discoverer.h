/**
 * @file    desktop/ui/components/launcher/app_discoverer.h
 * @brief   Discovers standalone apps via per-app manifest files.
 *
 * Each app ships an @c app.json manifest in @c <apps_dir>/<app_id>/.
 * AppDiscoverer scans the apps directory, parses each manifest, and returns
 * AppEntry values with absolute @c icon_path / @c exec_command (so launch and
 * icon rendering do not depend on the working directory).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
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
 * @brief  Discovers standalone apps from per-app @c app.json manifests.
 *
 * The discovery contract:
 * - Scans @c <active_root>/apps/<id>/app.json (the runtime deployment target
 *   — where built apps are installed, e.g. via CFDeskit's @c cmake --install).
 * - Fields: @c app_id, @c display_name, @c icon (relative to manifest),
 *   @c exec (relative to manifest).
 * - @c icon_path / @c exec_command are resolved to absolute paths
 *   (@c <app_dir>/<icon|exec>).
 * - Subdirs without @c app.json are skipped silently; manifests missing
 *   @c app_id or @c exec are skipped with a warning (no silent fallback).
 * - Empty/missing apps directory → INFO log, empty result (apps are optional).
 *
 * @ingroup components
 */
class AppDiscoverer {
  public:
    AppDiscoverer() = delete;

    /**
     * @brief   Discovers apps under @c <active_root>/apps/<id>/app.json.
     *
     * @return  Parsed AppEntry list (icon/exec resolved to absolute paths);
     *          empty if the apps directory is absent or has no manifests.
     * @throws  None.
     * @since   0.20
     * @ingroup components
     */
    static QList<AppEntry> discover();

    /**
     * @brief              Discovers apps under @p apps_dir.
     *
     * @param[in] apps_dir  Directory containing <id>/ subdirs with app.json.
     * @return             Parsed AppEntry list.
     * @throws             None.
     * @note               Used by discover(); exposed for unit tests.
     * @since              0.20
     * @ingroup            components
     */
    static QList<AppEntry> discoverFrom(const QString& apps_dir);
};

} // namespace cf::desktop::desktop_component
