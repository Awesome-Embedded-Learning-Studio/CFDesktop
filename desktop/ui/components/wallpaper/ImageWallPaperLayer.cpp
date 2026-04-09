#include "ImageWallPaperLayer.h"
#include "cflog.h"
#include "qt_format.h"

namespace cf::desktop::wallpaper {

// ============================================================
// ImageWallPaperLayer::Private
// ============================================================
struct ImageWallPaperLayer::Private {
    std::unique_ptr<WallPaperAccessStorage> storage;
    QImage current_image;
    ScalingMode scaling_mode{ScalingMode::Fill};
    QColor background_color{0x1c, 0x1b, 0x1f};
};

// ============================================================
// ImageWallPaperLayer
// ============================================================

ImageWallPaperLayer::ImageWallPaperLayer(ScalingMode mode, const QColor& bg)
    : d(std::make_unique<Private>()) {
    d->scaling_mode = mode;
    d->background_color = bg;
}

ImageWallPaperLayer::~ImageWallPaperLayer() = default;

void ImageWallPaperLayer::setTokenStorage(std::unique_ptr<WallPaperAccessStorage> storage) {
    d->storage = std::move(storage);
    if (!d->storage) {
        d->current_image = {};
        return;
    }
    // Load the image at the current cursor position
    auto token = d->storage->current();
    if (token) {
        loadImageFromToken(token);
    }
}

bool ImageWallPaperLayer::showNextOne() {
    if (!d->storage) {
        return false;
    }
    auto token = d->storage->toNext(true);
    if (!token) {
        return false;
    }
    return loadImageFromToken(token);
}

bool ImageWallPaperLayer::showPrevOne() {
    if (!d->storage) {
        return false;
    }
    auto token = d->storage->toPrev(true);
    if (!token) {
        return false;
    }
    return loadImageFromToken(token);
}

bool ImageWallPaperLayer::showTargetOne(const wallpaper_token_id_t& token) {
    if (!d->storage) {
        return false;
    }
    auto wk = d->storage->to(token);
    if (!wk) {
        return false;
    }
    return loadImageFromToken(wk);
}

QImage ImageWallPaperLayer::currentImage() const {
    return d->current_image;
}

ScalingMode ImageWallPaperLayer::scalingMode() const {
    return d->scaling_mode;
}

QColor ImageWallPaperLayer::backgroundColor() const {
    return d->background_color;
}

void ImageWallPaperLayer::setScalingMode(ScalingMode mode) {
    d->scaling_mode = mode;
}

void ImageWallPaperLayer::setBackgroundColor(const QColor& color) {
    d->background_color = color;
}

bool ImageWallPaperLayer::loadImageFromToken(const WeakPtr<WallPaperToken>& token) {
    if (!token) {
        return false;
    }

    const QString& path = token->sourcePath();
    if (path.isEmpty()) {
        log::warningftag("ImageWallPaperLayer", "Token has empty source path");
        d->current_image = {};
        return false;
    }

    QImage loaded;
    if (!loaded.load(path)) {
        log::warningftag("ImageWallPaperLayer", "Failed to load image from: {}", path);
        d->current_image = {};
        return false;
    }

    d->current_image = std::move(loaded);
    log::traceftag("ImageWallPaperLayer", "Loaded wallpaper: {} ({}x{})", path,
                   d->current_image.width(), d->current_image.height());
    notifyImageChanged();
    return true;
}

} // namespace cf::desktop::wallpaper
