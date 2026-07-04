/**
 * @file cflog_format_config.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CFLog formatter configuration for customizable log output
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "cflog_format_flags.h"
#include <atomic>
#include <string>

namespace cf::log {

/**
 * @brief Thread-safe configuration for formatters
 *
 * Provides atomic operations for enabling/disabling formatter flags
 * and customizing timestamp format.
 */
class FormatterConfig {
  public:
    /**
     * @brief Construct with specified flags and timestamp format
     * @param flags Initial formatter flags
     * @param timestamp_format strftime format string for timestamps (default: "%H:%M:%S")
     */
    explicit FormatterConfig(FormatterFlag flags = FormatterFlag::DEFAULT,
                             std::string timestamp_format = "%H:%M:%S")
        : flags_(static_cast<uint32_t>(flags)), timestamp_format_(std::move(timestamp_format)) {}

    // Thread-safe get/set operations
    [[nodiscard]] FormatterFlag get_flags() const noexcept {
        return static_cast<FormatterFlag>(flags_.load(std::memory_order_relaxed));
    }

    /**
     * @brief  Sets the formatter flags.
     *
     * @param[in] flags The flags to set.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void set_flags(FormatterFlag flags) noexcept {
        flags_.store(static_cast<uint32_t>(flags), std::memory_order_relaxed);
    }

    // Convenience methods
    /**
     * @brief  Enables a specific formatter flag.
     *
     * @param[in] flag The flag to enable.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void enable(FormatterFlag flag) noexcept {
        flags_.fetch_or(static_cast<uint32_t>(flag), std::memory_order_relaxed);
    }

    /**
     * @brief  Disables a specific formatter flag.
     *
     * @param[in] flag The flag to disable.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void disable(FormatterFlag flag) noexcept {
        flags_.fetch_and(~static_cast<uint32_t>(flag), std::memory_order_relaxed);
    }

    [[nodiscard]] bool is_enabled(FormatterFlag flag) const noexcept {
        return (flags_.load(std::memory_order_relaxed) & static_cast<uint32_t>(flag)) != 0;
    }

    // Timestamp format configuration
    /**
     * @brief  Sets the timestamp format string.
     *
     * @param[in] fmt The strftime format string for timestamps.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void set_timestamp_format(std::string fmt) { timestamp_format_ = std::move(fmt); }

    [[nodiscard]] const std::string& get_timestamp_format() const noexcept {
        return timestamp_format_;
    }

  private:
    std::atomic<uint32_t> flags_;
    std::string timestamp_format_;
};

} // namespace cf::log
