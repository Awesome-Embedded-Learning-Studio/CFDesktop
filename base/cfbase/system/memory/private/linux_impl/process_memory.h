/**
 * @file process_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Process Memory Usage Query (Linux)
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
 * @brief Query current process memory usage using /proc/self/status.
 *
 * @param[out] process Output parameter for process memory statistics.
 *
 * @throws     None.
 *
 * @note       Reads /proc/self/status to get VmRSS, VmSize, and VmPeak.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    system_memory
 */
void queryProcessMemory(ProcessMemory& process);

} // namespace linux_impl
} // namespace cf
