#include "IDesktopDisplaySizeStrategy.h"

namespace cf::desktop::platform_strategy {

DesktopBehaviors IDesktopDisplaySizeStrategy::query() const {
    return DesktopBehaviorFlag::AllowResize;
}

} // namespace cf::desktop::platform_strategy
