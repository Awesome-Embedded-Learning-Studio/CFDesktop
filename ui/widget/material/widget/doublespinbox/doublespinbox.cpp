/**
 * @file doublespinbox.cpp
 * @brief Material Design 3 DoubleSpinBox Implementation
 *
 * Implements a Material Design 3 double spin box with floating-point input
 * support. Features increment/decrement buttons, outline style, focus
 * indicator, and state layer effects.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "doublespinbox.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QApplication>
#include <QFontMetrics>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Constants (Material Design 3 specifications)
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
} // Surface
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(75, 75, 80);
} // On Surface Variant
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
} // Outline
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

DoubleSpinBox::DoubleSpinBox(QWidget* parent)
    : QDoubleSpinBox(parent), m_material(this,
                                         MaterialWidgetBase::Config{
                                             .useRipple = true,
                                             .useElevation = true,
                                             .useFocusIndicator = true,
                                             .initialElevation = 0,
                                         }),
      m_hoveringIncrementButton(false), m_hoveringDecrementButton(false),
      m_pressingIncrementButton(false), m_pressingDecrementButton(false) {

    // Disable native frame and background
    setFrame(false);
    setAttribute(Qt::WA_TranslucentBackground);
    // Hide the native up/down buttons - we'll draw our own Material Design buttons
    // Keyboard up/down arrow keys will still work
    setButtonSymbols(QDoubleSpinBox::NoButtons);

    // Make internal lineEdit transparent and borderless so it doesn't cover our custom drawing
    if (lineEdit()) {
        lineEdit()->setFrame(false);
        lineEdit()->setAttribute(Qt::WA_TranslucentBackground);
        lineEdit()->setStyleSheet("QLineEdit { background: transparent; border: none; }");
        // Set alignment to right-align numbers (standard for numeric input)
        lineEdit()->setAlignment(Qt::AlignRight);
    }

    // Set default font
    setFont(textFont());

    // Set cursor
    setCursor(Qt::IBeamCursor);

    // Set initial text color for lineEdit
    updateTextColor();
}

DoubleSpinBox::~DoubleSpinBox() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void DoubleSpinBox::enterEvent(QEnterEvent* event) {
    QDoubleSpinBox::enterEvent(event);
    m_material.onEnterEvent();
}

void DoubleSpinBox::leaveEvent(QEvent* event) {
    QDoubleSpinBox::leaveEvent(event);
    m_material.onLeaveEvent();

    // Reset button hover states
    m_hoveringIncrementButton = false;
    m_hoveringDecrementButton = false;
}

void DoubleSpinBox::mousePressEvent(QMouseEvent* event) {
    // Check if button area was clicked
    if (isOverButtons(event->pos())) {
        if (isOverIncrementButton(event->pos())) {
            m_pressingIncrementButton = true;
            // Trigger increment
            stepUp();
        } else if (isOverDecrementButton(event->pos())) {
            m_pressingDecrementButton = true;
            // Trigger decrement
            stepDown();
        }
        update();
        return;
    }

    QDoubleSpinBox::mousePressEvent(event);
    m_material.onMousePress(event->pos(), rect());
}

void DoubleSpinBox::mouseReleaseEvent(QMouseEvent* event) {
    // Reset button press states
    if (m_pressingIncrementButton || m_pressingDecrementButton) {
        m_pressingIncrementButton = false;
        m_pressingDecrementButton = false;
        update();
        return;
    }

    QDoubleSpinBox::mouseReleaseEvent(event);
    m_material.onMouseRelease();
}

void DoubleSpinBox::mouseMoveEvent(QMouseEvent* event) {
    QDoubleSpinBox::mouseMoveEvent(event);
    updateButtonHoverState(event->pos());
}

void DoubleSpinBox::focusInEvent(QFocusEvent* event) {
    QDoubleSpinBox::focusInEvent(event);
    m_material.onFocusIn();
}

void DoubleSpinBox::focusOutEvent(QFocusEvent* event) {
    QDoubleSpinBox::focusOutEvent(event);
    m_material.onFocusOut();
}

void DoubleSpinBox::changeEvent(QEvent* event) {
    QDoubleSpinBox::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
        updateTextColor();
        update();
    }
}

void DoubleSpinBox::resizeEvent(QResizeEvent* event) {
    QDoubleSpinBox::resizeEvent(event);
    // Constrain internal lineEdit to text area only, so it doesn't cover button area
    if (lineEdit()) {
        lineEdit()->setGeometry(textRect().toRect());
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void DoubleSpinBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF content = contentRect();
    QPainterPath shape = shapePath();

    // Step 1: Draw background
    drawBackground(p, shape);

    // Step 2: Draw state layer
    drawStateLayer(p, shape);

    // Step 3: Draw ripple
    drawRipple(p, shape);

    // Step 4: Draw outline
    drawOutline(p, shape);

    // Note: Text is drawn by the internal lineEdit widget, not here

    // Step 5: Draw buttons
    drawButtons(p, incrementButtonRect().united(decrementButtonRect()));

    // Step 6: Draw focus indicator
    drawFocusIndicator(p, shape);
}

// ============================================================================
// Size Hints
// ============================================================================

QSize DoubleSpinBox::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design spin box specifications:
    // - Height: 56dp
    // - Horizontal padding: 16dp
    // - Button width: 40dp
    float height = helper.dpToPx(56.0f);
    float hPadding = helper.dpToPx(16.0f);
    float buttonWidth = helper.dpToPx(40.0f);

    // Estimate text width based on decimals and range
    QFontMetricsF fm(font());
    QString sampleText = QString("-%1.%2")
                             .arg(qAbs(maximum()), 0, 'f', decimals())
                             .arg(QString('0').repeated(decimals()));
    float textWidth = fm.horizontalAdvance(sampleText);

    float contentWidth = hPadding * 2 + textWidth + buttonWidth;

    return QSize(int(std::ceil(contentWidth)), int(std::ceil(height)));
}

QSize DoubleSpinBox::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float height = helper.dpToPx(56.0f);
    float minWidth = helper.dpToPx(120.0f); // Minimum usable width

    return QSize(int(std::ceil(minWidth)), int(std::ceil(height)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor DoubleSpinBox::containerColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(SURFACE));
    } catch (...) {
        return fallbackSurface();
    }
}

CFColor DoubleSpinBox::textColor() const {
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

CFColor DoubleSpinBox::stateLayerColor() const {
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

CFColor DoubleSpinBox::outlineColor() const {
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

CFColor DoubleSpinBox::focusOutlineColor() const {
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
}

CFColor DoubleSpinBox::buttonIconColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurfaceVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_SURFACE_VARIANT));
    } catch (...) {
        return fallbackOnSurfaceVariant();
    }
}

float DoubleSpinBox::cornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    // Small corner radius (4dp) as per Material Design 3
    return helper.dpToPx(4.0f);
}

QFont DoubleSpinBox::textFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QDoubleSpinBox::font();
        font.setPixelSize(16);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QDoubleSpinBox::font();
        font.setPixelSize(16);
        return font;
    }
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF DoubleSpinBox::contentRect() const {
    return QRectF(rect());
}

QRectF DoubleSpinBox::textRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF content = contentRect();
    float hPadding = helper.dpToPx(16.0f);
    float buttonWidth = helper.dpToPx(40.0f);

    float left = hPadding;
    float top = 0;
    float availableWidth = content.width() - hPadding * 2 - buttonWidth;
    float height = content.height();

    return QRectF(left, top, availableWidth, height);
}

QRectF DoubleSpinBox::incrementButtonRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF content = contentRect();
    float buttonWidth = helper.dpToPx(40.0f);

    float right = content.width();
    float buttonHeight = content.height() / 2.0f;

    return QRectF(right - buttonWidth, 0, buttonWidth, buttonHeight);
}

QRectF DoubleSpinBox::decrementButtonRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF content = contentRect();
    float buttonWidth = helper.dpToPx(40.0f);
    float buttonHeight = content.height() / 2.0f;

    float right = content.width();
    float top = buttonHeight;

    return QRectF(right - buttonWidth, top, buttonWidth, buttonHeight);
}

QPainterPath DoubleSpinBox::shapePath() const {
    QRectF content = contentRect();
    return roundedRect(content, cornerRadius());
}

// ============================================================================
// Button Helpers
// ============================================================================

bool DoubleSpinBox::isOverIncrementButton(const QPoint& pos) const {
    return incrementButtonRect().contains(pos);
}

bool DoubleSpinBox::isOverDecrementButton(const QPoint& pos) const {
    return decrementButtonRect().contains(pos);
}

bool DoubleSpinBox::isOverButtons(const QPoint& pos) const {
    return isOverIncrementButton(pos) || isOverDecrementButton(pos);
}

void DoubleSpinBox::updateButtonHoverState(const QPoint& pos) {
    bool wasHoveringIncrement = m_hoveringIncrementButton;
    bool wasHoveringDecrement = m_hoveringDecrementButton;

    m_hoveringIncrementButton = isOverIncrementButton(pos);
    m_hoveringDecrementButton = isOverDecrementButton(pos);

    if (wasHoveringIncrement != m_hoveringIncrementButton ||
        wasHoveringDecrement != m_hoveringDecrementButton) {
        update();
    }
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void DoubleSpinBox::drawBackground(QPainter& p, const QPainterPath& shape) {
    CFColor bg = containerColor();

    // Handle disabled state
    if (!isEnabled()) {
        QColor color = bg.native_color();
        color.setAlphaF(0.38f); // 38% opacity for disabled
        p.fillPath(shape, color);
        return;
    }

    p.fillPath(shape, bg.native_color());
}

void DoubleSpinBox::drawStateLayer(QPainter& p, const QPainterPath& shape) {
    if (!isEnabled() || !m_material.stateMachine()) {
        return;
    }

    float opacity = m_material.stateMachine()->stateLayerOpacity();
    if (opacity <= 0.0f) {
        return;
    }

    CFColor stateColor = stateLayerColor();
    QColor color = stateColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);

    p.fillPath(shape, color);
}

void DoubleSpinBox::drawRipple(QPainter& p, const QPainterPath& shape) {
    if (m_material.ripple()) {
        // Set ripple color based on text color
        m_material.ripple()->setColor(textColor());
        m_material.ripple()->paint(&p, shape);
    }
}

void DoubleSpinBox::drawOutline(QPainter& p, const QPainterPath& shape) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float inset = 0.5f;

    QColor color;
    if (hasFocus()) {
        color = focusOutlineColor().native_color();
    } else {
        color = outlineColor().native_color();
    }

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.save();

    // Create inset path for outline
    QRectF shapeBounds = shape.boundingRect();
    QRectF insetRect = shapeBounds.adjusted(inset, inset, -inset, -inset);
    float adjustedRadius = std::max(0.0f, cornerRadius() - inset);
    QPainterPath insetShape = roundedRect(insetRect, adjustedRadius);

    // Calculate outline width
    float baseWidth = helper.dpToPx(1.0f);
    float activeWidth = helper.dpToPx(2.0f);
    float currentWidth = hasFocus() ? activeWidth : baseWidth;

    QPen pen(color, currentWidth);
    pen.setCosmetic(true); // Keep consistent width across DPI
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(insetShape);

    p.restore();
}

void DoubleSpinBox::drawText(QPainter& p, const QRectF& textRect) {
    // The internal lineEdit handles text display and cursor rendering.
    // This function is now a no-op - color updates are handled by updateTextColor().
    Q_UNUSED(p)
    Q_UNUSED(textRect)
}

void DoubleSpinBox::updateTextColor() {
    // Update the lineEdit text color based on current state
    if (lineEdit()) {
        QPalette pal = lineEdit()->palette();
        QColor textColorVal = textColor().native_color();
        if (!isEnabled()) {
            textColorVal.setAlphaF(0.38f);
        }
        pal.setColor(QPalette::Text, textColorVal);
        pal.setColor(QPalette::WindowText, textColorVal);
        lineEdit()->setPalette(pal);
    }
}

void DoubleSpinBox::drawButtons(QPainter& p, const QRectF& buttonRect) {
    Q_UNUSED(buttonRect)

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Draw increment button
    QRectF incRect = incrementButtonRect();
    if (!incRect.isEmpty()) {
        p.save();

        // Hover overlay for increment button
        if (m_hoveringIncrementButton || m_pressingIncrementButton) {
            QColor overlay = buttonIconColor().native_color();
            overlay.setAlphaF(m_pressingIncrementButton ? 0.12f : 0.08f);
            p.fillRect(incRect, overlay);
        }

        // Draw plus icon
        QColor iconColor = buttonIconColor().native_color();
        if (!isEnabled()) {
            iconColor.setAlphaF(0.38f);
        }

        p.setPen(QPen(iconColor, helper.dpToPx(2.0f)));
        p.setRenderHint(QPainter::Antialiasing);

        float cx = incRect.center().x();
        float cy = incRect.center().y();
        float size = helper.dpToPx(16.0f);
        float half = size / 2.0f;

        // Draw plus sign
        p.drawLine(QPointF(cx - half, cy), QPointF(cx + half, cy)); // Horizontal
        p.drawLine(QPointF(cx, cy - half), QPointF(cx, cy + half)); // Vertical

        p.restore();
    }

    // Draw decrement button
    QRectF decRect = decrementButtonRect();
    if (!decRect.isEmpty()) {
        p.save();

        // Hover overlay for decrement button
        if (m_hoveringDecrementButton || m_pressingDecrementButton) {
            QColor overlay = buttonIconColor().native_color();
            overlay.setAlphaF(m_pressingDecrementButton ? 0.12f : 0.08f);
            p.fillRect(decRect, overlay);
        }

        // Draw minus icon
        QColor iconColor = buttonIconColor().native_color();
        if (!isEnabled()) {
            iconColor.setAlphaF(0.38f);
        }

        p.setPen(QPen(iconColor, helper.dpToPx(2.0f)));
        p.setRenderHint(QPainter::Antialiasing);

        float cx = decRect.center().x();
        float cy = decRect.center().y();
        float size = helper.dpToPx(16.0f);
        float half = size / 2.0f;

        // Draw minus sign (horizontal only)
        p.drawLine(QPointF(cx - half, cy), QPointF(cx + half, cy));

        p.restore();
    }

    // Draw separator line between buttons
    p.save();
    QColor separatorColor = outlineColor().native_color();
    if (!isEnabled()) {
        separatorColor.setAlphaF(0.38f);
    }

    float separatorX = incrementButtonRect().left();
    float separatorY = incrementButtonRect().bottom();
    float separatorWidth = helper.dpToPx(1.0f);
    QRectF separatorRect(separatorX, separatorY - separatorWidth / 2, incrementButtonRect().width(),
                         separatorWidth);

    p.fillRect(separatorRect, separatorColor);
    p.restore();
}

void DoubleSpinBox::drawFocusIndicator(QPainter& p, const QPainterPath& shape) {
    if (m_material.focusIndicator() && hasFocus()) {
        m_material.focusIndicator()->paint(&p, shape, focusOutlineColor());
    }
}

} // namespace cf::ui::widget::material
