/**
 * @file physical_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Physical Memory Query (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "system/memory/memory_info.h"

namespace cf {
namespace linux_impl {

/**
 * @brief Query physical memory information using /proc/meminfo.
 *
 * @param[out] physical Output parameter for physical memory statistics.
 *
 * @throws     None.
 *
 * @note       Reads /proc/meminfo to get MemTotal, MemAvailable, and MemFree.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    system_memory
 */
void queryPhysicalMemory(PhysicalMemory& physical);

} // namespace linux_impl
} // namespace cf
