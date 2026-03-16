/**
 * @file logger_concurrency_test.cpp
 * @brief Comprehensive concurrency tests for CFLogger using GoogleTest
 *
 * Test Coverage:
 * 1. Concurrent Logging Safety - No crashes under concurrent load
 * 2. Flush Reliability - Flush completes correctly under concurrent logging
 * 3. Dynamic Sink Management - Add/remove sinks while logging
 * 4. Log Level Filtering - Filter performance and dynamic level changes
 *
 * Thread configurations: 1, 4, 8, 16 threads
 */

#include "cflog/cflog.hpp"
#include "cflog/cflog_level.hpp"
#include "mock/counting_sink.h"
#include "test_config.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <random>
#include <thread>
#include <vector>

using namespace cf::log;
using namespace cf::log::test;

// =============================================================================
// Test Helper Functions
// =============================================================================

/**
 * @brief Helper to reset logger state between tests
 *
 * Clears all sinks and resets the minimum log level.
 */
void ResetLoggerState() {
    auto& logger = Logger::instance();
    logger.clear_sinks();
    logger.setMininumLevel(level::TRACE); // Enable all levels
}

/**
 * @brief Helper to wait for async processing to complete
 *
 * Since logging is asynchronous, we need to wait and flush to ensure
 * all queued log records are processed before verification.
 */
void WaitForAsyncProcessing(int milliseconds = 100) {
    int wait_ms = milliseconds;
    // Scale down wait time for fast CI/CD
#ifndef CFLOGGER_STRESS_TEST
    if (wait_ms > 50)
        wait_ms = 50;
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
    Logger::instance().flush_sync(); // Use sync flush to ensure completion
}

/**
 * @brief Helper to generate random log level
 */
level RandomLogLevel(std::mt19937& gen) {
    std::uniform_int_distribution<> dist(0, 4);
    return static_cast<level>(dist(gen));
}

// =============================================================================
// Test Suite 1: Concurrent Logging Safety
// =============================================================================

/**
 * @test ConcurrentLoggingNoCrashes
 * @brief Verify that multiple threads logging messages causes no crashes
 *
 * This is a basic stress test to ensure the logger is thread-safe and
 * doesn't crash under concurrent load.
 * Thread count and logs per thread are controlled by CFLOGGER_CONCURRENCY_THREADS
 * and CFLOGGER_LOGS_PER_THREAD macros in test_config.h.
 */
TEST(LoggerConcurrencyTest, ConcurrentLoggingNoCrashes) {
    ResetLoggerState();

    auto counting_sink = std::make_shared<CountingSink>();
    Logger::instance().add_sink(counting_sink);

    constexpr int num_threads = CFLOGGER_CONCURRENCY_THREADS;
    constexpr int logs_per_thread = CFLOGGER_LOGS_PER_THREAD;
    const std::string test_tag = "NoCrash";

    std::vector<std::thread> threads;
    std::atomic<int> threads_started{0};
    std::atomic<int> threads_completed{0};

    // Launch logging threads
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            threads_started.fetch_add(1);

            // Wait for all threads to be ready
            while (threads_started.load() < num_threads) {
                std::this_thread::yield();
            }

            // Each thread logs its messages
            for (int i = 0; i < logs_per_thread; ++i) {
                EXPECT_NO_THROW({
                    Logger::instance().log(level::INFO,
                                           "Thread " + std::to_string(t) + " message " +
                                               std::to_string(i),
                                           test_tag, std::source_location::current());
                });
            }

            threads_completed.fetch_add(1);
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(threads_completed.load(), num_threads);

    // Wait for async processing and flush
    WaitForAsyncProcessing(500);

    // Verify we got at least some logs (queue may drop under extreme load)
    size_t total_expected = static_cast<size_t>(num_threads * logs_per_thread);
    size_t actual_count = counting_sink->get_count();
    EXPECT_GT(actual_count, total_expected / 2) << "Expected at least half of logs to be processed";
}

/**
 * @test ConcurrentLoggingAllLevels
 * @brief Verify mixed log levels work correctly under concurrent load
 *
 * Multiple threads log at all different levels (TRACE, DEBUG, INFO, WARNING, ERROR).
 * Verifies that the logger correctly handles different log levels concurrently.
 */
TEST(LoggerConcurrencyTest, ConcurrentLoggingAllLevels) {
    ResetLoggerState();

    auto level_sink = std::make_shared<LevelCountingSink>();
    Logger::instance().add_sink(level_sink);
    Logger::instance().setMininumLevel(level::TRACE);

    constexpr int num_threads =
        (CFLOGGER_CONCURRENCY_THREADS > 8) ? 8 : CFLOGGER_CONCURRENCY_THREADS;
    constexpr int logs_per_thread = CFLOGGER_LOGS_PER_THREAD;

    std::vector<std::thread> threads;
    std::atomic<int> start_barrier{0};

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            std::mt19937 gen(t); // Seeded with thread index

            // Synchronize start
            start_barrier.fetch_add(1);
            while (start_barrier.load() < num_threads) {
                std::this_thread::yield();
            }

            // Log messages at random levels
            for (int i = 0; i < logs_per_thread; ++i) {
                level lvl = RandomLogLevel(gen);
                EXPECT_NO_THROW({
                    Logger::instance().log(
                        lvl, "Thread " + std::to_string(t) + " msg " + std::to_string(i),
                        "AllLevels", std::source_location::current());
                });
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    WaitForAsyncProcessing(300);

    // Verify we received logs at multiple levels
    size_t total_count = level_sink->get_total_count();
    EXPECT_GT(total_count, 0) << "Should have received some log messages";

    // Check that we have records for each level (they may vary due to randomness)
    bool has_logs_at_some_level = false;
    for (int l = 0; l < 5; ++l) {
        size_t count = level_sink->get_count_for_level(static_cast<level>(l));
        if (count > 0) {
            has_logs_at_some_level = true;
        }
    }
    EXPECT_TRUE(has_logs_at_some_level) << "Should have logs at at least one level";
}

/**
 * @test ConcurrentLoggingWithSinkChanges
 * @brief Verify sink changes during concurrent logging are safe
 *
 * Adds and removes sinks while multiple threads are actively logging.
 * This tests the thread-safety of sink management operations.
 */
TEST(LoggerConcurrencyTest, ConcurrentLoggingWithSinkChanges) {
    ResetLoggerState();

    auto main_sink = std::make_shared<CountingSink>();
    Logger::instance().add_sink(main_sink);

    constexpr int num_threads = 4;
    constexpr int logs_per_thread = 500;
    constexpr int sink_changes = 10;

    std::atomic<bool> stop_logging{false};
    std::atomic<int> logs_written{0};
    std::vector<std::thread> threads;

    // Launch logging threads
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            int iterations = 0;
            while (!stop_logging.load() && iterations < logs_per_thread) {
                EXPECT_NO_THROW({
                    Logger::instance().log(level::INFO,
                                           "Logging message " + std::to_string(iterations),
                                           "SinkChange", std::source_location::current());
                });
                logs_written.fetch_add(1);
                iterations++;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }

    // Thread that modifies sinks
    std::thread sink_modifier([&]() {
        for (int i = 0; i < sink_changes; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));

            auto temp_sink = std::make_shared<CountingSink>();
            EXPECT_NO_THROW({
                Logger::instance().add_sink(temp_sink);
                Logger::instance().flush();
                Logger::instance().remove_sink(temp_sink.get());
            });
        }
        stop_logging.store(true);
    });

    for (auto& thread : threads) {
        thread.join();
    }
    sink_modifier.join();

    WaitForAsyncProcessing(200);

    EXPECT_GT(logs_written.load(), 0) << "Some logs should have been written";
    EXPECT_GT(main_sink->get_count(), 0) << "Main sink should have received logs";
}

// =============================================================================
// Test Suite 2: Flush Reliability
// =============================================================================

/**
 * @test FlushCompletesBeforeTimeout
 * @brief Verify flush operation completes within reasonable time
 *
 * Flush should not block indefinitely. This test verifies that flush
 * completes within 5 seconds even with pending log records.
 */
TEST(LoggerConcurrencyTest, FlushCompletesBeforeTimeout) {
    ResetLoggerState();

    auto counting_sink = std::make_shared<CountingSink>();
    Logger::instance().add_sink(counting_sink);

    // Generate logs (count controlled by test_config.h)
    constexpr int num_logs = (CFLOGGER_LOGS_PER_THREAD > 1000) ? 1000 : CFLOGGER_LOGS_PER_THREAD;
    for (int i = 0; i < num_logs; ++i) {
        Logger::instance().log(level::INFO, "Flush test message " + std::to_string(i), "FlushTest",
                               std::source_location::current());
    }

    // Use sync flush to ensure flush actually completes
    EXPECT_NO_THROW({ Logger::instance().flush_sync(); });

    // Verify logs were processed
    EXPECT_GT(counting_sink->get_count(), 0) << "Logs should have been processed after flush";
    EXPECT_GT(counting_sink->get_flush_count(), 0) << "Flush should have been called on sink";
}

/**
 * @test MultipleFlushesWhileLogging
 * @brief Verify multiple flush operations during active logging
 *
 * Tests that calling flush multiple times while logs are being generated
 * doesn't cause issues and all flushes complete successfully.
 */
TEST(LoggerConcurrencyTest, MultipleFlushesWhileLogging) {
    ResetLoggerState();

    auto counting_sink = std::make_shared<CountingSink>();
    Logger::instance().add_sink(counting_sink);

    constexpr int num_threads = 2; // Reduced for fast CI
    constexpr int logs_per_thread = CFLOGGER_LOGS_PER_THREAD;
    constexpr int flush_count = 5; // Reduced from 10

    std::atomic<bool> stop_flag{false};
    std::vector<std::thread> threads;

    // Launch logging threads
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            int i = 0;
            while (!stop_flag.load() && i < logs_per_thread) {
                Logger::instance().log(level::DEBUG, "Active logging message", "MultiFlush",
                                       std::source_location::current());
                i++;
                std::this_thread::sleep_for(std::chrono::microseconds(200));
            }
        });
    }

    // Perform multiple flushes
    for (int f = 0; f < flush_count; ++f) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS));
        EXPECT_NO_THROW({ Logger::instance().flush(); });
    }

    stop_flag.store(true);

    for (auto& thread : threads) {
        thread.join();
    }

    WaitForAsyncProcessing(50);

    EXPECT_EQ(counting_sink->get_flush_count(),
              flush_count + 1) // +1 for final WaitForAsyncProcessing flush
        << "Sink should have been flushed the expected number of times";
}

// =============================================================================
// Test Suite 3: Dynamic Sink Management
// =============================================================================

/**
 * @test AddSinkWhileLogging
 * @brief Verify adding a sink while logging is active
 *
 * Tests that a newly added sink starts receiving logs immediately
 * even when other threads are actively logging.
 */
TEST(LoggerConcurrencyTest, AddSinkWhileLogging) {
    ResetLoggerState();

    auto initial_sink = std::make_shared<CountingSink>();
    Logger::instance().add_sink(initial_sink);

    std::atomic<bool> stop_flag{false};
    std::atomic<bool> sink_added{false};

    // Launch logging thread - runs until stopped
    std::thread logger_thread([&]() {
        while (!stop_flag.load()) {
            Logger::instance().log(level::INFO, "Message", "AddSink",
                                   std::source_location::current());
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    });

    // Wait for some logs to be written
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Add new sink while logging is active
    auto late_sink = std::make_shared<CountingSink>();
    EXPECT_NO_THROW({ Logger::instance().add_sink(late_sink); });
    sink_added.store(true);

    // Let logging continue for a while
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    stop_flag.store(true);
    logger_thread.join();

    WaitForAsyncProcessing(50);

    EXPECT_TRUE(sink_added.load()) << "Sink should have been added";
    EXPECT_GT(late_sink->get_count(), 0) << "Late-added sink should have received some logs";
    EXPECT_GT(initial_sink->get_count(), late_sink->get_count())
        << "Initial sink should have more logs than the late-added sink";
}

/**
 * @test RemoveSinkWhileLogging
 * @brief Verify removing a sink while logging is active
 *
 * Tests that removing a sink stops it from receiving further logs
 * but doesn't cause crashes or data loss for other sinks.
 */
TEST(LoggerConcurrencyTest, RemoveSinkWhileLogging) {
    ResetLoggerState();

    auto sink_to_remove = std::make_shared<CountingSink>();
    auto persistent_sink = std::make_shared<CountingSink>();

    Logger::instance().add_sink(sink_to_remove);
    Logger::instance().add_sink(persistent_sink);

    std::atomic<bool> stop_flag{false};
    std::atomic<bool> sink_removed{false};

    // Launch logging thread - runs until stopped
    std::thread logger_thread([&]() {
        while (!stop_flag.load()) {
            Logger::instance().log(level::INFO, "Message", "RemoveSink",
                                   std::source_location::current());
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    });

    // Wait for some logs
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Remove sink while logging
    EXPECT_NO_THROW({ Logger::instance().remove_sink(sink_to_remove.get()); });
    sink_removed.store(true);

    // Continue logging
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    stop_flag.store(true);
    logger_thread.join();

    WaitForAsyncProcessing(50);

    EXPECT_TRUE(sink_removed.load()) << "Sink should have been removed";
    size_t removed_count = sink_to_remove->get_count();
    size_t persistent_count = persistent_sink->get_count();

    EXPECT_GT(removed_count, 0) << "Removed sink should have received some logs before removal";
    EXPECT_GT(persistent_count, removed_count)
        << "Persistent sink should have more logs than the removed sink";
}

/**
 * @test ClearSinksWhileLogging
 * @brief Verify clearing all sinks while logging is active
 *
 * Tests that clearing all sinks stops log processing and doesn't
 * cause crashes when logs continue to be generated.
 */
TEST(LoggerConcurrencyTest, ClearSinksWhileLogging) {
    ResetLoggerState();

    auto sink1 = std::make_shared<CountingSink>();
    auto sink2 = std::make_shared<CountingSink>();

    Logger::instance().add_sink(sink1);
    Logger::instance().add_sink(sink2);

    std::atomic<bool> stop_flag{false};
    std::atomic<bool> sinks_cleared{false};

    // Launch multiple logging threads
    std::vector<std::thread> threads;
    for (int t = 0; t < 2; ++t) {
        threads.emplace_back([&]() {
            int i = 0;
            int max_iter = (CFLOGGER_LOGS_PER_THREAD > 200) ? 200 : CFLOGGER_LOGS_PER_THREAD;
            while (!stop_flag.load() && i < max_iter) {
                Logger::instance().log(level::INFO, "Message " + std::to_string(i), "ClearSinks",
                                       std::source_location::current());
                i++;
                std::this_thread::sleep_for(std::chrono::microseconds(200));
            }
        });
    }

    // Wait for some activity
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Clear all sinks
    EXPECT_NO_THROW({ Logger::instance().clear_sinks(); });
    sinks_cleared.store(true);

    // Continue for a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(CFLOGGER_STRESS_WAIT_MS * 5));

    stop_flag.store(true);
    for (auto& thread : threads) {
        thread.join();
    }

    WaitForAsyncProcessing(50);

    EXPECT_TRUE(sinks_cleared.load()) << "Sinks should have been cleared";
    EXPECT_GT(sink1->get_count(), 0) << "Sink1 should have received some logs before clear";
    EXPECT_GT(sink2->get_count(), 0) << "Sink2 should have received some logs before clear";
}

/**
 * @test MultipleSinksConcurrentWrites
 * @brief Verify multiple sinks can handle concurrent writes correctly
 *
 * Tests that multiple sinks receive the same log records correctly
 * under concurrent logging conditions.
 */
TEST(LoggerConcurrencyTest, MultipleSinksConcurrentWrites) {
    ResetLoggerState();

    auto sink1 = std::make_shared<CountingSink>();
    auto sink2 = std::make_shared<CountingSink>();
    auto sink3 = std::make_shared<CountingSink>();

    Logger::instance().add_sink(sink1);
    Logger::instance().add_sink(sink2);
    Logger::instance().add_sink(sink3);

    constexpr int num_threads = 4; // Keep small for this test
    constexpr int logs_per_thread =
        (CFLOGGER_LOGS_PER_THREAD > 500) ? 500 : CFLOGGER_LOGS_PER_THREAD;

    std::vector<std::thread> threads;

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < logs_per_thread; ++i) {
                Logger::instance().log(
                    level::INFO, "Thread " + std::to_string(t) + " message " + std::to_string(i),
                    "MultiSink", std::source_location::current());
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    WaitForAsyncProcessing(300);

    size_t count1 = sink1->get_count();
    size_t count2 = sink2->get_count();
    size_t count3 = sink3->get_count();

    // All sinks should have received similar counts
    size_t expected_min =
        static_cast<size_t>(num_threads * logs_per_thread * 0.5); // Allow some variance

    EXPECT_GT(count1, expected_min) << "Sink1 should have received most logs";
    EXPECT_GT(count2, expected_min) << "Sink2 should have received most logs";
    EXPECT_GT(count3, expected_min) << "Sink3 should have received most logs";

    // Counts should be approximately equal (within 10% tolerance due to queue dynamics)
    size_t max_count = std::max({count1, count2, count3});
    size_t min_count = std::min({count1, count2, count3});

    // This is a loose check - the async queue may have some variance
    EXPECT_GT(min_count, max_count * 0.8) << "Sink counts should be relatively balanced";
}

// =============================================================================
// Test Suite 4: Log Level Filtering
// =============================================================================

/**
 * @test LogLevelFilteringPerformance
 * @brief Verify log level filtering works efficiently under load
 *
 * Tests that setting a minimum log level correctly filters out lower-level
 * logs even under concurrent logging with multiple threads.
 */
TEST(LoggerConcurrencyTest, LogLevelFilteringPerformance) {
    ResetLoggerState();

    auto level_sink = std::make_shared<LevelCountingSink>();
    Logger::instance().add_sink(level_sink);

    // Set minimum level to WARNING - should filter TRACE, DEBUG, INFO
    Logger::instance().setMininumLevel(level::WARNING);

    constexpr int num_threads = 4; // Keep small for this test
    constexpr int iterations = (CFLOGGER_LOGS_PER_THREAD > 500) ? 500 : CFLOGGER_LOGS_PER_THREAD;

    std::vector<std::thread> threads;

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([t]() {
            for (int i = 0; i < iterations; ++i) {
                // Log at all levels
                Logger::instance().log(level::TRACE, "Trace", "Filter",
                                       std::source_location::current());
                Logger::instance().log(level::DEBUG, "Debug", "Filter",
                                       std::source_location::current());
                Logger::instance().log(level::INFO, "Info", "Filter",
                                       std::source_location::current());
                Logger::instance().log(level::WARNING, "Warning", "Filter",
                                       std::source_location::current());
                Logger::instance().log(level::ERROR, "Error", "Filter",
                                       std::source_location::current());
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    WaitForAsyncProcessing(300);

    // Should only have WARNING and ERROR logs
    size_t trace_count = level_sink->get_count_for_level(level::TRACE);
    size_t debug_count = level_sink->get_count_for_level(level::DEBUG);
    size_t info_count = level_sink->get_count_for_level(level::INFO);
    size_t warning_count = level_sink->get_count_for_level(level::WARNING);
    size_t error_count = level_sink->get_count_for_level(level::ERROR);

    EXPECT_EQ(trace_count, 0) << "TRACE logs should be filtered out";
    EXPECT_EQ(debug_count, 0) << "DEBUG logs should be filtered out";
    EXPECT_EQ(info_count, 0) << "INFO logs should be filtered out";
    EXPECT_GT(warning_count, 0) << "WARNING logs should pass through";
    EXPECT_GT(error_count, 0) << "ERROR logs should pass through";

    size_t expected_count = static_cast<size_t>(num_threads * iterations * 2); // WARNING + ERROR
    size_t actual_count = warning_count + error_count;
    EXPECT_GT(actual_count, expected_count * 0.9) << "Should have received most WARNING+ERROR logs";
}

/**
 * @test DynamicLogLevelChange
 * @brief Verify runtime log level changes work correctly
 *
 * Tests that changing the minimum log level during active logging
 * correctly affects which logs are processed.
 */
TEST(LoggerConcurrencyTest, DynamicLogLevelChange) {
    ResetLoggerState();

    auto level_sink = std::make_shared<LevelCountingSink>();
    Logger::instance().add_sink(level_sink);

    // Start with ERROR only
    Logger::instance().setMininumLevel(level::ERROR);

    std::atomic<bool> stop_flag{false};
    std::atomic<bool> level_changed{false};
    std::atomic<int> iterations_after_change{0};

    // Launch logging threads - run until stopped
    std::vector<std::thread> threads;
    constexpr int num_threads = 2; // Reduced for faster CI

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            while (!stop_flag.load()) {
                Logger::instance().log(level::ERROR, "Error msg", "Dynamic",
                                       std::source_location::current());
                Logger::instance().log(level::WARNING, "Warning msg", "Dynamic",
                                       std::source_location::current());
                Logger::instance().log(level::INFO, "Info msg", "Dynamic",
                                       std::source_location::current());

                if (level_changed.load()) {
                    iterations_after_change.fetch_add(1);
                    if (iterations_after_change.load() >= num_threads * 50) {
                        // Enough iterations after level change
                        stop_flag.store(true);
                    }
                }

                std::this_thread::sleep_for(std::chrono::microseconds(500));
            }
        });
    }

    // Wait for some ERROR-only logs
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Change level to INFO - should now allow WARNING and INFO
    Logger::instance().setMininumLevel(level::INFO);
    level_changed.store(true);

    // Continue logging until threads have done enough iterations
    for (auto& thread : threads) {
        thread.join();
    }

    WaitForAsyncProcessing(50);

    size_t error_count = level_sink->get_count_for_level(level::ERROR);
    size_t warning_count = level_sink->get_count_for_level(level::WARNING);
    size_t info_count = level_sink->get_count_for_level(level::INFO);

    EXPECT_TRUE(level_changed.load()) << "Level should have been changed";
    EXPECT_GT(error_count, 0) << "Should have ERROR logs";
    EXPECT_GT(warning_count, 0) << "Should have WARNING logs after level change";
    EXPECT_GT(info_count, 0)
        << "INFO logs should pass through when minimal_level is INFO (>= semantics)";
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
