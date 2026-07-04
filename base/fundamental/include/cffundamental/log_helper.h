/**
 * @file    desktop/base/fundamental/include/cffundamental/log_helper.h
 * @brief   Provides logging helper functions for the desktop module.
 *
 * Contains utility functions for generating log filenames and related
 * logging operations.
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   N/A
 * @ingroup desktop_base
 */

#pragma once
#include <QString>

namespace cf::desktop::logger_helper {

/**
 * @brief   Gets the log filename for the desktop application.
 *
 * Returns the filename used for logging without path information.
 *
 * @return  QString containing the log filename.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup desktop_base
 */
QString log_filename();

} // namespace cf::desktop::logger_helper
