#include "system/cpu/cfcpu_profile.h"
#include "base/macro/system_judge.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/cpu_profile.h"
#elif defined(CFDESKTOP_OS_LINUX)
#    include "private/linux_impl/cpu_profile.h"
#endif
namespace cf {
expected<CPUProfileInfo, CPUProfileInfoError> getCPUProfileInfo() {
    return query_cpu_profile_info();
}

} // namespace cf
