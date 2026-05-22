/**
 * @file    desktop/ui/components/shell_layer_impl/wallpaper_src_chain.h
 * @brief   Provides the wallpaper image source chain.
 *
 * Defines the function that produces a PolicyChain of wallpaper
 * image paths for the shell layer.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "base/policy_chain/policy_chain.hpp"
#include <QStringList>

namespace cf::desktop::wallpaper {
/**
 * @brief Get the Wallpaper Load Image from here
 *
 * @return PolicyChain<QString>
 */
PolicyChain<QString> WallpaperImages();
} // namespace cf::desktop::wallpaper
