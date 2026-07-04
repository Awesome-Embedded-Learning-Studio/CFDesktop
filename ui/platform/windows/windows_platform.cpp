#include "windows_platform.h"
#include "IDesktopPropertyStrategy.h"
#include "components/IShellLayerStrategy.h"
#include "components/shell_layer_impl/WidgetShellLayer.h"
#include "components/shell_layer_impl/wallpaper_setup.h"
#include "display_backend_helper.h"
#include "platform_helper.h"
#include "shell_layer_helper.h"
#include "windows_display_server_backend.h"
#include "windows_factory.h"

namespace cf::desktop::platform_strategy {

PlatformFactoryAPI native_impl() {
    PlatformFactoryAPI api;
    api.creator_func = [](IDesktopPropertyStrategy::StrategyType t) {
        return windows::WindowsDeskProStrategyFactory::instance().create(t);
    };
    api.release_func = [](IDesktopPropertyStrategy* v) {
        windows::WindowsDeskProStrategyFactory::instance().release(v);
    };
    return api;
}

} // namespace cf::desktop::platform_strategy

namespace cf::desktop::platform {

DisplayBackendFactoryAPI native_display_impl() {
    DisplayBackendFactoryAPI api;
    api.creator_func = []() -> IDisplayServerBackend* {
        return new backend::windows::WindowsDisplayServerBackend();
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
