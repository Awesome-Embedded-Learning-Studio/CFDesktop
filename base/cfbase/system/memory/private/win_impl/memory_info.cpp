/**
 * @file memory_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Memory Information Query Implementation (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "memory_info.h"
#include "dimm_info.h"
#include "physical_memory.h"
#include "process_memory.h"
#include "swap_memory.h"

namespace cf {
namespace win_impl {

void getSystemMemoryInfo(MemoryInfo& info) {
    queryPhysicalMemory(info.physical);
    querySwapMemory(info.swap);
    queryProcessMemory(info.process);

    // Windows doesn't have the same concept as Linux buffers/cache
    info.cached.buffers_bytes = 0;
    info.cached.cached_bytes = 0;
    info.cached.shared_bytes = 0;
    info.cached.slab_bytes = 0;

    info.dimms.clear();
    queryDimmInfo(info.dimms);
}

} // namespace win_impl
} // namespace cf
