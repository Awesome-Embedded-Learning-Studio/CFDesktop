/**
 * @file    memory_info.h
 * @brief   Linux-specific memory implementation.
 *
 * @author  Charliechen114514
 * @date    2026-02-27
 * @version 0.1
 * @since   0.1
 * @ingroup base_memory
 */
#pragma once
#include "system/memory/memory_info.h"

namespace cf {
namespace linux_impl {

/**
 * @brief Queries system memory information on Linux platforms.
 *
 * Retrieves current memory statistics including physical memory,
 * swap space, cached memory, process memory usage, and DIMM information.
 *
 * @param[out] info MemoryInfo structure to store the retrieved memory statistics.
 * @throws None
 * @note Reads from /proc/meminfo, /proc/self/status, and dmidecode.
 * @warning dmidecode requires root privileges for full DIMM information.
 * @since 0.1
 * @ingroup system
 */
void getSystemMemoryInfo(MemoryInfo& info);

}
}
