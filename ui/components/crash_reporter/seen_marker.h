/**
 * @file    seen_marker.h
 * @brief   ".seen" marker so a crash report is shown only once.
 *
 * markCrashSeen() writes an empty {@link stem}.seen file next to a crash
 * .json; isCrashSeen() checks for it. The reporter skips seen reports.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash_reporter
 */

#pragma once

#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  Reports whether a crash .json has been marked seen.
 *
 * @param[in]  crash_json_path  Path to the crash .json.
 * @return     True when a sibling {@link stem}.seen marker exists.
 * @throws     None
 * @note       None
 * @warning    None
 * @since      0.19.0
 * @ingroup    crash_reporter
 */
bool isCrashSeen(const QString& crash_json_path);

/**
 * @brief  Marks a crash .json as seen (writes {@link stem}.seen).
 *
 * @param[in]  crash_json_path  Path to the crash .json.
 * @throws     None
 * @note       Best-effort; a failed write leaves the report re-showable.
 * @warning    None
 * @since      0.19.0
 * @ingroup    crash_reporter
 */
void markCrashSeen(const QString& crash_json_path);

} // namespace cf::desktop::desktop_component
