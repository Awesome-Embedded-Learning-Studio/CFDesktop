/**
 * @file    desktop/ui/components/shell_layer_impl/wallpaper_src_chain.h
 * @brief   Provides the wallpaper image source chain.
 *
 * Defines the function that produces a aex::PolicyChain of wallpaper
 * image paths for the shell layer.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "aex/policy_chain/policy_chain.hpp"
#include <QStringList>

namespace cf::desktop::wallpaper {
/**
 * @brief Get the Wallpaper Load Image from here
 *
 * @return aex::PolicyChain<QString>
 */
aex::PolicyChain<QString> WallpaperImages();
} // namespace cf::desktop::wallpaper
