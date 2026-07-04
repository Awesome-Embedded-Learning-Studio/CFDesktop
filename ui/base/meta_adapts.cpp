/**
 * @file meta_adapts.cpp
 * @brief Meta information helper implementation
 *
 * @see meta_adapts.h for interface documentation
 */

#include "meta_adapts.h"
#include "autogen/metainfo/build_info.h"
#include "autogen/metainfo/feature_config.h"
#include "autogen/metainfo/project_info.h"

#include <array>
#include <cstdio>

namespace cf::desktop::metainfo {

// ============================================================
// Implementation detail namespace
// ============================================================

namespace detail {

/**
 * @brief Accessor for auto-generated meta information
 *
 * This namespace alias provides internal access to the generated
 * metadata while hiding it from external users.
 */
namespace autogen = cf::desktop::autogen::metainfo;

} // namespace detail

// ============================================================
// Version implementation
// ============================================================

namespace {
// Static buffers for string representations
// Thread-safe: constant initialization, read-only after init
inline constexpr size_t VERSION_STRING_SIZE = 32; // "255.255.255" + null
inline constexpr size_t TIMESTAMP_SIZE = 20;      // "YYYY-MM-DD HH:MM:SS" + null

// Version string cache (per-Version instance not possible for constexpr)
// Instead, we use thread_local storage for to_string_view()
thread_local std::array<char, VERSION_STRING_SIZE> g_version_buffer{};
thread_local std::array<char, TIMESTAMP_SIZE> g_timestamp_buffer{};

// Flag to track if buffers are initialized
inline thread_local bool g_version_buffer_init = false;
inline thread_local bool g_timestamp_buffer_init = false;

} // anonymous namespace

std::string_view Version::to_string_view() const noexcept {
    // Format: "major.minor.patch"
    int len = std::snprintf(g_version_buffer.data(), VERSION_STRING_SIZE, "%u.%u.%u", major, minor,
                            patch);
    if (len > 0 && len < VERSION_STRING_SIZE) {
        return std::string_view(g_version_buffer.data(), static_cast<size_t>(len));
    }
    return "0.0.0";
}

// ============================================================
// Project implementation
// ============================================================

std::string_view Project::name() const noexcept {
    return detail::autogen::project_name;
}

Version Project::version() const noexcept {
    return Version{static_cast<uint8_t>(detail::autogen::project_version_major),
                   static_cast<uint8_t>(detail::autogen::project_version_minor),
                   static_cast<uint8_t>(detail::autogen::project_version_patch)};
}

std::string_view Project::version_string() const noexcept {
    return detail::autogen::project_version;
}

std::string_view Project::description() const noexcept {
    return detail::autogen::project_description;
}

std::string_view Project::author() const noexcept {
    return detail::autogen::project_author;
}

std::string_view Project::homepage_url() const noexcept {
    return detail::autogen::project_homepage_url;
}

std::string_view Project::repository_url() const noexcept {
    return detail::autogen::project_repository_url;
}

std::string_view Project::license() const noexcept {
    return detail::autogen::project_license;
}

std::string_view Project::copyright() const noexcept {
    return detail::autogen::project_copyright;
}

// ============================================================
// Build implementation
// ============================================================

std::string_view Build::compiler_name() const noexcept {
    return detail::autogen::build_compiler_name;
}

std::string_view Build::compiler_version() const noexcept {
    return detail::autogen::build_compiler_version;
}

std::string_view Build::compiler_id() const noexcept {
    return detail::autogen::build_compiler_id;
}

bool Build::is_msvc() const noexcept {
    return compiler_id().find("MSVC") != std::string_view::npos;
}

bool Build::is_gcc() const noexcept {
    return compiler_id().find("GNU") != std::string_view::npos;
}

bool Build::is_clang() const noexcept {
    std::string_view id = compiler_id();
    return id.find("Clang") != std::string_view::npos || id.find("LLVM") != std::string_view::npos;
}

bool Build::is_apple_clang() const noexcept {
    return compiler_id().find("AppleClang") != std::string_view::npos;
}

std::string_view Build::build_type() const noexcept {
    return detail::autogen::build_type;
}

bool Build::is_debug_build() const noexcept {
    return build_type() == "DEBUG";
}

bool Build::is_release_build() const noexcept {
    return build_type() == "RELEASE";
}

bool Build::is_relwithdebinfo_build() const noexcept {
    return build_type() == "RELWITHDEBINFO";
}

bool Build::is_minsizerel_build() const noexcept {
    return build_type() == "MINSIZEREL";
}

int Build::cxx_standard() const noexcept {
    return detail::autogen::build_cxx_standard;
}

bool Build::cxx_at_least(int standard) const noexcept {
    return cxx_standard() >= standard;
}

std::string_view Build::system_name() const noexcept {
    return detail::autogen::build_system_name;
}

std::string_view Build::system_processor() const noexcept {
    return detail::autogen::build_system_processor;
}

bool Build::is_linux() const noexcept {
    return system_name() == "Linux";
}

bool Build::is_windows() const noexcept {
    return system_name() == "Windows";
}

bool Build::is_macos() const noexcept {
    return system_name() == "Darwin";
}

std::string_view Build::build_date() const noexcept {
    return detail::autogen::build_date;
}

std::string_view Build::build_time() const noexcept {
    return detail::autogen::build_time;
}

std::string_view Build::build_timestamp() const noexcept {
    // Format: "YYYY-MM-DD HH:MM:SS"
    // Combine build_date and build_time directly
    std::string_view date = build_date(); // "YYYY-MM-DD"
    std::string_view time = build_time(); // "HH:MM:SS"

    // Format: "YYYY-MM-DD HH:MM:SS" (19 chars)
    g_timestamp_buffer[0] = '\0';
    size_t pos = 0;

    // Copy date
    for (size_t i = 0; i < date.size() && pos < TIMESTAMP_SIZE - 1; ++i) {
        g_timestamp_buffer[pos++] = date[i];
    }
    // Add space
    if (pos < TIMESTAMP_SIZE - 1) {
        g_timestamp_buffer[pos++] = ' ';
    }
    // Copy time
    for (size_t i = 0; i < time.size() && pos < TIMESTAMP_SIZE - 1; ++i) {
        g_timestamp_buffer[pos++] = time[i];
    }
    g_timestamp_buffer[pos] = '\0';

    return std::string_view(g_timestamp_buffer.data(), pos);
}

// ============================================================
// Features implementation
// ============================================================

Version Features::qt_version() const noexcept {
    return Version{static_cast<uint8_t>(detail::autogen::qt_version_major),
                   static_cast<uint8_t>(detail::autogen::qt_version_minor),
                   static_cast<uint8_t>(detail::autogen::qt_version_patch)};
}

std::string_view Features::qt_version_string() const noexcept {
    return detail::autogen::qt_version;
}

int Features::qt_major() const noexcept {
    return detail::autogen::qt_version_major;
}

int Features::qt_minor() const noexcept {
    return detail::autogen::qt_version_minor;
}

int Features::qt_patch() const noexcept {
    return detail::autogen::qt_version_patch;
}

bool Features::qt_at_least(uint8_t major, uint8_t minor, uint8_t patch) const noexcept {
    return qt_version().is_at_least(Version{major, minor, patch});
}

bool Features::qt_is(uint8_t major, uint8_t minor, uint8_t patch) const noexcept {
    return qt_version() == Version{major, minor, patch};
}

bool Features::is_qt6() const noexcept {
    return qt_major() == 6;
}

// ============================================================
// Global access points implementation
// ============================================================

Project project() noexcept {
    return Project{};
}

Build build() noexcept {
    return Build{};
}

Features features() noexcept {
    return Features{};
}

} // namespace cf::desktop::metainfo
