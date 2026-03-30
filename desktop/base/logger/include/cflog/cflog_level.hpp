/**
 * @file    desktop/base/logger/include/cflog/cflog_level.hpp
 * @brief   Log level enumeration and utilities.
 *
 * Defines the log levels used by CFLog and utility functions
 * for converting and manipulating log levels.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog
 */
#pragma once
#include "base/macro/build_type.h"
#include <string_view>

namespace cf::log {

/**
 * @brief  Log severity levels.
 *
 * Defines the available log severity levels from most verbose
 * to most severe.
 *
 * @ingroup cflog
 */
enum class level {
    TRACE,   ///< Trace level for verbose output.
    DEBUG,   ///< Debug level for diagnostic information.
    INFO,    ///< Info level for general informational messages.
    WARNING, ///< Warning level for potentially harmful situations.
    ERROR    ///< Error level for error events.
};

static constexpr const level kDEFAULT_LEVEL =
#if defined(CFDESKTOP_DEBUG_BUILD) || defined(CFDESKTOP_DEVELOP_BUILD)
    level::TRACE;
#else
    level::INFO;
#endif

/**
 * @brief  Default minimum log level.
 *
 * Messages below this level are filtered out.
 */

/**
 * @brief  Converts a log level to its string representation.
 *
 * @param[in] lvl The log level to convert.
 * @return        String view containing the level name.
 * @throws        None
 * @note          None
 * @warning       None
 * @since         0.1
 * @ingroup       cflog
 */
[[nodiscard]] constexpr std::string_view to_string(level lvl) noexcept {
    switch (lvl) {
        case level::TRACE:
            return "TRACE";
        case level::DEBUG:
            return "DEBUG";
        case level::INFO:
            return "INFO";
        case level::WARNING:
            return "WARNING";
        case level::ERROR:
            return "ERROR";
    }
}

/**
 * @brief  Casts a log level to the specified type.
 *
 * @tparam T    Target type for the cast.
 * @param[in] lvl The log level to cast.
 * @return        The log level as the specified type.
 * @throws        None
 * @note          None
 * @warning       None
 * @since         0.1
 * @ingroup       cflog
 */
template <typename T = int> [[nodiscard]] constexpr T as(level lvl) noexcept {
    return static_cast<T>(lvl);
}

} // namespace cf::log
