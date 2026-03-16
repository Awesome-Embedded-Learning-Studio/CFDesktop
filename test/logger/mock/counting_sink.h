/**
 * @file counting_sink.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Mock sink for testing - counts log records without writing them
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include <array>
#include <atomic>
#include <cflog/cflog_level.hpp>
#include <cflog/cflog_record.h>
#include <cflog/cflog_sink.h>
#include <mutex>
#include <thread>

namespace cf::log::test {

/**
 * @brief Lightweight counting sink for performance benchmarking
 *
 * This sink is optimized for minimal overhead - it only atomically
 * increments a counter without storing records or taking locks.
 * This makes it ideal for throughput testing.
 */
class CountingSink : public ISink {
  public:
    CountingSink() : count_(0), dropped_count_(0), flush_count_(0) {}

    bool write(const LogRecord& record) override {
        // Atomic increment with relaxed ordering for maximum performance
        count_.fetch_add(1, std::memory_order_relaxed);
        (void)record; // Suppress unused warning
        return true;
    }

    bool flush() override {
        flush_count_.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    // Test helper methods
    size_t get_count() const { return count_.load(std::memory_order_relaxed); }

    size_t get_dropped_count() const { return dropped_count_.load(std::memory_order_relaxed); }

    size_t get_flush_count() const { return flush_count_.load(std::memory_order_relaxed); }

    void reset() {
        count_.store(0, std::memory_order_relaxed);
        dropped_count_.store(0, std::memory_order_relaxed);
        flush_count_.store(0, std::memory_order_relaxed);
    }

    void increment_dropped() { dropped_count_.fetch_add(1, std::memory_order_relaxed); }

  private:
    std::atomic<size_t> count_;
    std::atomic<size_t> dropped_count_;
    std::atomic<size_t> flush_count_;
};

/**
 * @brief Counting sink that records log levels for detailed verification
 *
 * Tracks counts per log level for more detailed testing of level filtering.
 */
class LevelCountingSink : public ISink {
  public:
    LevelCountingSink() : total_count_(0), flush_count_(0) { records_by_level_.fill(0); }

    bool write(const LogRecord& record) override {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t level_idx = static_cast<size_t>(record.lvl);
        if (level_idx < records_by_level_.size()) {
            records_by_level_[level_idx]++;
        }
        total_count_++;
        return true;
    }

    bool flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        flush_count_++;
        return true;
    }

    size_t get_count_for_level(level lvl) const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t level_idx = static_cast<size_t>(lvl);
        if (level_idx < records_by_level_.size()) {
            return records_by_level_[level_idx];
        }
        return 0;
    }

    size_t get_total_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return total_count_;
    }

    size_t get_flush_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return flush_count_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        records_by_level_.fill(0);
        total_count_ = 0;
        flush_count_ = 0;
    }

  private:
    mutable std::mutex mutex_;
    std::array<size_t, 5> records_by_level_; // TRACE, DEBUG, INFO, WARNING, ERROR
    size_t total_count_;
    size_t flush_count_;
};

/**
 * @brief Blocking sink for testing flush behavior
 *
 * Can be configured to block writes to test queue behavior
 * and flush timing.
 */
class BlockingSink : public ISink {
  public:
    explicit BlockingSink(std::atomic<bool>& block_flag)
        : block_flag_(block_flag), write_count_(0) {}

    bool write(const LogRecord& record) override {
        (void)record;
        // Spin while blocking is enabled
        while (block_flag_.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        write_count_.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    bool flush() override { return true; }

    size_t get_write_count() const { return write_count_.load(std::memory_order_acquire); }

  private:
    std::atomic<bool>& block_flag_;
    std::atomic<size_t> write_count_;
};

} // namespace cf::log::test
