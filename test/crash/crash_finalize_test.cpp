/**
 * @file    crash_finalize_test.cpp
 * @brief   Unit tests for folding .pending snapshots into .json reports.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/crash_handler.h"
#include "crash_test_util.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

TEST(CrashFinalize, PendingFoldsIntoJsonWithLoggerTail) {
    const auto dir = crash_test::uniqueDir("cfcrash_finalize");

    {
        std::ofstream f(dir / "cfdesktop-999-11.pending");
        f << "CFDESKTOP_CRASH_V1\n";
        f << "signal=11\n";
        f << "signal_name=SIGSEGV\n";
        f << "pid=999\n";
        f << "timestamp=1720000000\n";
        f << "frames=2\n";
        f << "0x7f1000\n";
        f << "0x7f2000\n";
    }
    const auto logger = dir / "app.log";
    {
        std::ofstream f(logger);
        for (int i = 0; i < 60; ++i) {
            f << "log line " << i << "\n";
        }
    }

    cf::crash::CrashHandler::instance().setCrashesDir(dir.string());
    const auto count =
        cf::crash::CrashHandler::instance().finalizePendingReports(logger.string(), 50);

    EXPECT_EQ(count, 1U);
    EXPECT_TRUE(fs::exists(dir / "cfdesktop-999-11.json"));
    EXPECT_FALSE(fs::exists(dir / "cfdesktop-999-11.pending"));

    std::string json;
    {
        // Scope the stream so it closes before remove_all() — Windows refuses
        // to delete a file still held open, unlike Linux.
        std::ifstream in(dir / "cfdesktop-999-11.json");
        json.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }

    EXPECT_NE(json.find("\"signal\": 11"), std::string::npos);
    EXPECT_NE(json.find("\"pid\": 999"), std::string::npos);
    EXPECT_NE(json.find("\"signal_name\": \"SIGSEGV\""), std::string::npos);
    EXPECT_NE(json.find("\"0x7f1000\""), std::string::npos);
    EXPECT_NE(json.find("\"0x7f2000\""), std::string::npos);

    // Tail of 50 from 60 lines => lines 10..59 retained, 0..9 dropped.
    EXPECT_NE(json.find("\"log line 10\""), std::string::npos);
    EXPECT_NE(json.find("\"log line 59\""), std::string::npos);
    EXPECT_EQ(json.find("\"log line 9\""), std::string::npos);

    fs::remove_all(dir);
}

TEST(CrashFinalize, IgnoresFilesWithoutValidHeader) {
    const auto dir = crash_test::uniqueDir("cfcrash_finalize");
    {
        std::ofstream(dir / "garbage.pending") << "not a crash file\n";
    }
    const auto logger = dir / "app.log";
    std::ofstream(logger) << "single line\n";

    cf::crash::CrashHandler::instance().setCrashesDir(dir.string());
    const auto count =
        cf::crash::CrashHandler::instance().finalizePendingReports(logger.string(), 50);

    EXPECT_EQ(count, 0U);
    // Garbage file left untouched (not a recognized snapshot).
    EXPECT_TRUE(fs::exists(dir / "garbage.pending"));
    fs::remove_all(dir);
}
