/**
 * @file memory_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Memory Information Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "memory_info.h"
#include "cached_memory.h"
#include "dimm_info.h"
#include "physical_memory.h"
#include "process_memory.h"
#include "swap_memory.h"

namespace cf {
namespace linux_impl {

void getSystemMemoryInfo(MemoryInfo& info) {
    queryPhysicalMemory(info.physical);
    querySwapMemory(info.swap);
    queryCachedMemory(info.cached);
    queryProcessMemory(info.process);

    info.dimms.clear();
    queryDimmInfo(info.dimms);
}

} // namespace linux_impl
} // namespace cf
