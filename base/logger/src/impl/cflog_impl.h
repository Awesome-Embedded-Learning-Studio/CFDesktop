/**
 * @file    desktop/base/logger/src/impl/cflog_impl.h
 * @brief   Internal implementation of the CFLog logger.
 *
 * Provides the internal LoggerImpl class that manages log processing,
 * sink management, and asynchronous queue operations.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog_internal
 */
#pragma once

#include "cflog/cflog.hpp"
#include "cflog/cflog_sink.h"
#include <memory>

namespace cf::log {

class AsyncPostQueue;

/**
 * @brief  Internal implementation of the CFLog logger.
 *
 * Manages the asynchronous logging queue, log level filtering,
 * and sink management. This class is not part of the public API.
 *
 * @ingroup cflog_internal
 *
 * @note   Not thread-safe for multiple consumer operations.
 */
class LoggerImpl {
  public:
    /**
     * @brief  Constructs a new LoggerImpl.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    LoggerImpl();

    /**
     * @brief  Destroys the LoggerImpl.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    ~LoggerImpl();

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
     * @ingroup             cflog_internal
     */
    bool log(level log_level, std::string_view msg, std::string_view tag, std::source_location loc);

    /**
     * @brief  Asynchronously flushes pending log messages.
     *
     * Signals the queue to flush but does not wait for completion.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
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
     * @ingroup cflog_internal
     */
    void flush_sync();

    /**
     * @brief  Adds a sink to the logger.
     *
     * @param[in] sink Shared pointer to the sink to add.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog_internal
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
     * @ingroup    cflog_internal
     */
    void remove_sink(ISink* sink);

    /**
     * @brief  Removes all sinks from the logger.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void clear_sinks();

  private:
    std::unique_ptr<AsyncPostQueue> post_queue; ///< Async queue for log processing.
};

} // namespace cf::log
