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