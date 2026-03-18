/**
 * @file switch.cpp
 * @brief Material Design 3 Switch Implementation
 *
 * Implements a Material Design 3 switch (toggle) with animated thumb
 * position, track color transitions, state layers, and focus indicators.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "switch.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/material/cfmaterial_property_animation.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/elevation_controller.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/ripple_helper.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;

// ============================================================================
// Constants
// ============================================================================

namespace {
// Material Design 3 Switch specifications (in dp)
constexpr float TRACK_WIDTH_DP = 52.0f;
constexpr float TRACK_HEIGHT_DP = 32.0f;
constexpr float THUMB_DIAMETER_DP = 16.0f;
constexpr float THUMB_MARGIN_DP = 8.0f; // Distance from track edge
constexpr float TEXT_SPACING_DP = 12.0f;
constexpr float TOUCH_TARGET_DP = 48.0f;
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

Switch::Switch(QWidget* parent) : QCheckBox(parent) {
    // Set size policy
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Get animation factory from Application
    m_animationFactory = cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(
        application_support::Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_ripple = new RippleHelper(m_animationFactory, this);
    m_elevation = new MdElevationController(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Set ripple mode to bounded
    m_ripple->setMode(RippleHelper::Mode::Bounded);

    // Set initial elevation for thumb (level 1)
    m_elevation->setElevation(1);

    // Connect repaint signals
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&Switch::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&Switch::update));
    connect(m_elevation, &MdElevationController::pressOffsetChanged, this,
            QOverload<>::of(&Switch::update));

    // Initialize thumb position based on current state
    m_thumbPosition = isChecked() ? 1.0f : 0.0f;
    if (isChecked()) {
        m_stateMachine->onCheckedChanged(true);
    }

    // Set default cursor
    setCursor(Qt::PointingHandCursor);
}

Switch::Switch(const QString& text, QWidget* parent) : Switch(parent) {
    setText(text);
}

Switch::~Switch() {
    // Components are parented to this, Qt will delete them automatically
}

void Switch::setChecked(bool checked) {
    if (isChecked() == checked) {
        return;
    }
    QCheckBox::setChecked(checked);

    // When called from nextCheckState(), skip position update - animation
    // will handle it. Otherwise (direct call), snap immediately.
    if (!m_inNextCheckState) {
        if (m_stateMachine) {
            m_stateMachine->onCheckedChanged(checked);
        }
        m_thumbPosition = checked ? 1.0f : 0.0f;
        update();
    }
}

// ============================================================================
// Event Handlers
// ============================================================================

void Switch::enterEvent(QEnterEvent* event) {
    QCheckBox::enterEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverEnter();
    update();
}

void Switch::leaveEvent(QEvent* event) {
    QCheckBox::leaveEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverLeave();
    if (m_ripple)
        m_ripple->onCancel();
    update();
}

void Switch::mousePressEvent(QMouseEvent* event) {
    QCheckBox::mousePressEvent(event);
    if (m_stateMachine)
        m_stateMachine->onPress(event->pos());
    if (m_ripple)
        m_ripple->onPress(event->pos(), trackRect());
    if (m_elevation)
        m_elevation->setPressed(true);
    update();
}

void Switch::mouseReleaseEvent(QMouseEvent* event) {
    QCheckBox::mouseReleaseEvent(event);
    if (m_stateMachine)
        m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    if (m_elevation)
        m_elevation->setPressed(false);
    update();
}

void Switch::focusInEvent(QFocusEvent* event) {
    QCheckBox::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    update();
}

void Switch::focusOutEvent(QFocusEvent* event) {
    QCheckBox::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    update();
}

void Switch::changeEvent(QEvent* event) {
    QCheckBox::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        if (m_stateMachine) {
            if (isEnabled()) {
                m_stateMachine->onEnable();
            } else {
                m_stateMachine->onDisable();
            }
        }
        update();
    }
}

void Switch::nextCheckState() {
    // Set guard flag BEFORE calling base class - this prevents setChecked()
    // from snapping the position, since we'll animate it ourselves
    m_inNextCheckState = true;
    QCheckBox::nextCheckState();
    m_inNextCheckState = false;

    // Update state machine checked state
    if (m_stateMachine) {
        m_stateMachine->onCheckedChanged(isChecked());
    }

    // Animate thumb from current visual position to new target
    startThumbPositionAnimation(isChecked() ? 1.0f : 0.0f);
}

bool Switch::hitButton(const QPoint& pos) const {
    // For custom-drawn switch, entire widget area is clickable
    return rect().contains(pos);
}

// ============================================================================
// Size Hints
// ============================================================================

QSize Switch::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design 3 switch specifications:
    // - Track size: 52dp x 32dp
    // - Spacing between track and text: 12dp
    // - Touch target: 48dp minimum

    float trackW = trackWidth();
    float trackH = trackHeight();
    float spacing = helper.dpToPx(TEXT_SPACING_DP);
    float textWidth = text().isEmpty() ? 0.0f : fontMetrics().horizontalAdvance(text());

    float width = trackW + spacing + textWidth;

    // Ensure minimum 48dp width for easy clicking (even without text)
    float minWidth = helper.dpToPx(TOUCH_TARGET_DP);
    width = std::max(width, minWidth);

    float height = helper.dpToPx(TOUCH_TARGET_DP); // Fixed height for touch target

    return QSize(int(std::ceil(width)), int(std::ceil(height)));
}

QSize Switch::minimumSizeHint() const {
    return sizeHint();
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164); // Purple 700
}
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132); // Outline
}
inline CFColor fallbackSurface() {
    return CFColor(232, 226, 232); // Surface
}
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255); // White
}
inline CFColor fallbackOnSurface() {
    return CFColor(29, 27, 32); // On Surface (near black)
}
} // namespace

CFColor Switch::trackColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return isChecked() ? fallbackPrimary() : fallbackOutline();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        // Checked: PRIMARY, Unchecked: OUTLINE
        if (isChecked()) {
            return CFColor(colorScheme.queryColor(PRIMARY));
        }
        return CFColor(colorScheme.queryColor(OUTLINE));
    } catch (...) {
        return isChecked() ? fallbackPrimary() : fallbackOutline();
    }
}

CFColor Switch::thumbColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return isChecked() ? fallbackOnPrimary() : fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        // Checked: ON_PRIMARY (white), Unchecked: SURFACE
        if (isChecked()) {
            return CFColor(colorScheme.queryColor(ON_PRIMARY));
        }
        return CFColor(colorScheme.queryColor(SURFACE));
    } catch (...) {
        return isChecked() ? fallbackOnPrimary() : fallbackSurface();
    }
}

CFColor Switch::thumbIconColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return fallbackPrimary();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(PRIMARY));
    } catch (...) {
        return fallbackPrimary();
    }
}

CFColor Switch::stateLayerColor() const {
    return trackColor();
}

CFColor Switch::labelColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return fallbackOnSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_SURFACE));
    } catch (...) {
        return fallbackOnSurface();
    }
}

float Switch::trackCornerRadius() const {
    // Track uses full rounding (pill shape)
    return trackHeight() / 2.0f;
}

float Switch::thumbRadius() const {
    // Thumb is fully circular
    return thumbDiameter() / 2.0f;
}

// ============================================================================
// Dimension Helpers
// ============================================================================

float Switch::trackWidth() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(TRACK_WIDTH_DP);
}

float Switch::trackHeight() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(TRACK_HEIGHT_DP);
}

float Switch::thumbDiameter() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(THUMB_DIAMETER_DP);
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF Switch::trackRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Calculate vertical centering
    float trackH = trackHeight();
    float y = (height() - trackH) / 2.0f;

    // Track starts from left
    float x = 0;

    return QRectF(x, y, trackWidth(), trackH);
}

QRectF Switch::thumbRect() const {
    QRectF track = trackRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float margin = helper.dpToPx(THUMB_MARGIN_DP);

    // Calculate thumb position based on animation progress
    float maxTravel = track.width() - 2.0f * margin - thumbDiameter();
    float currentX = track.left() + margin + maxTravel * m_thumbPosition;
    float y = track.top() + (track.height() - thumbDiameter()) / 2.0f;

    return QRectF(currentX, y, thumbDiameter(), thumbDiameter());
}

QRectF Switch::textRect() const {
    QRectF track = trackRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float spacing = helper.dpToPx(TEXT_SPACING_DP);
    float x = track.right() + spacing;
    float y = 0;
    float w = width() - x;
    float h = height();

    return QRectF(x, y, w, h);
}

// ============================================================================
// Animation Helpers
// ============================================================================

void Switch::startThumbPositionAnimation(float target) {
    if (!m_animationFactory) {
        m_thumbPosition = target;
        update();
        return;
    }

    // Get cached animation from factory
    auto anim = m_animationFactory->createPropertyAnimation(
        &m_thumbPosition, m_thumbPosition, target, 200, cf::ui::base::Easing::Type::Standard, this);

    if (anim) {
        // IMPORTANT: Update range to fix cached animation's stale from/to values
        // The cached animation has old values from previous transition, which
        // would cause the thumb to jump to wrong position (bounce bug).
        if (auto* propAnim =
                dynamic_cast<cf::ui::components::material::CFMaterialPropertyAnimation*>(
                    anim.Get())) {
            propAnim->setRange(m_thumbPosition, target);
        }
        anim->start();
    } else {
        m_thumbPosition = target;
        update();
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void Switch::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF track = trackRect();

    // Step 1: Draw track background
    drawTrack(p, track);

    // Step 2: Draw ripple
    drawRipple(p, track);

    // Step 3: Draw thumb
    drawThumb(p, thumbRect());

    // Step 4: Draw text
    if (!text().isEmpty()) {
        drawText(p, textRect());
    }

    // Step 5: Draw focus indicator
    drawFocusIndicator(p, track);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void Switch::drawTrack(QPainter& p, const QRectF& rect) {
    CFColor tColor = trackColor();
    QColor color = tColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    // Handle state layer overlay
    if (m_stateMachine && isEnabled()) {
        float opacity = m_stateMachine->stateLayerOpacity();
        if (opacity > 0.0f) {
            CFColor stateColor = stateLayerColor();
            QColor stateQColor = stateColor.native_color();
            stateQColor.setAlphaF(opacity);

            // For unchecked state, blend with surface
            if (!isChecked()) {
                auto* app = application_support::Application::instance();
                if (app) {
                    try {
                        const auto& theme = app->currentTheme();
                        auto& colorScheme = theme.color_scheme();
                        QColor surface = CFColor(colorScheme.queryColor(SURFACE)).native_color();
                        int r = int(surface.red() * (1.0f - opacity) + stateQColor.red() * opacity);
                        int g =
                            int(surface.green() * (1.0f - opacity) + stateQColor.green() * opacity);
                        int b =
                            int(surface.blue() * (1.0f - opacity) + stateQColor.blue() * opacity);
                        color = QColor(r, g, b, color.alpha());
                    } catch (...) {
                        // Fallback to simple blend
                        int r = int(color.red() * (1.0f - opacity) + stateQColor.red() * opacity);
                        int g =
                            int(color.green() * (1.0f - opacity) + stateQColor.green() * opacity);
                        int b = int(color.blue() * (1.0f - opacity) + stateQColor.blue() * opacity);
                        color = QColor(r, g, b, color.alpha());
                    }
                } else {
                    int r = int(color.red() * (1.0f - opacity) + stateQColor.red() * opacity);
                    int g = int(color.green() * (1.0f - opacity) + stateQColor.green() * opacity);
                    int b = int(color.blue() * (1.0f - opacity) + stateQColor.blue() * opacity);
                    color = QColor(r, g, b, color.alpha());
                }
            } else {
                // Checked state: blend with primary
                int r = int(color.red() * (1.0f - opacity) + stateQColor.red() * opacity);
                int g = int(color.green() * (1.0f - opacity) + stateQColor.green() * opacity);
                int b = int(color.blue() * (1.0f - opacity) + stateQColor.blue() * opacity);
                color = QColor(r, g, b, color.alpha());
            }
        }
    }

    QPainterPath shape = roundedRect(rect, trackCornerRadius());
    p.fillPath(shape, color);
}

void Switch::drawThumb(QPainter& p, const QRectF& rect) {
    // Draw shadow first (for elevation effect)
    if (m_elevation && isEnabled()) {
        QPainterPath thumbShape = roundedRect(rect, thumbRadius());
        m_elevation->paintShadow(&p, thumbShape);
    }

    CFColor tColor = thumbColor();
    QColor color = tColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    // Draw thumb circle
    QPainterPath shape = roundedRect(rect, thumbRadius());
    p.fillPath(shape, color);

    // Optional: Draw check icon on thumb when checked
    // (Material Design 3 switches often show an icon when checked)
}

void Switch::drawRipple(QPainter& p, const QRectF& rect) {
    if (m_ripple) {
        // Update ripple color based on current state
        m_ripple->setColor(stateLayerColor());

        QPainterPath clipPath = roundedRect(rect, trackCornerRadius());
        m_ripple->paint(&p, clipPath);
    }
}

void Switch::drawText(QPainter& p, const QRectF& rect) {
    CFColor textColor = labelColor();

    if (!isEnabled()) {
        QColor color = textColor.native_color();
        color.setAlphaF(0.38f);
        p.setPen(color);
    } else {
        p.setPen(textColor.native_color());
    }

    // Use widget's font
    p.setFont(font());

    // Draw text vertically centered, left aligned
    QRectF textBounds = rect.adjusted(0, 2, 0, -2); // Small adjustment for visual centering
    p.drawText(textBounds, Qt::AlignLeft | Qt::AlignVCenter, text());
}

void Switch::drawFocusIndicator(QPainter& p, const QRectF& rect) {
    if (m_focusIndicator) {
        // Expand rect slightly for focus ring
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(4.0f);
        QRectF focusRect = rect.adjusted(-margin, -margin, margin, margin);

        QPainterPath shape = roundedRect(focusRect, trackCornerRadius() + margin);
        m_focusIndicator->paint(&p, shape, trackColor());
    }
}

} // namespace cf::ui::widget::material
