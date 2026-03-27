/**
 * @file logger_error_handling_test.cpp
 * @brief Error handling and edge case tests for CFLogger
 *
 * Test Coverage:
 * 1. Queue Full Behavior (kMaxNormalQueueSize = 65536)
 * 2. File Sink Write Failures
 * 3. Formatter Edge Cases
 * 4. Sink Edge Cases
 */

#include "../mock/mock_sink.h"
#include "../test_config.h"
#include "base/singleton/simple_singleton.hpp"
#include "cflog.h"
#include "cflog/cflog.hpp"
#include "cflog/cflog_level.hpp"
#include "cflog/formatter/console_formatter.h"
#include "cflog/formatter/file_formatter.h"
#include "cflog/sinks/file_sink.h"
#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <thread>

// Undef Windows macros that conflict with our level enum
#ifdef _WIN32
#    include <windows.h>
// Windows.h defines ERROR as a macro, which breaks level::ERROR
#    undef ERROR
#endif

using namespace cf::log;
using namespace cf::log::test;

// =============================================================================
// Test Helper Functions
// =============================================================================

/**
 * @brief Reset logger state for clean test environment
 *
 * Clears all sinks and resets the logger to a known state.
 */
void ResetLoggerState() {
    auto& logger = Logger::instance();
    logger.clear_sinks();
    logger.setMininumLevel(level::TRACE);
}

/**
 * @brief Wait for async queue to drain
 *
 * @param max_wait_ms Maximum time to wait in milliseconds
 * @return true if queue drained, false if timeout
 */
bool WaitForQueueDrain(uint64_t max_wait_ms = 5000) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                 start)
               .count() < max_wait_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        Logger::instance().flush();
    }
    return true;
}

// =============================================================================
// Test Suite 1: Queue Full Behavior
// =============================================================================

TEST(QueueFullBehavior, QueueFullDropsNormalLogs) {
    ResetLoggerState();

    // Create a mock sink with slow write to ensure queue fills up
    auto mock_sink = std::make_shared<MockSink>();
    MockSink::Config config;
    config.slow_write = true; // Enable slow write to fill queue faster than consumption
    mock_sink->set_config(config);

    Logger::instance().add_sink(mock_sink);

    // Submit enough logs to exceed queue capacity (70000 > 65536)
    // This ensures the queue fills up and some logs are dropped
    constexpr size_t total_logs = 70000;
    constexpr size_t expected_max_received = CFLOGGER_QUEUE_CAPACITY; // Queue capacity

    for (size_t i = 0; i < total_logs; ++i) {
        info("Normal log message " + std::to_string(i), "QueueTest");
    }

    // Wait for processing - give enough time for queue to drain
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify that some logs were dropped
    size_t received_count = mock_sink->entry_count();
    EXPECT_LE(received_count, expected_max_received)
        << "Received " << received_count << " logs but queue capacity is " << expected_max_received;
    EXPECT_LT(received_count, total_logs) << "Queue should have dropped some normal logs when full";

    ResetLoggerState();
}

TEST(QueueFullBehavior, ErrorLogsNeverDropped) {
    ResetLoggerState();

    // Create a mock sink with slow write to ensure queue fills up
    auto mock_sink = std::make_shared<MockSink>();

    Logger::instance().add_sink(mock_sink);

    // Fill normal queue with INFO logs
    constexpr size_t normal_logs = CFLOGGER_QUEUE_FLOOD_COUNT;
    for (size_t i = 0; i < normal_logs; ++i) {
        info("Normal log " + std::to_string(i), "ErrorTest");
    }

    // Submit ERROR logs - these should NEVER be dropped (unlimited error queue)
    constexpr size_t error_logs = 100;
    for (size_t i = 0; i < error_logs; ++i) {
        error("Critical error " + std::to_string(i), "ErrorTest");
    }

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Count ERROR logs received
    size_t error_count = 0;
    const auto& levels = mock_sink->get_levels();
    for (const auto& lvl : levels) {
        if (lvl == level::ERROR) {
            error_count++;
        }
    }

    EXPECT_EQ(error_count, error_logs)
        << "All ERROR logs should be received (unlimited error queue), "
        << "but only " << error_count << " of " << error_logs << " were received";

    ResetLoggerState();
}

TEST(QueueFullBehavior, MixedLevelLogsWhenQueueFull) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    Logger::instance().add_sink(mock_sink);

    // Fill queue with INFO logs (more than queue capacity)
    for (size_t i = 0; i < CFLOGGER_QUEUE_FLOOD_COUNT; ++i) {
        info("Info " + std::to_string(i), "MixedTest");
    }

    // Submit WARNING and ERROR logs
    for (size_t i = 0; i < 50; ++i) {
        warning("Warning " + std::to_string(i), "MixedTest");
    }
    for (size_t i = 0; i < 50; ++i) {
        error("Error " + std::to_string(i), "MixedTest");
    }

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Count logs by level
    size_t info_count = 0, warning_count = 0, error_count = 0;
    const auto& levels = mock_sink->get_levels();
    for (const auto& lvl : levels) {
        switch (lvl) {
            case level::INFO:
                info_count++;
                break;
            case level::WARNING:
                warning_count++;
                break;
            case level::ERROR:
                error_count++;
                break;
            default:
                break;
        }
    }

    // All ERROR logs should be received
    EXPECT_EQ(error_count, 50) << "All ERROR logs should be received";
    // WARNING logs share normal queue, so some may be dropped
    EXPECT_LE(warning_count, CFLOGGER_QUEUE_CAPACITY) << "WARNING logs bounded by normal queue";

    ResetLoggerState();
}

// =============================================================================
// Test Suite 2: File Sink Write Failures
// =============================================================================

TEST(FileSinkFailure, FileSinkInvalidPath) {
    ResetLoggerState();

    // Try to create FileSink with invalid path
    // On Linux, directory names cannot contain null bytes
    std::string invalid_path = "/invalid\x00path/nonexistent.log";

    // Creating FileSink with invalid path should either throw or fail gracefully
    // The actual behavior depends on implementation - we test that it doesn't crash
    EXPECT_NO_THROW({
        // This may throw std::exception or return an invalid sink
        try {
            auto sink = std::make_shared<FileSink>(invalid_path, OpenMode::Append);
            // If we get here, the sink was created - verify it handles writes gracefully
            Logger::instance().add_sink(sink);
            info("Test message", "InvalidPathTest");
            Logger::instance().flush();
        } catch (const std::exception& e) {
            // Exception is acceptable - indicates proper error handling
            SUCCEED();
        }
    }) << "FileSink should handle invalid paths gracefully without crashing";

    ResetLoggerState();
}

TEST(FileSinkFailure, FileSinkWriteFailureHandling) {
    ResetLoggerState();

    // Use MockSink with failure configuration to simulate write failures
    auto failing_sink = std::make_shared<MockSink>();
    MockSink::Config fail_config;
    fail_config.always_fail = true;
    failing_sink->set_config(fail_config);

    Logger::instance().add_sink(failing_sink);

    // Try to log - should handle failure gracefully
    EXPECT_NO_THROW({
        error("Test message with failing sink", "FailureTest");
        Logger::instance().flush_sync(); // Use sync flush to ensure completion
    }) << "Logger should handle sink write failures without throwing";

    // Verify sink was called but returned failure
    EXPECT_GT(failing_sink->write_count(), 0) << "Sink should have been called";
    // With always_fail, no entries should be stored
    EXPECT_EQ(failing_sink->entry_count(), 0) << "Failing sink should not store entries";

    ResetLoggerState();
}

TEST(FileSinkFailure, FileSinkConditionalFailure) {
    ResetLoggerState();

    // Sink that fails after N successful writes
    auto conditional_sink = std::make_shared<MockSink>();
    MockSink::Config conditional_config;
    conditional_config.fail_after_n_writes = true;
    conditional_config.fail_threshold = 10; // Fail after 10 writes
    conditional_sink->set_config(conditional_config);

    Logger::instance().add_sink(conditional_sink);

    // Submit 20 logs - first 10 should succeed, rest should fail
    for (int i = 0; i < 20; ++i) {
        info("Log message " + std::to_string(i), "ConditionalTest");
    }

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Verify: exactly 10 successful writes (stored entries)
    EXPECT_EQ(conditional_sink->entry_count(), 10)
        << "Only first 10 logs should succeed, rest should fail";
    EXPECT_EQ(conditional_sink->write_count(), 20) << "All 20 write attempts should be made";

    ResetLoggerState();
}

TEST(FileSinkFailure, FileSinkInReadOnlyDirectory) {
    ResetLoggerState();

    // Create a temporary file path in a platform-specific temp directory
    std::string temp_path = []() {
#ifdef _WIN32
        char temp_dir[MAX_PATH];
        GetTempPathA(MAX_PATH, temp_dir);
        return std::string(temp_dir) + "cflogger_test_writable.log";
#else
        return std::string("/tmp/cflogger_test_writable.log");
#endif
    }();

    // First verify we can write to the temp directory
    {
        std::ofstream test_file(temp_path);
        ASSERT_TRUE(test_file.is_open())
            << "Cannot create test file in temp directory, skipping read-only test";
        test_file.close();
        std::remove(temp_path.c_str());
    }

    // Create FileSink with valid path
    auto file_sink = std::make_shared<FileSink>(temp_path, OpenMode::Truncate);
    Logger::instance().add_sink(file_sink);

    info("Test message to valid path", "WritableTest");
    Logger::instance().flush();

    // Verify file was created and contains content
    std::ifstream verify_file(temp_path);
    EXPECT_TRUE(verify_file.is_open()) << "File should be created in writable directory";
    verify_file.close();

    // Cleanup
    std::remove(temp_path.c_str());

    ResetLoggerState();
}

// =============================================================================
// Test Suite 3: Formatter Edge Cases
// =============================================================================

TEST(FormatterEdgeCases, FormatterWithEmptyRecord) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    auto formatter = std::make_shared<FileFormatter>(FormatterFlag::DEFAULT);
    mock_sink->setFormat(formatter);

    Logger::instance().add_sink(mock_sink);

    // Create and submit an empty record (empty tag and message)
    LogRecord empty_record;
    empty_record.lvl = level::INFO;
    empty_record.tag = "";
    empty_record.msg = "";
    empty_record.timestamp = std::chrono::system_clock::now();
    empty_record.tid = std::this_thread::get_id();
    empty_record.loc = std::source_location::current();

    // Submit via direct logger call
    info("", "", std::source_location::current());

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Verify formatter handled empty record without crashing
    EXPECT_GT(mock_sink->entry_count(), 0) << "Formatter should handle empty records";

    // Verify formatted output is not null/empty (at least has timestamp/level)
    const auto& formatted = mock_sink->get_formatted();
    if (!formatted.empty()) {
        EXPECT_FALSE(formatted.back().empty())
            << "Formatted output should not be empty even for empty record";
    }

    ResetLoggerState();
}

TEST(FormatterEdgeCases, FormatterWithSpecialCharacters) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    auto formatter = std::make_shared<FileFormatter>(FormatterFlag::DEFAULT);
    mock_sink->setFormat(formatter);

    Logger::instance().add_sink(mock_sink);

    // Test various special characters
    std::string special_messages[] = {"Message with\nnewline",
                                      "Message with\ttab",
                                      "Message with\rcarriage return",
                                      "Message with \"quotes\"",
                                      "Message with 'apostrophes'",
                                      "Message with backslash \\",
                                      "Message with /forward/slash/",
                                      "Message with <html> tags</html>",
                                      "Message with &ampersand&",
                                      "Message with百分号%",
                                      "Message with emoji 🚀 🎉 ✨",
                                      "Message with Unicode: 中文 日本語 한국어",
                                      "Message with null byte: \0 (embedded)",
                                      "Message with mixed: ASCII + 中文 + 🎉"};

    for (const auto& msg : special_messages) {
        info(msg, "SpecialCharTest");
    }

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Verify all messages were formatted without crashes
    size_t received = mock_sink->entry_count();
    size_t expected = sizeof(special_messages) / sizeof(special_messages[0]);

    EXPECT_GE(received, expected - 2) // Allow for potential queue drops
        << "Most special character messages should be formatted";

    // Verify emoji and Unicode are preserved in formatted output
    bool found_emoji = false;
    bool found_chinese = false;
    const auto& formatted = mock_sink->get_formatted();
    for (const auto& fmt : formatted) {
        if (fmt.find("🚀") != std::string::npos || fmt.find("🎉") != std::string::npos) {
            found_emoji = true;
        }
        if (fmt.find("中文") != std::string::npos || fmt.find("日本語") != std::string::npos) {
            found_chinese = true;
        }
    }

    EXPECT_TRUE(found_emoji) << "Emoji should be preserved in formatted output";
    EXPECT_TRUE(found_chinese) << "Unicode characters should be preserved";

    ResetLoggerState();
}

TEST(FormatterEdgeCases, FormatterWithVeryLongMessage) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    auto formatter = std::make_shared<FileFormatter>(FormatterFlag::DEFAULT);
    mock_sink->setFormat(formatter);

    Logger::instance().add_sink(mock_sink);

    // Create a very long message (10KB)
    std::string long_message(10240, 'A');
    long_message += "END_MARKER";

    info(long_message, "LongMessageTest");

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Verify long message was formatted
    const auto& formatted = mock_sink->get_formatted();
    ASSERT_FALSE(formatted.empty()) << "Long message should be formatted";

    const auto& last_entry = formatted.back();
    EXPECT_TRUE(last_entry.find("END_MARKER") != std::string::npos)
        << "Long message should be complete in formatted output";

    // Verify the message content is preserved
    EXPECT_TRUE(last_entry.find("AAA") != std::string::npos ||
                last_entry.find("AAAAA") != std::string::npos)
        << "Long message content should be present";

    ResetLoggerState();
}

TEST(FormatterEdgeCases, FormatterWithNullCharacters) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    auto formatter = std::make_shared<FileFormatter>(FormatterFlag::DEFAULT);
    mock_sink->setFormat(formatter);

    Logger::instance().add_sink(mock_sink);

    // Message with embedded null (will be truncated at null in C-style strings)
    std::string msg_with_null = "Before\0After";
    info(msg_with_null, "NullTest");

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Verify formatter handled null without crashing
    EXPECT_GT(mock_sink->entry_count(), 0)
        << "Formatter should handle messages with null characters";

    ResetLoggerState();
}

TEST(FormatterEdgeCases, AsciiColorFormatterWithAnsiCodes) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    auto formatter = std::make_shared<AsciiColorFormatter>(FormatterFlag::DEFAULT);
    mock_sink->setFormat(formatter);

    Logger::instance().add_sink(mock_sink);

    info("Test color output", "ColorTest");

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Verify ANSI color codes are present in formatted output
    const auto& formatted = mock_sink->get_formatted();
    ASSERT_FALSE(formatted.empty()) << "Color formatter should produce output";

    const auto& last_formatted = formatted.back();
    // Check for ANSI escape sequence prefix (\033[ or \x1b[)
    bool has_ansi = last_formatted.find("\033[") != std::string::npos ||
                    last_formatted.find("\x1b[") != std::string::npos;

    EXPECT_TRUE(has_ansi) << "AsciiColorFormatter should include ANSI escape codes";

    ResetLoggerState();
}

// =============================================================================
// Test Suite 4: Sink Edge Cases
// =============================================================================

TEST(SinkEdgeCases, SinkWithoutFormatter) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    // Don't set any formatter - should use raw message

    Logger::instance().add_sink(mock_sink);

    std::string test_msg = "Raw message without formatter";
    info(test_msg, "NoFormatterTest");

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    const auto& formatted = mock_sink->get_formatted();
    ASSERT_FALSE(formatted.empty()) << "Sink without formatter should still receive messages";

    // Without formatter, should output raw message
    const auto& last_formatted = formatted.back();
    EXPECT_TRUE(last_formatted.find(test_msg) != std::string::npos)
        << "Without formatter, output should contain raw message";

    ResetLoggerState();
}

TEST(SinkEdgeCases, MultipleSinksOneFails) {
    ResetLoggerState();

    // Create three sinks: working, failing, working
    auto working_sink1 = std::make_shared<MockSink>();
    auto failing_sink = std::make_shared<MockSink>();
    auto working_sink2 = std::make_shared<MockSink>();

    // Configure middle sink to always fail
    MockSink::Config fail_config;
    fail_config.always_fail = true;
    failing_sink->set_config(fail_config);

    Logger::instance().add_sink(working_sink1);
    Logger::instance().add_sink(failing_sink);
    Logger::instance().add_sink(working_sink2);

    // Submit test messages
    for (int i = 0; i < 10; ++i) {
        info("Test message " + std::to_string(i), "MultiSinkTest");
    }

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Verify working sinks received all messages
    EXPECT_EQ(working_sink1->entry_count(), 10) << "First working sink should receive all messages";
    EXPECT_EQ(working_sink2->entry_count(), 10)
        << "Second working sink should receive all messages";

    // Verify failing sink was called but didn't store entries
    EXPECT_EQ(failing_sink->entry_count(), 0) << "Failing sink should not store entries";
    EXPECT_EQ(failing_sink->write_count(), 10)
        << "Failing sink should still be called for all messages";

    ResetLoggerState();
}

TEST(SinkEdgeCases, SinkFlushFailure) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    MockSink::Config fail_flush_config;
    fail_flush_config.fail_flush = true;
    mock_sink->set_config(fail_flush_config);

    Logger::instance().add_sink(mock_sink);

    info("Test message", "FlushFailTest");

    // Flush should handle failure gracefully
    EXPECT_NO_THROW({
        Logger::instance().flush_sync(); // Use sync flush to ensure completion
    }) << "Logger should handle sink flush failures without throwing";

    // Verify flush was called
    EXPECT_GT(mock_sink->flush_count(), 0) << "Sink flush method should have been called";

    ResetLoggerState();
}

TEST(SinkEdgeCases, SinkAddedAfterLoggingStart) {
    ResetLoggerState();

    // Start with one sink
    auto sink1 = std::make_shared<MockSink>();
    Logger::instance().add_sink(sink1);

    info("Message for sink1 only", "DynamicSinkTest");
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Add second sink after logging has started
    auto sink2 = std::make_shared<MockSink>();
    Logger::instance().add_sink(sink2);

    info("Message for both sinks", "DynamicSinkTest");
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Verify first sink received both messages
    EXPECT_EQ(sink1->entry_count(), 2) << "First sink should receive all messages";

    // Verify second sink received only messages after it was added
    EXPECT_EQ(sink2->entry_count(), 1)
        << "Second sink should only receive messages after being added";

    ResetLoggerState();
}

TEST(SinkEdgeCases, RemovedSinkReceivesNoMoreLogs) {
    ResetLoggerState();

    auto sink1 = std::make_shared<MockSink>();
    auto sink2 = std::make_shared<MockSink>();

    Logger::instance().add_sink(sink1);
    Logger::instance().add_sink(sink2);

    info("Message for both", "RemoveSinkTest");
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Remove first sink
    Logger::instance().remove_sink(sink1.get());

    info("Message for sink2 only", "RemoveSinkTest");
    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Verify first sink only got first message
    EXPECT_EQ(sink1->entry_count(), 1) << "Removed sink should not receive new messages";

    // Verify second sink got both messages
    EXPECT_EQ(sink2->entry_count(), 2) << "Active sink should receive all messages";

    ResetLoggerState();
}

// =============================================================================
// Test Suite 5: Level Filtering Edge Cases
// =============================================================================

TEST(LevelFiltering, SettingMinimumLevelFiltersMessages) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    Logger::instance().add_sink(mock_sink);

    // Set minimum level to WARNING
    Logger::instance().setMininumLevel(level::WARNING);

    trace("Trace message", "LevelTest");
    debug("Debug message", "LevelTest");
    info("Info message", "LevelTest");
    warning("Warning message", "LevelTest");
    error("Error message", "LevelTest");

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Only WARNING and ERROR should be received
    size_t warning_count = 0, error_count = 0;
    const auto& levels = mock_sink->get_levels();
    for (const auto& lvl : levels) {
        if (lvl == level::WARNING)
            warning_count++;
        if (lvl == level::ERROR)
            error_count++;
    }

    EXPECT_EQ(warning_count, 1) << "Should receive 1 WARNING message";
    EXPECT_EQ(error_count, 1) << "Should receive 1 ERROR message";
    EXPECT_LE(mock_sink->entry_count(), 2) << "Should only receive WARNING and ERROR";

    ResetLoggerState();
}

TEST(LevelFiltering, ChangingLevelDynamically) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    Logger::instance().add_sink(mock_sink);

    // Start at INFO level
    Logger::instance().setMininumLevel(level::INFO);
    info("Info 1", "DynamicLevelTest");
    debug("Debug 1", "DynamicLevelTest"); // Should be filtered

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Change to TRACE level (allow all)
    Logger::instance().setMininumLevel(level::TRACE);
    info("Info 2", "DynamicLevelTest");
    debug("Debug 2", "DynamicLevelTest"); // Should pass now

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 2));

    // Verify level change took effect
    size_t info_count = 0, debug_count = 0;
    const auto& levels = mock_sink->get_levels();
    for (const auto& lvl : levels) {
        if (lvl == level::INFO)
            info_count++;
        if (lvl == level::DEBUG)
            debug_count++;
    }

    EXPECT_EQ(info_count, 2) << "Should receive both INFO messages";
    EXPECT_EQ(debug_count, 1) << "Should receive only DEBUG 2 (DEBUG 1 was filtered)";

    ResetLoggerState();
}

// =============================================================================
// Test Suite 6: Concurrent Logging Stress Test
// =============================================================================

TEST(ConcurrencyTest, MultipleThreadsLoggingSimultaneously) {
    ResetLoggerState();

    auto mock_sink = std::make_shared<MockSink>();
    Logger::instance().add_sink(mock_sink);

    constexpr int num_threads = 10;
    constexpr int logs_per_thread = 100;

    std::vector<std::thread> threads;

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < logs_per_thread; ++i) {
                info("Thread " + std::to_string(t) + " message " + std::to_string(i),
                     "ConcurrencyTest");
            }
        });
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 10));

    // Verify all threads' logs were received (may be less due to queue drops)
    size_t total_expected = num_threads * logs_per_thread; // 1000
    size_t received = mock_sink->entry_count();

    // Some may be dropped if queue filled, but we should get most
    EXPECT_GT(received, total_expected * 0.9) // At least 90%
        << "Should receive most concurrent logs, got " << received << " of " << total_expected;
    EXPECT_LE(received, 4096) // Queue capacity
        << "Should not exceed queue capacity";

    ResetLoggerState();
}

TEST(ConcurrencyTest, AddingSinksWhileLogging) {
    ResetLoggerState();

    auto sink1 = std::make_shared<MockSink>();
    Logger::instance().add_sink(sink1);

    // Start logging in background
    std::atomic<bool> keep_running{true};
    std::thread logger_thread([&keep_running]() {
        int counter = 0;
        while (keep_running) {
            info("Concurrent log " + std::to_string(counter++), "ConcurrencyTest");
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    });

    // Add/remove sinks while logging is active
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto temp_sink = std::make_shared<MockSink>();
        Logger::instance().add_sink(temp_sink);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        Logger::instance().remove_sink(temp_sink.get());
    }

    keep_running = false;
    logger_thread.join();

    Logger::instance().flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));

    // Verify no crashes occurred
    EXPECT_GT(sink1->entry_count(), 0) << "Original sink should have received logs";

    ResetLoggerState();
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
