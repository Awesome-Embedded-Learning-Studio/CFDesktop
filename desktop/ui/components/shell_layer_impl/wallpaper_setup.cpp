#include "wallpaper_setup.h"
#include "cflog.h"
#include "cfpath/desktop_main_path_resolvers.h"
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
    const QStringList pic_filters = request_filterlist(FilterType::Pictures);

    // Primary source: policy chain (config source_path -> Pictures), flat scan.
    auto picture_dirent = WallpaperImages().execute();
    QStringList pictures;
    if (picture_dirent.has_value() && !picture_dirent->isEmpty()) {
        pictures = filter_target(picture_dirent.value(), pic_filters);
    }

    // Fallback: bundled Wallpapers directory, scanned recursively so resource
    // packs installed under wallpapers/<pack>/ are discovered. Tried only when
    // the primary source yielded nothing, so users with their own pictures are
    // unaffected; an empty Pictures directory now falls through to the bundled
    // pack instead of leaving the desktop without a wallpaper.
    if (pictures.isEmpty()) {
        const QString wallpapers_dir = path::DesktopMainPathProvider::instance().absolutePath(
            path::DesktopMainPathProvider::PathType::Wallpapers);
        pictures = filter_target_recursive(wallpapers_dir, pic_filters);
    }

    if (pictures.isEmpty()) {
        return layer; // No wallpaper available anywhere.
    }

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
