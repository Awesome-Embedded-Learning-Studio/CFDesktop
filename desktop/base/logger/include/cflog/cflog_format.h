/**
 * @file    desktop/base/logger/include/cflog/cflog_format.h
 * @brief   Interface for formatting log records.
 *
 * Defines the IFormatter interface that all formatters must implement.
 * Formatters convert log records into human-readable strings.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include <memory>
#include <string>

namespace cf::log {

// Forward declarations
struct LogRecord;
class FormatterConfig;

/**
 * @brief  Interface for formatting log records.
 *
 * Formatters convert log records into human-readable strings
 * according to their specific formatting rules.
 *
 * @ingroup cflog
 *
 * @note   Implementations should be thread-safe for format operations.
 *
 * @code
 * class MyFormatter : public IFormatter {
 * public:
 *     std::string format_me(const LogRecord& r) override {
 *         // Format the record
 *         return formatted_string;
 *     }
 * };
 * @endcode
 */
class IFormatter {
  public:
    /**
     * @brief  Virtual destructor.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    virtual ~IFormatter() = default;

    /**
     * @brief  Formats a log record into a string.
     *
     * @param[in] r The log record to format.
     * @return     Formatted string representation of the record.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog
     */
    [[nodiscard]] virtual std::string format_me(const LogRecord& r) = 0;

    /**
     * @brief  Checks if this formatter supports runtime configuration.
     *
     * @return true if configurable, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    [[nodiscard]] virtual bool configurable() const { return false; }

    /**
     * @brief  Sets the formatter configuration.
     *
     * @param[in] config The configuration to apply.
     * @return           true if configuration was applied, false if not supported.
     * @throws           None
     * @note             Default implementation returns false.
     * @warning          None
     * @since            0.1
     * @ingroup          cflog
     */
    virtual bool set_config(std::shared_ptr<FormatterConfig> config) {
        // Default: not configurable
        (void)config;
        return false;
    }

    /**
     * @brief  Gets the current formatter configuration.
     *
     * @return Current configuration or nullptr if not configured.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog
     */
    [[nodiscard]] virtual std::shared_ptr<FormatterConfig> get_config() const { return nullptr; }
};

} // namespace cf::log
