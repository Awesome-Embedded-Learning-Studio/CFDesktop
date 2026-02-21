#include "system/cpu/cfcpu.h"
#include "base/macro/system_judge.h"
#include "private/cpu_host.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/cpu.h"
#elif CFDESKTOP_OS_LINUX
#    include "private/linux_impl/cpu.h"
#endif

namespace {
static cf::CPUInfoHost infoHost;
} // namespace

namespace cf {
expected<CPUInfoView, CPUInfoErrorType> getCPUInfo() {
    // Query CPU info and fill the static infoHost
    auto result = query_cpu_info(infoHost);
    if (!result) {
        return cf::unexpected(result.error());
    }

    // Convert CPUInfoHost to CPUInfoView
    CPUInfoView view;
    view.model = infoHost.model;
    view.manufacturer = infoHost.manufest;
    view.arch = infoHost.arch;
    return view;
}

} // namespace cf
