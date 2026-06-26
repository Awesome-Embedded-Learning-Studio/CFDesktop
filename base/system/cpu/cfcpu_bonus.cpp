#include "system/cpu/cfcpu_bonus.h"
#include "aex/helpers/once_init.hpp"
#include "aex/macro/system_judge.h"
#include "aex/span/span.h"
#include "private/cpu_host.h"
#include <string>
#include <string_view>
#include <vector>

#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/cpu_bonus.h"
#elif defined(CFDESKTOP_OS_LINUX)
#    include "private/linux_impl/cpu_bonus.h"
#endif

namespace {
class CPUBonusInfoInit : public aex::CallOnceInit<cf::CPUBonusInfoHost> {
  public:
    cf::CPUBonusInfoViewError error() const { return error_code; }

  protected:
    bool init_resources() override {
        auto src = query_cpu_bonus_info(resource);
        bool isOk = src.has_value();
        if (!isOk) {
            error_code = src.error();
        }
        return isOk;
    }
    bool force_do_reinit() override { return init_resources(); }

  private:
    cf::CPUBonusInfoViewError error_code{cf::CPUBonusInfoViewError::NoError};
};

static CPUBonusInfoInit bonusInfoInit;

aex::span<const std::string_view> toView(const std::vector<std::string>& src_array) {
    static std::vector<std::string_view> views;
    views.clear();
    views.reserve(src_array.size());
    for (const auto& item : src_array) {
        views.push_back(item);
    }
    return aex::span<const std::string_view>(views.data(), views.size());
}

} // namespace

namespace cf {

aex::expected<CPUBonusInfoView, CPUBonusInfoViewError> getCPUBonusInfo(bool force_refresh) {
    if (force_refresh) {
        bonusInfoInit.force_reinit();
    }

    auto& result = bonusInfoInit.get_resources();
    if (bonusInfoInit.error() != cf::CPUBonusInfoViewError::NoError) {
        return aex::unexpected(bonusInfoInit.error());
    }

    // Convert CPUBonusInfoHost to CPUBonusInfoView
    CPUBonusInfoView view;

    view.features = toView(result.features);
    view.cache_size = aex::span<const uint32_t>(result.cache_size.data(), result.cache_size.size());

    view.has_big_little = result.has_big_little;
    view.big_core_count = result.big_core_count;
    view.little_core_count = result.little_core_count;

    view.temperature = result.temperature;

    return view;
}

} // namespace cf
