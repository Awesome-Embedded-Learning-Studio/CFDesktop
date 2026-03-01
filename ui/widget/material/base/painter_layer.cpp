/**
 * @file painter_layer.cpp
 * @brief Material Design Painter Layer Implementation
 *
 * Provides basic color and opacity overlay rendering for state layers.
 * This is the simplest component in Layer 4, serving as the foundation
 * for more complex behavior components.
 *
 * @author Material Design Framework Team
 * @date 2026-02-28
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "painter_layer.h"
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material::base {

/**
 * @brief Constructor - initializes default values.
 *
 * @param parent QObject parent for memory management.
 */
PainterLayer::PainterLayer(QObject* parent)
    : QObject(parent), cached_color_(Qt::black), opacity_(1.0f) {}

/**
 * @brief Paints the layer with current color and opacity.
 *
 * This method fills the clip path with the cached color,
 * applying the current opacity value. Used in paintEvent
 * of Material widgets.
 *
 * @param painter QPainter instance (must be active).
 * @param clipPath Path to clip the drawing area.
 */
void PainterLayer::paint(QPainter* painter, const QPainterPath& clipPath) {
    if (!painter || opacity_ <= 0.0f) {
        return;
    }

    painter->save();
    painter->setClipPath(clipPath);

    // Convert CFColor to QColor and apply opacity
    QColor color = cached_color_.native_color();
    color.setAlphaF(color.alphaF() * opacity_);

    painter->fillPath(clipPath, color);
    painter->restore();
}

} // namespace cf::ui::widget::material::base
