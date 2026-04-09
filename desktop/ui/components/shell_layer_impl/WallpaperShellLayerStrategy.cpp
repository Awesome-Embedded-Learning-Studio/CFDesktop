#include "WallpaperShellLayerStrategy.h"
#include "IShellLayer.h"
#include "cflog.h"
#include "qt_format.h"
#include "wallpaper/WallPaperLayer.h"

#include <QImage>
#include <QPainter>
#include <algorithm>

namespace cf::desktop {

// ============================================================
// WallpaperShellLayerStrategy::Private
// ============================================================
struct WallpaperShellLayerStrategy::Private {
    std::unique_ptr<wallpaper::WallPaperLayer> wallpaper_layer;
    QImage cached_scaled_image;
    QRect current_geometry;
    WeakPtr<IShellLayer> layer;
    WeakPtr<WindowManager> window_manager;
};

// ============================================================
// WallpaperShellLayerStrategy
// ============================================================

WallpaperShellLayerStrategy::WallpaperShellLayerStrategy(
    std::unique_ptr<wallpaper::WallPaperLayer> wallpaper_layer)
    : d(std::make_unique<Private>()) {
    d->wallpaper_layer = std::move(wallpaper_layer);
    if (d->wallpaper_layer) {
        d->wallpaper_layer->setImageChangedCallback([this]() { onWallpaperChanged(); });
    }
    log::infoftag("WallpaperShellLayerStrategy", "Created with wallpaper layer");
}

WallpaperShellLayerStrategy::~WallpaperShellLayerStrategy() = default;

void WallpaperShellLayerStrategy::activate(WeakPtr<IShellLayer> layer, WeakPtr<WindowManager> wm) {
    log::traceftag("WallpaperShellLayerStrategy", "Activated");
    d->layer = layer;
    d->window_manager = wm;

    // Load initial image if available
    if (d->wallpaper_layer && !d->wallpaper_layer->currentImage().isNull()) {
        rescaleImage();
    }
}

void WallpaperShellLayerStrategy::deactivate() {
    log::traceftag("WallpaperShellLayerStrategy", "Deactivated");
    d->layer.Reset();
    d->window_manager.Reset();
    d->cached_scaled_image = {};
}

void WallpaperShellLayerStrategy::onGeometryChanged(const QRect& available) {
    log::traceftag("WallpaperShellLayerStrategy", "Geometry changed: {}", available);
    if (d->current_geometry == available) {
        return;
    }
    d->current_geometry = available;
    rescaleImage();
    if (auto l = d->layer.Get()) {
        l->requestRepaint();
    }
}

QImage WallpaperShellLayerStrategy::currentBackgroundImage() const {
    return d->cached_scaled_image;
}

QColor WallpaperShellLayerStrategy::backgroundColor() const {
    if (d->wallpaper_layer) {
        return d->wallpaper_layer->backgroundColor();
    }
    return QColor(0x1c, 0x1b, 0x1f);
}

void WallpaperShellLayerStrategy::rescaleImage() {
    if (!d->wallpaper_layer) {
        return;
    }

    QImage raw = d->wallpaper_layer->currentImage();
    if (raw.isNull() || d->current_geometry.isEmpty()) {
        d->cached_scaled_image = {};
        return;
    }

    const QSize target = d->current_geometry.size();
    const auto mode = d->wallpaper_layer->scalingMode();

    QImage scaled;
    switch (mode) {
        case wallpaper::ScalingMode::Fill: {
            // Scale to cover, then crop center
            QSize scaled_size = raw.size().scaled(target, Qt::KeepAspectRatioByExpanding);
            scaled = raw.scaled(scaled_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            int x = (scaled.width() - target.width()) / 2;
            int y = (scaled.height() - target.height()) / 2;
            d->cached_scaled_image = scaled.copy(x, y, target.width(), target.height());
            return;
        }
        case wallpaper::ScalingMode::Fit: {
            // Scale to fit within, preserving aspect ratio
            QSize scaled_size = raw.size().scaled(target, Qt::KeepAspectRatio);
            scaled = raw.scaled(scaled_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            // Create background with letterbox bars
            QImage result(target, QImage::Format_RGB32);
            result.fill(d->wallpaper_layer->backgroundColor());
            QPainter painter(&result);
            int x = (target.width() - scaled.width()) / 2;
            int y = (target.height() - scaled.height()) / 2;
            painter.drawImage(x, y, scaled);
            d->cached_scaled_image = std::move(result);
            return;
        }
        case wallpaper::ScalingMode::Stretch: {
            // Scale to exact dimensions
            d->cached_scaled_image =
                raw.scaled(target, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            return;
        }
    }
    d->cached_scaled_image = {};
}

void WallpaperShellLayerStrategy::onWallpaperChanged() {
    log::traceftag("WallpaperShellLayerStrategy", "Wallpaper image changed, rescaling");
    rescaleImage();
    if (auto l = d->layer.Get()) {
        l->requestRepaint();
    }
}

} // namespace cf::desktop
