#include "wallpaper_setup.h"
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
    auto layer = std::make_unique<ImageWallPaperLayer>();

    // Resolve wallpaper source through policy chain
    auto chain = WallpaperImages();
    auto source = chain.execute();

    if (source.has_value() && !source->isEmpty()) {
        auto storage = std::make_unique<WallPaperAccessStorage>();
        auto token = WallPaperTokenFactory::fromFile(*source).create();
        storage->addToken(std::move(token));
        layer->setTokenStorage(std::move(storage));
    }

    return layer;
}

} // namespace

std::unique_ptr<IShellLayerStrategy> create_wallpaper_strategy() {
    return std::make_unique<WallpaperShellLayerStrategy>(make_layer());
}

} // namespace cf::desktop::wallpaper
