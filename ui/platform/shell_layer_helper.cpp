#include "shell_layer_helper.h"

namespace cf::desktop::platform {

/* Forward declaration -- each platform provides this */
ShellLayerFactoryAPI native_shell_layer_impl();

ShellLayerFactoryAPI native_shell_layer() noexcept {
    return native_shell_layer_impl();
}

} // namespace cf::desktop::platform
