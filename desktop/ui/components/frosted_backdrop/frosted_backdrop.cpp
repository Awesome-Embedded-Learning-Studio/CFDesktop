/**
 * @file    frosted_backdrop.cpp
 * @brief   Cached frosted-glass (acrylic) backdrop renderer implementation.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "frosted_backdrop.h"

#include <QLinearGradient>
#include <QPainter>
#include <QPoint>
#include <QRandomGenerator>
#include <QSize>

#include <algorithm>
#include <cstddef>
#include <vector>

namespace cf::desktop {

namespace {

/// @brief Box-blurs one scanline using a running-sum window.
///
/// Edge pixels are clamped (extended) so the window stays full at the borders.
/// Operates per-channel on premultiplied RGBA values; averaging premultiplied
/// pixels yields a valid premultiplied average, so the result stays correct.
///
/// @param[out] dst     Destination scanline.
/// @param[in]  src     Source scanline.
/// @param[in]  n       Pixel count.
/// @param[in]  radius  Box half-width; window size is 2*radius+1.
/// @throws None
void blurScanline(QRgb* dst, const QRgb* src, int n, int radius) {
    if (n <= 0 || radius < 1) {
        return;
    }
    const int window = 2 * radius + 1;
    const auto clamp = [n](int x) { return x < 0 ? 0 : (x >= n ? n - 1 : x); };

    // Prime the running sums for the window centered at x = 0.
    long long sr = 0, sg = 0, sb = 0, sa = 0;
    for (int k = -radius; k <= radius; ++k) {
        const QRgb p = src[clamp(k)];
        sr += qRed(p);
        sg += qGreen(p);
        sb += qBlue(p);
        sa += qAlpha(p);
    }

    for (int x = 0; x < n; ++x) {
        dst[x] = qRgba(static_cast<int>(sr / window), static_cast<int>(sg / window),
                       static_cast<int>(sb / window), static_cast<int>(sa / window));
        // Slide the window one pixel to the right.
        const QRgb out_p = src[clamp(x - radius)];
        const QRgb in_p = src[clamp(x + radius + 1)];
        sr += qRed(in_p) - qRed(out_p);
        sg += qGreen(in_p) - qGreen(out_p);
        sb += qBlue(in_p) - qBlue(out_p);
        sa += qAlpha(in_p) - qAlpha(out_p);
    }
}

/// @brief Applies @p passes separable (horizontal then vertical) box blurs.
/// @param[in,out] img    Image to blur (converted to ARGB32_Premultiplied).
/// @param[in]     radius Box half-width in the image's own pixel space.
/// @param[in]     passes Number of full H+V passes (3 ~ Gaussian).
/// @throws None
void boxBlur(QImage& img, int radius, int passes) {
    if (img.isNull() || radius < 1 || passes < 1) {
        return;
    }
    if (img.format() != QImage::Format_ARGB32_Premultiplied) {
        img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }
    const int w = img.width();
    const int h = img.height();
    if (w < 1 || h < 1) {
        return;
    }
    const std::size_t buf_len = static_cast<std::size_t>(std::max(w, h));
    std::vector<QRgb> in(buf_len);
    std::vector<QRgb> out(buf_len);

    for (int p = 0; p < passes; ++p) {
        // Horizontal pass: blur each row in place.
        for (int y = 0; y < h; ++y) {
            auto* line = reinterpret_cast<QRgb*>(img.scanLine(y));
            blurScanline(out.data(), line, w, radius);
            std::copy_n(out.data(), static_cast<std::size_t>(w), line);
        }
        // Vertical pass: blur each column in place.
        for (int x = 0; x < w; ++x) {
            for (int y = 0; y < h; ++y) {
                in[static_cast<std::size_t>(y)] = reinterpret_cast<QRgb*>(img.scanLine(y))[x];
            }
            blurScanline(out.data(), in.data(), h, radius);
            for (int y = 0; y < h; ++y) {
                reinterpret_cast<QRgb*>(img.scanLine(y))[x] = out[static_cast<std::size_t>(y)];
            }
        }
    }
}

/// @brief Returns a lazily-built, process-cached acrylic noise tile.
///
/// A small grayscale-noise RGBA tile, built once and shared across all panels.
/// Drawn at low opacity to add the subtle "acrylic" grain that distinguishes
/// frosted glass from a flat translucent fill.
///
/// @return Reference to the shared noise tile pixmap.
/// @throws None
const QPixmap& grainTile() {
    static QPixmap tile;
    if (!tile.isNull()) {
        return tile;
    }
    constexpr int kTile = 128;
    QImage noise(kTile, kTile, QImage::Format_ARGB32);
    noise.fill(Qt::transparent);
    auto* gen = QRandomGenerator::global();
    for (int y = 0; y < kTile; ++y) {
        auto* line = reinterpret_cast<QRgb*>(noise.scanLine(y));
        for (int x = 0; x < kTile; ++x) {
            const int v = gen->bounded(256); // 0..255 gray.
            const int a = gen->bounded(48);  // Low alpha so it stays subtle.
            line[x] = qRgba(v, v, v, a);
        }
    }
    tile = QPixmap::fromImage(noise);
    return tile;
}

} // namespace

FrostedBackdrop::Key FrostedBackdrop::makeKey(const QImage& source, const QRect& strip,
                                              qreal device_pixel_ratio,
                                              const FrostedParams& params) noexcept {
    Key k;
    k.img_key = source.cacheKey();
    k.rect = strip;
    k.dpr = device_pixel_ratio;
    k.params = params;
    return k;
}

QPixmap FrostedBackdrop::build(const QImage& source, const QRect& strip,
                               const FrostedParams& params) {
    QPixmap out(strip.size());
    out.fill(Qt::transparent);

    QPainter painter(&out);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1. Blurred wallpaper strip placed at its offset within the panel rect.
    const QRect crop_rect = strip.intersected(source.rect());
    if (!crop_rect.isEmpty()) {
        const int ds = std::max(1, params.downsample);
        const QSize small_size(std::max(1, crop_rect.width() / ds),
                               std::max(1, crop_rect.height() / ds));

        QImage small = source.copy(crop_rect).scaled(small_size, Qt::IgnoreAspectRatio,
                                                     Qt::SmoothTransformation);
        if (small.format() != QImage::Format_ARGB32_Premultiplied) {
            small = small.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        }
        const int small_radius = std::max(1, params.blur_radius_px / ds);
        boxBlur(small, small_radius, std::max(1, params.box_passes));

        const QImage blurred =
            small.scaled(crop_rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        const QPoint offset = crop_rect.topLeft() - strip.topLeft();
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(offset, blurred);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    // 2. Surface tint (SourceOver so the blurred wallpaper shows through).
    QColor tint = params.tint;
    tint.setAlphaF(std::clamp(params.tint_alpha, 0.0, 1.0));
    painter.fillRect(out.rect(), tint);

    // 3. Acrylic grain overlay.
    if (params.enable_grain) {
        const QPixmap& grain = grainTile();
        if (!grain.isNull()) {
            painter.setOpacity(0.06);
            for (int y = 0; y < out.height(); y += grain.height()) {
                for (int x = 0; x < out.width(); x += grain.width()) {
                    painter.drawPixmap(x, y, grain);
                }
            }
            painter.setOpacity(1.0);
        }
    }

    // 4. Optional specular top-edge gloss.
    if (params.top_highlight && out.height() > 0) {
        QLinearGradient gloss(0, 0, 0, out.height() * 0.5);
        QColor c(255, 255, 255);
        c.setAlphaF(0.10);
        gloss.setColorAt(0.0, c);
        c.setAlphaF(0.0);
        gloss.setColorAt(1.0, c);
        painter.fillRect(QRect(0, 0, out.width(), out.height()), gloss);
    }

    painter.end();
    return out;
}

bool FrostedBackdrop::isCacheValid(const QImage& source, const QRect& strip,
                                   qreal device_pixel_ratio, const FrostedParams& params) const {
    if (source.isNull() || strip.isEmpty() || cache_.isNull()) {
        return false;
    }
    return key_ == makeKey(source, strip, device_pixel_ratio, params);
}

QPixmap FrostedBackdrop::render(const QImage& source, const QRect& strip, qreal device_pixel_ratio,
                                const FrostedParams& params) {
    if (source.isNull() || strip.isEmpty()) {
        return {};
    }
    const Key k = makeKey(source, strip, device_pixel_ratio, params);
    if (!cache_.isNull() && k == key_) {
        return cache_;
    }
    cache_ = build(source, strip, params);
    key_ = k;
    return cache_;
}

void FrostedBackdrop::invalidate() noexcept {
    cache_ = {};
    key_ = {};
}

} // namespace cf::desktop
