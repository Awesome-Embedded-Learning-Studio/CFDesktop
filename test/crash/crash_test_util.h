/**
 * @file    crash_test_util.h
 * @brief   Shared helpers for CrashHandler unit tests (unique temp dirs).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#pragma once

#include <filesystem>
#include <string>

#ifdef _WIN32
#    include <process.h>
#else
#    include <unistd.h>
#endif

namespace crash_test {

/// @brief Returns the current process id (portable across Linux/Windows).
inline unsigned long currentPid() {
#ifdef _WIN32
    return static_cast<unsigned long>(_getpid());
#else
    return static_cast<unsigned long>(::getpid());
#endif
}

/// @brief Creates a fresh per-test temp directory tagged with pid + counter.
inline std::filesystem::path uniqueDir(const char* tag) {
    static unsigned counter = 0;
    ++counter;
    auto dir =
        std::filesystem::temp_directory_path() /
        (std::string(tag) + "_" + std::to_string(currentPid()) + "_" + std::to_string(counter));
    std::filesystem::create_directories(dir);
    return dir;
}

} // namespace crash_test
