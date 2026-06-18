/**
 * @file radiobutton.cpp
 * @brief Material Design 3 RadioButton Implementation
 *
 * Implements a Material Design 3 radio button with circular selection area,
 * inner circle scale animation, ripple effects, and focus indicators.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "radiobutton.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/easing.h"
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
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// Material Design 3 specifications for RadioButton
namespace {
constexpr float RADIO_SIZE_DP = 20.0f;        // Outer ring diameter
constexpr float INNER_CIRCLE_SIZE_DP = 10.0f; // Inner circle diameter (50% of outer)
constexpr float TOUCH_TARGET_DP = 48.0f;      // Minimum touch target size
constexpr float TEXT_SPACING_DP = 8.0f;       // Spacing between radio and text
constexpr float STROKE_WIDTH_DP = 2.0f;       // Outer ring stroke width

constexpr float INNER_CIRCLE_SCALE_UNCHECKED = 0.0f;
constexpr float INNER_CIRCLE_SCALE_CHECKED = 1.0f;
} // namespace

// Fallback colors when theme is not available
namespace {
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255);
} // White
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
} // Outline
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
inline CFColor fallbackError() {
    return CFColor(186, 26, 26);
} // Error red
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

RadioButton::RadioButton(QWidget* parent)
    : QRadioButton(parent), m_material(this, MaterialWidgetBase::Config{.useElevation = false}) {
    setFont(labelFont());
    m_innerCircleScale = isChecked() ? INNER_CIRCLE_SCALE_CHECKED : INNER_CIRCLE_SCALE_UNCHECKED;
}

RadioButton::RadioButton(const QString& text, QWidget* parent) : RadioButton(parent) {
    setText(text);
}

RadioButton::~RadioButton() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void RadioButton::enterEvent(QEnterEvent* event) {
    QRadioButton::enterEvent(event);
    m_material.onEnterEvent();
}

void RadioButton::leaveEvent(QEvent* event) {
    QRadioButton::leaveEvent(event);
    m_material.onLeaveEvent();
}

void RadioButton::mousePressEvent(QMouseEvent* event) {
    QRadioButton::mousePressEvent(event);
    if (m_pressEffectEnabled) {
        QRectF radioRect = calculateRadioRect();
        m_material.onMousePress(event->pos(), radioRect.united(calculateTextRect(radioRect)));
    } else {
        m_material.stateMachine()->onPress(event->pos());
        update();
    }
}

void RadioButton::mouseReleaseEvent(QMouseEvent* event) {
    QRadioButton::mouseReleaseEvent(event);
    if (m_pressEffectEnabled) {
        m_material.onMouseRelease();
    } else {
        m_material.stateMachine()->onRelease();
        if (m_material.ripple())
            m_material.ripple()->onRelease();
        update();
    }
}

void RadioButton::focusInEvent(QFocusEvent* event) {
    QRadioButton::focusInEvent(event);
    m_material.onFocusIn();
}

void RadioButton::focusOutEvent(QFocusEvent* event) {
    QRadioButton::focusOutEvent(event);
    m_material.onFocusOut();
}

void RadioButton::changeEvent(QEvent* event) {
    QRadioButton::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

void RadioButton::nextCheckState() {
    bool wasChecked = isChecked();
    QRadioButton::nextCheckState();
    bool isCheckedNow = isChecked();

    if (wasChecked != isCheckedNow) {
        if (m_material.stateMachine()) {
            m_material.stateMachine()->onCheckedChanged(isCheckedNow);
        }
        startInnerCircleAnimation(isCheckedNow);
    }
}

void RadioButton::setChecked(bool checked) {
    bool wasChecked = isChecked();
    if (wasChecked == checked) {
        return;
    }

    QRadioButton::setChecked(checked);
    bool isCheckedNow = isChecked();

    if (wasChecked != isCheckedNow) {
        if (m_material.stateMachine()) {
            m_material.stateMachine()->onCheckedChanged(isCheckedNow);
        }
        m_innerCircleScale =
            isCheckedNow ? INNER_CIRCLE_SCALE_CHECKED : INNER_CIRCLE_SCALE_UNCHECKED;
        update();
    }
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

bool RadioButton::hasError() const {
    return m_hasError;
}

void RadioButton::setError(bool error) {
    if (m_hasError != error) {
        m_hasError = error;
        update();
    }
}

bool RadioButton::pressEffectEnabled() const {
    return m_pressEffectEnabled;
}

void RadioButton::setPressEffectEnabled(bool enabled) {
    if (m_pressEffectEnabled != enabled) {
        m_pressEffectEnabled = enabled;
        update();
    }
}

bool RadioButton::hitButton(const QPoint& pos) const {
    return rect().contains(pos);
}

// ============================================================================
// Size Hints
// ============================================================================

QSize RadioButton::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float leftPadding = helper.dpToPx(12.0f);
    float radioSize = helper.dpToPx(RADIO_SIZE_DP);
    float textSpacing = helper.dpToPx(TEXT_SPACING_DP);

    float textWidth = text().isEmpty() ? 0.0f : fontMetrics().horizontalAdvance(text());

    float totalWidth = leftPadding + radioSize + textSpacing + textWidth;
    float height = helper.dpToPx(TOUCH_TARGET_DP);

    return QSize(int(std::ceil(totalWidth)), int(std::ceil(height)));
}

QSize RadioButton::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float leftPadding = helper.dpToPx(12.0f);
    float radioSize = helper.dpToPx(RADIO_SIZE_DP);
    float textSpacing = helper.dpToPx(TEXT_SPACING_DP);
    float minTextWidth = text().isEmpty() ? 0 : fontMetrics().horizontalAdvance("M");

    float totalWidth = leftPadding + radioSize + textSpacing + minTextWidth;
    float height = helper.dpToPx(TOUCH_TARGET_DP);

    return QSize(int(std::ceil(totalWidth)), int(std::ceil(height)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor RadioButton::radioColor() const {
    if (m_hasError) {
        return errorColor();
    }

    if (isChecked()) {
        auto* app = Application::instance();
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
    } else {
        auto* app = Application::instance();
        if (!app) {
            return fallbackOutline();
        }

        try {
            const auto& theme = app->currentTheme();
            auto& colorScheme = theme.color_scheme();
            return CFColor(colorScheme.queryColor(OUTLINE));
        } catch (...) {
            return fallbackOutline();
        }
    }
}

CFColor RadioButton::onRadioColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnPrimary();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_PRIMARY));
    } catch (...) {
        return fallbackOnPrimary();
    }
}

CFColor RadioButton::stateLayerColor() const {
    if (isChecked()) {
        return radioColor();
    } else {
        auto* app = Application::instance();
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
}

CFColor RadioButton::errorColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackError();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ERROR));
    } catch (...) {
        return fallbackError();
    }
}

QFont RadioButton::labelFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QRadioButton::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Normal);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QRadioButton::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Normal);
        return font;
    }
}

// ============================================================================
// Layout Calculations
// ============================================================================

QRectF RadioButton::calculateRadioRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float radioSize = helper.dpToPx(RADIO_SIZE_DP);
    float y = (height() - radioSize) / 2.0f;
    float x = helper.dpToPx(12.0f);

    return QRectF(x, y, radioSize, radioSize);
}

QRectF RadioButton::calculateTextRect(const QRectF& radioRect) const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float textSpacing = helper.dpToPx(TEXT_SPACING_DP);

    float x = radioRect.right() + textSpacing;
    return QRectF(x, 0.0f, width() - x, height());
}

// ============================================================================
// Animation Helpers
// ============================================================================

void RadioButton::startInnerCircleAnimation(bool checked) {
    float targetScale = checked ? INNER_CIRCLE_SCALE_CHECKED : INNER_CIRCLE_SCALE_UNCHECKED;
    float fromScale = m_innerCircleScale;

    auto factory = aex::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
        Application::animationFactory());

    if (!factory) {
        m_innerCircleScale = targetScale;
        update();
        return;
    }

    auto anim =
        factory->createPropertyAnimation(&m_innerCircleScale, fromScale, targetScale, 200,
                                         cf::ui::base::Easing::Type::EmphasizedDecelerate, this);

    if (anim) {
        if (auto* propAnim = dynamic_cast<CFMaterialPropertyAnimation*>(anim.Get())) {
            propAnim->setRange(fromScale, targetScale);
        }
        anim->start();
    } else {
        m_innerCircleScale = targetScale;
        update();
    }
}

// =============================================================================
// Paint Event
// =============================================================================

void RadioButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF radioRect = calculateRadioRect();
    QRectF textRect = calculateTextRect(radioRect);

    // Step 1: Draw state layer (behind everything)
    drawStateLayer(p, radioRect);

    // Step 2: Draw ripple
    drawRipple(p, radioRect);

    // Step 3: Draw outer ring
    drawOuterRing(p, radioRect);

    // Step 4: Draw inner circle (when checked)
    drawInnerCircle(p, radioRect);

    // Step 5: Draw text label
    drawText(p, textRect);

    // Step 6: Draw focus indicator
    drawFocusIndicator(p, radioRect);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void RadioButton::drawStateLayer(QPainter& p, const QRectF& radioRect) {
    if (!isEnabled() || !m_material.stateMachine()) {
        return;
    }

    float opacity = m_material.stateMachine()->stateLayerOpacity();
    if (opacity <= 0.0f) {
        return;
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float stateLayerSize = helper.dpToPx(RADIO_SIZE_DP * 2.0f); // 40dp
    QPointF center = radioRect.center();
    QRectF stateLayerRect(center.x() - stateLayerSize / 2.0f, center.y() - stateLayerSize / 2.0f,
                          stateLayerSize, stateLayerSize);

    QPainterPath circlePath;
    circlePath.addEllipse(stateLayerRect);

    CFColor stateColor = stateLayerColor();
    QColor color = stateColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);

    p.fillPath(circlePath, color);
}

void RadioButton::drawRipple(QPainter& p, const QRectF& radioRect) {
    if (!m_material.ripple() || !m_pressEffectEnabled) {
        return;
    }

    m_material.ripple()->setColor(stateLayerColor());

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float stateLayerSize = helper.dpToPx(RADIO_SIZE_DP * 2.0f); // 40dp
    QPointF center = radioRect.center();
    QRectF stateLayerRect(center.x() - stateLayerSize / 2.0f, center.y() - stateLayerSize / 2.0f,
                          stateLayerSize, stateLayerSize);

    QPainterPath clipPath;
    clipPath.addEllipse(stateLayerRect);

    m_material.ripple()->paint(&p, clipPath);
}

void RadioButton::drawOuterRing(QPainter& p, const QRectF& radioRect) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float strokeWidth = helper.dpToPx(STROKE_WIDTH_DP);

    CFColor ringColor = radioColor();
    QColor color = ringColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.save();

    float inset = strokeWidth / 2.0f;
    QRectF insetRect = radioRect.adjusted(inset, inset, -inset, -inset);

    QPainterPath ringPath;
    ringPath.addEllipse(insetRect);

    QPen pen(color, strokeWidth);
    pen.setCosmetic(false);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    p.drawPath(ringPath);

    p.restore();
}

void RadioButton::drawInnerCircle(QPainter& p, const QRectF& radioRect) {
    if (!isChecked() && m_innerCircleScale <= 0.01f) {
        return;
    }

    float outerRadius = radioRect.width() / 2.0f;
    float innerRadius = outerRadius * 0.5f;
    float scaledRadius = innerRadius * m_innerCircleScale;

    QPointF center = radioRect.center();

    p.save();

    QPainterPath innerCirclePath;
    innerCirclePath.addEllipse(center, scaledRadius, scaledRadius);

    CFColor fillColor = radioColor();
    QColor color = fillColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.fillPath(innerCirclePath, color);

    p.restore();
}

void RadioButton::drawText(QPainter& p, const QRectF& textRect) {
    if (text().isEmpty()) {
        return;
    }

    CFColor textColor;
    if (m_hasError) {
        textColor = errorColor();
    } else {
        auto* app = Application::instance();
        if (!app) {
            textColor = fallbackOnSurface();
        } else {
            try {
                const auto& theme = app->currentTheme();
                auto& colorScheme = theme.color_scheme();
                textColor = CFColor(colorScheme.queryColor(ON_SURFACE));
            } catch (...) {
                textColor = fallbackOnSurface();
            }
        }
    }

    if (!isEnabled()) {
        QColor color = textColor.native_color();
        color.setAlphaF(0.38f);
        p.setPen(color);
    } else {
        p.setPen(textColor.native_color());
    }

    QFont font = labelFont();
    p.setFont(font);

    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
}

void RadioButton::drawFocusIndicator(QPainter& p, const QRectF& radioRect) {
    if (!m_material.focusIndicator()) {
        return;
    }

    QPainterPath focusPath;
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float focusPadding = helper.dpToPx(4.0f);
    focusPath.addEllipse(
        radioRect.adjusted(-focusPadding, -focusPadding, focusPadding, focusPadding));

    CFColor indicatorColor = radioColor();
    m_material.focusIndicator()->paint(&p, focusPath, indicatorColor);
}

} // namespace cf::ui::widget::material
