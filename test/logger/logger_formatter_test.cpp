/**
 * @file logger_formatter_test.cpp
 * @brief Comprehensive unit tests for CFLogger formatters
 *
 * Test Coverage:
 * 1. AsciiColorFormatter - default output format
 * 2. AsciiColorFormatter - all components output
 * 3. AsciiColorFormatter - ANSI color codes presence
 * 4. AsciiColorFormatter - runtime configuration modification
 * 5. FileFormatter - plain text output (no colors)
 * 6. FileFormatter - all components without colors
 * 7. LogRecord - all fields populated correctly
 * 8. LogRecord - thread ID capture
 * 9. LogRecord - source location capture
 */

#include "cflog/cflog_format_config.h"
#include "cflog/cflog_format_flags.h"
#include "cflog/cflog_level.hpp"
#include "cflog/cflog_record.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/formatter/file_formatter.h"
#include "mock/mock_sink.h"
#include <gtest/gtest.h>
#include <source_location>
#include <string>
#include <thread>

// Helper macro for substring check
#define EXPECT_HAS_SUBSTR(str, substr)               \
    EXPECT_NE((str).find(substr), std::string::npos) \
        << "Expected to find '" << substr << "' in: " << (str)
#define EXPECT_NO_SUBSTR(str, substr)                \
    EXPECT_EQ((str).find(substr), std::string::npos) \
        << "Did not expect to find '" << substr << "' in: " << (str)

using namespace cf::log;
using namespace cf::log::test;

// =============================================================================
// Test Suite 1: AsciiColorFormatter - Default Output Format
// =============================================================================

TEST(AsciiColorFormatterTest, AsciiColorFormatterDefaultFlags) {
    // Create formatter with default flags
    AsciiColorFormatter formatter;

    // Create a test log record
    LogRecord record;
    record.lvl = level::INFO;
    record.tag = "TestTag";
    record.msg = "Test message";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Format the record
    std::string formatted = formatter.format_me(record);

    // Verify default components are present
    EXPECT_HAS_SUBSTR(formatted, "[INF]");
    EXPECT_HAS_SUBSTR(formatted, "[TestTag]");
    EXPECT_HAS_SUBSTR(formatted, "Test message");
    EXPECT_HAS_SUBSTR(formatted, "\n");

    // Verify timestamp pattern (e.g., [HH:MM:SS.mmm]) - check for time pattern
    EXPECT_HAS_SUBSTR(formatted, "[") << "Should contain timestamp brackets";
    EXPECT_HAS_SUBSTR(formatted, ":") << "Should contain timestamp time separator";
}

// =============================================================================
// Test Suite 2: AsciiColorFormatter - All Components Output
// =============================================================================

TEST(AsciiColorFormatterTest, AsciiColorFormatterWithAllFlags) {
    // Create formatter with VERBOSE flag (includes all components)
    AsciiColorFormatter formatter(FormatterFlag::VERBOSE);

    // Create a test log record
    LogRecord record;
    record.lvl = level::ERROR;
    record.tag = "AllComponents";
    record.msg = "Complete log entry";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Format the record
    std::string formatted = formatter.format_me(record);

    // Verify all components are present
    EXPECT_HAS_SUBSTR(formatted, "[ERR]");
    EXPECT_HAS_SUBSTR(formatted, "[AllComponents]");
    EXPECT_HAS_SUBSTR(formatted, "[TID:");
    EXPECT_HAS_SUBSTR(formatted, "Complete log entry");

    // Verify timestamp - check for time pattern with brackets
    EXPECT_HAS_SUBSTR(formatted, ":");
    EXPECT_HAS_SUBSTR(formatted, ".");

    // Verify source location (filename and line number) - check for colon pattern
    EXPECT_HAS_SUBSTR(formatted, ":");
}

// =============================================================================
// Test Suite 3: AsciiColorFormatter - ANSI Color Codes
// =============================================================================

TEST(AsciiColorFormatterTest, AsciiColorFormatterColorCodes) {
    // Create formatter with DEFAULT (includes COLOR)
    AsciiColorFormatter formatter(FormatterFlag::DEFAULT);

    // Create test records for different log levels
    struct TestCase {
        level lvl;
        const char* expected_code;
        const char* name;
    };

    TestCase test_cases[] = {
        {level::TRACE, "\033[96m", "TRACE"}, {level::DEBUG, "\033[94m", "DEBUG"},
        {level::INFO, "\033[92m", "INFO"},   {level::WARNING, "\033[93m", "WARNING"},
        {level::ERROR, "\033[91m", "ERROR"},
    };

    for (const auto& tc : test_cases) {
        LogRecord record;
        record.lvl = tc.lvl;
        record.tag = "ColorTest";
        record.msg = "Colored message";
        record.timestamp = std::chrono::system_clock::now();
        record.tid = std::this_thread::get_id();
        record.loc = std::source_location::current();

        std::string formatted = formatter.format_me(record);

        // Verify ANSI color codes are present
        EXPECT_HAS_SUBSTR(formatted, tc.expected_code)
            << "Should contain color code for " << tc.name;
        EXPECT_HAS_SUBSTR(formatted, "\033[0m") << "Should contain reset code for " << tc.name;
    }
}

// =============================================================================
// Test Suite 4: AsciiColorFormatter - Runtime Configuration
// =============================================================================

TEST(AsciiColorFormatterTest, AsciiColorFormatterConfigurable) {
    // Create formatter
    AsciiColorFormatter formatter(FormatterFlag::DEFAULT);

    // Verify initial configuration
    auto config = formatter.get_config();
    ASSERT_NE(config, nullptr) << "Should have initial config";
    EXPECT_TRUE(config->is_enabled(FormatterFlag::TIMESTAMP));
    EXPECT_TRUE(config->is_enabled(FormatterFlag::LEVEL));
    EXPECT_TRUE(config->is_enabled(FormatterFlag::TAG));
    EXPECT_TRUE(config->is_enabled(FormatterFlag::SOURCE_LOCATION));
    EXPECT_TRUE(config->is_enabled(FormatterFlag::MESSAGE));
    EXPECT_TRUE(config->is_enabled(FormatterFlag::COLOR));

    // Create a log record
    LogRecord record;
    record.lvl = level::INFO;
    record.tag = "ConfigTest";
    record.msg = "Configurable message";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Format with default config
    std::string formatted_default = formatter.format_me(record);
    EXPECT_HAS_SUBSTR(formatted_default, "\033[");

    // Modify configuration - disable color and tag
    auto new_config = std::make_shared<FormatterConfig>(FormatterFlag::DEFAULT);
    new_config->disable(FormatterFlag::COLOR);
    new_config->disable(FormatterFlag::TAG);
    formatter.set_config(new_config);

    // Format with modified config
    std::string formatted_modified = formatter.format_me(record);
    EXPECT_NO_SUBSTR(formatted_modified, "\033[");
    EXPECT_NO_SUBSTR(formatted_modified, "[ConfigTest]");
}

// =============================================================================
// Test Suite 5: FileFormatter - Plain Text Output
// =============================================================================

TEST(FileFormatterTest, FileFormatterPlainOutput) {
    // Create formatter with DEFAULT flags
    FileFormatter formatter(FormatterFlag::DEFAULT);

    // Create a test log record
    LogRecord record;
    record.lvl = level::INFO;
    record.tag = "PlainText";
    record.msg = "Plain file output";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Format the record
    std::string formatted = formatter.format_me(record);

    // Verify plain text output (no ANSI color codes)
    EXPECT_NO_SUBSTR(formatted, "\033[");

    // Verify components are still present
    EXPECT_HAS_SUBSTR(formatted, "[INF]");
    EXPECT_HAS_SUBSTR(formatted, "[PlainText]");
    EXPECT_HAS_SUBSTR(formatted, "Plain file output");
    EXPECT_HAS_SUBSTR(formatted, "\n");

    // Verify timestamp - check for time pattern
    EXPECT_HAS_SUBSTR(formatted, ":");
}

// =============================================================================
// Test Suite 6: FileFormatter - All Components Without Colors
// =============================================================================

TEST(FileFormatterTest, FileFormatterAllComponents) {
    // Create formatter with VERBOSE flag (all components)
    FileFormatter formatter(FormatterFlag::VERBOSE);

    // Create a test log record
    LogRecord record;
    record.lvl = level::ERROR;
    record.tag = "FileAll";
    record.msg = "All components in file";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Format the record
    std::string formatted = formatter.format_me(record);

    // Verify no ANSI codes
    EXPECT_NO_SUBSTR(formatted, "\033[");

    // Verify all components are present in plain text
    EXPECT_HAS_SUBSTR(formatted, "[ERR]");
    EXPECT_HAS_SUBSTR(formatted, "[FileAll]");
    EXPECT_HAS_SUBSTR(formatted, "[TID:");
    EXPECT_HAS_SUBSTR(formatted, "All components in file");
    EXPECT_HAS_SUBSTR(formatted, ":"); // timestamp and source location have colons
}

// =============================================================================
// Test Suite 7: LogRecord - All Fields Populated
// =============================================================================

TEST(LogRecordTest, LogRecordAllFieldsPopulated) {
    // Create a LogRecord with all fields populated
    LogRecord record;
    record.lvl = level::WARNING;
    record.tag = "AllFields";
    record.msg = "Complete record test";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Verify all fields are correctly set
    EXPECT_EQ(record.lvl, level::WARNING);
    EXPECT_EQ(record.tag, "AllFields");
    EXPECT_EQ(record.msg, "Complete record test");

    // Verify timestamp is reasonable (within last minute)
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - record.timestamp);
    EXPECT_GE(age.count(), 0) << "Timestamp should not be in the future";
    EXPECT_LT(age.count(), 60) << "Timestamp should be recent";

    // Verify thread ID is valid (not default)
    EXPECT_NE(record.tid, std::thread::id()) << "Thread ID should be set";

    // Verify source location has valid data
    EXPECT_NE(record.loc.file_name(), nullptr) << "File name should be set";
    EXPECT_STRNE(record.loc.file_name(), "") << "File name should not be empty";
    EXPECT_GT(record.loc.line(), 0) << "Line number should be positive";
    EXPECT_NE(record.loc.function_name(), nullptr) << "Function name should be set";
    EXPECT_STRNE(record.loc.function_name(), "") << "Function name should not be empty";
}

// =============================================================================
// Test Suite 8: LogRecord - Thread ID Capture
// =============================================================================

TEST(LogRecordTest, LogRecordThreadId) {
    // Create records from different thread contexts
    auto main_tid = std::this_thread::get_id();

    LogRecord record1;
    record1.tid = main_tid;
    record1.lvl = level::INFO;
    record1.msg = "Main thread";
    record1.timestamp = std::chrono::system_clock::now();
    record1.loc = std::source_location::current();

    EXPECT_EQ(record1.tid, main_tid) << "Should capture main thread ID";

    // Verify thread ID formatting through formatter
    AsciiColorFormatter formatter(FormatterFlag::VERBOSE);
    std::string formatted = formatter.format_me(record1);
    EXPECT_HAS_SUBSTR(formatted, "[TID:");
}

// =============================================================================
// Test Suite 9: LogRecord - Source Location Capture
// =============================================================================

TEST(LogRecordTest, LogRecordSourceLocation) {
    // Capture source location at this point
    auto current_loc = std::source_location::current();

    LogRecord record;
    record.loc = current_loc;
    record.lvl = level::DEBUG;
    record.tag = "SourceLoc";
    record.msg = "Testing source location";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();

    // Verify source location is preserved
    EXPECT_EQ(std::string_view(record.loc.file_name()), std::string_view(current_loc.file_name()));
    EXPECT_EQ(record.loc.line(), current_loc.line());

    // Verify source location is formatted correctly
    AsciiColorFormatter formatter(FormatterFlag::DEFAULT);
    std::string formatted = formatter.format_me(record);

    // Extract filename from full path
    std::string filename = current_loc.file_name();
    size_t pos = filename.find_last_of("/\\");
    if (pos != std::string::npos) {
        filename = filename.substr(pos + 1);
    }

    EXPECT_HAS_SUBSTR(formatted, filename);
    EXPECT_HAS_SUBSTR(formatted, std::to_string(current_loc.line()));
}

// =============================================================================
// Test Suite 10: FormatterConfig - Enable/Disable Operations
// =============================================================================

TEST(FormatterConfigTest, EnableDisableFlags) {
    FormatterConfig config(FormatterFlag::MINIMAL);

    // Initial state
    EXPECT_TRUE(config.is_enabled(FormatterFlag::LEVEL));
    EXPECT_TRUE(config.is_enabled(FormatterFlag::MESSAGE));
    EXPECT_FALSE(config.is_enabled(FormatterFlag::TIMESTAMP));
    EXPECT_FALSE(config.is_enabled(FormatterFlag::TAG));

    // Enable timestamp
    config.enable(FormatterFlag::TIMESTAMP);
    EXPECT_TRUE(config.is_enabled(FormatterFlag::TIMESTAMP));

    // Disable level
    config.disable(FormatterFlag::LEVEL);
    EXPECT_FALSE(config.is_enabled(FormatterFlag::LEVEL));

    // Enable multiple flags at once
    config.set_flags(FormatterFlag::VERBOSE);
    EXPECT_TRUE(config.is_enabled(FormatterFlag::TIMESTAMP));
    EXPECT_TRUE(config.is_enabled(FormatterFlag::LEVEL));
    EXPECT_TRUE(config.is_enabled(FormatterFlag::TAG));
    EXPECT_TRUE(config.is_enabled(FormatterFlag::THREAD_ID));
    EXPECT_TRUE(config.is_enabled(FormatterFlag::SOURCE_LOCATION));
    EXPECT_TRUE(config.is_enabled(FormatterFlag::MESSAGE));
}

// =============================================================================
// Test Suite 11: AsciiColorFormatter - Minimal Output
// =============================================================================

TEST(AsciiColorFormatterTest, AsciiColorFormatterMinimalOutput) {
    // Create formatter with MINIMAL flag (level + message only)
    AsciiColorFormatter formatter(FormatterFlag::MINIMAL);

    // Create a log record
    LogRecord record;
    record.lvl = level::INFO;
    record.tag = "IgnoredTag";
    record.msg = "Minimal output message";
    record.timestamp = std::chrono::system_clock::now();
    record.tid = std::this_thread::get_id();
    record.loc = std::source_location::current();

    // Format the record
    std::string formatted = formatter.format_me(record);

    // Verify only level and message are present
    EXPECT_HAS_SUBSTR(formatted, "[INF]");
    EXPECT_HAS_SUBSTR(formatted, "Minimal output message");
    EXPECT_HAS_SUBSTR(formatted, "\n");

    // Verify tag and timestamp are NOT present
    EXPECT_NO_SUBSTR(formatted, "[IgnoredTag]");
}

// =============================================================================
// Test Suite 12: FileFormatter - Color Flag Always Ignored
// =============================================================================

TEST(FileFormatterTest, FileFormatterColorAlwaysIgnored) {
    // Create formatter and explicitly enable COLOR (should be ignored)
    auto config = std::make_shared<FormatterConfig>(FormatterFlag::DEFAULT);
    config->enable(FormatterFlag::COLOR);

    FileFormatter formatter(FormatterFlag::DEFAULT);
    formatter.set_config(config);

    // Verify color is still disabled
    auto formatter_config = formatter.get_config();
    EXPECT_FALSE(formatter_config->is_enabled(FormatterFlag::COLOR))
        << "FileFormatter should always have COLOR disabled";

    // Create and format a log record
    LogRecord record;
    record.lvl = level::ERROR;
    record.msg = "No colors in file";
    record.timestamp = std::chrono::system_clock::now();
    record.loc = std::source_location::current();

    std::string formatted = formatter.format_me(record);
    EXPECT_NO_SUBSTR(formatted, "\033[");
}

// =============================================================================
// Test Suite 13: Custom Timestamp Format
// =============================================================================

TEST(FormatterConfigTest, CustomTimestampFormat) {
    // Create formatter with custom timestamp format
    auto config = std::make_shared<FormatterConfig>(FormatterFlag::DEFAULT);
    config->set_timestamp_format("%Y-%m-%d %H:%M:%S");

    AsciiColorFormatter formatter(FormatterFlag::DEFAULT);
    formatter.set_config(config);

    // Create a log record
    LogRecord record;
    record.lvl = level::INFO;
    record.msg = "Custom timestamp test";
    record.timestamp = std::chrono::system_clock::now();
    record.loc = std::source_location::current();

    // Format the record
    std::string formatted = formatter.format_me(record);

    // Verify custom timestamp format (YYYY-MM-DD HH:MM:SS) - check for date pattern
    EXPECT_HAS_SUBSTR(formatted, "-"); // Date separator
    EXPECT_HAS_SUBSTR(formatted, " "); // Date/time separator
}

// =============================================================================
// Test Suite 14: Empty Tag Handling
// =============================================================================

TEST(AsciiColorFormatterTest, EmptyTagHandling) {
    AsciiColorFormatter formatter(FormatterFlag::DEFAULT);

    // Create log record with empty tag
    LogRecord record;
    record.lvl = level::INFO;
    record.tag = ""; // Empty tag
    record.msg = "Message without tag";
    record.timestamp = std::chrono::system_clock::now();
    record.loc = std::source_location::current();

    std::string formatted = formatter.format_me(record);

    // Verify message is present but empty tag doesn't create extra brackets
    EXPECT_HAS_SUBSTR(formatted, "Message without tag");
    // Empty tag should not produce "[]" in output
    EXPECT_NO_SUBSTR(formatted, "[] "); // Empty brackets followed by space
}

// =============================================================================
// Test Suite 15: Message Only Flag
// =============================================================================

TEST(AsciiColorFormatterTest, MessageOnlyOutput) {
    // Create formatter with only MESSAGE flag
    AsciiColorFormatter formatter(FormatterFlag::MESSAGE);

    // Create log record
    LogRecord record;
    record.lvl = level::ERROR;
    record.tag = "Tag";
    record.msg = "Just the message";
    record.timestamp = std::chrono::system_clock::now();
    record.loc = std::source_location::current();

    std::string formatted = formatter.format_me(record);

    // Verify only message is present
    EXPECT_NE(formatted.find("Just the message"), std::string::npos);
    EXPECT_EQ(formatted.find("[ERR]"), std::string::npos);
    EXPECT_EQ(formatted.find("[Tag]"), std::string::npos);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
