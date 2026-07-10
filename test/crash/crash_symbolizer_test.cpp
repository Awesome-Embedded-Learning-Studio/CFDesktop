/**
 * @file    crash_symbolizer_test.cpp
 * @brief   Unit tests for addr2line output parsing.
 *
 * Exercises parseAddr2LineOutput() (pure string parse, no subprocess) so the
 * symbolizer's line-pairing / file:line splitting is covered without depending
 * on addr2line being installed.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/symbolizer.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(CrashSymbolizer, ParsesFunctionAndFileLine) {
    const std::vector<std::string> lines = {
        "cf::crash::CrashHandler::install",
        "/home/cf/crash_handler.cpp:120",
        "main",
        "/home/cf/main.cpp:27",
    };
    const auto out = cf::crash::parseAddr2LineOutput(lines, 2);
    ASSERT_EQ(out.size(), 2u);
    EXPECT_EQ(out[0].function, "cf::crash::CrashHandler::install");
    EXPECT_EQ(out[0].file, "/home/cf/crash_handler.cpp");
    EXPECT_EQ(out[0].line, "120");
    EXPECT_EQ(out[1].function, "main");
    EXPECT_EQ(out[1].line, "27");
}

TEST(CrashSymbolizer, HandlesUnresolvedAddr) {
    // addr2line emits "??\n??:0" for an address it cannot resolve.
    const std::vector<std::string> lines = {"??", "??:0"};
    const auto out = cf::crash::parseAddr2LineOutput(lines, 1);
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(out[0].function, "??");
    EXPECT_EQ(out[0].file, "??");
    EXPECT_EQ(out[0].line, "0");
}

TEST(CrashSymbolizer, TruncatesWhenLinesRunOut) {
    // One line is not enough for even a single frame (need 2 lines/frame).
    const std::vector<std::string> lines = {"only_one"};
    const auto out = cf::crash::parseAddr2LineOutput(lines, 3);
    EXPECT_TRUE(out.empty());
}

TEST(CrashSymbolizer, HandlesFewerFramesThanRequested) {
    // 2 frames requested, but only one function/file:line pair available.
    const std::vector<std::string> lines = {"foo", "bar.cpp:5"};
    const auto out = cf::crash::parseAddr2LineOutput(lines, 2);
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(out[0].function, "foo");
    EXPECT_EQ(out[0].line, "5");
}

TEST(CrashSymbolizer, EmptyInput) {
    const std::vector<std::string> lines;
    const auto out = cf::crash::parseAddr2LineOutput(lines, 0);
    EXPECT_TRUE(out.empty());
}
