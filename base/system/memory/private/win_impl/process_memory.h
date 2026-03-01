/**
 * @file process_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Process Memory Usage Query (Windows)
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
 * @brief Query current process memory usage using GetProcessMemoryInfo.
 *
 * @param[out] process Output parameter for process memory statistics.
 *
 * @throws     None.
 *
 * @note       None.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    system_memory
 */
void queryProcessMemory(ProcessMemory& process);

} // namespace win_impl
} // namespace cf
