/**
 * @file    embedded_platform.cpp
 * @brief   Native platform factory hooks for the embedded DirectRender backend.
 *
 * Provides the per-platform factory entry points (native_impl,
 * native_display_impl, native_shell_layer_impl) consumed by the platform
 * helpers when CFDESKTOP_EMBEDDED is enabled. The display backend is the
 * EmbeddedDisplayServerBackend; the shell layer reuses the QWidget-based
 * WidgetShellLayer and wallpaper strategy.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#include "embedded_display_size_policy.h"
#include "embedded_display_server_backend.h"

#include "IDesktopPropertyStrategy.h"
#include "components/IShellLayerStrategy.h"
#include "components/shell_layer_impl/WidgetShellLayer.h"
#include "components/shell_layer_impl/wallpaper_setup.h"
#include "display_backend_helper.h"
#include "platform_helper.h"
#include "shell_layer_helper.h"

#include <memory>

namespace cf::desktop::platform_strategy {

PlatformFactoryAPI native_impl() {
    PlatformFactoryAPI api;
    // The embedded target owns a single shared size policy (function-local
    // static). create() lends a borrowed pointer; release() is a no-op.
    api.creator_func = [](IDesktopPropertyStrategy::StrategyType t)
        -> IDesktopPropertyStrategy* {
        if (t != IDesktopPropertyStrategy::StrategyType::DisplaySizePolicy) {
            return nullptr;
        }
        static auto policy = std::make_unique<embedded::EmbeddedDisplaySizePolicy>();
        return policy.get();
    };
    api.release_func = [](IDesktopPropertyStrategy* /*policy*/) {
        // Factory-owned; nothing to release.
    };
    return api;
}

} // namespace cf::desktop::platform_strategy

namespace cf::desktop::platform {

DisplayBackendFactoryAPI native_display_impl() {
    DisplayBackendFactoryAPI api;
    api.creator_func = []() -> IDisplayServerBackend* {
        return new backend::embedded::EmbeddedDisplayServerBackend();
    };
    api.release_func = [](IDisplayServerBackend* p) { delete p; };
    return api;
}

ShellLayerFactoryAPI native_shell_layer_impl() {
    ShellLayerFactoryAPI api;
    api.shell_creator = [](QWidget* parent) -> IShellLayer* {
        return new WidgetShellLayer(parent);
    };
    api.shell_releaser = [](IShellLayer* p) { delete p; };
    api.strategy_creator = []() -> std::unique_ptr<IShellLayerStrategy> {
        return wallpaper::create_wallpaper_strategy();
    };
    return api;
}

} // namespace cf::desktop::platform
