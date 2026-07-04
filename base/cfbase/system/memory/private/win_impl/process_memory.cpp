/**
 * @file process_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Process Memory Usage Query Implementation (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "process_memory.h"

// clang-format off
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <Psapi.h>
// clang-format on

#pragma comment(lib, "psapi.lib")

namespace cf {
namespace win_impl {

void queryProcessMemory(ProcessMemory& process) {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    pmc.cb = sizeof(pmc);

    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                             sizeof(pmc))) {
        process.vm_rss_bytes = pmc.WorkingSetSize;
        process.vm_size_bytes = pmc.PagefileUsage;
        process.vm_peak_bytes = pmc.PeakPagefileUsage;
    } else {
        process.vm_rss_bytes = 0;
        process.vm_size_bytes = 0;
        process.vm_peak_bytes = 0;
    }
}

} // namespace win_impl
} // namespace cf
