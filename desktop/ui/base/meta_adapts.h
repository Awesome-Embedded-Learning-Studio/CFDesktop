/**
 * @file meta_adapts.h
 * @brief Meta information helper for CFDesktop.
 *
 * Provides structured, type-safe access to project build and runtime metadata.
 * All compile-time information is available via constexpr functions for zero
 * runtime overhead.
 *
 * @author CFDesktop Team
 * @date 2026-03-27
 * @version N/A
 * @since N/A
 * @ingroup desktop_ui_base
 */

#pragma once

#include <compare>
#include <cstdint>
#include <string_view>

namespace cf::desktop::metainfo {

// ============================================================
// Version Type - Full semantic version support
// ============================================================

/**
 * @brief Semantic version with comprehensive comparison and operations.
 *
 * Provides semantic versioning capabilities including:
 * - Three-component version (major.minor.patch)
 * - Comparison operators (spaceship)
 * - Version range checking
 * - String parsing
 *
 * @throws None
 * @note All methods are constexpr for compile-time evaluation.
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t patch = 0;

    // ========== Basic comparison ==========
    constexpr auto operator<=>(const Version&) const noexcept = default;

    // ========== Version information ==========
    /**
     * @brief Convert to packed 32-bit integer.
     *
     * @return Packed version as 0xMMmmpp (MM=major, mm=minor, pp=patch).
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr uint32_t to_uint32() const noexcept {
        return (uint32_t{major} << 16) | (uint32_t{minor} << 8) | patch;
    }

    /**
     * @brief Get string representation of version.
     *
     * @return String view in format "major.minor.patch".
     * @throws None
     * @note The returned string_view has static storage duration.
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view to_string_view() const noexcept;

    // ========== Version predicates ==========
    /**
     * @brief Check if this is a pre-release version (major == 0).
     *
     * @return True if major version is 0.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool is_pre_release() const noexcept { return major == 0; }

    /**
     * @brief Check if this is a stable version (major > 0).
     *
     * @return True if major version is greater than 0.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool is_stable() const noexcept { return major > 0; }

    // ========== Semantic version comparison ==========
    /**
     * @brief Check if version matches a specific major version.
     *
     * @param[in] m Major version to compare against.
     * @return True if major version matches.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool matches_major(uint8_t m) const noexcept { return major == m; }

    /**
     * @brief Check if version matches specific major and minor versions.
     *
     * @param[in] m        Major version to compare against.
     * @param[in] minor_ver Minor version to compare against.
     * @return True if both major and minor versions match.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool matches_minor(uint8_t m, uint8_t minor_ver) const noexcept {
        return major == m && minor == minor_ver;
    }

    // ========== Version range checking ==========
    /**
     * @brief Check if this version is at least the given version.
     *
     * @param[in] v Version to compare against.
     * @return True if this version is greater than or equal to v.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool is_at_least(Version v) const noexcept { return to_uint32() >= v.to_uint32(); }

    /**
     * @brief Check if this version is less than the given version.
     *
     * @param[in] v Version to compare against.
     * @return True if this version is less than v.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool is_less_than(Version v) const noexcept { return to_uint32() < v.to_uint32(); }

    /**
     * @brief Check if version is within [min, max) range (max exclusive).
     *
     * @param[in] min Minimum version (inclusive).
     * @param[in] max Maximum version (exclusive).
     * @return True if version is in range [min, max).
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool is_in_range(Version min, Version max) const noexcept {
        return is_at_least(min) && is_less_than(max);
    }

    /**
     * @brief Check if version is within [min, max] range (max inclusive).
     *
     * @param[in] min Minimum version (inclusive).
     * @param[in] max Maximum version (inclusive).
     * @return True if version is in range [min, max].
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    constexpr bool is_in_range_inclusive(Version min, Version max) const noexcept {
        return to_uint32() >= min.to_uint32() && to_uint32() <= max.to_uint32();
    }

    // ========== Static factories ==========
    /**
     * @brief Create Version from packed 32-bit integer.
     *
     * @param[in] v Packed version as 0xMMmmpp.
     * @return Version struct extracted from packed integer.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    static constexpr Version from_uint32(uint32_t v) noexcept {
        return Version{static_cast<uint8_t>((v >> 16) & 0xFF),
                       static_cast<uint8_t>((v >> 8) & 0xFF), static_cast<uint8_t>(v & 0xFF)};
    }

    /**
     * @brief Parse version from string view.
     *
     * @param[in] sv String in format "major.minor.patch".
     * @return Parsed version, or {0,0,0} if parsing fails.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    static constexpr Version parse(std::string_view sv) noexcept;
};

// ============================================================
// Implementation detail namespace
// ============================================================

namespace detail {
// Forward declaration - actual definition in cpp
struct AutogenAccess;
} // namespace detail

// ============================================================
// Project Information (Compile-time)
// ============================================================

/**
 * @brief Project metadata (compile-time determined).
 *
 * Provides access to project name, version, description, author, and URLs.
 * All methods are constexpr for zero runtime overhead.
 *
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
struct Project {
    /**
     * @brief Get project name.
     *
     * @return Project name as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view name() const noexcept;

    /**
     * @brief Get project version as Version struct.
     *
     * @return Version struct containing major, minor, and patch.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    Version version() const noexcept;

    /**
     * @brief Get project version as string.
     *
     * @return Version string in format "major.minor.patch".
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view version_string() const noexcept;

    /**
     * @brief Get project description.
     *
     * @return Project description as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view description() const noexcept;

    /**
     * @brief Get project author.
     *
     * @return Project author as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view author() const noexcept;

    /**
     * @brief Get project homepage URL.
     *
     * @return Homepage URL as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view homepage_url() const noexcept;

    /**
     * @brief Get project repository URL.
     *
     * @return Repository URL as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view repository_url() const noexcept;

    /**
     * @brief Get project license (SPDX identifier).
     *
     * @return License identifier (e.g., "MIT").
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view license() const noexcept;

    /**
     * @brief Get copyright notice string.
     *
     * @return Copyright notice (e.g., "Copyright (c) 2026-present").
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view copyright() const noexcept;
};

// ============================================================
// Build Information (Compile-time)
// ============================================================

/**
 * @brief Build metadata (compile-time determined).
 *
 * Provides compiler, build type, platform, and timestamp information.
 * All methods are constexpr for zero runtime overhead.
 *
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
struct Build {
    /**
     * @brief Get compiler name (GCC, Clang, MSVC, etc.).
     *
     * @return Compiler name as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view compiler_name() const noexcept;

    /**
     * @brief Get compiler version string.
     *
     * @return Compiler version as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view compiler_version() const noexcept;

    /**
     * @brief Get compiler ID (CMAKE_CXX_COMPILER_ID).
     *
     * @return Compiler ID as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view compiler_id() const noexcept;

    // ========== Compiler detection helpers ==========
    /**
     * @brief Check if built with MSVC.
     *
     * @return True if built with Microsoft Visual C++.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_msvc() const noexcept;

    /**
     * @brief Check if built with GCC.
     *
     * @return True if built with GNU Compiler Collection.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_gcc() const noexcept;

    /**
     * @brief Check if built with Clang (including AppleClang).
     *
     * @return True if built with Clang or AppleClang.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_clang() const noexcept;

    /**
     * @brief Check if built with AppleClang.
     *
     * @return True if built with Apple's Clang compiler.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_apple_clang() const noexcept;

    // ========== Build type ==========
    /**
     * @brief Get build type string (DEBUG, RELEASE, etc.).
     *
     * @return Build type as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view build_type() const noexcept;

    /**
     * @brief Check if this is a debug build.
     *
     * @return True if build type is DEBUG.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_debug_build() const noexcept;

    /**
     * @brief Check if this is a release build.
     *
     * @return True if build type is RELEASE.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_release_build() const noexcept;

    /**
     * @brief Check if this is a relwithdebinfo build.
     *
     * @return True if build type is RELWITHDEBINFO.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_relwithdebinfo_build() const noexcept;

    /**
     * @brief Check if this is a minsizerel build.
     *
     * @return True if build type is MINSIZEREL.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_minsizerel_build() const noexcept;

    // ========== C++ standard ==========
    /**
     * @brief Get C++ standard version (17, 20, 23, etc.).
     *
     * @return C++ standard year as integer.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    int cxx_standard() const noexcept;

    /**
     * @brief Check if C++ standard is at least the given version.
     *
     * @param[in] standard C++ standard year to compare against.
     * @return True if current standard is at least the given version.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool cxx_at_least(int standard) const noexcept;

    // ========== Platform information ==========
    /**
     * @brief Get system name (Linux, Windows, Darwin, etc.).
     *
     * @return System name as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view system_name() const noexcept;

    /**
     * @brief Get system processor architecture.
     *
     * @return Processor architecture as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view system_processor() const noexcept;

    // ========== Platform detection helpers ==========
    /**
     * @brief Check if building for Linux.
     *
     * @return True if target system is Linux.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_linux() const noexcept;

    /**
     * @brief Check if building for Windows.
     *
     * @return True if target system is Windows.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_windows() const noexcept;

    /**
     * @brief Check if building for macOS.
     *
     * @return True if target system is macOS (Darwin).
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_macos() const noexcept;

    // ========== Build timestamp ==========
    /**
     * @brief Get build date (YYYY-MM-DD format).
     *
     * @return Build date as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view build_date() const noexcept;

    /**
     * @brief Get build time (HH:MM:SS format).
     *
     * @return Build time as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view build_time() const noexcept;

    /**
     * @brief Get full build timestamp ("YYYY-MM-DD HH:MM:SS" format).
     *
     * @return Full build timestamp as string view.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view build_timestamp() const noexcept;
};

// ============================================================
// Features Configuration (Compile-time)
// ============================================================

/**
 * @brief Feature configuration (compile-time determined).
 *
 * Provides information about linked library versions and features.
 *
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
struct Features {
    /**
     * @brief Get Qt version as Version struct.
     *
     * @return Qt version struct containing major, minor, and patch.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    Version qt_version() const noexcept;

    /**
     * @brief Get Qt version as string.
     *
     * @return Qt version string in format "major.minor.patch".
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    std::string_view qt_version_string() const noexcept;

    /**
     * @brief Get Qt major version number.
     *
     * @return Qt major version as integer.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    int qt_major() const noexcept;

    /**
     * @brief Get Qt minor version number.
     *
     * @return Qt minor version as integer.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    int qt_minor() const noexcept;

    /**
     * @brief Get Qt patch version number.
     *
     * @return Qt patch version as integer.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    int qt_patch() const noexcept;

    // ========== Qt version helpers ==========
    /**
     * @brief Check if Qt version is at least the given version.
     *
     * @param[in] major Qt major version to compare against.
     * @param[in] minor Qt minor version to compare against (default 0).
     * @param[in] patch Qt patch version to compare against (default 0).
     * @return True if Qt version is at least the specified version.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool qt_at_least(uint8_t major, uint8_t minor = 0, uint8_t patch = 0) const noexcept;

    /**
     * @brief Check if Qt version is exactly the given version.
     *
     * @param[in] major Qt major version to compare against.
     * @param[in] minor Qt minor version to compare against (default 0).
     * @param[in] patch Qt patch version to compare against (default 0).
     * @return True if Qt version exactly matches the specified version.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool qt_is(uint8_t major, uint8_t minor = 0, uint8_t patch = 0) const noexcept;

    /**
     * @brief Check if Qt 6 is being used.
     *
     * @return True if Qt major version is 6.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup desktop_ui_base
     */
    bool is_qt6() const noexcept;
};

// ============================================================
// Global access points
// ============================================================

/**
 * @brief Get project information.
 *
 * @return Project struct with compile-time project metadata.
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
Project project() noexcept;

/**
 * @brief Get build information.
 *
 * @return Build struct with compile-time build metadata.
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
Build build() noexcept;

/**
 * @brief Get feature configuration.
 *
 * @return Features struct with compile-time feature metadata.
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup desktop_ui_base
 */
Features features() noexcept;

// ============================================================
// Inline constexpr implementations
// ============================================================

inline constexpr Version Version::parse(std::string_view sv) noexcept {
    Version result{0, 0, 0};

    size_t pos = 0;
    uint8_t current = 0;
    uint8_t* target = &result.major;

    for (size_t i = 0; i < sv.size() && pos < 3; ++i) {
        char c = sv[i];
        if (c >= '0' && c <= '9') {
            current = static_cast<uint8_t>(current * 10 + (c - '0'));
        } else if (c == '.') {
            target[pos++] = current;
            current = 0;
            if (pos < 3) {
                target = &result.major + pos;
            }
        }
    }
    if (pos < 3) {
        target[pos] = current;
    }

    return result;
}

} // namespace cf::desktop::metainfo
