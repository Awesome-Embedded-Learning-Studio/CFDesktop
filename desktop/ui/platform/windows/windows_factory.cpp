#include "windows_factory.h"
#include "cflog.h"
#include "windows_display_size_policy.h"
#include <memory>

namespace cf::desktop::platform_strategy::windows {

WindowsDeskProStrategyFactory::WindowsDeskProStrategyFactory() {}

WindowsDeskProStrategyFactory::~WindowsDeskProStrategyFactory() = default;

IDesktopPropertyStrategy*
WindowsDeskProStrategyFactory::create(IDesktopPropertyStrategy::StrategyType t) {
    switch (t) {
        case IDesktopPropertyStrategy::StrategyType::Unavailable:
            return nullptr;
        case IDesktopPropertyStrategy::StrategyType::DisplaySizePolicy: {
            if (!display_size_policy_) {
                display_size_policy_ = std::make_unique<WindowsDisplaySizePolicy>();
            }
            return display_size_policy_.get();
        }
        case IDesktopPropertyStrategy::StrategyType::Extensions:
            cf::log::warningftag("WindowsFactory",
                                 "Extensions strategy not yet implemented for Windows");
            break;
    }
    return nullptr;
}

void WindowsDeskProStrategyFactory::release(IDesktopPropertyStrategy* policy) {
    if (display_size_policy_.get() == policy) {
        /* If is this one, erase it */
        display_size_policy_.reset();
        return;
    }

    /* Others, we shell log and ignores */
    cf::log::warningftag("WindowsFactory",
                         "Attempting to release strategy not created from this factory, ignoring",
                         "WindowsDeskProStrategyFactory");
}

} // namespace cf::desktop::platform_strategy::windows
