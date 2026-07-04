/**
 * @file    desktop/ui/components/wallpaper/wallpaper_setup.h
 * @brief   Factory for creating a wallpaper shell-layer strategy.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include <memory>

namespace cf::desktop {
class IShellLayerStrategy;
}

namespace cf::desktop::wallpaper {

/**
 * @brief Create a wallpaper shell layer strategy with the default wallpaper layer.
 *
 * @return std::unique_ptr<IShellLayerStrategy>
 */
std::unique_ptr<IShellLayerStrategy> create_wallpaper_strategy();

} // namespace cf::desktop::wallpaper
