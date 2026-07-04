/**
 * @file    desktop/base/logger/include/cflog/sinks/file_sink.h
 * @brief   File sink for writing log records to files.
 *
 * Defines the FileSink class that writes log records to a file.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include "cflog/cflog_export.h"
#include "cflog/cflog_sink.h"
#include <fstream>
#include <string>

namespace cf::log {

/**
 * @brief  File open modes for FileSink.
 *
 * Defines how the file sink opens the log file.
 *
 * @ingroup cflog
 */
enum class OpenMode {
    Append,  ///< Append to existing file.
    Truncate ///< Truncate file on open.
};

/**
 * @brief  Sink that writes log records to a file.
 *
 * Writes formatted log records to a file on disk.
 *
 * @ingroup cflog
 *
 * @note   Not thread-safe for multiple threads writing to the same file.
 *
 * @code
 * FileSink sink("app.log", OpenMode::Append);
 * sink.write(log_record);
 * sink.flush();
 * @endcode
 */
class CFLOG_API FileSink : public ISink {
  public:
    /**
     * @brief  Constructs a new FileSink.
     *
     * @param[in] filepath Path to the log file.
     * @param[in] mode     File open mode (append or truncate).
     * @throws             None
     * @note               The file is opened immediately.
     * @warning            None
     * @since              0.1
     * @ingroup            cflog
     */
    explicit FileSink(const std::string& filepath, OpenMode mode = OpenMode::Append);

    /**
     * @brief  Destroys the FileSink and closes the file.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    ~FileSink() override;

    // Disable copy
    FileSink(const FileSink&) = delete;
    FileSink& operator=(const FileSink&) = delete;

    /**
     * @brief  Writes a log record to the file.
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
     * @brief  Flushes the file output stream.
     *
     * @return true if the flush succeeded, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    bool flush() override;

  private:
    std::ofstream file_;   ///< Output file stream.
    std::string filepath_; ///< Path to the log file.
    OpenMode mode_;        ///< File open mode.
};

} // namespace cf::log
