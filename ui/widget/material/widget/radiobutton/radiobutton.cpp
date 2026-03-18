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
#include "cfmaterial_animation_factory.h"
#include "components/material/cfmaterial_property_animation.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
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
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// Material Design 3 specifications for RadioButton
namespace {
// Radio button size specifications (in dp)
constexpr float RADIO_SIZE_DP = 20.0f;        // Outer ring diameter
constexpr float INNER_CIRCLE_SIZE_DP = 10.0f; // Inner circle diameter (50% of outer)
constexpr float TOUCH_TARGET_DP = 48.0f;      // Minimum touch target size
constexpr float TEXT_SPACING_DP = 8.0f;       // Spacing between radio and text
constexpr float STROKE_WIDTH_DP = 2.0f;       // Outer ring stroke width

// Inner circle animation scale values
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

RadioButton::RadioButton(QWidget* parent) : QRadioButton(parent) {
    // Get animation factory from Application
    m_animationFactory =
        cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_ripple = new RippleHelper(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Set ripple mode to bounded (clipped by widget bounds)
    m_ripple->setMode(RippleHelper::Mode::Bounded);

    // Connect repaint signals
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&RadioButton::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&RadioButton::update));

    // Set default font
    setFont(labelFont());

    // Initialize inner circle scale based on checked state
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
    if (m_stateMachine) {
        m_stateMachine->onHoverEnter();
    }
    update();
}

void RadioButton::leaveEvent(QEvent* event) {
    QRadioButton::leaveEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onHoverLeave();
    }
    if (m_ripple) {
        m_ripple->onCancel();
    }
    update();
}

void RadioButton::mousePressEvent(QMouseEvent* event) {
    QRadioButton::mousePressEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onPress(event->pos());
    }
    if (m_ripple && m_pressEffectEnabled) {
        // Calculate radio rect for ripple clipping
        QRectF radioRect = calculateRadioRect();
        m_ripple->onPress(event->pos(), radioRect.united(calculateTextRect(radioRect)));
    }
    update();
}

void RadioButton::mouseReleaseEvent(QMouseEvent* event) {
    QRadioButton::mouseReleaseEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onRelease();
    }
    if (m_ripple && m_pressEffectEnabled) {
        m_ripple->onRelease();
    }
    update();
}

void RadioButton::focusInEvent(QFocusEvent* event) {
    QRadioButton::focusInEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onFocusIn();
    }
    if (m_focusIndicator) {
        m_focusIndicator->onFocusIn();
    }
    update();
}

void RadioButton::focusOutEvent(QFocusEvent* event) {
    QRadioButton::focusOutEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onFocusOut();
    }
    if (m_focusIndicator) {
        m_focusIndicator->onFocusOut();
    }
    update();
}

void RadioButton::changeEvent(QEvent* event) {
    QRadioButton::changeEvent(event);
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

void RadioButton::nextCheckState() {
    bool wasChecked = isChecked();
    QRadioButton::nextCheckState();
    bool isCheckedNow = isChecked();

    // Update inner circle animation when checked state changes
    if (wasChecked != isCheckedNow) {
        if (m_stateMachine) {
            m_stateMachine->onCheckedChanged(isCheckedNow);
        }
        startInnerCircleAnimation(isCheckedNow);
    }
}

void RadioButton::setChecked(bool checked) {
    bool wasChecked = isChecked();
    if (wasChecked == checked) {
        return; // No change
    }

    QRadioButton::setChecked(checked);
    bool isCheckedNow = isChecked();

    // Sync inner circle scale with checked state
    // This handles programmatic setChecked() calls which don't trigger nextCheckState
    if (wasChecked != isCheckedNow) {
        if (m_stateMachine) {
            m_stateMachine->onCheckedChanged(isCheckedNow);
        }
        // For programmatic checked change, set scale immediately without animation
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
    // For custom-drawn radio button, entire widget area is clickable
    // This ensures proper click handling even when text is empty
    return rect().contains(pos);
}

// ============================================================================
// Size Hints
// ============================================================================

QSize RadioButton::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design 3 specifications:
    // - Radio size: 20dp
    // - Spacing between radio and text: 8dp
    // - Left padding (for stroke clipping): 12dp
    // - Touch target: 48dp minimum

    float leftPadding = helper.dpToPx(12.0f);
    float radioSize = helper.dpToPx(RADIO_SIZE_DP);
    float textSpacing = helper.dpToPx(TEXT_SPACING_DP);

    // Text width
    float textWidth = text().isEmpty() ? 0.0f : fontMetrics().horizontalAdvance(text());

    // Total width
    float totalWidth = leftPadding + radioSize + textSpacing + textWidth;

    // Height is at least the touch target size
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
    // Error state takes priority
    if (m_hasError) {
        return errorColor();
    }

    // Checked state uses primary, unchecked uses outline
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
    // State layer uses the same color as the radio (primary when checked, onSurface when unchecked)
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
        // Fallback to system font with reasonable size
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

    // Add left padding (for focus indicator and to prevent stroke clipping)
    // This ensures the outer ring stroke is not clipped at the left edge
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

    if (!m_animationFactory) {
        // No factory, set directly
        m_innerCircleScale = targetScale;
        update();
        return;
    }

    // Create property animation
    auto anim = m_animationFactory->createPropertyAnimation(
        &m_innerCircleScale, fromScale, targetScale, 200,
        cf::ui::base::Easing::Type::EmphasizedDecelerate, this);

    if (anim) {
        // IMPORTANT: Update range to fix cached animation's stale from/to values
        if (auto* propAnim =
                dynamic_cast<cf::ui::components::material::CFMaterialPropertyAnimation*>(
                    anim.Get())) {
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

    // Calculate layout
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
    if (!isEnabled() || !m_stateMachine) {
        return;
    }

    float opacity = m_stateMachine->stateLayerOpacity();
    if (opacity <= 0.0f) {
        return;
    }

    // MD3 spec: state layer is 40dp circle (2x radio size), centered on radio
    // This corresponds to the 48dp touch target
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float stateLayerSize = helper.dpToPx(RADIO_SIZE_DP * 2.0f); // 40dp
    QPointF center = radioRect.center();
    QRectF stateLayerRect(center.x() - stateLayerSize / 2.0f, center.y() - stateLayerSize / 2.0f,
                          stateLayerSize, stateLayerSize);

    // Create circular state layer path
    QPainterPath circlePath;
    circlePath.addEllipse(stateLayerRect);

    CFColor stateColor = stateLayerColor();
    QColor color = stateColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);

    p.fillPath(circlePath, color);
}

void RadioButton::drawRipple(QPainter& p, const QRectF& radioRect) {
    if (!m_ripple || !m_pressEffectEnabled) {
        return;
    }

    // Set ripple color based on state
    m_ripple->setColor(stateLayerColor());

    // MD3 spec: ripple is clipped to 40dp circle (2x radio size), centered on radio
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float stateLayerSize = helper.dpToPx(RADIO_SIZE_DP * 2.0f); // 40dp
    QPointF center = radioRect.center();
    QRectF stateLayerRect(center.x() - stateLayerSize / 2.0f, center.y() - stateLayerSize / 2.0f,
                          stateLayerSize, stateLayerSize);

    // Create clipping path for the state layer circle
    QPainterPath clipPath;
    clipPath.addEllipse(stateLayerRect);

    m_ripple->paint(&p, clipPath);
}

void RadioButton::drawOuterRing(QPainter& p, const QRectF& radioRect) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float strokeWidth = helper.dpToPx(STROKE_WIDTH_DP);

    CFColor ringColor = radioColor();
    QColor color = ringColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f); // 38% opacity for disabled
    }

    p.save();

    // Inset the rect by half the stroke width to ensure the stroke stays within bounds
    // This prevents the left edge from being clipped when x=0
    float inset = strokeWidth / 2.0f;
    QRectF insetRect = radioRect.adjusted(inset, inset, -inset, -inset);

    // Create the outer ring path from the inset rect
    QPainterPath ringPath;
    ringPath.addEllipse(insetRect);

    // Set pen for stroking the ring
    QPen pen(color, strokeWidth);
    pen.setCosmetic(false); // Use device pixels for consistent stroke width
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    p.drawPath(ringPath);

    p.restore();
}

void RadioButton::drawInnerCircle(QPainter& p, const QRectF& radioRect) {
    if (!isChecked() && m_innerCircleScale <= 0.01f) {
        return;
    }

    // Calculate inner circle dimensions based on scale
    float outerRadius = radioRect.width() / 2.0f;
    float innerRadius = outerRadius * 0.5f; // Inner circle is 50% of outer
    float scaledRadius = innerRadius * m_innerCircleScale;

    QPointF center = radioRect.center();

    p.save();

    // Create inner circle path
    QPainterPath innerCirclePath;
    innerCirclePath.addEllipse(center, scaledRadius, scaledRadius);

    CFColor fillColor = radioColor();
    QColor color = fillColor.native_color();

    // Handle disabled state
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

    // Use label font
    QFont font = labelFont();
    p.setFont(font);

    // Draw text with vertical center alignment and left alignment
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
}

void RadioButton::drawFocusIndicator(QPainter& p, const QRectF& radioRect) {
    if (!m_focusIndicator) {
        return;
    }

    // Create focus indicator path (slightly larger than the radio button)
    QPainterPath focusPath;
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float focusPadding = helper.dpToPx(4.0f); // 4dp padding for focus ring
    focusPath.addEllipse(
        radioRect.adjusted(-focusPadding, -focusPadding, focusPadding, focusPadding));

    // Use the radio color for the focus indicator
    CFColor indicatorColor = radioColor();
    m_focusIndicator->paint(&p, focusPath, indicatorColor);
}

} // namespace cf::ui::widget::material
