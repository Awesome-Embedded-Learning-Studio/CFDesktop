#include "WallpaperShellLayerStrategy.h"
#include "IShellLayer.h"
#include "cflog.h"
#include "qt_format.h"
#include "wallpaper/WallPaperAccessStorage.h"
#include "wallpaper/WallPaperEngine.h"
#include "wallpaper/WallPaperLayer.h"

#include <QEasingCurve>
#include <QImage>
#include <QPainter>
#include <QVariantAnimation>
#include <algorithm>
#include <random>

namespace cf::desktop {

namespace {
/// Log tag for the wallpaper shell layer strategy.
constexpr const char* kLogTag = "WallpaperShellLayerStrategy";
} // namespace

// ============================================================
// WallpaperShellLayerStrategy::Private
// ============================================================
struct WallpaperShellLayerStrategy::Private {
    std::unique_ptr<wallpaper::WallPaperLayer> wallpaper_layer;
    std::unique_ptr<wallpaper::WallPaperEngine> engine;
    QImage cached_scaled_image;
    QRect current_geometry;
    aex::WeakPtr<IShellLayer> layer;
    aex::WeakPtr<WindowManager> window_manager;

    // Transition state.
    bool transitioning{false};
    wallpaper::SwitchingMode pending_mode{wallpaper::SwitchingMode::Movement};
    QImage previous_scaled; ///< Outgoing frame (already scaled to geometry).
    QImage current_scaled;  ///< Incoming frame during a transition.
    std::unique_ptr<QVariantAnimation> anim;
    std::mt19937 rng;
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
        // The engine drives timed rotation; on each tick it asks us to begin a
        // transition. It needs a non-owning layer pointer only for its size guard.
        wallpaper::WallPaperLayer* raw = d->wallpaper_layer.get();
        d->engine = std::make_unique<wallpaper::WallPaperEngine>(
            raw, [this](wallpaper::SwitchingMode mode) { beginTransition(mode); }, nullptr);
    }
    log::infoftag(kLogTag, "Created with wallpaper layer and rotation engine");
}

WallpaperShellLayerStrategy::~WallpaperShellLayerStrategy() = default;

void WallpaperShellLayerStrategy::activate(aex::WeakPtr<IShellLayer> layer,
                                           aex::WeakPtr<WindowManager> wm) {
    log::traceftag(kLogTag, "Activated");
    d->layer = layer;
    d->window_manager = wm;

    // Load initial image if available.
    if (d->wallpaper_layer && !d->wallpaper_layer->currentImage().isNull()) {
        rescaleImage();
    }
    if (d->engine) {
        d->engine->start();
    }
}

void WallpaperShellLayerStrategy::deactivate() {
    log::traceftag(kLogTag, "Deactivated");
    if (d->engine) {
        d->engine->stop();
    }
    resetTransition();
    d->layer.Reset();
    d->window_manager.Reset();
    d->cached_scaled_image = {};
}

void WallpaperShellLayerStrategy::onGeometryChanged(const QRect& available) {
    log::traceftag(kLogTag, "Geometry changed: QRect({}, {}, {}, {})", available.x(), available.y(),
                   available.width(), available.height());
    if (d->current_geometry == available) {
        return;
    }
    // A geometry change invalidates the pre-scaled transition buffers; abort
    // any in-flight transition, then re-scale the current image for the new
    // geometry (handled below by rescaleImage()).
    if (d->transitioning) {
        resetTransition();
    }
    d->current_geometry = available;
    rescaleImage();
    requestLayerRepaint();
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
            // Scale to cover, then crop center.
            QSize scaled_size = raw.size().scaled(target, Qt::KeepAspectRatioByExpanding);
            scaled = raw.scaled(scaled_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            int x = (scaled.width() - target.width()) / 2;
            int y = (scaled.height() - target.height()) / 2;
            d->cached_scaled_image = scaled.copy(x, y, target.width(), target.height());
            return;
        }
        case wallpaper::ScalingMode::Fit: {
            // Scale to fit within, preserving aspect ratio.
            QSize scaled_size = raw.size().scaled(target, Qt::KeepAspectRatio);
            scaled = raw.scaled(scaled_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            // Create background with letterbox bars.
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
            // Scale to exact dimensions.
            d->cached_scaled_image =
                raw.scaled(target, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            return;
        }
    }
    d->cached_scaled_image = {};
}

void WallpaperShellLayerStrategy::onWallpaperChanged() {
    log::traceftag(kLogTag, "Wallpaper image changed, rescaling");
    if (d->transitioning) {
        // The layer now holds the new image: re-scale it into the cache, stash
        // it as the incoming frame, and start the per-frame animation.
        rescaleImage();
        d->current_scaled = d->cached_scaled_image;
        if (d->current_scaled.isNull()) {
            // Target image failed to load: abort and restore the outgoing frame.
            log::warningftag(kLogTag, "Transition target image is null; aborting transition");
            d->cached_scaled_image = d->previous_scaled;
            resetTransition();
            requestLayerRepaint();
            return;
        }
        startTransitionAnim();
        return;
    }
    // Instant (non-animated) path.
    rescaleImage();
    requestLayerRepaint();
}

void WallpaperShellLayerStrategy::requestLayerRepaint() {
    if (auto l = d->layer.Get()) {
        l->requestRepaint();
    }
}

void WallpaperShellLayerStrategy::triggerNextWallpaper() {
    if (d->engine) {
        beginTransition(d->engine->mode());
    }
}

void WallpaperShellLayerStrategy::beginTransition(wallpaper::SwitchingMode mode) {
    if (d->transitioning) {
        return; // Already mid-transition; ignore the new request.
    }
    if (!d->wallpaper_layer || d->cached_scaled_image.isNull()) {
        return; // Nothing to transition from.
    }
    d->pending_mode = mode;
    d->previous_scaled = d->cached_scaled_image;
    d->transitioning = true;
    if (!advanceLayerToNext()) {
        // No switch occurred (e.g. storage shrank to <=1); roll back state.
        d->transitioning = false;
        d->previous_scaled = {};
    }
    // Otherwise onWallpaperChanged() (fired by the layer switch) takes over.
}

bool WallpaperShellLayerStrategy::advanceLayerToNext() {
    if (!d->wallpaper_layer) {
        return false;
    }
    auto& storage = d->wallpaper_layer->tokenStorage();
    if (storage.size() <= 1) {
        return false;
    }
    const auto ids = storage.tokenIds();
    auto current = storage.current();
    const wallpaper::wallpaper_token_id_t current_id = current ? current->id() : QString{};
    const auto selector = d->engine ? d->engine->selector() : wallpaper::Selector::Sequential;
    const wallpaper::wallpaper_token_id_t next_id =
        wallpaper::selectNextWallpaper(ids, current_id, selector, d->rng);
    if (next_id.isEmpty()) {
        return false;
    }
    return d->wallpaper_layer->showTargetOne(next_id);
}

void WallpaperShellLayerStrategy::startTransitionAnim() {
    d->anim = std::make_unique<QVariantAnimation>();
    d->anim->setStartValue(qreal(0.0));
    d->anim->setEndValue(qreal(1.0));
    d->anim->setDuration(d->engine ? d->engine->animationDurationMs() : 2000);
    d->anim->setEasingCurve(d->engine ? d->engine->easing() : QEasingCurve::InOutCubic);
    QObject::connect(d->anim.get(), &QVariantAnimation::valueChanged,
                     [this](const QVariant& value) { composeFrame(value.toReal()); });
    QObject::connect(d->anim.get(), &QVariantAnimation::finished, [this]() { finishTransition(); });
    composeFrame(0.0); // Publish the first frame immediately.
    d->anim->start();
}

void WallpaperShellLayerStrategy::composeFrame(qreal progress) {
    if (!d->transitioning) {
        return;
    }
    d->cached_scaled_image =
        wallpaper::composeTransitionFrame(d->previous_scaled, d->current_scaled, progress,
                                          d->pending_mode, d->current_geometry.size());
    requestLayerRepaint();
}

void WallpaperShellLayerStrategy::finishTransition() {
    if (!d->transitioning) {
        return;
    }
    d->cached_scaled_image = d->current_scaled; // Settle on the new frame.
    d->previous_scaled = {};
    d->current_scaled = {};
    d->transitioning = false;
    d->anim.reset();
    requestLayerRepaint();
}

void WallpaperShellLayerStrategy::resetTransition() {
    if (d->anim) {
        d->anim->stop();
    }
    d->anim.reset();
    d->transitioning = false;
    d->previous_scaled = {};
    d->current_scaled = {};
}

} // namespace cf::desktop
