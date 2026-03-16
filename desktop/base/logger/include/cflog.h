/**
 * @file cflog.h
 * @brief Convenient logging functions for CFLog
 * @date 2026-03-16
 */
#pragma once

#include "cflog/cflog_level.hpp"
#include <source_location>
#include <string_view>

namespace cf::log {

void trace(std::string_view msg, std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());

void debug(std::string_view msg, std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());

void info(std::string_view msg, std::string_view tag = "CFLog",
          std::source_location loc = std::source_location::current());

void warning(std::string_view msg, std::string_view tag = "CFLog",
             std::source_location loc = std::source_location::current());

void error(std::string_view msg, std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());

/**
 * @brief  Sets the minimum log level.
 *
 * Only messages at or above this level are processed.
 *
 * @param[in] lvl Minimum log level to set.
 * @throws     None
 * @note       None
 * @warning    None
 * @since      N/A
 * @ingroup    cflog
 */
void set_level(level lvl);

/**
 * @brief  Flushes all pending log messages.
 *
 * Blocks until all queued messages are written to sinks.
 *
 * @throws     None
 * @note       This operation may block if the queue is full.
 * @warning    None
 * @since      N/A
 * @ingroup    cflog
 */
void flush();

} // namespace cf::log
