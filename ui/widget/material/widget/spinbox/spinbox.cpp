/**
 * @file spinbox.cpp
 * @brief Material Design 3 SpinBox Implementation
 *
 * Implements a Material Design 3 spin box with outline style, increment/decrement
 * buttons, focus indicator, and state layer effects.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "spinbox.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "cfmaterial_animation_factory.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/ripple_helper.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QFontMetrics>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Material Design 3 SpinBox Specifications
// ============================================================================
namespace {
// Size specifications in dp
constexpr float kHeightDp = 56.0f;            // Total height
constexpr float kHorizontalPaddingDp = 16.0f; // Left/right padding
constexpr float kButtonWidthDp = 40.0f;       // Increment/decrement button width
constexpr float kButtonSpacingDp = 0.0f;      // Spacing between stacked buttons
constexpr float kCornerRadiusDp = 4.0f;       // ShapeSmall corner radius
constexpr float kOutlineWidthDp = 1.0f;       // Outline border width

// Icon specifications
constexpr float kIconSizeDp = 24.0f;     // Button icon size
constexpr float kIconStrokeWidth = 2.5f; // Icon stroke width
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

SpinBox::SpinBox(QWidget* parent)
    : QSpinBox(parent), m_hoveringIncrementButton(false), m_hoveringDecrementButton(false),
      m_pressingIncrementButton(false), m_pressingDecrementButton(false) {

    // Disable native frame and background
    setFrame(false);
    setAttribute(Qt::WA_TranslucentBackground);

    // Hide the native up/down buttons - we'll draw our own Material Design buttons
    // Keyboard up/down arrow keys will still work
    setButtonSymbols(QSpinBox::NoButtons);

    // Make internal lineEdit transparent and borderless so it doesn't cover our custom drawing
    if (lineEdit()) {
        lineEdit()->setFrame(false);
        lineEdit()->setAttribute(Qt::WA_TranslucentBackground);
        lineEdit()->setStyleSheet("QLineEdit { background: transparent; border: none; }");
        // Set alignment to right-align numbers (standard for numeric input)
        lineEdit()->setAlignment(Qt::AlignRight);
    }

    // Get animation factory from Application
    m_animationFactory =
        cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_ripple = new RippleHelper(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Set ripple mode to bounded for button areas
    m_ripple->setMode(RippleHelper::Mode::Bounded);

    // Connect repaint signals
    connect(m_ripple, &RippleHelper::repaintNeeded, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));

    // Set default font
    setFont(textFont());

    // Set cursor
    setCursor(Qt::IBeamCursor);

    // Set initial text color for lineEdit
    updateTextColor();
}

SpinBox::~SpinBox() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void SpinBox::enterEvent(QEnterEvent* event) {
    QSpinBox::enterEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverEnter();
    update();
}

void SpinBox::leaveEvent(QEvent* event) {
    QSpinBox::leaveEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverLeave();
    if (m_ripple)
        m_ripple->onCancel();

    // Reset button hover state
    m_hoveringIncrementButton = false;
    m_hoveringDecrementButton = false;

    update();
}

void SpinBox::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();

    // Check if button area was clicked first
    if (isOverButtons(pos)) {
        if (isOverIncrementButton(pos)) {
            m_pressingIncrementButton = true;
            // Trigger increment
            stepUp();
        } else if (isOverDecrementButton(pos)) {
            m_pressingDecrementButton = true;
            // Trigger decrement
            stepDown();
        }
        update();
        return;
    }

    // Handle text area click
    QSpinBox::mousePressEvent(event);
    if (m_stateMachine)
        m_stateMachine->onPress(pos);
    if (m_ripple)
        m_ripple->onPress(pos, rect());
    update();
}

void SpinBox::mouseReleaseEvent(QMouseEvent* event) {
    // Reset button press states
    if (m_pressingIncrementButton || m_pressingDecrementButton) {
        m_pressingIncrementButton = false;
        m_pressingDecrementButton = false;
        update();
        return;
    }

    QSpinBox::mouseReleaseEvent(event);
    if (m_stateMachine)
        m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    update();
}

void SpinBox::mouseMoveEvent(QMouseEvent* event) {
    QSpinBox::mouseMoveEvent(event);
    updateButtonHoverState(event->pos());
}

void SpinBox::focusInEvent(QFocusEvent* event) {
    QSpinBox::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    update();
}

void SpinBox::focusOutEvent(QFocusEvent* event) {
    QSpinBox::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    update();
}

void SpinBox::changeEvent(QEvent* event) {
    QSpinBox::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        if (m_stateMachine) {
            if (isEnabled()) {
                m_stateMachine->onEnable();
            } else {
                m_stateMachine->onDisable();
            }
        }
        updateTextColor();
        update();
    }
}

void SpinBox::resizeEvent(QResizeEvent* event) {
    QSpinBox::resizeEvent(event);
    // Constrain internal lineEdit to text area only, so it doesn't cover button area
    if (lineEdit()) {
        lineEdit()->setGeometry(textRect().toRect());
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize SpinBox::sizeHint() const {
    // Material Design spin box specifications:
    // - Height: 56dp
    // - Horizontal padding: 16dp
    // - Button width: 40dp (total 40dp for vertical stack)
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float height = helper.dpToPx(kHeightDp);
    float hPadding = helper.dpToPx(kHorizontalPaddingDp);
    float buttonWidth = helper.dpToPx(kButtonWidthDp);

    // Calculate text width based on maximum possible value
    QString maxText = textFromValue(maximum());
    float textWidth = fontMetrics().horizontalAdvance(maxText);

    // Total width = padding + text + padding + button area + padding
    float totalWidth = hPadding + textWidth + hPadding + buttonWidth + hPadding;

    return QSize(int(std::ceil(totalWidth)), int(std::ceil(height)));
}

QSize SpinBox::minimumSizeHint() const {
    // Minimum width based on a reasonable range of values
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float height = helper.dpToPx(kHeightDp);
    float hPadding = helper.dpToPx(kHorizontalPaddingDp);
    float buttonWidth = helper.dpToPx(kButtonWidthDp);

    // Minimum text width (approximately 3-4 digits)
    float minTextWidth = fontMetrics().horizontalAdvance("1000");
    float totalWidth = hPadding + minTextWidth + hPadding + buttonWidth + hPadding;

    return QSize(int(std::ceil(totalWidth)), int(std::ceil(height)));
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF SpinBox::contentRect() const {
    // Return the full widget rect as content rect
    return QRectF(rect());
}

QRectF SpinBox::textRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float hPadding = helper.dpToPx(kHorizontalPaddingDp);
    float buttonWidth = helper.dpToPx(kButtonWidthDp);

    QRectF content = contentRect();
    float left = content.left() + hPadding;
    float width = content.width() - hPadding * 2 - buttonWidth;

    return QRectF(left, content.top(), width, content.height());
}

QRectF SpinBox::incrementButtonRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float buttonWidth = helper.dpToPx(kButtonWidthDp);
    float halfButtonHeight = contentRect().height() / 2.0f;

    QRectF content = contentRect();
    float left = content.right() - buttonWidth;

    // Increment button is the top half
    return QRectF(left, content.top(), buttonWidth, halfButtonHeight);
}

QRectF SpinBox::decrementButtonRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float buttonWidth = helper.dpToPx(kButtonWidthDp);
    float halfButtonHeight = contentRect().height() / 2.0f;

    QRectF content = contentRect();
    float left = content.right() - buttonWidth;

    // Decrement button is the bottom half
    return QRectF(left, content.top() + halfButtonHeight, buttonWidth, halfButtonHeight);
}

QPainterPath SpinBox::shapePath() const {
    QRectF content = contentRect();
    float radius = cornerRadius();
    return roundedRect(content, radius);
}

// ============================================================================
// Button Helpers
// ============================================================================

bool SpinBox::isOverIncrementButton(const QPoint& pos) const {
    return incrementButtonRect().contains(pos);
}

bool SpinBox::isOverDecrementButton(const QPoint& pos) const {
    return decrementButtonRect().contains(pos);
}

bool SpinBox::isOverButtons(const QPoint& pos) const {
    return isOverIncrementButton(pos) || isOverDecrementButton(pos);
}

void SpinBox::updateButtonHoverState(const QPoint& pos) {
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
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
} // Surface
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
} // Outline
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(73, 69, 79);
} // On Surface Variant
} // namespace

CFColor SpinBox::containerColor() const {
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

CFColor SpinBox::textColor() const {
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

CFColor SpinBox::stateLayerColor() const {
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

CFColor SpinBox::outlineColor() const {
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

CFColor SpinBox::focusOutlineColor() const {
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

CFColor SpinBox::buttonIconColor() const {
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

float SpinBox::cornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(kCornerRadiusDp);
}

QFont SpinBox::textFont() const {
    auto* app = Application::instance();
    if (!app) {
        // Fallback to system font with reasonable size
        QFont font = QSpinBox::font();
        font.setPixelSize(16);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QSpinBox::font();
        font.setPixelSize(16);
        return font;
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void SpinBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF content = contentRect();
    QPainterPath shape = shapePath();

    // Step 1: Draw background
    drawBackground(p, shape);

    // Step 2: Draw state layer (hover/pressed overlay)
    drawStateLayer(p, shape);

    // Step 3: Draw ripple
    drawRipple(p, shape);

    // Step 4: Draw outline
    drawOutline(p, shape);

    // Note: Text is drawn by the internal lineEdit widget, not here

    // Step 5: Draw increment/decrement buttons
    drawButtons(p, QRectF(incrementButtonRect().left(), incrementButtonRect().top(),
                          decrementButtonRect().width(),
                          incrementButtonRect().height() + decrementButtonRect().height()));

    // Step 6: Draw focus indicator
    drawFocusIndicator(p, shape);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void SpinBox::drawBackground(QPainter& p, const QPainterPath& shape) {
    CFColor bg = containerColor();

    // Handle disabled state
    if (!isEnabled()) {
        QColor color = bg.native_color();
        p.fillPath(shape, color);
        return;
    }

    p.fillPath(shape, bg.native_color());
}

void SpinBox::drawStateLayer(QPainter& p, const QPainterPath& shape) {
    if (!isEnabled() || !m_stateMachine) {
        return;
    }

    float opacity = m_stateMachine->stateLayerOpacity();
    if (opacity <= 0.0f) {
        return;
    }

    CFColor stateColor = stateLayerColor();
    QColor color = stateColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);

    p.fillPath(shape, color);
}

void SpinBox::drawRipple(QPainter& p, const QPainterPath& shape) {
    if (m_ripple) {
        // Set ripple color based on text color
        m_ripple->setColor(textColor());
        m_ripple->paint(&p, shape);
    }
}

void SpinBox::drawOutline(QPainter& p, const QPainterPath& shape) {
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

void SpinBox::drawText(QPainter& p, const QRectF& textRect) {
    // The internal lineEdit handles text display and cursor rendering.
    // This function is now a no-op - color updates are handled by updateTextColor().
    Q_UNUSED(p)
    Q_UNUSED(textRect)
}

void SpinBox::updateTextColor() {
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

void SpinBox::drawButtons(QPainter& p, const QRectF& buttonRect) {
    Q_UNUSED(buttonRect)

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Draw increment button (up arrow)
    QRectF incRect = incrementButtonRect();
    if (!incRect.isEmpty()) {
        p.save();

        // Hover overlay for increment button
        if (m_hoveringIncrementButton || m_pressingIncrementButton) {
            QColor overlay = buttonIconColor().native_color();
            overlay.setAlphaF(m_pressingIncrementButton ? 0.12f : 0.08f);
            p.fillRect(incRect, overlay);
        }

        // Draw up arrow icon
        QColor iconColor = buttonIconColor().native_color();
        if (!isEnabled()) {
            iconColor.setAlphaF(0.38f);
        }

        p.setPen(QPen(iconColor, helper.dpToPx(2.0f)));
        p.setRenderHint(QPainter::Antialiasing);

        float cx = incRect.center().x();
        float cy = incRect.center().y();
        float size = helper.dpToPx(12.0f); // Arrow size
        float half = size / 2.0f;

        // Draw up arrow (triangle)
        QPainterPath upArrow;
        upArrow.moveTo(cx, cy - half);               // Top point
        upArrow.lineTo(cx - half, cy + half * 0.5f); // Bottom left
        upArrow.lineTo(cx + half, cy + half * 0.5f); // Bottom right
        upArrow.closeSubpath();

        p.fillPath(upArrow, iconColor);

        p.restore();
    }

    // Draw decrement button (down arrow)
    QRectF decRect = decrementButtonRect();
    if (!decRect.isEmpty()) {
        p.save();

        // Hover overlay for decrement button
        if (m_hoveringDecrementButton || m_pressingDecrementButton) {
            QColor overlay = buttonIconColor().native_color();
            overlay.setAlphaF(m_pressingDecrementButton ? 0.12f : 0.08f);
            p.fillRect(decRect, overlay);
        }

        // Draw down arrow icon
        QColor iconColor = buttonIconColor().native_color();
        if (!isEnabled()) {
            iconColor.setAlphaF(0.38f);
        }

        p.setPen(QPen(iconColor, helper.dpToPx(2.0f)));
        p.setRenderHint(QPainter::Antialiasing);

        float cx = decRect.center().x();
        float cy = decRect.center().y();
        float size = helper.dpToPx(12.0f); // Arrow size
        float half = size / 2.0f;

        // Draw down arrow (triangle)
        QPainterPath downArrow;
        downArrow.moveTo(cx, cy + half);               // Bottom point
        downArrow.lineTo(cx - half, cy - half * 0.5f); // Top left
        downArrow.lineTo(cx + half, cy - half * 0.5f); // Top right
        downArrow.closeSubpath();

        p.fillPath(downArrow, iconColor);

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

void SpinBox::drawFocusIndicator(QPainter& p, const QPainterPath& shape) {
    if (m_focusIndicator && hasFocus()) {
        m_focusIndicator->paint(&p, shape, focusOutlineColor());
    }
}

} // namespace cf::ui::widget::material
