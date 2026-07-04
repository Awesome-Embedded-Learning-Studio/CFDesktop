/**
 * @file proc_parser.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Modern C++17 utilities for parsing Linux /proc and /sys files
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "base/linux/proc_parser.h"

#include <cstdio>

namespace cf {

namespace {

// Compile-time ARM implementer to vendor mapping table
struct ArmImplementer {
    uint32_t code;
    const char* name;
};

constexpr ArmImplementer arm_implementers[] = {
    {0x41, "ARM"}, {0x42, "Broadcom"}, {0x43, "Cavium"},  {0x44, "DEC"},     {0x4E, "NVIDIA"},
    {0x50, "APM"}, {0x51, "Qualcomm"}, {0x53, "Samsung"}, {0x56, "Marvell"}, {0x69, "Intel"}};

constexpr size_t num_implementers = sizeof(arm_implementers) / sizeof(arm_implementers[0]);

} // anonymous namespace

std::string_view trim_whitespace(std::string_view sv) noexcept {
    return ltrim_whitespace(rtrim_whitespace(sv));
}

std::string_view ltrim_whitespace(std::string_view sv) noexcept {
    const size_t start = sv.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos) {
        return {};
    }
    return sv.substr(start);
}

std::string_view rtrim_whitespace(std::string_view sv) noexcept {
    const size_t end = sv.find_last_not_of(" \t\r\n");
    if (end == std::string_view::npos) {
        return {};
    }
    return sv.substr(0, end + 1);
}

std::string_view parse_cpuinfo_field(std::string_view line, std::string_view field) noexcept {
    // Find colon separator
    const size_t colon_pos = line.find(':');
    if (colon_pos == std::string_view::npos) {
        return {};
    }

    // Get key and value as views (no copies!)
    const std::string_view key = line.substr(0, colon_pos);
    const std::string_view value = line.substr(colon_pos + 1);

    // Trim key
    const std::string_view trimmed_key = ltrim_whitespace(key);

    // Check if key matches requested field
    if (trimmed_key != field) {
        return {};
    }

    // Trim and return value
    return ltrim_whitespace(value);
}

std::optional<uint32_t> parse_cache_size(std::string_view size_str) noexcept {
    if (size_str.empty()) {
        return std::nullopt;
    }

    // Parse number manually to avoid std::stoul exceptions
    uint32_t size = 0;
    size_t pos = 0;

    for (; pos < size_str.size(); ++pos) {
        const char c = size_str[pos];
        if (c >= '0' && c <= '9') {
            // Check for overflow
            if (size > (UINT32_MAX - 9) / 10) {
                return std::nullopt;
            }
            size = size * 10 + static_cast<uint32_t>(c - '0');
        } else {
            break; // Reached unit specifier
        }
    }

    if (pos == 0) {
        return std::nullopt; // No digits found
    }

    // Parse unit
    if (pos < size_str.size()) {
        const char unit = size_str[pos];
        if (unit == 'K' || unit == 'k') {
            // Already in KB
        } else if (unit == 'M' || unit == 'm') {
            if (size > UINT32_MAX / 1024) {
                return std::nullopt; // Overflow
            }
            size *= 1024;
        } else if (unit == 'G' || unit == 'g') {
            if (size > UINT32_MAX / (1024 * 1024)) {
                return std::nullopt; // Overflow
            }
            size *= 1024 * 1024;
        }
    }

    return size;
}

std::optional<uint32_t> parse_uint32(std::string_view str) noexcept {
    if (str.empty()) {
        return std::nullopt;
    }

    uint32_t value = 0;
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            // Check for overflow
            if (value > (UINT32_MAX - 9) / 10) {
                return std::nullopt;
            }
            value = value * 10 + static_cast<uint32_t>(c - '0');
        } else {
            return std::nullopt; // Invalid character
        }
    }

    return value;
}

std::optional<uint32_t> parse_hex_uint32(std::string_view str) noexcept {
    if (str.empty()) {
        return std::nullopt;
    }

    // Skip "0x" or "0X" prefix if present
    if (str.size() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str = str.substr(2);
    }

    if (str.empty()) {
        return std::nullopt;
    }

    uint32_t value = 0;
    for (char c : str) {
        value <<= 4;
        if (c >= '0' && c <= '9') {
            value |= static_cast<uint32_t>(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            value |= static_cast<uint32_t>(c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            value |= static_cast<uint32_t>(c - 'A' + 10);
        } else {
            return std::nullopt; // Invalid hex digit
        }
    }

    return value;
}

std::optional<uint32_t> read_uint32_file(const char* path) noexcept {
    FILE* file = fopen(path, "r");
    if (!file) {
        return std::nullopt;
    }

    uint32_t value = 0;
    if (fscanf(file, "%u", &value) != 1) {
        fclose(file);
        return std::nullopt;
    }

    fclose(file);
    return value;
}

std::string_view arm_implementer_to_vendor(uint32_t impl_val) noexcept {
    for (size_t i = 0; i < num_implementers; ++i) {
        if (arm_implementers[i].code == impl_val) {
            return arm_implementers[i].name;
        }
    }
    return "Unknown";
}

} // namespace cf
