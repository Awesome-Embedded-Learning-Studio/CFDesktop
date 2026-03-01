/**
 * @file    base/include/system/cpu/cfcpu.h
 * @brief   Declares the basic CPU information query interface.
 *
 * Provides cross-platform access to CPU model name, architecture, and
 * manufacturer information.
 *
 * @author  Charliechen114514
 * @date    2026-02-21
 * @version 0.1
 * @since   0.1
 * @ingroup system_cpu
 */
#pragma once
#include "base/expected/expected.hpp"
#include <string_view>

namespace cf {

/**
 * @brief  Error types for CPU basic information queries.
 *
 * @ingroup system_cpu
 */
enum class CPUInfoErrorType {
    CPU_QUERY_NOERROR,       ///< Query completed successfully.
    CPU_QUERY_GENERAL_FAILED ///< General query failure occurred.
};

/**
 * @brief  Read-only view of basic CPU information.
 *
 * Provides string views into the underlying CPU information storage.
 * The returned string views remain valid for the lifetime of the
 * CPU info cache.
 *
 * @ingroup system_cpu
 */
struct CPUInfoView {
    std::string_view model;        ///< CPU model name (e.g., "Intel Core i7").
    std::string_view arch;         ///< CPU architecture (e.g., "x86_64", "aarch64").
    std::string_view manufacturer; ///< CPU manufacturer name.
};

/**
 * @brief  Retrieves basic CPU information with lazy initialization.
 *
 * This function performs lazy initialization on first call and caches
 * the result. Subsequent calls return the cached data unless
 * `force_refresh` is set to true.
 *
 * @param[in] force_refresh If true, forces re-querying CPU information.
 *                          Default is false.
 *
 * @return     `expected<CPUInfoView, CPUInfoErrorType>` containing CPU
 *             information on success, or an error type on failure.
 *
 * @throws     None (error reporting via expected type).
 *
 * @note       The returned string views are only valid as long as the
 *             internal cache remains unchanged. Callers should not
 *             store these views beyond the current query cycle.
 *
 * @warning    On Windows platforms, this function requires WMI services.
 *             Querying may fail if WMI is unavailable or misconfigured.
 *
 * @since      0.1
 * @ingroup    system_cpu
 */
expected<CPUInfoView, CPUInfoErrorType> getCPUInfo(bool force_refresh = false);

} // namespace cf
