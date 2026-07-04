/**
 * @file swap_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Swap Space Query Implementation (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "swap_memory.h"
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace cf {
namespace win_impl {

void querySwapMemory(SwapMemory& swap) {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    swap.total_bytes = status.ullTotalPageFile;
    swap.free_bytes = status.ullAvailPageFile;
}

} // namespace win_impl
} // namespace cf
