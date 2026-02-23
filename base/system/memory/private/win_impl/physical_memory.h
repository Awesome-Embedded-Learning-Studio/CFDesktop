/**
 * @file physical_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Physical Memory Query (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "system/memory/memory_info.h"

namespace cf {
namespace win_impl {

/**
 * @brief Query physical memory information using GlobalMemoryStatusEx.
 *
 * @param physical Output parameter for physical memory statistics
 */
void queryPhysicalMemory(PhysicalMemory& physical);

} // namespace win_impl
} // namespace cf
