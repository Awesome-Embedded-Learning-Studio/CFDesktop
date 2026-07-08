/**
 * @file    crash_retention_test.cpp
 * @brief   Unit tests for the crash report retention policy.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/crash_handler.h"
#include "crash_test_util.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

namespace {
/// @brief Writes an empty .json and stamps its mtime to now + age seconds.
void touchJsonWithMtime(const fs::path& path, int age_seconds) {
    std::ofstream(path) << "{}";
    const auto tp = fs::file_time_type::clock::now() + std::chrono::seconds(age_seconds);
    fs::last_write_time(path, tp);
}
} // namespace

TEST(CrashRetention, PrunesOldestBeyondCap) {
    const auto dir = crash_test::uniqueDir("cfcrash_retention");
    // 25 reports, i=0 oldest .. i=24 newest.
    for (int i = 0; i < 25; ++i) {
        touchJsonWithMtime(dir / (std::to_string(i) + ".json"), i);
    }

    cf::crash::CrashHandler::instance().setCrashesDir(dir.string());
    const auto removed = cf::crash::CrashHandler::instance().pruneReports(20);

    EXPECT_EQ(removed, 5U);

    std::size_t remaining = 0;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".json") {
            ++remaining;
        }
    }
    EXPECT_EQ(remaining, 20U);

    // Oldest five removed; newest twenty kept.
    EXPECT_FALSE(fs::exists(dir / "0.json"));
    EXPECT_FALSE(fs::exists(dir / "4.json"));
    EXPECT_TRUE(fs::exists(dir / "5.json"));
    EXPECT_TRUE(fs::exists(dir / "24.json"));

    fs::remove_all(dir);
}

TEST(CrashRetention, KeepsAllWhenUnderCap) {
    const auto dir = crash_test::uniqueDir("cfcrash_retention");
    for (int i = 0; i < 5; ++i) {
        touchJsonWithMtime(dir / (std::to_string(i) + ".json"), i);
    }

    cf::crash::CrashHandler::instance().setCrashesDir(dir.string());
    const auto removed = cf::crash::CrashHandler::instance().pruneReports(20);

    EXPECT_EQ(removed, 0U);
    fs::remove_all(dir);
}
