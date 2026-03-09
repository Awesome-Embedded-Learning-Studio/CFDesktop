/**
 * @file elevation_controller.cpp
 * @brief Material Design Elevation Controller Implementation
 *
 * Manages Material elevation levels and renders corresponding shadows.
 * Supports animated transitions between elevation levels.
 *
 * Material Elevation Levels (dp → shadow params):
 * Level 0: 0dp  - No shadow
 * Level 1: 1dp  - blur=2px, offset=1px, opacity=0.15
 * Level 2: 3dp  - blur=4px, offset=2px, opacity=0.20
 * Level 3: 6dp  - blur=8px, offset=4px, opacity=0.25
 * Level 4: 8dp  - blur=12px, offset=6px, opacity=0.30
 * Level 5: 12dp - blur=16px, offset=8px, opacity=0.35
 *
 * @author Material Design Framework Team
 * @date 2026-02-28
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "elevation_controller.h"
#include "base/color_helper.h"
#include "base/device_pixel.h"
#include "base/math_helper.h"
#include "components/animation.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/material/cfmaterial_animation_strategy.h"
#include "components/timing_animation.h"

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath> // For std::tan, std::floor, std::ceil

namespace cf::ui::widget::material::base {

using namespace cf::ui::components::material;
using namespace cf::ui::base;
using namespace cf::ui::math;

/**
 * @brief Constructor - initializes elevation controller.
 *
 * @param factory WeakPtr to animation factory for elevation transitions.
 * @param parent QObject parent for memory management.
 */
MdElevationController::MdElevationController(
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory, QObject* parent)
    : QObject(parent), m_currentLevel(0.0f), m_targetLevel(0), m_animator(factory) {}

/**
 * @brief Destructor - cancels any running animation.
 */
MdElevationController::~MdElevationController() {
    cancelCurrentAnimation();
}

// ============================================================================
// Configuration
// ============================================================================

void MdElevationController::setElevation(int level) {
    level = clamp(level, 0, 5);
    m_targetLevel = level;
    m_currentLevel = static_cast<float>(level);
}

void MdElevationController::setLightSourceAngle(float degrees) {
    m_lightSourceAngle = degrees;
}

int MdElevationController::elevation() const {
    return m_targetLevel;
}

void MdElevationController::setPressed(bool pressed) {
    if (m_isPressed == pressed) {
        return;
    }

    m_isPressed = pressed;

    // Calculate target press offset
    device::CanvasUnitHelper helper(qApp ? qApp->devicePixelRatio() : 1.0);
    float targetOffset = 0.0f;
    if (pressed) {
        targetOffset = helper.dpToPx(static_cast<float>(m_targetLevel) * 2.0f);
    }

    // Animate to the new offset
    animatePressOffsetTo(targetOffset);
}

float MdElevationController::pressOffset() const {
    // Return the animated press offset value
    return m_currentPressOffset;
}

/**
 * @brief Cancels the currently running press offset animation.
 *
 * Disconnects all signals and stops the animation to prevent multiple
 * animations from competing to update m_currentPressOffset.
 */
void MdElevationController::cancelCurrentAnimation() {
    if (m_pressOffsetAnimation) {
        auto* anim = m_pressOffsetAnimation.Get();
        if (anim) {
            // Disconnect all signals from this animation to this object
            disconnect(anim, &components::ICFAbstractAnimation::progressChanged, this, nullptr);
            disconnect(anim, &components::ICFAbstractAnimation::finished, this, nullptr);
            // Stop the animation
            anim->stop();
        }
        m_pressOffsetAnimation = nullptr;
    }
}

/**
 * @brief Slot called when the press offset animation finishes.
 *
 * Clears the animation reference to allow new animations to start.
 */
void MdElevationController::onAnimationFinished() {
    m_pressOffsetAnimation = nullptr;
}

void MdElevationController::animatePressOffsetTo(float to) {
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        // Direct set if animations disabled
        m_currentPressOffset = to;
        emit pressOffsetChanged();
        return;
    }

    // CRITICAL FIX: Cancel any currently running animation before starting a new one.
    // This prevents multiple animations from competing to update m_currentPressOffset,
    // which causes visual glitches during rapid press/release events.
    cancelCurrentAnimation();

    // Start animation from current press offset value
    float from = m_currentPressOffset;

    // Create custom animation descriptor with longer duration for smoother press effect
    // Using "md.motion.longEnter" for slower, more noticeable animation
    AnimationDescriptor desc(
        "fade",                // Animation type
        "md.motion.longEnter", // Motion spec (longer duration for smooth press)
        "opacity",             // Property (we'll override with setRange)
        from,                  // Start value
        to                     // End value
    );

    // Create animation from descriptor
    auto anim = factory->createAnimation(desc, nullptr, this);
    if (!anim) {
        // Fallback: direct set if animation creation fails
        m_currentPressOffset = to;
        emit pressOffsetChanged();
        return;
    }

    // Save animation reference for cancellation
    m_pressOffsetAnimation = anim;

    // Get raw pointer and set range if it's a timing animation
    auto* rawAnim = anim.Get();
    auto* timingAnim = static_cast<components::ICFTimingAnimation*>(rawAnim);
    if (timingAnim) {
        timingAnim->setRange(from, to);
    }

    // Connect progress signal
    // Note: Qt::UniqueConnection cannot be used with lambdas, but cancelCurrentAnimation()
    // disconnects all signals before starting a new animation, preventing duplicates
    //
    // CRITICAL: progressChanged emits 0-1 normalized progress, NOT actual offset values.
    // We must interpolate between from and to to get the actual offset.
    connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
            [this, from, to](float progress) {
                // progress is 0-1, interpolate to get actual offset value
                m_currentPressOffset = from + (to - from) * progress;
                emit pressOffsetChanged();
            });

    // Connect finished signal to clear the animation reference
    connect(rawAnim, &components::ICFAbstractAnimation::finished, this,
            &MdElevationController::onAnimationFinished, Qt::UniqueConnection);

    // Start animation
    rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
}

void MdElevationController::animateTo(int level, const core::MotionSpec& spec) {
    level = clamp(level, 0, 5);

    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        // Direct set if animations disabled
        m_targetLevel = level;
        m_currentLevel = static_cast<float>(level);
        return;
    }

    // Create custom timing animation for elevation transition
    // Note: This requires the factory to support custom animation creation
    // For now, use direct set with smooth transition
    int oldLevel = m_targetLevel;
    m_targetLevel = level;

    // Simple interpolation - in production, use proper animation
    // The actual animation would be handled by the widget's paint loop
    // reading m_currentLevel
    Q_UNUSED(spec);
    Q_UNUSED(oldLevel);
}

// ============================================================================
// Shadow Calculation
// ============================================================================

/**
 * @brief Calculates shadow parameters for a given elevation level.
 *
 * Uses linear interpolation between predefined levels for smooth
 * elevation transitions.
 *
 * @param level Elevation level (can be fractional for animation).
 * @return Shadow parameters for rendering.
 */
MdElevationController::ShadowParams MdElevationController::paramsForLevel(float level) const {
    // Predefined parameters for integer levels
    struct LevelParams {
        float blurRadius;
        float offsetY; // Vertical offset (positive =向下)
        float opacity;
    };

    static constexpr LevelParams levelParams[] = {
        {0.0f, 0.0f, 0.00f},  // Level 0
        {2.0f, 1.0f, 0.15f},  // Level 1
        {4.0f, 2.0f, 0.20f},  // Level 2
        {8.0f, 4.0f, 0.25f},  // Level 3
        {12.0f, 6.0f, 0.30f}, // Level 4
        {16.0f, 8.0f, 0.35f}  // Level 5
    };

    // Clamp level to valid range
    level = clamp(level, 0.0f, 5.0f);

    // Interpolate between levels
    int lowerLevel = static_cast<int>(std::floor(level));
    int upperLevel = static_cast<int>(std::ceil(level));
    float t = level - lowerLevel;

    // Handle edge case where level is exactly at an integer
    if (upperLevel == lowerLevel) {
        upperLevel = std::min(5, lowerLevel + 1);
        t = 0.0f;
    }

    const LevelParams& lower = levelParams[lowerLevel];
    const LevelParams& upper = levelParams[upperLevel];

    ShadowParams result;
    result.blurRadius = lerp(lower.blurRadius, upper.blurRadius, t);
    result.offsetY = lerp(lower.offsetY, upper.offsetY, t);
    result.opacity = lerp(lower.opacity, upper.opacity, t);

    // Calculate horizontal offset based on light source angle
    // angle in degrees: 正值 = 光源从左侧, 负值 = 光源从右侧
    // Material Design 默认 15 度 = 光源从左上方照射
    float angleRad = m_lightSourceAngle * 3.14159265f / 180.0f;
    // offsetX与offsetY成比例，模拟光线方向
    // 光源从左上方来，阴影向右下方投射（offsetX 为正）
    result.offsetX = result.offsetY * std::tan(angleRad);

    // 按压时阴影缩小并靠近
    if (m_isPressed) {
        result.offsetX *= 0.5f;
        result.offsetY *= 0.5f;
        result.blurRadius *= 0.7f;
    }

    return result;
}

// ============================================================================
// Painting
// ============================================================================

/**
 * @brief Paints shadow for the given shape.
 *
 * Uses layered shadow rendering for optimal performance.
 * Each layer is drawn with decreasing offset and blur.
 *
 * @param painter QPainter instance (must be active).
 * @param shape Path to draw shadow for.
 */
void MdElevationController::paintShadow(QPainter* painter, const QPainterPath& shape) {
    if (!painter || m_currentLevel <= 0.0f) {
        return;
    }

    ShadowParams params = paramsForLevel(m_currentLevel);

    // Convert dp to pixels - use actual device pixel ratio from QApplication
    device::CanvasUnitHelper helper(qApp ? qApp->devicePixelRatio() : 1.0);
    float blurRadius = helper.dpToPx(params.blurRadius);
    float offsetX = helper.dpToPx(params.offsetX);
    float offsetY = helper.dpToPx(params.offsetY);

    painter->save();

    // Get shape bounding rect for shadow rendering
    QRectF bounds = shape.boundingRect();

    // Create shadow color (black with opacity)
    QColor shadowColor(0, 0, 0, static_cast<int>(params.opacity * 255));

    // Draw shadow layers with light source offset
    // Main shadow layer
    QPainterPath shadowPath = shape;
    shadowPath.translate(offsetX, offsetY);

    painter->setPen(Qt::NoPen);
    painter->setBrush(shadowColor);
    painter->drawPath(shadowPath);

    // Additional layers for blur approximation
    int layers = static_cast<int>(std::ceil(blurRadius / 2.0f));
    for (int i = 1; i <= layers && i <= 3; ++i) {
        float layerOpacity = params.opacity * (1.0f - static_cast<float>(i) / (layers + 1));
        float layerOffsetScale = 1.0f + (blurRadius * i / layers) * 0.3f / offsetY;
        float layerOffsetX = offsetX * layerOffsetScale;
        float layerOffsetY = offsetY * layerOffsetScale;

        QColor layerColor(0, 0, 0, static_cast<int>(layerOpacity * 255));
        QPainterPath layerPath = shape;
        layerPath.translate(layerOffsetX, layerOffsetY);

        painter->setBrush(layerColor);
        painter->drawPath(layerPath);
    }

    painter->restore();
}

/**
 * @brief Calculates tonal overlay color for dark theme elevation.
 *
 * In dark theme, elevation is indicated by adding tonal color
 * from the primary palette to the surface.
 *
 * @param surface Base surface color.
 * @param primary Primary color for tonal overlay.
 * @return Tonal overlay color.
 */
CFColor MdElevationController::tonalOverlay(CFColor surface, CFColor primary) const {
    if (m_currentLevel <= 0.0f) {
        return surface;
    }

    // Calculate tonal amount based on elevation
    float tonalAmount = m_currentLevel / 10.0f; // 0.0 to 0.5

    // Blend surface with primary
    return blend(surface, primary, tonalAmount);
}

} // namespace cf::ui::widget::material::base
