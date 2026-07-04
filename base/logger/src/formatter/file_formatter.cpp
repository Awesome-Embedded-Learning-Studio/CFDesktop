/**
 * @file file_formatter.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CFLog file formatter implementation
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cflog/formatter/file_formatter.h"
#include "cflog/cflog_format_config.h"
#include "cflog/cflog_format_flags.h"
#include "cflog/cflog_level.hpp"
#include "cflog/cflog_record.h"
#include <chrono>
#include <ctime>
#include <format>
#include <sstream>

namespace cf::log {

namespace {
constexpr std::string_view level_short(level lvl) {
    switch (lvl) {
        case level::TRACE:
            return "TRC";
        case level::DEBUG:
            return "DBG";
        case level::INFO:
            return "INF";
        case level::WARNING:
            return "WRN";
        case level::ERROR:
            return "ERR";
    }
    return "???";
}

auto get_time_parts(const cflog_timestamp_t& tp) {
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(tp);
    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(ms);
    int frac = static_cast<int>((ms - secs).count());

    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return std::make_tuple(tm, frac);
}

std::string_view basename(std::string_view path) {
    size_t pos = path.find_last_of("/\\");
    return pos != std::string_view::npos ? path.substr(pos + 1) : path;
}

constexpr const char* separator = " ";
constexpr const char* source_separator = " | ";
} // namespace

// ============================================================================
// Constructor
// ============================================================================

FileFormatter::FileFormatter(FormatterFlag flags)
    : config_(std::make_shared<FormatterConfig>(flags)) {
    // Always disable COLOR for file output
    config_->disable(FormatterFlag::COLOR);
}

// ============================================================================
// IFormatter implementation
// ============================================================================

bool FileFormatter::set_config(std::shared_ptr<FormatterConfig> config) {
    if (!config)
        return false;
    config_ = std::move(config);
    // Ensure color is always disabled for file output
    config_->disable(FormatterFlag::COLOR);
    return true;
}

std::shared_ptr<FormatterConfig> FileFormatter::get_config() const {
    return config_;
}

// ============================================================================
// Component formatting helpers (plain text, no ANSI colors)
// ============================================================================

std::string FileFormatter::format_timestamp(const LogRecord& r) const {
    auto [tm, frac] = get_time_parts(r.timestamp);
    const auto& fmt = config_->get_timestamp_format();

    char time_buf[64] = {0};
    std::strftime(time_buf, sizeof(time_buf), fmt.c_str(), &tm);

    return std::format("[{}.{:03d}]", time_buf, frac);
}

std::string FileFormatter::format_level(level lvl) const {
    auto short_lvl = level_short(lvl);
    return std::format("[{}]", short_lvl);
}

std::string FileFormatter::format_tag(const std::string& tag) const {
    if (tag.empty())
        return "";
    return std::format("[{}] ", tag);
}

std::string FileFormatter::format_thread_id(const std::thread::id& tid) const {
    // Format thread ID as hexadecimal (e.g., 0x3039)
    std::stringstream ss;
    ss << std::hex << tid;

    std::string tid_str = ss.str();
    // Add 0x prefix
    if (tid_str.empty() || tid_str[0] != '0') {
        tid_str = "0x" + tid_str;
    }

    return std::format("[TID:{}]", tid_str);
}

std::string FileFormatter::format_source_location(const std::source_location& loc) const {
    auto file = basename(loc.file_name());
    return std::format("[{}:{}]", file, loc.line());
}

std::string FileFormatter::format_message(const std::string& msg) const {
    return msg + "\n";
}

// ============================================================================
// Main formatting method
// ============================================================================

std::string FileFormatter::format_me(const LogRecord& r) {
    if (!config_) {
        // Fallback to basic format if no config
        return std::format("[{}]", r.msg);
    }

    auto flags = config_->get_flags();
    std::string result;

    // Helper to add separator between components
    auto add_sep = [&]() {
        if (!result.empty()) {
            result += separator;
        }
    };

    // Format timestamp
    if (flags & FormatterFlag::TIMESTAMP) {
        result += format_timestamp(r);
    }

    // Format level
    if (flags & FormatterFlag::LEVEL) {
        add_sep();
        result += format_level(r.lvl);
    }

    // Format tag
    if (flags & FormatterFlag::TAG && !r.tag.empty()) {
        add_sep();
        result += format_tag(r.tag);
    }

    // Format thread ID
    if (flags & FormatterFlag::THREAD_ID) {
        add_sep();
        result += format_thread_id(r.tid);
    }

    // Format source location
    if (flags & FormatterFlag::SOURCE_LOCATION) {
        add_sep();
        result += format_source_location(r.loc);
    }

    // Format message (separator depends on whether we have other components)
    if (flags & FormatterFlag::MESSAGE) {
        if (!result.empty()) {
            result += source_separator;
        }
        result += format_message(r.msg);
    }

    return result;
}

} // namespace cf::log
