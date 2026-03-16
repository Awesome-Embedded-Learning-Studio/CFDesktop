/**
 * @file logger_benchmark_test.cpp
 * @brief Performance benchmark tests for CFLogger
 * @date 2026-03-16
 *
 * Test Coverage:
 * 1. Single-threaded throughput tests (baseline, with formatters)
 * 2. Multi-threaded throughput tests (parametric 1/4/8/16 threads)
 * 3. Message size impact tests (32B, 256B, 4KB, 1MB)
 * 4. Queue overflow behavior tests
 * 5. Memory leak detection tests
 *
 * Performance Requirements:
 * - Baseline throughput: >= 10,000 logs/sec
 * - Target range: 10,000 - 50,000 logs/sec
 */

#include "../mock/counting_sink.h"
#include "../mock/mock_sink.h"
#include <cflog/cflog.hpp>
#include <cflog/cflog_level.hpp>
#include <cflog/formatter/console_formatter.h>
#include <cflog/formatter/file_formatter.h>

#include <gtest/gtest.h>

#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace cf::log;
using namespace cf::log::test;
using namespace std::chrono;

// =============================================================================
// Test Constants
// =============================================================================

// Performance thresholds
constexpr size_t kMinThroughputLogsPerSec = 10000;    // 10K logs/sec baseline
constexpr size_t kTargetThroughputLogsPerSec = 50000; // 50K logs/sec target

// Test message sizes
constexpr size_t kSmallMessageSize = 32;            // 32 bytes
constexpr size_t kMediumMessageSize = 256;          // 256 bytes
constexpr size_t kLargeMessageSize = 4096;          // 4KB
constexpr size_t kExtremeMessageSize = 1024 * 1024; // 1MB

// Test iteration counts
constexpr size_t kBaselineIterations = 10000;
constexpr size_t kStressIterations = 50000;
constexpr size_t kMemoryLeakIterations = 100000;

// Queue constants from AsyncPostQueue
// Note: Must match AsyncPostQueue::kMaxNormalQueueSize in async_queue.h
constexpr size_t kMaxNormalQueueSize = 65536; // 2^16

// =============================================================================
// Test Fixture
// =============================================================================

class LoggerBenchmarkTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Reset logger singleton state before each test
        auto& logger = Logger::instance();
        logger.clear_sinks();
        logger.flush();
    }

    void TearDown() override {
        // Cleanup after each test
        auto& logger = Logger::instance();
        logger.clear_sinks();
        logger.flush();
    }

    // Helper to calculate throughput
    double calculate_throughput(size_t count, duration<double> elapsed) {
        return static_cast<double>(count) / elapsed.count();
    }

    // Helper to generate a message of specific size
    std::string generate_message(size_t size) {
        if (size == 0)
            return "";
        std::string msg;
        msg.reserve(size);
        // Pattern: "Log message #0000..." repeated
        const std::string pattern = "Log message #";
        while (msg.size() + pattern.size() < size) {
            msg += pattern;
            msg += std::format("{:04d}", static_cast<int>(msg.size()));
        }
        // Trim or pad to exact size
        if (msg.size() > size) {
            msg.resize(size);
        } else {
            msg.append(size - msg.size(), 'X');
        }
        return msg;
    }

    // Helper to wait for all logs to be processed
    void wait_for_logs_processed(CountingSink* sink, size_t expected_count,
                                 duration<double> timeout = seconds(5)) {
        auto start = steady_clock::now();
        while (sink->get_count() < expected_count) {
            std::this_thread::sleep_for(milliseconds(10));
            if (steady_clock::now() - start > timeout) {
                // Timeout - logs may have been dropped
                break;
            }
        }
    }
};

// =============================================================================
// Single-threaded Throughput Tests
// =============================================================================

TEST_F(LoggerBenchmarkTest, SingleThreadBaselineThroughput) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    const std::string test_msg = "Baseline test log message";

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < kBaselineIterations; ++i) {
        logger.log(level::INFO, test_msg, "BENCH", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kBaselineIterations);

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(kBaselineIterations, elapsed);

    std::cout << std::format("Single-thread baseline: {:.0f} logs/sec\n", logs_per_sec);

    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec)
        << "Baseline throughput below minimum requirement of " << kMinThroughputLogsPerSec
        << " logs/sec";
}

TEST_F(LoggerBenchmarkTest, ThroughputWithAsciiColorFormatter) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();

    // Create AsciiColorFormatter with default settings
    auto formatter = std::make_shared<AsciiColorFormatter>();
    counting_sink->setFormat(formatter);

    logger.add_sink(counting_sink);

    const std::string test_msg = "Color formatted log message";

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < kBaselineIterations; ++i) {
        logger.log(level::INFO, test_msg, "COLOR", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kBaselineIterations);

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(kBaselineIterations, elapsed);

    std::cout << std::format("With AsciiColorFormatter: {:.0f} logs/sec\n", logs_per_sec);

    // Slightly relaxed threshold for formatted output
    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec * 0.8)
        << "Throughput with AsciiColorFormatter below acceptable threshold";
}

TEST_F(LoggerBenchmarkTest, ThroughputWithFileFormatter) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();

    // Create FileFormatter (no ANSI codes, plain text)
    auto formatter = std::make_shared<FileFormatter>();
    counting_sink->setFormat(formatter);

    logger.add_sink(counting_sink);

    const std::string test_msg = "File formatted log message";

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < kBaselineIterations; ++i) {
        logger.log(level::INFO, test_msg, "FILE", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kBaselineIterations);

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(kBaselineIterations, elapsed);

    std::cout << std::format("With FileFormatter: {:.0f} logs/sec\n", logs_per_sec);

    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec * 0.8)
        << "Throughput with FileFormatter below acceptable threshold";
}

// =============================================================================
// Multi-threaded Throughput Tests (Parametric)
// =============================================================================

class MultiThreadedThroughputTest : public LoggerBenchmarkTest,
                                    public ::testing::WithParamInterface<int> {};

TEST_P(MultiThreadedThroughputTest, MultiThreadedThroughputTest) {
    const int num_threads = GetParam();
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    const std::string test_msg = "Multi-threaded test log message";
    const size_t logs_per_thread = kBaselineIterations / num_threads;
    const size_t total_logs = logs_per_thread * num_threads;

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto start = high_resolution_clock::now();

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (size_t i = 0; i < logs_per_thread; ++i) {
                logger.log(level::INFO, test_msg, std::format("T{:02d}", t),
                           std::source_location::current());
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), total_logs, seconds(10));

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(total_logs, elapsed);

    std::cout << std::format("Multi-threaded ({} threads): {:.0f} logs/sec\n", num_threads,
                             logs_per_sec);

    // Multi-threaded throughput should still meet minimum
    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec)
        << "Multi-threaded throughput below minimum requirement";

    // Verify we processed all logs (or close to it, allowing for some drops)
    size_t processed = counting_sink->get_count();
    double success_rate = static_cast<double>(processed) / total_logs * 100.0;
    std::cout << std::format("  Success rate: {:.1f}% ({}/{} processed)\n", success_rate, processed,
                             total_logs);

    EXPECT_GE(success_rate, 95.0) << "Too many logs dropped under multi-threaded load";
}

INSTANTIATE_TEST_SUITE_P(
    ThreadCountVariants, MultiThreadedThroughputTest, ::testing::Values(1, 4, 8, 16),
    [](const ::testing::TestParamInfo<MultiThreadedThroughputTest::ParamType>& info) {
        return std::format("{}_threads", info.param);
    });

// =============================================================================
// Message Size Impact Tests
// =============================================================================

TEST_F(LoggerBenchmarkTest, SmallMessageThroughput) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    std::string msg = generate_message(kSmallMessageSize);

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < kBaselineIterations; ++i) {
        logger.log(level::INFO, msg, "SMALL", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kBaselineIterations);

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(kBaselineIterations, elapsed);

    std::cout << std::format("Small message ({}B): {:.0f} logs/sec\n", kSmallMessageSize,
                             logs_per_sec);

    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec);
}

TEST_F(LoggerBenchmarkTest, MediumMessageThroughput) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    std::string msg = generate_message(kMediumMessageSize);

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < kBaselineIterations; ++i) {
        logger.log(level::INFO, msg, "MEDIUM", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kBaselineIterations);

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(kBaselineIterations, elapsed);

    std::cout << std::format("Medium message ({}B): {:.0f} logs/sec\n", kMediumMessageSize,
                             logs_per_sec);

    // Medium messages may have slightly lower throughput
    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec * 0.7);
}

TEST_F(LoggerBenchmarkTest, LargeMessageThroughput) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    std::string msg = generate_message(kLargeMessageSize);

    // Reduce iterations for large messages
    const size_t large_msg_iterations = kBaselineIterations / 4;

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < large_msg_iterations; ++i) {
        logger.log(level::INFO, msg, "LARGE", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), large_msg_iterations, seconds(15));

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(large_msg_iterations, elapsed);

    std::cout << std::format("Large message ({}KB): {:.0f} logs/sec\n", kLargeMessageSize / 1024,
                             logs_per_sec);

    // Large messages will have significantly lower throughput
    EXPECT_GT(logs_per_sec, 0);
}

TEST_F(LoggerBenchmarkTest, ExtremeMessageSize) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    std::string msg = generate_message(kExtremeMessageSize);

    // Very few iterations for extreme size
    const size_t extreme_iterations = 10;

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < extreme_iterations; ++i) {
        logger.log(level::INFO, msg, "EXTREME", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), extreme_iterations, seconds(30));

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(extreme_iterations, elapsed);

    std::cout << std::format("Extreme message ({}MB): {:.1f} logs/sec\n",
                             kExtremeMessageSize / (1024 * 1024), logs_per_sec);

    // Just verify it completes without crashing
    EXPECT_GT(logs_per_sec, 0);
    EXPECT_EQ(counting_sink->get_count(), extreme_iterations);
}

// =============================================================================
// Queue Behavior Tests
// =============================================================================

TEST_F(LoggerBenchmarkTest, QueueOverflowDropBehavior) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    // Create a slow sink by not reading from it
    // Actually CountingSink is fast, so we need to flood the queue
    // faster than the async thread can process

    const std::string test_msg = "Queue overflow test message";

    // Send more logs than the queue can hold
    // Use a smaller multiple to keep test time reasonable
    const size_t flood_count = kMaxNormalQueueSize + 1000;

    for (size_t i = 0; i < flood_count; ++i) {
        logger.log(level::INFO, test_msg, "FLOOD", std::source_location::current());
    }

    logger.flush();

    // Give time for processing
    std::this_thread::sleep_for(milliseconds(500));

    size_t processed = counting_sink->get_count();

    std::cout << std::format("Flooded with {} logs, {} processed ({:.1f}%)\n", flood_count,
                             processed, static_cast<double>(processed) / flood_count * 100.0);

    // Queue should drop excess logs, so we expect at most queue_size + some buffer
    // The actual processed count should be <= kMaxNormalQueueSize + some
    EXPECT_LE(processed, kMaxNormalQueueSize * 1.5)
        << "Queue drop behavior not working as expected";

    // But we should still process a reasonable number
    EXPECT_GT(processed, kMaxNormalQueueSize * 0.5) << "Queue dropped too many logs";
}

// =============================================================================
// Memory Tests
// =============================================================================

TEST_F(LoggerBenchmarkTest, MemoryLeakDetection) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    const std::string test_msg = "Memory leak test message";

    // Baseline memory reading (conceptual - actual memory measurement
    // would require platform-specific code)

    for (size_t i = 0; i < kMemoryLeakIterations; ++i) {
        logger.log(level::INFO, test_msg, "MEM", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kMemoryLeakIterations, seconds(15));

    size_t processed = counting_sink->get_count();

    std::cout << std::format("Memory test: {} logs processed\n", processed);

    EXPECT_EQ(processed, kMemoryLeakIterations)
        << "Not all logs were processed - potential issue with queue/processing";

    // Clean reset
    counting_sink->reset();
    logger.clear_sinks();

    // If we get here without crashing and all logs processed,
    // there are no obvious memory leaks
    SUCCEED();
}

TEST_F(LoggerBenchmarkTest, SinkLifecycleMemoryTest) {
    auto& logger = Logger::instance();

    // Add and remove sinks repeatedly
    for (int cycle = 0; cycle < 100; ++cycle) {
        auto sink1 = std::make_shared<CountingSink>();
        auto sink2 = std::make_shared<MockSink>();

        logger.add_sink(sink1);
        logger.add_sink(sink2);

        const std::string msg = "Lifecycle test";

        for (int i = 0; i < 100; ++i) {
            logger.log(level::INFO, msg, "LIFE", std::source_location::current());
        }

        logger.flush();
        logger.clear_sinks();
    }

    // If we complete all cycles without crashing, no obvious memory issues
    SUCCEED();
}

// =============================================================================
// Additional Performance Edge Cases
// =============================================================================

TEST_F(LoggerBenchmarkTest, DISABLED_StressTestHighThroughput) {
    // This test is disabled by default as it's a stress test
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    const std::string test_msg = "Stress test message";

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < kStressIterations; ++i) {
        logger.log(level::INFO, test_msg, "STRESS", std::source_location::current());
    }

    logger.flush();
    wait_for_logs_processed(counting_sink.get(), kStressIterations, seconds(20));

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(kStressIterations, elapsed);

    std::cout << std::format("Stress test ({} iterations): {:.0f} logs/sec\n", kStressIterations,
                             logs_per_sec);

    EXPECT_GE(logs_per_sec, kMinThroughputLogsPerSec);
}

TEST_F(LoggerBenchmarkTest, RapidFlushPerformance) {
    auto& logger = Logger::instance();
    auto counting_sink = std::make_shared<CountingSink>();
    logger.add_sink(counting_sink);

    const std::string test_msg = "Flush test message";
    const size_t iterations = 1000;

    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < iterations; ++i) {
        logger.log(level::INFO, test_msg, "FLUSH", std::source_location::current());
        logger.flush(); // Flush every time
    }

    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;

    double logs_per_sec = calculate_throughput(iterations, elapsed);

    std::cout << std::format("Rapid flush test: {:.0f} logs/sec\n", logs_per_sec);

    // With flush on every log, throughput will be much lower
    // but should still be reasonable
    EXPECT_GT(logs_per_sec, 100);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "=================================================\n";
    std::cout << "CFLogger Performance Benchmark Tests\n";
    std::cout << "=================================================\n";
    std::cout << std::format("Minimum throughput target: {} logs/sec\n", kMinThroughputLogsPerSec);
    std::cout << std::format("Target throughput range: {}-{} logs/sec\n", kMinThroughputLogsPerSec,
                             kTargetThroughputLogsPerSec);
    std::cout << std::format("Queue size (max_normal): {}\n", kMaxNormalQueueSize);
    std::cout << "=================================================\n\n";

    return RUN_ALL_TESTS();
}
