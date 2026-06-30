#include "wallpaper_setup.h"
#include "cfconfig.hpp"
#include "cflog.h"
#include "cfpath/desktop_main_path_resolvers.h"
#include "filter_target.h"
#include "shell_layer_impl/WallpaperShellLayerStrategy.h"
#include "wallpaper/ImageWallPaperLayer.h"
#include "wallpaper/WallPaperAccessStorage.h"
#include "wallpaper/WallPaperToken.h"
#include "wallpaper_src_chain.h"

#include <QString>

namespace cf::desktop::wallpaper {

namespace {
/// Tag for wallpaper setup log lines.
constexpr const char* kLogTag = "WallpaperSetup";

/**
 * @brief  Reads the configured wallpaper scaling mode.
 *
 * @return Scaling mode; ScalingMode::Fill on unknown values (with a warning).
 */
ScalingMode scaling_from_config() {
    auto wp = cf::config::ConfigStore::instance().domain("wallpaper");
    const auto value =
        wp.query<std::string>(cf::config::KeyView{.group = "wallpaper", .key = "scaling"}, "fill");
    if (value == "fit") {
        return ScalingMode::Fit;
    }
    if (value == "stretch") {
        return ScalingMode::Stretch;
    }
    if (value == "fill") {
        return ScalingMode::Fill;
    }
    log::warningftag(kLogTag, "Unknown scaling '{}', falling back to fill", value);
    return ScalingMode::Fill;
}

/**
 * @brief  Reads the configured wallpaper background color.
 *
 * @return Background color; default dark grey on invalid input (with a warning).
 */
QColor background_from_config() {
    auto wp = cf::config::ConfigStore::instance().domain("wallpaper");
    const auto value = wp.query<std::string>(
        cf::config::KeyView{.group = "wallpaper", .key = "background_color"}, "#1c1b1f");
    QColor color(QString::fromStdString(value));
    if (!color.isValid()) {
        log::warningftag(kLogTag, "Invalid background_color '{}', falling back to #1c1b1f", value);
        return QColor(0x1c, 0x1b, 0x1f);
    }
    return color;
}

/**
 * @brief Wallpaper Layer Inits
 *
 * @return std::unique_ptr<WallPaperLayer>
 */
std::unique_ptr<WallPaperLayer> make_layer() {
    using namespace base::filesystem;

    auto layer =
        std::make_unique<ImageWallPaperLayer>(scaling_from_config(), background_from_config());
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
