/**
 * @file    desktop/base/file_operations/file_op.h
 * @brief   File and directory utility operations for CFDesktop.
 *
 * Provides utilities for creating files/directories, accessing application
 * runtime paths, concatenating file paths, and checking path existence.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup desktop_base
 */
#pragma once
#include <QString>

namespace cf::desktop::base::filesystem {

/**
 * @brief  Creates a file or directory at the specified path.
 *
 * This function automatically creates parent directories if they do not exist.
 * The type of resource created (file vs. directory) is determined by:
 * - Paths ending with a separator are treated as directories.
 * - Paths with a file extension (containing '.') are treated as files.
 * - All other paths are treated as directories.
 *
 * @param[in]  path  The absolute or relative path of the file/directory to create.
 * @return           True if the file/directory was created successfully or already
 *                   exists, false if the operation failed.
 * @throws           None
 * @note             If the path already exists, the function returns true without
 *                   modification. For files, an empty file is created. For
 *                   directories, all necessary parent directories are created
 *                   recursively.
 * @warning          None
 * @since            0.1
 * @ingroup          desktop_base
 */
bool create_anyway(const QString& path);

/**
 * @brief  Returns the application runtime directory path.
 *
 * Gets the platform-specific directory where the application stores
 * runtime data and temporary files.
 *
 * @return     The absolute path to the application runtime directory.
 * @throws     None
 * @note       None
 * @warning    None
 * @since      0.1
 * @ingroup    desktop_base
 */
QString app_runtime_dir();

/**
 * @brief  Returns the absolute path by concatenating directory and base names.
 *
 * Combines a directory entry path with a relative path component to form
 * a complete absolute path.
 *
 * @param[in]  dirent   The base directory path.
 * @param[in]  rest_dir The relative path component to append.
 * @return              The concatenated absolute file path.
 * @throws              None
 * @note                None
 * @warning             None
 * @since               N/A
 * @ingroup             desktop_base
 */
QString concat_filepath(const QString& dirent, const QString& rest_dir);

/**
 * @brief  Checks whether a file or directory exists at the specified path.
 *
 * @param[in]  path  The absolute or relative path to check.
 * @return           True if the path exists, false otherwise.
 * @throws           None
 * @note             None
 * @warning          None
 * @since            N/A
 * @ingroup          desktop_base
 */
bool exsited(const QString& path);

} // namespace cf::desktop::base::filesystem
