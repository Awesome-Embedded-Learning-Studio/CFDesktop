/**
 * @file proc_parser.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Modern C++17 utilities for parsing Linux /proc and /sys files
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace cf {

/**
 * @brief Parse a "key: value" line from /proc/cpuinfo or similar files
 * @param line The line to parse (as string_view to avoid copies)
 * @param field The field name to look for
 * @return string_view pointing to the value, or empty string_view if not found
 *
 * NOTE: The returned string_view is only valid as long as the input line is valid.
 *       Caller must ensure the source data outlives the returned view.
 */
std::string_view parse_cpuinfo_field(std::string_view line, std::string_view field) noexcept;

/**
 * @brief Trim whitespace from both ends of a string_view
 * @param sv The string view to trim
 * @return Trimmed string_view
 */
std::string_view trim_whitespace(std::string_view sv) noexcept;

/**
 * @brief Left-trim whitespace from a string_view
 * @param sv The string view to trim
 * @return Trimmed string_view
 */
std::string_view ltrim_whitespace(std::string_view sv) noexcept;

/**
 * @brief Right-trim whitespace from a string_view
 * @param sv The string view to trim
 * @return Trimmed string_view
 */
std::string_view rtrim_whitespace(std::string_view sv) noexcept;

/**
 * @brief Parse a cache size string (e.g., "32K", "1M", "2G") to kilobytes
 * @param size_str The size string (e.g., "32K", "1M")
 * @return Size in kilobytes, or nullopt if invalid format
 */
std::optional<uint32_t> parse_cache_size(std::string_view size_str) noexcept;

/**
 * @brief Parse a decimal string_view to uint32_t (no exceptions)
 * @param str The string view to parse
 * @return Parsed value, or nullopt if invalid
 */
std::optional<uint32_t> parse_uint32(std::string_view str) noexcept;

/**
 * @brief Parse a hexadecimal string_view to uint32_t (no exceptions)
 * @param str The string view to parse (may include "0x" prefix)
 * @return Parsed value, or nullopt if invalid
 */
std::optional<uint32_t> parse_hex_uint32(std::string_view str) noexcept;

/**
 * @brief Read a single uint32_t value from a file
 * @param path Path to the file
 * @return The value, or nullopt if file cannot be read
 */
std::optional<uint32_t> read_uint32_file(const char* path) noexcept;

/**
 * @brief Get ARM vendor name from implementer ID
 * @param impl_val The implementer ID (e.g., 0x41 for ARM)
 * @return Vendor name as string_view, or "Unknown" if not found
 */
std::string_view arm_implementer_to_vendor(uint32_t impl_val) noexcept;

} // namespace cf
