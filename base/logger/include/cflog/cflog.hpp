/**
 * @file    desktop/base/logger/include/cflog/cflog.hpp
 * @brief   Main logger interface for CFLog.
 *
 * Provides the Logger class which is the main entry point for
 * logging operations in CFLog.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include "cflog_export.h"
#include "cflog_level.hpp"
#include <atomic>
#include <memory>
#include <source_location>
#include <string_view>

namespace cf::log {

class LoggerImpl; // Hide The Impl
class ISink;

/**
 * @brief  Main logger class for CFLog.
 *
 * Provides thread-safe logging with level filtering and
 * multiple sink support.
 *
 * @ingroup cflog
 *
 * @note   Thread-safe for all operations.
 *
 * @code
 * auto& logger = Logger::instance();
 * logger.log(level::INFO, "Hello, world!", "MyApp", std::source_location::current());
 * @endcode
 */
class CFLOG_API Logger {
  public:
    /**
     * @brief  Constructor.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    Logger();

    /**
     * @brief  Destructor.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    ~Logger();

    /**
     * @brief   Intentionally leaks to avoid deadlock on Ctrl+C
     *          that occurs with Meyer Singletons.
     *
     * @return Logger&
     */
    static Logger& instance();

    /**
     * @brief  Logs a message with the specified level and metadata.
     *
     * @param[in] log_level The log level of this message.
     * @param[in] msg       The message content.
     * @param[in] tag       The tag associated with this message.
     * @param[in] loc       Source code location of the log call.
     * @return              true if the message was queued, false if dropped.
     * @throws              None
     * @note                Messages below the minimum level are dropped.
     * @warning             None
     * @since               0.1
     * @ingroup             cflog
     */
    bool log(level log_level, std::string_view msg, std::string_view tag, std::source_location loc);

    /**
     * @brief  Asynchronously flushes pending log messages.
     *
     * Returns immediately without waiting for completion.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    void flush();

    /**
     * @brief  Synchronously flushes pending log messages.
     *
     * Waits for all pending messages to be processed.
     *
     * @throws None
     * @note   This operation blocks until the queue is empty.
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    void flush_sync();

    /**
     * @brief  Sets the minimum log level.
     *
     * @param[in] lvl Minimum log level to set.
     * @throws     None
     * @note       Messages below this level are filtered out.
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void setMininumLevel(const level lvl) { minimal_level = lvl; }

    /**
     * @brief  Adds a sink to receive log records.
     *
     * @param[in] sink Shared pointer to the sink to add.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void add_sink(std::shared_ptr<ISink> sink);

    /**
     * @brief  Removes a sink from the logger.
     *
     * @param[in] sink Pointer to the sink to remove.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    void remove_sink(ISink* sink);

    /**
     * @brief  Removes all sinks from the logger.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    void clear_sinks();

  private:
    std::atomic<level> minimal_level{kDEFAULT_LEVEL}; ///< Minimum log level.
    std::shared_ptr<LoggerImpl> logger_impl;          ///< Internal implementation.
};

} // namespace cf::log
