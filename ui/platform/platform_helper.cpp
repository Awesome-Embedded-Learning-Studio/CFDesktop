#include "platform_helper.h"

namespace cf::desktop::platform_strategy {

/* Exposed Outliers */
PlatformFactoryAPI native_impl();

PlatformFactoryAPI native() noexcept {
    return native_impl();
}

} // namespace cf::desktop::platform_strategy