/**
 * @file    crash_report_test.cpp
 * @brief   Unit tests for CrashReport JSON serialization.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash
 */

#include "cfcrash/crash_report.h"

#include <csignal>
#include <gtest/gtest.h>
#include <string>

TEST(CrashReport, SerializesAllFields) {
    cf::crash::CrashReport r;
    r.timestamp = 1720000000;
    r.pid = 12345;
    r.signal = 11;
    r.signal_name = "SIGSEGV";
    r.raw_frames = {"0x7f0100", "0x7f0200"};
    r.last_logs = {"line a", "line b"};

    const std::string json = r.toJson();

    EXPECT_NE(json.find("\"timestamp\": 1720000000"), std::string::npos);
    EXPECT_NE(json.find("\"pid\": 12345"), std::string::npos);
    EXPECT_NE(json.find("\"signal\": 11"), std::string::npos);
    EXPECT_NE(json.find("\"signal_name\": \"SIGSEGV\""), std::string::npos);
    EXPECT_NE(json.find("\"0x7f0100\""), std::string::npos);
    EXPECT_NE(json.find("\"line a\""), std::string::npos);
}

TEST(CrashReport, EmptyArraysRenderAsBrackets) {
    cf::crash::CrashReport r;
    const std::string json = r.toJson();

    EXPECT_NE(json.find("\"raw_frames\": []"), std::string::npos);
    EXPECT_NE(json.find("\"last_logs\": []"), std::string::npos);
}

TEST(CrashReport, EscapesSpecialCharsInLogs) {
    cf::crash::CrashReport r;
    r.last_logs = {"has \"quote\" and \\slash and \ttab"};
    const std::string json = r.toJson();

    EXPECT_NE(json.find("\\\"quote\\\""), std::string::npos);
    EXPECT_NE(json.find("\\\\slash"), std::string::npos);
    EXPECT_NE(json.find("\\t"), std::string::npos);
}

TEST(CrashReport, SignalNameMapping) {
    EXPECT_EQ(cf::crash::signalName(SIGSEGV), "SIGSEGV");
    EXPECT_EQ(cf::crash::signalName(SIGABRT), "SIGABRT");
    EXPECT_EQ(cf::crash::signalName(SIGFPE), "SIGFPE");
    EXPECT_EQ(cf::crash::signalName(SIGILL), "SIGILL");
    EXPECT_EQ(cf::crash::signalName(99999), "UNKNOWN");
}
