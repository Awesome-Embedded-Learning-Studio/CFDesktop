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
#include "base/easing.h"
#include "base/geometry_helper.h"
#include "components/material/cfmaterial_property_animation.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;

// ============================================================================
// Constants
// ============================================================================

namespace {
constexpr float TRACK_WIDTH_DP = 52.0f;
constexpr float TRACK_HEIGHT_DP = 32.0f;
constexpr float THUMB_DIAMETER_DP = 16.0f;
constexpr float THUMB_MARGIN_DP = 8.0f;
constexpr float TEXT_SPACING_DP = 12.0f;
constexpr float TOUCH_TARGET_DP = 48.0f;
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

Switch::Switch(QWidget* parent)
    : QCheckBox(parent),
      m_material(this, MaterialWidgetBase::Config{.useElevation = true, .initialElevation = 1}) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_thumbPosition = isChecked() ? 1.0f : 0.0f;
    if (isChecked()) {
        m_material.stateMachine()->onCheckedChanged(true);
    }

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

    if (!m_inNextCheckState) {
        if (m_material.stateMachine()) {
            m_material.stateMachine()->onCheckedChanged(checked);
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
    m_material.onEnterEvent();
}

void Switch::leaveEvent(QEvent* event) {
    QCheckBox::leaveEvent(event);
    m_material.onLeaveEvent();
}

void Switch::mousePressEvent(QMouseEvent* event) {
    QCheckBox::mousePressEvent(event);
    m_material.onMousePress(event->pos(), trackRect());
}

void Switch::mouseReleaseEvent(QMouseEvent* event) {
    QCheckBox::mouseReleaseEvent(event);
    m_material.onMouseRelease();
}

void Switch::focusInEvent(QFocusEvent* event) {
    QCheckBox::focusInEvent(event);
    m_material.onFocusIn();
}

void Switch::focusOutEvent(QFocusEvent* event) {
    QCheckBox::focusOutEvent(event);
    m_material.onFocusOut();
}

void Switch::changeEvent(QEvent* event) {
    QCheckBox::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

void Switch::nextCheckState() {
    m_inNextCheckState = true;
    QCheckBox::nextCheckState();
    m_inNextCheckState = false;

    if (m_material.stateMachine()) {
        m_material.stateMachine()->onCheckedChanged(isChecked());
    }

    startThumbPositionAnimation(isChecked() ? 1.0f : 0.0f);
}

bool Switch::hitButton(const QPoint& pos) const {
    return rect().contains(pos);
}

// ============================================================================
// Size Hints
// ============================================================================

QSize Switch::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float trackW = trackWidth();
    float spacing = helper.dpToPx(TEXT_SPACING_DP);
    float textWidth = text().isEmpty() ? 0.0f : fontMetrics().horizontalAdvance(text());

    float width = trackW + spacing + textWidth;
    float minWidth = helper.dpToPx(TOUCH_TARGET_DP);
    width = std::max(width, minWidth);

    float height = helper.dpToPx(TOUCH_TARGET_DP);

    return QSize(int(std::ceil(width)), int(std::ceil(height)));
}

QSize Switch::minimumSizeHint() const {
    return sizeHint();
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
}
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
}
inline CFColor fallbackSurface() {
    return CFColor(232, 226, 232);
}
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255);
}
inline CFColor fallbackOnSurface() {
    return CFColor(29, 27, 32);
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
    return trackHeight() / 2.0f;
}

float Switch::thumbRadius() const {
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
    float trackH = trackHeight();
    float y = (height() - trackH) / 2.0f;
    float x = 0;
    return QRectF(x, y, trackWidth(), trackH);
}

QRectF Switch::thumbRect() const {
    QRectF track = trackRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float margin = helper.dpToPx(THUMB_MARGIN_DP);

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
    auto factory = aex::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
        application_support::Application::animationFactory());

    if (!factory) {
        m_thumbPosition = target;
        update();
        return;
    }

    auto anim = factory->createPropertyAnimation(&m_thumbPosition, m_thumbPosition, target, 200,
                                                 cf::ui::base::Easing::Type::Standard, this);

    if (anim) {
        if (auto* propAnim = dynamic_cast<CFMaterialPropertyAnimation*>(anim.Get())) {
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

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    if (m_material.stateMachine() && isEnabled()) {
        float opacity = m_material.stateMachine()->stateLayerOpacity();
        if (opacity > 0.0f) {
            CFColor stateColor = stateLayerColor();
            QColor stateQColor = stateColor.native_color();
            stateQColor.setAlphaF(opacity);

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

void Switch::drawRipple(QPainter& p, const QRectF& rect) {
    if (m_material.ripple()) {
        m_material.ripple()->setColor(stateLayerColor());

        QPainterPath clipPath = roundedRect(rect, trackCornerRadius());
        m_material.ripple()->paint(&p, clipPath);
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

    p.setFont(font());

    QRectF textBounds = rect.adjusted(0, 2, 0, -2);
    p.drawText(textBounds, Qt::AlignLeft | Qt::AlignVCenter, text());
}

void Switch::drawFocusIndicator(QPainter& p, const QRectF& rect) {
    if (m_material.focusIndicator()) {
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(4.0f);
        QRectF focusRect = rect.adjusted(-margin, -margin, margin, margin);

        QPainterPath shape = roundedRect(focusRect, trackCornerRadius() + margin);
        m_material.focusIndicator()->paint(&p, shape, trackColor());
    }
}

} // namespace cf::ui::widget::material
