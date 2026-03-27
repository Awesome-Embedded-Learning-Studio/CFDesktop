/**
 * @file mock_sink.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief MockSink for capturing log records in tests
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "cflog/cflog_format.h"
#include <cflog/cflog_level.hpp>
#include <cflog/cflog_record.h>
#include <cflog/cflog_sink.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>

namespace cf::log::test {

class MockSink : public ISink {
  public:
    struct Config {
        bool always_fail{false};         // Always return false from write()
        bool fail_after_n_writes{false}; // Fail after N successful writes
        uint64_t fail_threshold{0};      // N value for fail_after_n_writes
        bool fail_flush{false};          // Return false from flush()
        bool slow_write{false};          // Add delay in write() to simulate slow sink
    };

    MockSink() : MockSink(Config{}) {}

    explicit MockSink(const Config& config) : config_(config) {}

    bool write(const LogRecord& record) override {
        // Simulate slow write if configured
        if (config_.slow_write) {
            // Longer delay to ensure queue fills up during tests
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::lock_guard<std::mutex> lock(mutex_);
        write_count_++;

        // Check if we should fail
        if (config_.always_fail) {
            return false;
        }

        if (config_.fail_after_n_writes && write_count_ > config_.fail_threshold) {
            return false;
        }

        std::string formatted_output;
        if (formatter_) {
            formatted_output = formatter_->format_me(record);
        } else {
            formatted_output = record.msg;
        }
        formatted_.push_back(formatted_output);

        // Store level for filtering tests
        levels_.push_back(record.lvl);
        // Store tag for filtering tests
        tags_.push_back(record.tag);
        // Store message for searching
        messages_.push_back(record.msg);

        cv_.notify_one();
        return true;
    }

    bool flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        flush_count_++;
        cv_.notify_all();
        return !config_.fail_flush;
    }

    /**
     * @brief Wait for at least n records to be captured
     */
    bool waitForRecords(size_t n, int timeout_ms = 1000) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                            [&]() { return formatted_.size() >= n; });
    }

    /**
     * @brief Check if a record with specific message exists
     */
    bool hasMessage(const std::string& message) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& msg : messages_) {
            if (msg.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Check if a record with specific tag exists
     */
    bool hasTag(const std::string& tag) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& t : tags_) {
            if (t == tag) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Count records with a specific log level
     */
    size_t countByLevel(level lvl) const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = 0;
        for (const auto& l : levels_) {
            if (l == lvl) {
                count++;
            }
        }
        return count;
    }

    // Helper methods for testing
    [[nodiscard]] size_t record_count() const noexcept { return formatted_.size(); }

    [[nodiscard]] size_t entry_count() const noexcept { return formatted_.size(); }

    [[nodiscard]] bool empty() const noexcept { return formatted_.empty(); }

    void clear() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        formatted_.clear();
        levels_.clear();
        tags_.clear();
        messages_.clear();
        write_count_ = 0;
        flush_count_ = 0;
    }

    [[nodiscard]] const std::string& last_formatted() const {
        static const std::string empty;
        std::lock_guard<std::mutex> lock(mutex_);
        if (formatted_.empty())
            return empty;
        return formatted_.back();
    }

    [[nodiscard]] uint64_t write_count() const { return write_count_; }

    [[nodiscard]] uint64_t flush_count() const { return flush_count_; }

    void set_config(const Config& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
    }

    [[nodiscard]] const std::deque<std::string>& get_formatted() const { return formatted_; }

    [[nodiscard]] const std::deque<level>& get_levels() const { return levels_; }

    [[nodiscard]] const std::deque<std::string>& get_tags() const { return tags_; }

    [[nodiscard]] const std::deque<std::string>& get_messages() const { return messages_; }

  private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    Config config_;
    std::deque<std::string> formatted_;
    std::deque<level> levels_;
    std::deque<std::string> tags_;
    std::deque<std::string> messages_;
    std::atomic<uint64_t> write_count_{0};
    std::atomic<uint64_t> flush_count_{0};
};

} // namespace cf::log::test
