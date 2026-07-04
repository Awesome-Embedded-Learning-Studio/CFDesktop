/**
 * @file cached_memory.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Linux-specific Cached Memory Query
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
 * @brief Query Linux-specific cached memory information using /proc/meminfo.
 *
 * @param[out] cached Output parameter for cached memory statistics.
 *
 * @throws     None.
 *
 * @note       Reads /proc/meminfo to get Buffers, Cached, Shmem, and Slab.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    system_memory
 */
void queryCachedMemory(CachedMemory& cached);

} // namespace linux_impl
} // namespace cf
