/**
 * @file    base/include/base/linux/proc_parser.h
 * @brief   Provides utilities for parsing Linux /proc and /sys files.
 *
 * Offers modern C++17 utilities for parsing common Linux pseudo-filesystem
 * formats without exceptions or allocations where possible.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup base_linux
 */
#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace cf {

/**
 * @brief  Parses a "key: value" line from /proc/cpuinfo or similar files.
 *
 * Searches for the specified field name and returns the corresponding
 * value as a string view. The returned view is only valid as long as
 * the input line remains valid.
 *
 * @param[in] line  The line to parse. Expected format: "key: value".
 * @param[in] field The field name to search for.
 *
 * @return          string_view pointing to the value, or empty
 *                  string_view if the field is not found.
 *
 * @throws          None (returns empty string_view on error).
 *
 * @note            The returned string_view is only valid as long as the
 *                  input line is valid. The calling code must ensure
 *                  the source data outlives the returned view.
 *
 * @warning         The input line must contain a colon ':' separator.
 *
 * @since           0.1
 * @ingroup         base_linux
 */
std::string_view parse_cpuinfo_field(std::string_view line, std::string_view field) noexcept;

/**
 * @brief  Removes whitespace from both ends of a string view.
 *
 * @param[in] sv The string view to trim.
 *
 * @return        Trimmed string view with leading and trailing
 *                whitespace removed.
 *
 * @throws        None.
 *
 * @note          Whitespace includes spaces, tabs, newlines, and
 *                carriage returns.
 *
 * @warning       None.
 *
 * @since         0.1
 * @ingroup       base_linux
 */
std::string_view trim_whitespace(std::string_view sv) noexcept;

/**
 * @brief  Removes leading whitespace from a string view.
 *
 * @param[in] sv The string view to trim.
 *
 * @return        Trimmed string view with leading whitespace removed.
 *
 * @throws        None.
 *
 * @note          Whitespace includes spaces, tabs, newlines, and
 *                carriage returns.
 *
 * @warning       None.
 *
 * @since         0.1
 * @ingroup       base_linux
 */
std::string_view ltrim_whitespace(std::string_view sv) noexcept;

/**
 * @brief  Removes trailing whitespace from a string view.
 *
 * @param[in] sv The string view to trim.
 *
 * @return        Trimmed string view with trailing whitespace removed.
 *
 * @throws        None.
 *
 * @note          Whitespace includes spaces, tabs, newlines, and
 *                carriage returns.
 *
 * @warning       None.
 *
 * @since         0.1
 * @ingroup       base_linux
 */
std::string_view rtrim_whitespace(std::string_view sv) noexcept;

/**
 * @brief  Parses a cache size string to kilobytes.
 *
 * Converts human-readable cache size strings (e.g., "32K", "1M", "2G")
 * to their equivalent in kilobytes.
 *
 * @param[in] size_str The size string to parse. Supported suffixes:
 *                     'K' (kilobytes), 'M' (megabytes), 'G' (gigabytes).
 *
 * @return             Size in kilobytes, or std::nullopt if the format
 *                     is invalid.
 *
 * @throws             None (returns std::nullopt on error).
 *
 * @warning            Case-sensitive: suffixes must be uppercase.
 *
 * @since              0.1
 * @ingroup            base_linux
 */
std::optional<uint32_t> parse_cache_size(std::string_view size_str) noexcept;

/**
 * @brief  Parses a decimal string to a 32-bit unsigned integer.
 *
 * @param[in] str The string view to parse. Must contain only decimal
 *                digits (0-9).
 *
 * @return        Parsed value, or std::nullopt if the string is invalid
 *                or represents a value exceeding UINT32_MAX.
 *
 * @throws        None (returns std::nullopt on error).
 *
 * @warning       Does not support sign characters or whitespace.
 *
 * @since         0.1
 * @ingroup       base_linux
 */
std::optional<uint32_t> parse_uint32(std::string_view str) noexcept;

/**
 * @brief  Parses a hexadecimal string to a 32-bit unsigned integer.
 *
 * @param[in] str The string view to parse. May include "0x" or "0X"
 *                prefix.
 *
 * @return        Parsed value, or std::nullopt if the string is invalid
 *                or represents a value exceeding UINT32_MAX.
 *
 * @throws        None (returns std::nullopt on error).
 *
 * @warning       Case-insensitive for hex digits (a-f, A-F).
 *
 * @since         0.1
 * @ingroup       base_linux
 */
std::optional<uint32_t> parse_hex_uint32(std::string_view str) noexcept;

/**
 * @brief  Reads a single uint32_t value from a file.
 *
 * Reads the entire file content and attempts to parse it as a decimal
 * or hexadecimal unsigned 32-bit integer.
 *
 * @param[in] path Path to the file to read.
 *
 * @return        The parsed value, or std::nullopt if the file cannot
 *                be read or contains invalid data.
 *
 * @throws        None (returns std::nullopt on error).
 *
 * @warning       This function performs file I/O and may fail for
 *               various reasons including permission errors and
 *               non-existent files.
 *
 * @since         0.1
 * @ingroup       base_linux
 */
std::optional<uint32_t> read_uint32_file(const char* path) noexcept;

/**
 * @brief  Converts an ARM implementer ID to a vendor name.
 *
 * Maps ARM CPU implementer IDs (hexadecimal values) to their
 * corresponding vendor names.
 *
 * @param[in] impl_val The implementer ID (e.g., 0x41 for ARM).
 *
 * @return             Vendor name as a string view, or "Unknown" if
 *                     the implementer ID is not recognized.
 *
 * @throws             None.
 *
 * @note               Known implementers include: ARM (0x41),
 *                     Broadcom (0x42), Cavium (0x43), DEC (0x44),
 *                     Intel (0x69), Qualcomm (0x51), etc.
 *
 * @since              0.1
 * @ingroup            base_linux
 */
std::string_view arm_implementer_to_vendor(uint32_t impl_val) noexcept;

} // namespace cf
