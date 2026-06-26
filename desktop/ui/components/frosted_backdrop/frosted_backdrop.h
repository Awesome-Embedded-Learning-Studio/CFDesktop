/**
 * @file    frosted_backdrop.h
 * @brief   Cached frosted-glass (acrylic) backdrop renderer for shell panels.
 *
 * WSL/X11 provides no compositor backdrop blur, so a shell panel cannot rely on
 * real translucency to look "frosted". Instead each panel paints its own
 * blurred-and-tinted strip of the wallpaper. FrostedBackdrop renders that strip
 * (downsample -> separable box blur -> smooth upscale -> surface tint ->
 * optional acrylic grain -> optional top highlight) and caches the result by a
 * key derived from the source image, the strip rect, the tint, the blur radius,
 * the device-pixel-ratio, and the full parameter set. Steady repaints (clock
 * ticks, hover) are O(1) cache hits with no blur work.
 *
 * The renderer depends only on Qt Gui; it knows nothing of the shell, panels,
 * or themes. Callers resolve theme colors into FrostedParams.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QRect>

namespace cf::desktop {

/**
 * @brief  Tunable parameters for a frosted-glass render.
 *
 * Every member is part of the render cache key: changing any value forces the
 * next render() to rebuild the pixmap.
 *
 * @ingroup components
 */
struct FrostedParams {
    QColor tint{255, 255, 255}; ///< Surface tint baked into the glass.
    qreal tint_alpha{0.60};     ///< 0..1 tint opacity; lower shows more wallpaper.
    int blur_radius_px{18};     ///< Logical blur radius; downsampled internally.
    int downsample{4};          ///< Crop is shrunk by this factor before blur.
    int box_passes{3};          ///< Separable box-blur passes (~Gaussian).
    bool enable_grain{true};    ///< Overlay a subtle acrylic noise texture.
    bool top_highlight{false};  ///< Draw a specular top-edge gloss.

    /// @brief Value equality (all members); used by the render cache key.
    bool operator==(const FrostedParams&) const = default;
};

/**
 * @brief  Renders and caches a frosted-glass strip from a wallpaper image.
 *
 * One instance per panel. render() is deterministic: identical inputs yield an
 * identical pixmap, and repeated calls with unchanged inputs return the cached
 * pixmap without recomputing.
 *
 * @ingroup components
 */
class FrostedBackdrop {
  public:
    /**
     * @brief  Constructs an empty (uncached) renderer.
     *
     * @throws None
     * @since  0.20
     */
    FrostedBackdrop() = default;

    /**
     * @brief  Returns the cached frosted pixmap for @p source cropped to
     *         @p strip, rebuilding only when the cache key changes.
     *
     * @param[in] source             Full-screen wallpaper image (logical px).
     * @param[in] strip              Region of @p source the panel covers, in
     *                               the same coordinate space as @p source.
     * @param[in] device_pixel_ratio Current device-pixel-ratio (cache key).
     * @param[in] params             Render parameters (cache key).
     *
     * @return Pixmap at @p strip size, or a null pixmap when @p source is null
     *         or @p strip is empty (caller falls back to a flat fill).
     *
     * @throws None
     * @note   Deterministic: identical inputs produce identical output.
     * @warning Returns null when @p source is null; callers must handle it.
     * @since  0.20
     */
    QPixmap render(const QImage& source, const QRect& strip, qreal device_pixel_ratio,
                   const FrostedParams& params);

    /**
     * @brief  Reports whether the cache matches the given inputs.
     *
     * @param[in] source             Wallpaper image to test against.
     * @param[in] strip              Strip rect to test against.
     * @param[in] device_pixel_ratio Device-pixel-ratio to test against.
     * @param[in] params             Render parameters to test against.
     *
     * @return true when a valid cached pixmap exists for these exact inputs.
     *
     * @throws None
     * @since  0.20
     */
    bool isCacheValid(const QImage& source, const QRect& strip, qreal device_pixel_ratio,
                      const FrostedParams& params) const;

    /**
     * @brief  Forces the next render() to rebuild the pixmap.
     *
     * @throws None
     * @since  0.20
     */
    void invalidate() noexcept;

  private:
    /// Cache key: captures every input that affects the rendered output.
    struct Key {
        qint64 img_key{0};      ///< Source QImage::cacheKey().
        QRect rect{};           ///< Strip rect.
        qreal dpr{1.0};         ///< Device-pixel-ratio.
        FrostedParams params{}; ///< Full render parameter set.
        bool operator==(const Key&) const = default;
    };

    /// @brief Builds the key from current inputs (never throws).
    static Key makeKey(const QImage& source, const QRect& strip, qreal device_pixel_ratio,
                       const FrostedParams& params) noexcept;

    /// @brief Renders the frosted pixmap for @p source / @p strip (no caching).
    static QPixmap build(const QImage& source, const QRect& strip, const FrostedParams& params);

    Key key_{};
    QPixmap cache_{};
};

} // namespace cf::desktop
