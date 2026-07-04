/**
 * @file base/system/memory/private/win_impl/memory_info.h
 * @brief Windows implementation for system memory information queries.
 *
 * Provides internal functions to query system memory statistics on Windows
 * platforms using GlobalMemoryStatusEx.
 *
 * @author Charliechen114514
 * @date 2026-02-23
 * @version 0.1
 * @since 0.1
 * @ingroup system
 */
#pragma once
#include "system/memory/memory_info.h"

namespace cf {
namespace win_impl {
/**
 * @brief Queries system memory information on Windows platforms.
 *
 * Retrieves current memory statistics including total physical memory,
 * available memory, and page file usage using GlobalMemoryStatusEx.
 *
 * @param[out] info MemoryInfo structure to store the retrieved memory statistics.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup system
 */
void getSystemMemoryInfo(MemoryInfo& info);
} // namespace win_impl
} // namespace cf
