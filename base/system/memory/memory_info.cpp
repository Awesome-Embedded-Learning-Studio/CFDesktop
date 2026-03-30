/**
 * @file    memory_info.cpp
 * @brief   Implementation of memory information detection.
 *
 * Provides the cross-platform entry point for retrieving system
 * memory information. Dispatches to the appropriate platform-specific
 * implementation (Windows or Linux) at compile time.
 *
 * @author  Charliechen114514
 * @date    2026-02-23
 * @version 0.1
 * @since   0.1
 * @ingroup system_memory
 */

#include "system/memory/memory_info.h"
#include "base/macro/system_judge.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/memory_info.h"
#elif defined(CFDESKTOP_OS_LINUX)
#    include "private/linux_impl/memory_info.h"
#endif

namespace cf {

void getSystemMemoryInfo(MemoryInfo& info) {
#ifdef CFDESKTOP_OS_WINDOWS
    win_impl::getSystemMemoryInfo(info);
#elif defined(CFDESKTOP_OS_LINUX)
    linux_impl::getSystemMemoryInfo(info);
#endif
}

} // namespace cf