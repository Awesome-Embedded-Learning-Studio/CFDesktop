/**
 * @file    desktop/main/init/boot_stage_info.h
 * @brief   Defines the boot stage information structure.
 *
 * Provides a data structure for capturing and reporting information about
 * initialization stages during the desktop session boot process.
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   N/A
 * @ingroup desktop_main
 */

#pragma once
#include <QString>

namespace cf::desktop::init_session {

/**
 * @brief  Encapsulates information about a boot stage.
 *
 * Stores details about initialization stages including descriptive text
 * and the stage index for tracking boot progress.
 *
 * @ingroup desktop_main
 */
struct BootStageInfo {
    QString info;    ///< Descriptive information about the boot stage.
    int stage_index; ///< Sequential index identifying the boot stage.
};

} // namespace cf::desktop::init_session
