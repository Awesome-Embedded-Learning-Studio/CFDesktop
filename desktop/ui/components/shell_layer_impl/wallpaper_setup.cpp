#include "wallpaper_setup.h"
#include "cflog.h"
#include "filter_target.h"
#include "shell_layer_impl/WallpaperShellLayerStrategy.h"
#include "wallpaper/ImageWallPaperLayer.h"
#include "wallpaper/WallPaperAccessStorage.h"
#include "wallpaper/WallPaperToken.h"
#include "wallpaper_src_chain.h"

namespace cf::desktop::wallpaper {

namespace {
/**
 * @brief Wallpaper Layer Inits
 *
 * @return std::unique_ptr<WallPaperLayer>
 */
std::unique_ptr<WallPaperLayer> make_layer() {
    using namespace base::filesystem;

    auto layer = std::make_unique<ImageWallPaperLayer>();
    // Resolve wallpaper pictures through policy chain
    auto picture_dirent = WallpaperImages().execute();
    if (!picture_dirent.has_value() || picture_dirent->isEmpty()) {
        return layer; // return out!
    }

    auto pictures = filter_target(picture_dirent.value(), request_filterlist(FilterType::Pictures));
    auto storage = std::make_unique<WallPaperAccessStorage>();
    storage->addTokens(WallPaperTokenFactory::fromFiles(pictures));
    log::tracef("Initialized with {} files", storage->size());
    layer->setTokenStorage(std::move(storage));
    return layer;
}

} // namespace

std::unique_ptr<IShellLayerStrategy> create_wallpaper_strategy() {
    return std::make_unique<WallpaperShellLayerStrategy>(make_layer());
}

} // namespace cf::desktop::wallpaper
