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
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionSlider>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;

// ============================================================================
// Constants
// ============================================================================

namespace {
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

Slider::Slider(QWidget* parent)
    : QSlider(Qt::Horizontal, parent),
      m_material(this, MaterialWidgetBase::Config{.useElevation = true, .initialElevation = 1}) {
    if (orientation() == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
    setCursor(Qt::PointingHandCursor);
}

Slider::Slider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent),
      m_material(this, MaterialWidgetBase::Config{.useElevation = true, .initialElevation = 1}) {
    if (orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
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
    m_material.onEnterEvent();
}

void Slider::leaveEvent(QEvent* event) {
    QSlider::leaveEvent(event);
    m_material.onLeaveEvent();
}

void Slider::mousePressEvent(QMouseEvent* event) {
    QSlider::mousePressEvent(event);
    m_lastPressPos = event->pos();
    m_material.onMousePress(event->pos(), thumbRect());
}

void Slider::mouseReleaseEvent(QMouseEvent* event) {
    QSlider::mouseReleaseEvent(event);
    m_material.onMouseRelease();
}

void Slider::mouseMoveEvent(QMouseEvent* event) {
    QSlider::mouseMoveEvent(event);
}

void Slider::focusInEvent(QFocusEvent* event) {
    QSlider::focusInEvent(event);
    m_material.onFocusIn();
}

void Slider::focusOutEvent(QFocusEvent* event) {
    QSlider::focusOutEvent(event);
    m_material.onFocusOut();
}

void Slider::changeEvent(QEvent* event) {
    QSlider::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize Slider::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float touchTarget = helper.dpToPx(TOUCH_TARGET_DP);

    if (orientation() == Qt::Horizontal) {
        return QSize(200, int(std::ceil(touchTarget)));
    } else {
        return QSize(int(std::ceil(touchTarget)), 200);
    }
}

QSize Slider::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float touchTarget = helper.dpToPx(TOUCH_TARGET_DP);

    if (orientation() == Qt::Horizontal) {
        return QSize(120, int(std::ceil(touchTarget)));
    } else {
        return QSize(int(std::ceil(touchTarget)), 120);
    }
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
}
inline CFColor fallbackSurfaceVariant() {
    return CFColor(230, 225, 229);
}
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255);
}
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
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

    return static_cast<float>(curVal - minVal) / static_cast<float>(maxVal - minVal);
}

QRectF Slider::trackRect() const {
    float tHeight = trackHeight();
    float tDiameter = thumbDiameter();

    if (orientation() == Qt::Horizontal) {
        float y = (height() - tHeight) / 2.0f;
        float thumbRadius = tDiameter / 2.0f;
        float x = thumbRadius;
        float w = width() - 2.0f * thumbRadius;
        return QRectF(x, y, w, tHeight);
    } else {
        float x = (width() - tHeight) / 2.0f;
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
        float thumbCenter = track.left() + track.width() * ratio;
        float activeWidth = thumbCenter - track.left();
        return QRectF(track.left(), track.top(), activeWidth, track.height());
    } else {
        float thumbCenter = track.bottom() - track.height() * ratio;
        float activeHeight = track.bottom() - thumbCenter;
        return QRectF(track.left(), thumbCenter, track.width(), activeHeight);
    }
}

QRectF Slider::thumbRect() const {
    QRectF track = trackRect();
    float ratio = thumbPosition();
    float tDiameter = thumbDiameter();

    if (orientation() == Qt::Horizontal) {
        float thumbCenter = track.left() + track.width() * ratio;
        float x = thumbCenter - tDiameter / 2.0f;
        float y = (height() - tDiameter) / 2.0f;
        return QRectF(x, y, tDiameter, tDiameter);
    } else {
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

    // Draw inactive track
    if (orientation() == Qt::Horizontal) {
        qreal rightWidth = track.right() - thumb.right();
        if (rightWidth > 0) {
            QRectF rightInactive(thumb.right(), track.top(), rightWidth, track.height());
            drawInactiveTrack(p, rightInactive);
        }
    } else {
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

    // Draw active track
    QRectF activeRect = activeTrackRect();
    if (!activeRect.isEmpty()) {
        drawActiveTrack(p, activeRect);
    }

    // Draw tick marks
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

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    if (m_material.stateMachine() && isEnabled()) {
        float opacity = m_material.stateMachine()->stateLayerOpacity();
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
        float tickY = trackRect.bottom() + helper.dpToPx(TICK_MARK_TO_TRACK_GAP_DP);

        for (int i = 0; i <= numTicks; ++i) {
            float ratio = static_cast<float>(i) / static_cast<float>(numTicks);
            float x = trackRect.left() + trackRect.width() * ratio;
            p.drawLine(QPointF(x, tickY), QPointF(x, tickY + tickLength));
        }
    } else {
        float tickX = trackRect.right() + helper.dpToPx(TICK_MARK_TO_TRACK_GAP_DP);

        for (int i = 0; i <= numTicks; ++i) {
            float ratio = static_cast<float>(i) / static_cast<float>(numTicks);
            float y = trackRect.bottom() - trackRect.height() * ratio;
            p.drawLine(QPointF(tickX, y), QPointF(tickX + tickLength, y));
        }
    }
}

void Slider::drawThumb(QPainter& p, const QRectF& rect) {
    if (m_material.elevation() && isEnabled()) {
        QPainterPath thumbShape = roundedRect(rect, thumbRadius());
        m_material.elevation()->paintShadow(&p, thumbShape);
    }

    CFColor tColor = thumbColor();
    QColor color = tColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    QPainterPath shape = roundedRect(rect, thumbRadius());
    p.fillPath(shape, color);
}

void Slider::drawRipple(QPainter& p, const QRectF& rect) {
    if (m_material.ripple()) {
        m_material.ripple()->setColor(stateLayerColor());

        QPainterPath clipPath = roundedRect(rect, thumbRadius());
        m_material.ripple()->paint(&p, clipPath);
    }
}

void Slider::drawFocusIndicator(QPainter& p, const QRectF& trackRect) {
    if (m_material.focusIndicator()) {
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(FOCUS_RING_MARGIN_DP);

        QRectF focusRect = trackRect.adjusted(-margin, -margin, margin, margin);

        QPainterPath shape = roundedRect(focusRect, trackHeight() / 2.0f + margin);
        m_material.focusIndicator()->paint(&p, shape, activeTrackColor());
    }
}

} // namespace cf::ui::widget::material
