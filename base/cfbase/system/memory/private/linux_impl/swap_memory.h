/**
 * @file swap_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Swap Space Query (Linux)
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
 * @brief Query swap space information using /proc/meminfo.
 *
 * @param[out] swap Output parameter for swap space statistics.
 *
 * @throws     None.
 *
 * @note       Reads /proc/meminfo to get SwapTotal and SwapFree.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    system_memory
 */
void querySwapMemory(SwapMemory& swap);

} // namespace linux_impl
} // namespace cf
