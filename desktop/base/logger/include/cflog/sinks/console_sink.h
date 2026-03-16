/**
 * @file    desktop/base/logger/include/cflog/sinks/console_sink.h
 * @brief   Console sink for writing log records to stdout.
 *
 * Defines the ConsoleSink class that writes log records to
 * the standard output stream.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include "cflog/cflog_sink.h"

namespace cf::log {

/**
 * @brief  Sink that writes log records to the console.
 *
 * Writes formatted log records to stdout for immediate
 * display to the user.
 *
 * @ingroup cflog
 *
 * @note   Not thread-safe for multiple threads writing simultaneously.
 *
 * @code
 * ConsoleSink sink;
 * sink.write(log_record);
 * sink.flush();
 * @endcode
 */
class ConsoleSink : public ISink {
  public:
    /**
     * @brief  Writes a log record to the console.
     *
     * @param[in] record The log record to write.
     * @return          true if the write succeeded, false otherwise.
     * @throws          None
     * @note            None
     * @warning         None
     * @since           0.1
     * @ingroup         cflog
     */
    bool write(const LogRecord& record) override;

    /**
     * @brief  Flushes the console output stream.
     *
     * @return true if the flush succeeded, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    bool flush() override;
};

} // namespace cf::log
