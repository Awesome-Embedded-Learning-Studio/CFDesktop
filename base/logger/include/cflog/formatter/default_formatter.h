/**
 * @file default_formatter.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CFLog default formatter - simplest formatter that only outputs the message
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "cflog/cflog_format.h"
#include "cflog/cflog_record.h"

namespace cf::log {

/**
 * @brief Default formatter that outputs only the message content
 *
 * This is the simplest formatter implementation used as the default
 * for all Sinks. It returns the log message without any additional
 * formatting, prefixes, or metadata.
 */
class DefaultFormatter : public IFormatter {
  public:
    DefaultFormatter() = default;

    /**
     * @brief Format a log record - returns only the message content
     * @param r The log record to format
     * @return The raw message string
     */
    [[nodiscard]] std::string format_me(const LogRecord& r) override { return r.msg; }

    /**
     * @brief This formatter is not configurable
     * @return false
     */
    [[nodiscard]] bool configurable() const override { return false; }
};

} // namespace cf::log
