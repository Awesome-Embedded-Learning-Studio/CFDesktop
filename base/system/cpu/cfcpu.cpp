#include "system/cpu/cfcpu.h"
#include "base/helpers/once_init.hpp"
#include "base/macro/system_judge.h"
#include "private/cpu_host.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/cpu_info.h"
#elif CFDESKTOP_OS_LINUX
#    include "private/linux_impl/cpu_info.h"
#endif

namespace {
class CPUHostInfoIniter : public cf::CallOnceInit<cf::CPUInfoHost> {
  public:
    cf::CPUInfoErrorType error() const { return error_code; }

  protected:
    bool init_resources() override {
        // will be filled by query_cpu_info
        auto src = query_cpu_basic_info(resource);
        bool isOk = src.has_value();
        if (!isOk) {
            error_code = src.error();
        }
        return isOk;
    }
    bool force_do_reinit() override { return init_resources(); }

  private:
    cf::CPUInfoErrorType error_code{cf::CPUInfoErrorType::CPU_QUERY_NOERROR};
};

static CPUHostInfoIniter cpu_initer;

} // namespace

namespace cf {
expected<CPUInfoView, CPUInfoErrorType> getCPUInfo(bool force_refresh) {
    if (force_refresh) {
        cpu_initer.force_reinit();
    }

    auto& result = cpu_initer.get_resources();
    if (cpu_initer.error() != cf::CPUInfoErrorType::CPU_QUERY_NOERROR) {
        return cf::unexpected(cpu_initer.error());
    }

    // Convert CPUInfoHost to CPUInfoView
    CPUInfoView view;
    view.model = result.model;
    view.manufacturer = result.manufest;
    view.arch = result.arch;
    return view;
}

} // namespace cf
