#include "linux_wsl_factory.h"
#include "cflog.h"
#include "linux_wsl_display_size_policy.h"
#include <memory>
namespace cf::desktop::platform_strategy::wsl {

WSLDeskProStrategyFactory::WSLDeskProStrategyFactory() {}

WSLDeskProStrategyFactory::~WSLDeskProStrategyFactory() = default;

IDesktopPropertyStrategy*
WSLDeskProStrategyFactory::create(IDesktopPropertyStrategy::StrategyType t) {
    switch (t) {
        case IDesktopPropertyStrategy::StrategyType::Unavailable:
            return nullptr;
        case IDesktopPropertyStrategy::StrategyType::DisplaySizePolicy: {
            if (!sz_policy) {
                sz_policy = std::make_unique<DisplaySizePolicyMaker>();
            }
            return sz_policy.get();
        }
        case IDesktopPropertyStrategy::StrategyType::Extensions:
            break;
    }
    return nullptr;
}

void WSLDeskProStrategyFactory::release(IDesktopPropertyStrategy* policy) {
    if (sz_policy.get() == policy) {
        /* If is this one, erase it */
        sz_policy.reset();
        return;
    }

    /* Others, we shell log and ignores */
    log::warning("Meeting One Missing Up strategy releases which not created from here, desktop "
                 "might be crashed so to ignore",
                 "WSLDeskProStrategyFactory");
}

} // namespace cf::desktop::platform_strategy::wsl