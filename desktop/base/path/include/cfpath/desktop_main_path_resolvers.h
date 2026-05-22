/**
 * @file    desktop/base/path/include/cfpath/desktop_main_path_resolvers.h
 * @brief   Provides path resolution for desktop main directories and resources.
 *
 * Contains DesktopMainPathProvider class for managing and resolving paths
 * to standard desktop directories (Home, Desktop, Documents, etc.).
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   N/A
 * @ingroup desktop_base
 */

#pragma once
#include "base/singleton/singleton.hpp"
#include <QString>
#include <cstdint>

namespace cf::desktop::path {
/**
 * @brief  Provider for desktop main directory paths.
 *
 * Manages resolution of standard desktop directory paths including Home,
 * Desktop, Documents, Downloads, Music, Pictures, Videos, Apps, and Runtime.
 *
 * @ingroup desktop_base
 */
class DesktopMainPathProvider : public cf::Singleton<DesktopMainPathProvider> {
  public:
    /**
     * @brief Allows Singleton base to access private constructor.
     */
    friend class cf::Singleton<DesktopMainPathProvider>;
    /// @brief Single source of truth for all path types.
    /// Add new entries here — enum, string table, and count are derived automatically.
#define CF_PATH_TYPES \
    X(Home)           \
    X(Desktop)        \
    X(Documents)      \
    X(Downloads)      \
    X(Music)          \
    X(Pictures)       \
    X(Videos)         \
    X(Apps)           \
    X(Runtime)

    /**
     * @brief  Standard desktop path types.
     *
     * Defines enumeration values for common desktop directory types.
     *
     * @ingroup desktop_base
     */
    enum class PathType {
#define X(name) name,
        CF_PATH_TYPES
#undef X
    };

    /// @brief String names indexed by PathType enum value.
    static constexpr const char* kPathNames[] = {
#define X(name) #name,
        CF_PATH_TYPES
#undef X
    };

    static constexpr uint8_t PathTypeCnt = sizeof(kPathNames) / sizeof(kPathNames[0]);

#undef CF_PATH_TYPES

    /**
     * @brief   Checks and creates missing path directories.
     *
     * Verifies that all required path directories exist. Creates any
     * missing directories.
     *
     * @return  true if all paths are valid or created successfully,
     *          false otherwise.
     * @throws  None
     * @note    None
     * @warning None
     * @since   N/A
     * @ingroup desktop_base
     */
    bool setup();

    /**
     * @brief   Resolves the absolute path for a given path type.
     *
     * Returns the absolute path string for the specified desktop path type.
     *
     * @param[in] p The PathType enum value to resolve.
     * @return      QString containing the absolute path.
     * @throws      None
     * @note        None
     * @warning     None
     * @since       N/A
     * @ingroup     desktop_base
     */
    QString absolutePath(const PathType p);

    /**
     * @brief Checks if the directory for a given path type exists.
     *
     * @param[in] p The path type to check.
     * @return true if the directory exists, false otherwise.
     */
    bool component_exsited(const PathType p);

    /**
     * @brief Creates the directory for a given path type.
     *
     * Should be called when component_exsited() returns false.
     *
     * @param[in] p The path type to create.
     * @return true if creation succeeded, false otherwise.
     */
    bool request_created(const PathType p);

  private:
    /**
     * @brief  Constructs the path provider with a desktop root directory.
     *
     * @param[in] desktop_active_root Pointer to the desktop active root path.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_base
     */
    DesktopMainPathProvider(const QString& desktop_active_root);
    /// @brief View of the root directory path. Ownership: observer.
    QString root{};
};
} // namespace cf::desktop::path
