/**
 * @file cflog.h
 * @brief Convenient logging functions for CFLog
 * @date 2026-03-16
 */
#pragma once

#include "cflog/cflog.hpp"
#include "cflog/cflog_export.h"
#include <format>
#include <source_location>
#include <string_view>

namespace cf::log {

CFLOG_API void trace(std::string_view msg, std::string_view tag = "CFLog",
                     std::source_location loc = std::source_location::current());

CFLOG_API void debug(std::string_view msg, std::string_view tag = "CFLog",
                     std::source_location loc = std::source_location::current());

CFLOG_API void info(std::string_view msg, std::string_view tag = "CFLog",
                    std::source_location loc = std::source_location::current());

CFLOG_API void warning(std::string_view msg, std::string_view tag = "CFLog",
                       std::source_location loc = std::source_location::current());

CFLOG_API void error(std::string_view msg, std::string_view tag = "CFLog",
                     std::source_location loc = std::source_location::current());

// ============================================================================
// Formatted logging functions (std::format style)
// ============================================================================

namespace detail {
/**
 * @brief  Helper struct for formatted logging at compile-time specified level.
 *
 * Constructs a temporary object that formats and logs a message upon creation.
 * Uses std::format for argument formatting and captures source location.
 *
 * @tparam Level Compile-time log level for this helper.
 * @tparam Args Types of format arguments.
 * @param[in] tag     Log tag identifying the source.
 * @param[in] fmt     Format string following std::format specifications.
 * @param[in] args    Arguments to format into the log message.
 * @param[in] loc     Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <level Level, typename... Args> struct log_helper {
    log_helper(std::string_view tag, std::format_string<Args...> fmt, Args&&... args,
               std::source_location loc = std::source_location::current()) {
        Logger::instance().log(Level, std::format(fmt, std::forward<Args>(args)...), tag, loc);
    }
};
} // namespace detail

// Default tag versions (tag = "CFLog")
/**
 * @brief  Temporary object for trace-level formatted logging with default tag.
 *
 * Creates a temporary object that logs a TRACE level message upon construction
 * using std::format syntax. Uses "CFLog" as the default tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct tracef {
    tracef(std::format_string<Args...> fmt, Args&&... args,
           std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::TRACE, Args...>{"CFLog", fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> tracef(std::format_string<Args...>, Args&&...) -> tracef<Args...>;

/**
 * @brief  Temporary object for debug-level formatted logging with default tag.
 *
 * Creates a temporary object that logs a DEBUG level message upon construction
 * using std::format syntax. Uses "CFLog" as the default tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct debugf {
    debugf(std::format_string<Args...> fmt, Args&&... args,
           std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::DEBUG, Args...>{"CFLog", fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> debugf(std::format_string<Args...>, Args&&...) -> debugf<Args...>;

/**
 * @brief  Temporary object for info-level formatted logging with default tag.
 *
 * Creates a temporary object that logs an INFO level message upon construction
 * using std::format syntax. Uses "CFLog" as the default tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct infof {
    infof(std::format_string<Args...> fmt, Args&&... args,
          std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::INFO, Args...>{"CFLog", fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> infof(std::format_string<Args...>, Args&&...) -> infof<Args...>;

/**
 * @brief  Temporary object for warning-level formatted logging with default tag.
 *
 * Creates a temporary object that logs a WARNING level message upon construction
 * using std::format syntax. Uses "CFLog" as the default tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct warningf {
    warningf(std::format_string<Args...> fmt, Args&&... args,
             std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::WARNING, Args...>{"CFLog", fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> warningf(std::format_string<Args...>, Args&&...) -> warningf<Args...>;

/**
 * @brief  Temporary object for error-level formatted logging with default tag.
 *
 * Creates a temporary object that logs an ERROR level message upon construction
 * using std::format syntax. Uses "CFLog" as the default tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct errorf {
    errorf(std::format_string<Args...> fmt, Args&&... args,
           std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::ERROR, Args...>{"CFLog", fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> errorf(std::format_string<Args...>, Args&&...) -> errorf<Args...>;

// Custom tag versions
/**
 * @brief  Temporary object for trace-level formatted logging with custom tag.
 *
 * Creates a temporary object that logs a TRACE level message upon construction
 * using std::format syntax and a caller-specified tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] tag  Custom log tag identifying the source.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct traceftag {
    traceftag(std::string_view tag, std::format_string<Args...> fmt, Args&&... args,
              std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::TRACE, Args...>{tag, fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> traceftag(std::string_view, std::format_string<Args...>, Args&&...)
    -> traceftag<Args...>;

/**
 * @brief  Temporary object for debug-level formatted logging with custom tag.
 *
 * Creates a temporary object that logs a DEBUG level message upon construction
 * using std::format syntax and a caller-specified tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] tag  Custom log tag identifying the source.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct debugftag {
    debugftag(std::string_view tag, std::format_string<Args...> fmt, Args&&... args,
              std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::DEBUG, Args...>{tag, fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> debugftag(std::string_view, std::format_string<Args...>, Args&&...)
    -> debugftag<Args...>;

/**
 * @brief  Temporary object for info-level formatted logging with custom tag.
 *
 * Creates a temporary object that logs an INFO level message upon construction
 * using std::format syntax and a caller-specified tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] tag  Custom log tag identifying the source.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct infoftag {
    infoftag(std::string_view tag, std::format_string<Args...> fmt, Args&&... args,
             std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::INFO, Args...>{tag, fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> infoftag(std::string_view, std::format_string<Args...>, Args&&...)
    -> infoftag<Args...>;

/**
 * @brief  Temporary object for warning-level formatted logging with custom tag.
 *
 * Creates a temporary object that logs a WARNING level message upon construction
 * using std::format syntax and a caller-specified tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] tag  Custom log tag identifying the source.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct warningftag {
    warningftag(std::string_view tag, std::format_string<Args...> fmt, Args&&... args,
                std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::WARNING, Args...>{tag, fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> warningftag(std::string_view, std::format_string<Args...>, Args&&...)
    -> warningftag<Args...>;

/**
 * @brief  Temporary object for error-level formatted logging with custom tag.
 *
 * Creates a temporary object that logs an ERROR level message upon construction
 * using std::format syntax and a caller-specified tag.
 *
 * @tparam Args Types of format arguments.
 * @param[in] tag  Custom log tag identifying the source.
 * @param[in] fmt  Format string following std::format specifications.
 * @param[in] args Arguments to format into the log message.
 * @param[in] loc  Source location where the log was triggered.
 * @throws            None
 * @note              None
 * @warning           None
 * @since             N/A
 * @ingroup           logger
 */
template <typename... Args> struct errorftag {
    errorftag(std::string_view tag, std::format_string<Args...> fmt, Args&&... args,
              std::source_location loc = std::source_location::current()) {
        detail::log_helper<level::ERROR, Args...>{tag, fmt, std::forward<Args>(args)..., loc};
    }
};
template <typename... Args> errorftag(std::string_view, std::format_string<Args...>, Args&&...)
    -> errorftag<Args...>;

/**
 * @brief  Sets the minimum log level.
 *
 * Only messages at or above this level are processed.
 *
 * @param[in] lvl Minimum log level to set.
 * @return    None
 * @throws     None
 * @note       None
 * @warning    None
 * @since      N/A
 * @ingroup    cflog
 */
CFLOG_API void set_level(level lvl);

/**
 * @brief  Flushes all pending log messages.
 *
 * Blocks until all queued messages are written to sinks.
 *
 * @return    None
 * @throws     None
 * @note       This operation may block if the queue is full.
 * @warning    None
 * @since      N/A
 * @ingroup    cflog
 */
CFLOG_API void flush();

} // namespace cf::log
