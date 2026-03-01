/**
 * @file    base/include/system/cpu/cfcpu_bonus.h
 * @brief   Declares the CPU extended information query interface.
 *
 * Provides access to CPU features, cache sizes, big.LITTLE architecture
 * detection, and temperature information. Note that some fields may be
 * unavailable on certain platforms or architectures.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup system_cpu
 */
#pragma once

#include "base/expected/expected.hpp"
#include "base/span/span.h"
#include <cstdint>
#include <optional>
#include <string_view>

namespace cf {

/**
 * @brief  Error types for CPU bonus information queries.
 *
 * @ingroup system_cpu
 */
enum class CPUBonusInfoViewError {
    NoError,     ///< Query completed successfully.
    GeneralError ///< General query failure occurred.
};

/**
 * @brief  Extended CPU information with optional fields.
 *
 * Contains CPU features, cache information, big.LITTLE architecture
 * details, and temperature data. Some fields may be empty or unavailable
 * depending on platform and hardware support.
 *
 * @ingroup system_cpu
 */
struct CPUBonusInfoView {
    /// CPU feature flags (e.g., "neon", "aes", "avx2").
    cf::span<const std::string_view> features;

    /// Cache sizes in kilobytes. Order: L1, L2, L3 (if available).
    cf::span<const uint32_t> cache_size;

    /// Indicates whether CPU uses big.LITTLE or DynamIQ architecture.
    bool has_big_little = false;

    /// Number of high-performance (big) cores. Valid when has_big_little
    /// is true.
    uint32_t big_core_count = 0;

    /// Number of power-efficient (little) cores. Valid when
    /// has_big_little is true.
    uint32_t little_core_count = 0;

    /// CPU temperature in Celsius. May be unavailable on some platforms.
    std::optional<uint16_t> temperature;
};

/**
 * @brief  Retrieves extended CPU information with lazy initialization.
 *
 * This function performs lazy initialization on first call and caches
 * the result. Subsequent calls return the cached data unless
 * `force_refresh` is set to true. Some fields may be unavailable
 * depending on platform and hardware support.
 *
 * @param[in] force_refresh If true, forces re-querying CPU information.
 *                          Default is false.
 *
 * @return     `expected<CPUBonusInfoView, CPUBonusInfoViewError>`
 *             containing extended CPU information on success, or an
 *             error type on failure.
 *
 * @throws     None (error reporting via expected type).
 *
 * @note       The returned spans are only valid as long as the internal
 *             cache remains unchanged. The temperature field may be
 *             `std::nullopt` if thermal information is unavailable.
 *
 * @warning    Feature detection is platform-specific. Not all features
 *             may be detected on all platforms. big.LITTLE detection
 *             requires specific hardware support.
 *
 * @since      0.1
 * @ingroup    system_cpu
 */
expected<CPUBonusInfoView, CPUBonusInfoViewError> getCPUBonusInfo(bool force_refresh = false);

} // namespace cf
