/**
 * @file    desktop/base/logger/include/cflog/formatter/console_formatter.h
 * @brief   Console formatter with ASCII color support.
 *
 * Defines the AsciiColorFormatter class that formats log records
 * with ASCII color codes for console output.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once

#include "cflog/cflog_export.h"
#include "cflog/cflog_format.h"
#include "cflog/cflog_format_config.h"
#include <memory>
#include <source_location>
#include <thread>

namespace cf::log {

// Forward declarations
enum class level : int;

/**
 * @brief  ASCII color formatter for console output.
 *
 * Formats log records with ASCII color codes for improved
 * readability in console terminals.
 *
 * @ingroup cflog
 *
 * @note   Thread-safe for format operations.
 */
class CFLOG_API AsciiColorFormatter : public IFormatter {
  public:
    /**
     * @brief  Constructs a new AsciiColorFormatter.
     *
     * @param[in] flags Initial formatter flags (default: DEFAULT).
     * @throws          None
     * @note            None
     * @warning         None
     * @since           0.1
     * @ingroup         cflog
     */
    explicit AsciiColorFormatter(FormatterFlag flags = FormatterFlag::DEFAULT);

    // IFormatter implementation
    [[nodiscard]] std::string format_me(const LogRecord& r) override;
    [[nodiscard]] bool configurable() const override { return true; }

    /**
     * @brief  Sets the formatter configuration.
     *
     * @param[in] config The configuration to apply.
     * @return           true if configuration was applied, false if not supported.
     * @throws           None
     * @note             None
     * @warning          None
     * @since            0.1
     * @ingroup          cflog
     */
    bool set_config(std::shared_ptr<FormatterConfig> config) override;

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
    [[nodiscard]] std::shared_ptr<FormatterConfig> get_config() const override;

  protected:
    std::shared_ptr<FormatterConfig> config_; ///< Formatter configuration.

    // Component formatting helpers (only called when enabled)
    std::string format_timestamp(const LogRecord& r) const;
    std::string format_level(level lvl) const;
    std::string format_tag(const std::string& tag) const;
    std::string format_thread_id(const std::thread::id& tid) const;
    std::string format_source_location(const std::source_location& loc) const;
    std::string format_message(const std::string& msg) const;
};

} // namespace cf::log
