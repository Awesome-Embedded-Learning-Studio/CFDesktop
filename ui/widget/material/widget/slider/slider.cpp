/**
 * @file slider.cpp
 * @brief Material Design 3 Slider Implementation
 *
 * Implements a Material Design 3 slider with horizontal/vertical orientations,
 * active/inactive track portions, thumb with elevation, tick marks, and state layers.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "slider.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/elevation_controller.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/ripple_helper.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionSlider>

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
// Material Design 3 Slider specifications (in dp)
constexpr float TRACK_HEIGHT_DP = 8.0f;
constexpr float THUMB_DIAMETER_DP = 20.0f;
constexpr float TOUCH_TARGET_DP = 48.0f;
constexpr float TICK_MARK_LENGTH_DP = 8.0f;
constexpr float TICK_MARK_WIDTH_DP = 2.0f;
constexpr float TICK_MARK_TO_TRACK_GAP_DP = 4.0f;
constexpr float FOCUS_RING_MARGIN_DP = 4.0f;
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

Slider::Slider(QWidget* parent) : QSlider(Qt::Horizontal, parent) {
    // Set size policy
    if (orientation() == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

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
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&Slider::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&Slider::update));
    connect(m_elevation, &MdElevationController::pressOffsetChanged, this,
            QOverload<>::of(&Slider::update));

    // Set default cursor
    setCursor(Qt::PointingHandCursor);
}

Slider::Slider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent) {
    // Set size policy
    if (orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

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
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&Slider::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&Slider::update));
    connect(m_elevation, &MdElevationController::pressOffsetChanged, this,
            QOverload<>::of(&Slider::update));

    // Set default cursor
    setCursor(Qt::PointingHandCursor);
}

Slider::~Slider() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void Slider::enterEvent(QEnterEvent* event) {
    QSlider::enterEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverEnter();
    update();
}

void Slider::leaveEvent(QEvent* event) {
    QSlider::leaveEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverLeave();
    if (m_ripple)
        m_ripple->onCancel();
    update();
}

void Slider::mousePressEvent(QMouseEvent* event) {
    QSlider::mousePressEvent(event);
    m_lastPressPos = event->pos();
    if (m_stateMachine)
        m_stateMachine->onPress(event->pos());
    if (m_ripple)
        m_ripple->onPress(event->pos(), thumbRect());
    if (m_elevation)
        m_elevation->setPressed(true);
    update();
}

void Slider::mouseReleaseEvent(QMouseEvent* event) {
    QSlider::mouseReleaseEvent(event);
    if (m_stateMachine)
        m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    if (m_elevation)
        m_elevation->setPressed(false);
    update();
}

void Slider::mouseMoveEvent(QMouseEvent* event) {
    QSlider::mouseMoveEvent(event);
    // Update ripple center if dragging
    if (m_ripple && isSliderDown()) {
        // Could update ripple position here for smooth follow effect
    }
}

void Slider::focusInEvent(QFocusEvent* event) {
    QSlider::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    update();
}

void Slider::focusOutEvent(QFocusEvent* event) {
    QSlider::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    update();
}

void Slider::changeEvent(QEvent* event) {
    QSlider::changeEvent(event);
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

// ============================================================================
// Size Hints
// ============================================================================

QSize Slider::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design 3 slider specifications:
    // - Track height: 4dp
    // - Thumb diameter: 20dp
    // - Touch target: 48dp minimum

    float touchTarget = helper.dpToPx(TOUCH_TARGET_DP);

    if (orientation() == Qt::Horizontal) {
        int width = 200; // Minimum horizontal width
        int height = int(std::ceil(touchTarget));
        return QSize(width, height);
    } else {
        int width = int(std::ceil(touchTarget));
        int height = 200; // Minimum vertical height
        return QSize(width, height);
    }
}

QSize Slider::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float touchTarget = helper.dpToPx(TOUCH_TARGET_DP);

    if (orientation() == Qt::Horizontal) {
        int width = 120; // Minimum horizontal width
        int height = int(std::ceil(touchTarget));
        return QSize(width, height);
    } else {
        int width = int(std::ceil(touchTarget));
        int height = 120; // Minimum vertical height
        return QSize(width, height);
    }
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164); // Purple 700
}
inline CFColor fallbackSurfaceVariant() {
    return CFColor(230, 225, 229); // Surface variant
}
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255); // White
}
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132); // Outline
}
} // namespace

CFColor Slider::activeTrackColor() const {
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

CFColor Slider::inactiveTrackColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return isEnabled() ? fallbackSurfaceVariant() : fallbackOutline();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        if (isEnabled()) {
            return CFColor(colorScheme.queryColor(SURFACE_VARIANT));
        }
        return CFColor(colorScheme.queryColor(OUTLINE));
    } catch (...) {
        return isEnabled() ? fallbackSurfaceVariant() : fallbackOutline();
    }
}

CFColor Slider::thumbColor() const {
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

CFColor Slider::stateLayerColor() const {
    return activeTrackColor();
}

float Slider::trackHeight() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(TRACK_HEIGHT_DP);
}

float Slider::thumbRadius() const {
    // Thumb is fully circular
    return thumbDiameter() / 2.0f;
}

float Slider::thumbDiameter() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(THUMB_DIAMETER_DP);
}

// ============================================================================
// Layout Helpers
// ============================================================================

float Slider::thumbPosition() const {
    int minVal = minimum();
    int maxVal = maximum();
    int curVal = value();

    if (maxVal <= minVal) {
        return 0.0f;
    }

    // Calculate position ratio (0.0 to 1.0)
    float ratio = static_cast<float>(curVal - minVal) / static_cast<float>(maxVal - minVal);
    return ratio;
}

QRectF Slider::trackRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float tHeight = trackHeight();
    float tDiameter = thumbDiameter();

    QRectF contentRect = rect();
    if (orientation() == Qt::Horizontal) {
        // Horizontal: center track vertically
        float y = (height() - tHeight) / 2.0f;
        // Leave space for thumb at ends
        float thumbRadius = tDiameter / 2.0f;
        float x = thumbRadius;
        float w = width() - 2.0f * thumbRadius;
        return QRectF(x, y, w, tHeight);
    } else {
        // Vertical: center track horizontally
        float x = (width() - tHeight) / 2.0f;
        // Leave space for thumb at ends
        float thumbRadius = tDiameter / 2.0f;
        float y = thumbRadius;
        float h = height() - 2.0f * thumbRadius;
        return QRectF(x, y, tHeight, h);
    }
}

QRectF Slider::activeTrackRect() const {
    QRectF track = trackRect();
    float ratio = thumbPosition();

    if (orientation() == Qt::Horizontal) {
        // Active track should extend to thumb center, not thumb edge
        float thumbCenter = track.left() + track.width() * ratio;
        float activeWidth = thumbCenter - track.left();
        return QRectF(track.left(), track.top(), activeWidth, track.height());
    } else {
        // Vertical: active part extends to thumb center from bottom
        float thumbCenter = track.bottom() - track.height() * ratio;
        float activeHeight = track.bottom() - thumbCenter;
        float y = thumbCenter;
        return QRectF(track.left(), y, track.width(), activeHeight);
    }
}

QRectF Slider::thumbRect() const {
    QRectF track = trackRect();
    float ratio = thumbPosition();
    float tDiameter = thumbDiameter();

    if (orientation() == Qt::Horizontal) {
        // Calculate thumb center position
        float thumbCenter = track.left() + track.width() * ratio;
        float x = thumbCenter - tDiameter / 2.0f;
        float y = (height() - tDiameter) / 2.0f;
        return QRectF(x, y, tDiameter, tDiameter);
    } else {
        // Vertical: calculate from bottom
        float thumbCenter = track.bottom() - track.height() * ratio;
        float x = (width() - tDiameter) / 2.0f;
        float y = thumbCenter - tDiameter / 2.0f;
        return QRectF(x, y, tDiameter, tDiameter);
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void Slider::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF track = trackRect();
    QRectF thumb = thumbRect();

    // Draw inactive track (only the portion after the thumb)
    if (orientation() == Qt::Horizontal) {
        // Right inactive track: 从滑块右边缘到轨道终点
        qreal rightWidth = track.right() - thumb.right();
        if (rightWidth > 0) {
            QRectF rightInactive(thumb.right(), track.top(), rightWidth, track.height());
            drawInactiveTrack(p, rightInactive);
        }
    } else {
        // Vertical: 类似逻辑，从下往上
        qreal bottomHeight = track.bottom() - thumb.bottom();
        if (bottomHeight > 0) {
            QRectF bottomInactive(track.left(), thumb.bottom(), track.width(), bottomHeight);
            drawInactiveTrack(p, bottomInactive);
        }

        qreal topHeight = thumb.top() - track.top();
        if (topHeight > 0) {
            QRectF topInactive(track.left(), track.top(), track.width(), topHeight);
            drawInactiveTrack(p, topInactive);
        }
    }

    // Draw active track (延伸到滑块中心)
    QRectF activeRect = activeTrackRect();
    if (!activeRect.isEmpty()) {
        drawActiveTrack(p, activeRect);
    }

    // Draw tick marks (if enabled)
    if (tickPosition() != QSlider::NoTicks) {
        drawTickMarks(p, track);
    }

    // Draw thumb
    drawThumb(p, thumb);

    // Draw ripple
    drawRipple(p, thumb);

    // Draw focus indicator
    drawFocusIndicator(p, track);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void Slider::drawInactiveTrack(QPainter& p, const QRectF& rect) {
    CFColor tColor = inactiveTrackColor();
    QColor color = tColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    QPainterPath shape = roundedRect(rect, trackHeight() / 2.0f);
    p.fillPath(shape, color);
}

void Slider::drawActiveTrack(QPainter& p, const QRectF& rect) {
    if (rect.width() <= 0 || rect.height() <= 0) {
        return;
    }

    CFColor tColor = activeTrackColor();
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

            int r = int(color.red() * (1.0f - opacity) + stateQColor.red() * opacity);
            int g = int(color.green() * (1.0f - opacity) + stateQColor.green() * opacity);
            int b = int(color.blue() * (1.0f - opacity) + stateQColor.blue() * opacity);
            color = QColor(r, g, b, color.alpha());
        }
    }

    QPainterPath shape = roundedRect(rect, trackHeight() / 2.0f);
    p.fillPath(shape, color);
}

void Slider::drawTickMarks(QPainter& p, const QRectF& trackRect) {
    CFColor tColor = inactiveTrackColor();
    QColor color = tColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    p.setPen(QPen(color, helper.dpToPx(TICK_MARK_WIDTH_DP)));

    float tickLength = helper.dpToPx(TICK_MARK_LENGTH_DP);
    int tickInterval = this->tickInterval();
    if (tickInterval <= 0) {
        tickInterval = 1;
    }

    int minVal = minimum();
    int maxVal = maximum();
    int numTicks = (maxVal - minVal) / tickInterval;

    if (orientation() == Qt::Horizontal) {
        // Draw ticks below the track
        float tickY = trackRect.bottom() + helper.dpToPx(TICK_MARK_TO_TRACK_GAP_DP);

        for (int i = 0; i <= numTicks; ++i) {
            float ratio = static_cast<float>(i) / static_cast<float>(numTicks);
            float x = trackRect.left() + trackRect.width() * ratio;
            p.drawLine(QPointF(x, tickY), QPointF(x, tickY + tickLength));
        }
    } else {
        // Draw ticks to the right of the track
        float tickX = trackRect.right() + helper.dpToPx(TICK_MARK_TO_TRACK_GAP_DP);

        for (int i = 0; i <= numTicks; ++i) {
            float ratio = static_cast<float>(i) / static_cast<float>(numTicks);
            float y = trackRect.bottom() - trackRect.height() * ratio;
            p.drawLine(QPointF(tickX, y), QPointF(tickX + tickLength, y));
        }
    }
}

void Slider::drawThumb(QPainter& p, const QRectF& rect) {
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
}

void Slider::drawRipple(QPainter& p, const QRectF& rect) {
    if (m_ripple) {
        // Update ripple color based on current state
        m_ripple->setColor(stateLayerColor());

        QPainterPath clipPath = roundedRect(rect, thumbRadius());
        m_ripple->paint(&p, clipPath);
    }
}

void Slider::drawFocusIndicator(QPainter& p, const QRectF& trackRect) {
    if (m_focusIndicator) {
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(FOCUS_RING_MARGIN_DP);

        QRectF focusRect;
        if (orientation() == Qt::Horizontal) {
            focusRect = trackRect.adjusted(-margin, -margin, margin, margin);
        } else {
            focusRect = trackRect.adjusted(-margin, -margin, margin, margin);
        }

        QPainterPath shape = roundedRect(focusRect, trackHeight() / 2.0f + margin);
        m_focusIndicator->paint(&p, shape, activeTrackColor());
    }
}

} // namespace cf::ui::widget::material
