/**
 * @file    desktop/base/file_operations/filter_target.h
 * @brief   File-system filtering utilities for directory traversal.
 *
 * Provides filter types and functions for listing files in a directory
 * that match specific criteria (e.g., pictures, all files).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.1
 * @ingroup filesystem
 */

#pragma once
#include <QStringList>

namespace cf::desktop::base::filesystem {

/**
 * @brief  File filter categories for directory traversal.
 *
 * @ingroup filesystem
 */
enum class FilterType {
    Pictures, ///< Common image file extensions.
    AllFiles, ///< All regular files.
    All,      ///< All entries including hidden files.
    Dirent    ///< Directory entries only.
};

QString request_filter(const FilterType what);
QStringList& request_filterlist(const FilterType what);

/**
 * @brief Get the file associate the filters
 *
 * @param dirent_path
 * @param filters
 * @return QStringList
 */
QStringList filter_target(const QString& dirent_path, const QStringList& filters);
} // namespace cf::desktop::base::filesystem
