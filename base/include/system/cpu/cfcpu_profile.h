/**
 * @file    base/include/system/cpu/cfcpu_profile.h
 * @brief   Declares the CPU performance information query interface.
 *
 * Provides cross-platform access to CPU core counts, frequencies, and
 * current usage percentage.
 *
 * @author  Charliechen114514
 * @date    2026-02-21
 * @version 0.1
 * @since   0.1
 * @ingroup system_cpu
 */
#pragma once
#include "base/expected/expected.hpp"
#include <cstdint>

namespace cf {

/**
 * @brief  Error types for CPU profile information queries.
 *
 * @ingroup system_cpu
 */
enum class CPUProfileInfoError {
    CPUProfileInfoGeneralError ///< General query failure occurred.
};

/**
 * @brief  CPU performance and capacity information.
 *
 * Contains data about CPU cores, clock frequencies, and current usage.
 * All frequency values are in MHz. Usage percentage is in the range
 * [0.0, 100.0].
 *
 * @ingroup system_cpu
 */
struct CPUProfileInfo {
    uint16_t logical_cnt;       ///< Number of logical CPU threads.
    uint16_t physical_cnt;      ///< Number of physical CPU cores.
    uint32_t current_frequecy;  ///< Current CPU frequency in MHz.
    uint32_t max_frequency;     ///< Maximum CPU frequency in MHz.
    float cpu_usage_percentage; ///< Current CPU usage as percentage (0-100).
};

/**
 * @brief  Retrieves CPU performance information.
 *
 * Queries the operating system for current CPU performance metrics
 * including core counts, frequencies, and usage percentage. This
 * function performs a real-time query each time it is called and
 * does not cache results.
 *
 * @return     `expected<CPUProfileInfo, CPUProfileInfoError>` containing
 *             CPU performance data on success, or an error type on
 *             failure.
 *
 * @throws     None (error reporting via expected type).
 *
 * @note       CPU usage percentage is calculated since the last query.
 *             The first call may return inaccurate values.
 *
 * @warning    Frequency values may be reported as 0 on platforms where
 *             frequency scaling is not available or accessible.
 *
 * @since      0.1
 * @ingroup    system_cpu
 */
expected<CPUProfileInfo, CPUProfileInfoError> getCPUProfileInfo();

} // namespace cf
