/**
 * @file ripple_helper.cpp
 * @brief Material Design Ripple Helper Implementation
 *
 * Manages ripple effect lifecycle and rendering following Material Design 3.
 * Ripples expand from touch point with fade-out on release.
 *
 * @author Material Design Framework Team
 * @date 2026-02-28
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "ripple_helper.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/timing_animation.h"

#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>

namespace cf::ui::widget::material::base {

using namespace cf::ui::components::material;
using namespace cf::ui::components;

/**
 * @brief Constructor - initializes ripple controller.
 *
 * @param factory aex::WeakPtr to animation factory for ripple animations.
 * @param parent QObject parent for memory management.
 */
RippleHelper::RippleHelper(aex::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                           QObject* parent)
    : QObject(parent), m_mode(Mode::Bounded), m_color(Qt::black), m_animator(factory) {}

// ============================================================================
// Configuration
// ============================================================================

void RippleHelper::setMode(Mode mode) {
    m_mode = mode;
}

void RippleHelper::setColor(const cf::ui::base::CFColor& color) {
    m_color = color;
}

// ============================================================================
// Event Handlers
// ============================================================================

/**
 * @brief Calculates maximum ripple radius.
 *
 * Uses diagonal length from touch point to farthest corner.
 *
 * @param rect Widget bounding rectangle.
 * @param center Touch point (ripple center).
 * @return Maximum radius in pixels.
 */
float RippleHelper::maxRadius(const QRectF& rect, const QPointF& center) const {
    // Calculate distance to each corner
    QPointF topLeft = rect.topLeft();
    QPointF topRight = rect.topRight();
    QPointF bottomLeft = rect.bottomLeft();
    QPointF bottomRight = rect.bottomRight();

    float d1 = std::hypot(center.x() - topLeft.x(), center.y() - topLeft.y());
    float d2 = std::hypot(center.x() - topRight.x(), center.y() - topRight.y());
    float d3 = std::hypot(center.x() - bottomLeft.x(), center.y() - bottomLeft.y());
    float d4 = std::hypot(center.x() - bottomRight.x(), center.y() - bottomRight.y());

    return std::max({d1, d2, d3, d4});
}

void RippleHelper::onPress(const QPoint& pos, const QRectF& widgetRect) {
    // Performance mode check
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        return; // Skip ripples if animations disabled
    }

    // Cancel any existing ripples before starting a new one
    // This prevents visual artifacts from rapid clicks
    onCancel();

    // Create new ripple
    MdRipple ripple;
    ripple.center = QPointF(pos);
    ripple.radius = 0.0f;
    ripple.opacity = 1.0f;
    ripple.releasing = false;

    // Calculate final radius
    ripple.maxRadius = maxRadius(widgetRect, ripple.center);

    // Store ripple
    m_ripples.append(ripple);

    // Start expand animation
    auto anim = factory->getAnimation("md.animation.rippleExpand");
    if (anim) {
        // Use index to track which ripple this animation controls
        int index = m_ripples.size() - 1;

        // Get raw pointer and set range if it's a timing animation
        auto* rawAnim = anim.Get();
        auto* timingAnim = static_cast<components::ICFTimingAnimation*>(rawAnim);
        if (timingAnim) {
            timingAnim->setRange(0.0f, 1.0f);
        }

        connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
                [this, index](float progress) {
                    if (index >= 0 && index < m_ripples.size()) {
                        m_ripples[index].radius = m_ripples[index].maxRadius * progress;
                        emit repaintNeeded();
                    }
                });

        connect(rawAnim, &components::ICFAbstractAnimation::finished, this, [this, index]() {
            // Animation done but ripple may still be in releasing state
            emit repaintNeeded();
        });

        rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
    }

    emit repaintNeeded();
}

void RippleHelper::onRelease() {
    // Trigger fade-out for all non-releasing ripples
    auto* factory = m_animator.Get();
    if (!factory)
        return;

    for (int i = 0; i < m_ripples.size(); ++i) {
        if (!m_ripples[i].releasing) {
            m_ripples[i].releasing = true;

            // Start fade animation
            auto anim = factory->getAnimation("md.animation.rippleFade");
            if (anim) {
                int index = i; // Capture for lambda

                // Get raw pointer and set range if it's a timing animation
                auto* rawAnim = anim.Get();
                auto* timingAnim = static_cast<components::ICFTimingAnimation*>(rawAnim);
                if (timingAnim) {
                    timingAnim->setRange(1.0f, 0.0f);
                }

                connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
                        [this, index](float progress) {
                            if (index >= 0 && index < m_ripples.size()) {
                                m_ripples[index].opacity = progress;
                                emit repaintNeeded();
                            }
                        });

                connect(rawAnim, &components::ICFAbstractAnimation::finished, this,
                        [this, index]() {
                            // Remove finished ripple
                            if (index >= 0 && index < m_ripples.size()) {
                                m_ripples.removeAt(index);
                                emit repaintNeeded();
                            }
                        });

                rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
            }
        }
    }

    emit repaintNeeded();
}

void RippleHelper::onCancel() {
    // Remove all active ripples immediately
    if (!m_ripples.isEmpty()) {
        m_ripples.clear();
        emit repaintNeeded();
    }
}

// ============================================================================
// Painting
// ============================================================================

namespace {
// Material Design 3 ripple fixed opacity (12% as per MD3 specs)
constexpr float RIPPLE_FIXED_OPACITY = 0.12f;
} // namespace

void RippleHelper::paint(QPainter* painter, const QPainterPath& clipPath) {
    if (m_ripples.isEmpty() || !painter) {
        return;
    }

    painter->save();

    // Apply clipping for bounded mode
    if (m_mode == Mode::Bounded) {
        painter->setClipPath(clipPath);
    }

    // Draw each ripple
    for (const auto& ripple : m_ripples) {
        if (ripple.radius <= 0.0f || ripple.opacity <= 0.0f) {
            continue;
        }

        // Create radial gradient for smooth ripple edge
        QRadialGradient gradient(ripple.center, ripple.radius);

        QColor color = m_color.native_color();
        // Apply fixed ripple opacity (Material Design 3 spec)
        // The ripple.opacity controls the fade-out animation, while RIPPLE_FIXED_OPACITY
        // ensures the ripple always has the correct visual strength
        color.setAlphaF(RIPPLE_FIXED_OPACITY * ripple.opacity);

        // Center is solid, edge fades slightly
        gradient.setColorAt(0.0f, color);
        gradient.setColorAt(0.7f, color);
        gradient.setColorAt(1.0f, QColor(color.red(), color.green(), color.blue(), 0));

        painter->setBrush(QBrush(gradient));
        painter->setPen(Qt::NoPen);

        painter->drawEllipse(ripple.center, ripple.radius, ripple.radius);
    }

    painter->restore();
}

// ============================================================================
// State Query
// ============================================================================

bool RippleHelper::hasActiveRipple() const {
    return !m_ripples.isEmpty();
}

} // namespace cf::ui::widget::material::base
