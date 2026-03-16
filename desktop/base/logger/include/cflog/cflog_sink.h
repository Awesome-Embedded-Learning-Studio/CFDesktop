/**
 * @file    desktop/base/logger/include/cflog/cflog_sink.h
 * @brief   Sink interface for writing log records.
 *
 * Defines the ISink interface that all log sinks must implement.
 * Sinks are responsible for writing formatted log records to
 * their respective outputs (console, file, etc.).
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include <memory>

namespace cf::log {

class IFormatter;
struct LogRecord;

/**
 * @brief  Interface for log output sinks.
 *
 * Sinks receive formatted log records and write them to
 * their respective outputs. Implementations may write to
 * console, files, network, or other destinations.
 *
 * @ingroup cflog
 *
 * @note   Implementations should be thread-safe for write operations.
 *
 * @code
 * class MySink : public ISink {
 * public:
 *     bool write(const LogRecord& record) override {
 *         // Write record to output
 *         return true;
 *     }
 *     bool flush() override {
 *         // Flush output
 *         return true;
 *     }
 * };
 * @endcode
 */
class ISink {
  public:
    /**
     * @brief  Virtual destructor.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    virtual ~ISink() = default;

    /**
     * @brief  Writes a log record to the sink output.
     *
     * @param[in] record The log record to write.
     * @return          true if the write succeeded, false otherwise.
     * @throws          None
     * @note            None
     * @warning         None
     * @since           0.1
     * @ingroup         cflog
     */
    virtual bool write(const LogRecord& record) = 0;

    /**
     * @brief  Flushes any buffered output.
     *
     * @return true if the flush succeeded, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    virtual bool flush() = 0;

    /**
     * @brief  Sets the formatter for this sink.
     *
     * @param[in] formatter Shared pointer to the formatter to use.
     * @return              true if the formatter was set, false otherwise.
     * @throws              None
     * @note                None
     * @warning             None
     * @since               0.1
     * @ingroup             cflog
     */
    virtual bool setFormat(std::shared_ptr<IFormatter> formatter) {
        formatter_ = std::move(formatter);
        return true;
    }

  protected:
    /**
     * @brief  Checks if this sink has a formatter configured.
     *
     * @return true if a formatter is available, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    virtual bool formatable() const { return formatter_ != nullptr; }

    /**
     * @brief  Applies formatting to the output.
     *
     * @return true if formatting succeeded, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    virtual bool actFormat() { return true; }

    /// @brief Pointer to the formatter for this sink.
    std::shared_ptr<IFormatter> formatter_;
};

} // namespace cf::log
