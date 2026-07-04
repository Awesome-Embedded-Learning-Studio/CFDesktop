/**
 * @file    icon_mask.h
 * @brief   Re-tint a monochrome PNG mask to a theme color.
 *
 * Taskbar tiles ship their icons as single-color (white) PNG masks and recolor
 * them at runtime to the active theme's on-surface token. tintedIconMask() does
 * that recolor in one call (load mask -> SourceIn fill) so TaskbarIcon and
 * StartButton share one implementation. Returns a null pixmap on any miss so
 * callers fall back to drawing an initial letter / built-in glyph.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  Loads the mask at @p path and fills its silhouette with @p color.
 *
 * Uses QPainter::CompositionMode_SourceIn so the mask's alpha channel is kept
 * and every opaque pixel becomes @p color: the standard way to theme a flat
 * single-color icon without shipping one asset per color.
 *
 * @param[in] path   Qt resource or filesystem path to the PNG mask.
 * @param[in] color  Solid color to paint the silhouette.
 *
 * @return The tinted pixmap, or a null pixmap when @p path is empty or the
 *         resource cannot be loaded (caller falls back to a glyph or letter).
 *
 * @throws None
 * @note   Preserves the source device-pixel-ratio for crisp high-dpi tiles.
 * @warning None
 * @since  0.20
 * @ingroup components
 */
inline QPixmap tintedIconMask(const QString& path, const QColor& color) {
    if (path.isEmpty()) {
        return {};
    }
    QPixmap mask{path};
    if (mask.isNull()) {
        return {};
    }
    QPixmap out(mask.size());
    out.setDevicePixelRatio(mask.devicePixelRatio());
    out.fill(Qt::transparent);
    QPainter painter{&out};
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(0, 0, mask);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(out.rect(), color);
    return out;
}

} // namespace cf::desktop::desktop_component
