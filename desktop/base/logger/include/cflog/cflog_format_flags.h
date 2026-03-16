/**
 * @file cflog_format_flags.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CFLog formatter flags for configurable component output
 * @version 0.1
 * @date 2026-03-16
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <cstdint>

namespace cf::log {

/**
 * @brief Flags for controlling which log components are formatted
 *
 * These flags can be combined using bitwise operators to configure
 * which components appear in formatted log output.
 */
enum FormatterFlag : uint32_t {
    NONE = 0,
    TIMESTAMP = 1 << 0,       ///< Format timestamp
    LEVEL = 1 << 1,           ///< Format log level
    TAG = 1 << 2,             ///< Format tag
    THREAD_ID = 1 << 3,       ///< Format thread ID
    SOURCE_LOCATION = 1 << 4, ///< Format file:line
    MESSAGE = 1 << 5,         ///< Format message (usually always needed)
    COLOR = 1 << 6,           ///< Enable ANSI colors

    // Common presets
    DEFAULT = TIMESTAMP | LEVEL | TAG | SOURCE_LOCATION | MESSAGE,
    MINIMAL = LEVEL | MESSAGE,
    VERBOSE = TIMESTAMP | LEVEL | TAG | THREAD_ID | SOURCE_LOCATION | MESSAGE,
};

// Bitwise operators for combining flags
[[nodiscard]] constexpr FormatterFlag operator|(FormatterFlag a, FormatterFlag b) {
    return static_cast<FormatterFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

[[nodiscard]] constexpr FormatterFlag operator&(FormatterFlag a, FormatterFlag b) {
    return static_cast<FormatterFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

[[nodiscard]] constexpr FormatterFlag operator^(FormatterFlag a, FormatterFlag b) {
    return static_cast<FormatterFlag>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

[[nodiscard]] constexpr FormatterFlag operator~(FormatterFlag a) {
    return static_cast<FormatterFlag>(~static_cast<uint32_t>(a));
}

constexpr FormatterFlag& operator|=(FormatterFlag& a, FormatterFlag b) {
    return a = a | b;
}

constexpr FormatterFlag& operator&=(FormatterFlag& a, FormatterFlag b) {
    return a = a & b;
}

constexpr FormatterFlag& operator^=(FormatterFlag& a, FormatterFlag b) {
    return a = a ^ b;
}

} // namespace cf::log
