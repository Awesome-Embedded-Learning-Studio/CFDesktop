#include "display_backend_helper.h"

namespace cf::desktop::platform {

/* Forward declaration -- each platform provides this */
DisplayBackendFactoryAPI native_display_impl();

DisplayBackendFactoryAPI native_display() noexcept {
    return native_display_impl();
}

} // namespace cf::desktop::platform
