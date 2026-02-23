/**
 * @file physical_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Physical Memory Query Implementation (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "physical_memory.h"
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace cf {
namespace win_impl {

void queryPhysicalMemory(PhysicalMemory& physical) {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    physical.total_bytes = status.ullTotalPhys;
    physical.available_bytes = status.ullAvailPhys;
    physical.free_bytes = status.ullAvailPhys; // Windows: AvailPhys ~= Free
}

} // namespace win_impl
} // namespace cf
