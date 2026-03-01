/**
 * @file swap_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Swap Space Query (Windows)
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
 * @brief Query swap space information using GlobalMemoryStatusEx.
 *
 * @param[out] swap Output parameter for swap space statistics.
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
void querySwapMemory(SwapMemory& swap);

} // namespace win_impl
} // namespace cf
