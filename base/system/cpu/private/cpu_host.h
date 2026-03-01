/**
 * @file    base/system/cpu/private/cpu_host.h
 * @brief   Internal data structures for CPU information storage.
 *
 * Defines host-side storage structures used by platform-specific
 * implementations. These structures own the underlying data that
 * is exposed as views in the public API.
 *
 * @warning  This is an internal header. Do not include directly.
 *
 * @author  Charliechen114514
 * @date    2026-02-21
 * @version 0.1
 * @since   0.1
 * @ingroup system_cpu_internal
 */
#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace cf {

/**
 * @brief  Internal storage for basic CPU information.
 *
 * Owns the string data that is exposed as string views in CPUInfoView.
 * Used by platform-specific implementations to store query results.
 *
 * @ingroup system_cpu_internal
 */
struct CPUInfoHost {
    std::string model;    ///< CPU model name. Ownership: owner.
    std::string manufest; ///< CPU manufacturer name. Ownership: owner.
    std::string arch;     ///< CPU architecture identifier. Ownership: owner.
};

/**
 * @brief  Internal storage for extended CPU information.
 *
 * Owns the data that is exposed as spans in CPUBonusInfoView.
 * Used by platform-specific implementations to store query results.
 *
 * @ingroup system_cpu_internal
 */
struct CPUBonusInfoHost {
    /// CPU feature flag strings. Ownership: owner.
    std::vector<std::string> features;

    /// Cache sizes in kilobytes. Order: L1, L2, L3. Ownership: owner.
    std::vector<uint32_t> cache_size;

    /// Indicates presence of big.LITTLE or DynamIQ architecture.
    bool has_big_little = false;

    /// Number of high-performance (big) CPU cores.
    uint32_t big_core_count = 0;

    /// Number of power-efficient (little) CPU cores.
    uint32_t little_core_count = 0;

    /// CPU temperature in degrees Celsius, if available.
    std::optional<uint16_t> temperature;
};

} // namespace cf
