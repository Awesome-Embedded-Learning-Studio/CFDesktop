/**
 * @file focus_ring.cpp
 * @brief Material Design Focus Indicator Implementation
 *
 * Draws focus ring following Material Design 3 specifications.
 * Ring width: 3dp, inset: 3dp from widget boundary.
 *
 * @author Material Design Framework Team
 * @date 2026-02-28
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "focus_ring.h"
#include "base/device_pixel.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/timing_animation.h"

#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>

namespace cf::ui::widget::material::base {

using namespace cf::ui::components::material;
using namespace cf::ui::components;
using namespace cf::ui::base;

/**
 * @brief Constructor - initializes focus indicator.
 *
 * @param factory WeakPtr to animation factory for fade animation.
 * @param parent QObject parent for memory management.
 */
MdFocusIndicator::MdFocusIndicator(
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory, QObject* parent)
    : QObject(parent), m_progress(0.0f), m_animator(factory) {}

// ============================================================================
// Event Handlers
// ============================================================================

void MdFocusIndicator::onFocusIn() {
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        // Direct set if animations disabled
        m_progress = 1.0f;
        return;
    }

    // Get fade animation for ring appearance
    auto anim = factory->getAnimation("md.animation.fadeIn");
    if (!anim) {
        m_progress = 1.0f;
        return;
    }

    // Get raw pointer and set range if it's a timing animation
    auto* rawAnim = anim.Get();
    auto* timingAnim = static_cast<components::ICFTimingAnimation*>(rawAnim);
    if (timingAnim) {
        timingAnim->setRange(0.0f, 1.0f);
    }

    connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
            [this](float progress) { m_progress = progress; });

    rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
}

void MdFocusIndicator::onFocusOut() {
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        // Direct set if animations disabled
        m_progress = 0.0f;
        return;
    }

    // Get fade animation for ring disappearance
    auto anim = factory->getAnimation("md.animation.fadeOut");
    if (!anim) {
        m_progress = 0.0f;
        return;
    }

    // Get raw pointer and set range if it's a timing animation
    auto* rawAnim = anim.Get();
    auto* timingAnim = static_cast<components::ICFTimingAnimation*>(rawAnim);
    if (timingAnim) {
        timingAnim->setRange(1.0f, 0.0f);
    }

    connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
            [this](float progress) { m_progress = progress; });

    rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
}

// ============================================================================
// Painting
// ============================================================================

/**
 * @brief Paints the focus ring.
 *
 * The ring is drawn outside the widget shape with:
 * - Width: 3dp
 * - Inset from boundary: 3dp
 * - Opacity based on animation progress
 *
 * @param painter QPainter instance (must be active).
 * @param shape Widget shape path for focus ring outline.
 * @param indicatorColor Ring color (typically onSurface or similar).
 */
void MdFocusIndicator::paint(QPainter* painter, const QPainterPath& shape,
                             const CFColor& indicatorColor) {
    if (m_progress <= 0.0f || !painter) {
        return;
    }

    // Convert dp to pixels
    // Note: Using device pixel ratio 1.0 for now. In production, get from QApplication.
    device::CanvasUnitHelper helper(1.0);
    float ringWidth = helper.dpToPx(3.0f); // 3dp ring width
    float inset = helper.dpToPx(3.0f);     // 3dp inset

    painter->save();

    // Create stroker for the ring
    QPainterPathStroker stroker;
    stroker.setWidth(ringWidth);
    stroker.setCapStyle(Qt::SquareCap);
    stroker.setJoinStyle(Qt::MiterJoin);

    // Create offset path (inset from widget boundary)
    QPainterPathStroker insetStroker;
    insetStroker.setWidth(inset * 2); // Inset on both sides
    QPainterPath innerShape = insetStroker.createStroke(shape);
    // Subtract to get the inset outline
    QPainterPath outlinePath = shape.subtracted(innerShape);

    // Create the ring path
    QPainterPath ringPath = stroker.createStroke(outlinePath);

    // Apply opacity based on animation progress
    QColor color = indicatorColor.native_color();
    color.setAlphaF(color.alphaF() * m_progress);

    // Draw the ring
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawPath(ringPath);

    painter->restore();
}

} // namespace cf::ui::widget::material::base
