#include "system/cpu/cfcpu_profile.h"
#include "aex/macro/system_judge.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/cpu_profile.h"
#elif defined(CFDESKTOP_OS_LINUX)
#    include "private/linux_impl/cpu_profile.h"
#endif
namespace cf {
aex::expected<CPUProfileInfo, CPUProfileInfoError> getCPUProfileInfo() {
    return query_cpu_profile_info();
}

} // namespace cf
