/**
 * @file    desktop/base/logger/include/cflog/cflog_record.h
 * @brief   Log record structure for CFLog.
 *
 * Defines the LogRecord structure that holds all information
 * about a single log event.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include <chrono>
#include <source_location>
#include <string>
#include <thread>

namespace cf::log {

/// @brief Forward declaration of log level enumeration.
enum class level : int;

/// @brief Timestamp type for log records.
using cflog_timestamp_t = std::chrono::system_clock::time_point;

/**
 * @brief  Log record containing all information about a log event.
 *
 * Stores the log level, tag, message, timestamp, thread ID,
 * and source location of a log event.
 *
 * @ingroup cflog
 */
struct LogRecord {
    level lvl;                   ///< Log level of this record.
    std::string tag;             ///< Tag associated with this record.
    std::string msg;             ///< Log message content.
    cflog_timestamp_t timestamp; ///< Timestamp when the log was created.
    std::thread::id tid;         ///< Thread ID that created the log.
    std::source_location loc;    ///< Source code location of the log call.

    // Explicit move operations to ensure efficient transfer
    LogRecord(LogRecord&&) noexcept = default;
    LogRecord& operator=(LogRecord&&) noexcept = default;

    // Keep copy operations as default
    LogRecord(const LogRecord&) = default;
    LogRecord& operator=(const LogRecord&) = default;

    // Default constructor
    LogRecord() = default;
};

} // namespace cf::log
