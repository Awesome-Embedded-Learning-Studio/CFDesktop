/**
 * @file button.cpp
 * @brief Material Design 3 Button Implementation
 *
 * Implements a Material Design 3 button with 5 variants: Filled, Tonal,
 * Outlined, Text, and Elevated. Supports ripple effects, state layers,
 * elevation shadows, and focus indicators.
 *
 * @author Material Design Framework Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "button.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
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
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Constructor / Destructor
// ============================================================================

Button::Button(ButtonVariant variant, QWidget* parent)
    : QPushButton(parent), m_material(this,
                                      MaterialWidgetBase::Config{
                                          .useElevation = true,
                                          .initialElevation = 2,
                                      }),
      variant_(variant) {
    setFont(labelFont());
}

Button::Button(const QString& text, ButtonVariant variant, QWidget* parent)
    : Button(variant, parent) {
    setText(text);
}

Button::~Button() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers (moved from inline in header)
// ============================================================================

void Button::enterEvent(QEnterEvent* event) {
    QPushButton::enterEvent(event);
    m_material.onEnterEvent();
}

void Button::leaveEvent(QEvent* event) {
    QPushButton::leaveEvent(event);
    m_material.onLeaveEvent();
}

void Button::mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);
    if (m_pressEffectEnabled) {
        m_material.onMousePress(event->pos(), rect());
    } else {
        m_material.stateMachine()->onPress(event->pos());
        if (m_material.ripple())
            m_material.ripple()->onPress(event->pos(), rect());
        update();
    }
}

void Button::mouseReleaseEvent(QMouseEvent* event) {
    QPushButton::mouseReleaseEvent(event);
    if (m_pressEffectEnabled) {
        m_material.onMouseRelease();
    } else {
        m_material.stateMachine()->onRelease();
        if (m_material.ripple())
            m_material.ripple()->onRelease();
        update();
    }
}

void Button::focusInEvent(QFocusEvent* event) {
    QPushButton::focusInEvent(event);
    m_material.onFocusIn();
}

void Button::focusOutEvent(QFocusEvent* event) {
    QPushButton::focusOutEvent(event);
    m_material.onFocusOut();
}

void Button::changeEvent(QEvent* event) {
    QPushButton::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

int Button::elevation() const {
    return m_material.elevation() ? m_material.elevation()->elevation() : 0;
}

void Button::setElevation(int level) {
    if (m_material.elevation()) {
        m_material.elevation()->setElevation(level);
        update();
    }
}

void Button::setLightSourceAngle(float degrees) {
    if (m_material.elevation()) {
        m_material.elevation()->setLightSourceAngle(degrees);
        update();
    }
}

float Button::lightSourceAngle() const {
    return m_material.elevation() ? m_material.elevation()->lightSourceAngle() : 15.0f;
}

void Button::setLeadingIcon(const QIcon& icon) {
    leadingIcon_ = icon;
    updateGeometry();
    update();
}

Button::ButtonVariant Button::variant() const {
    return variant_;
}

void Button::setVariant(ButtonVariant variant) {
    if (variant_ != variant) {
        variant_ = variant;
        // 变体改变时保持 elevation 不变（统一使用 level 2）
        updateGeometry();
        update();
    }
}

bool Button::pressEffectEnabled() const {
    return m_pressEffectEnabled;
}

void Button::setPressEffectEnabled(bool enabled) {
    if (m_pressEffectEnabled != enabled) {
        m_pressEffectEnabled = enabled;
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize Button::sizeHint() const {
    // Material Design button specifications:
    // - Height: 40dp (content area)
    // - Horizontal padding: 24dp
    // - Icon size: 18dp, 8dp gap from text
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // 内容区域高度
    float contentHeight = helper.dpToPx(40.0f);
    float hPadding = helper.dpToPx(24.0f);
    float iconWidth = 0;
    float iconGap = 0;

    if (!leadingIcon_.isNull()) {
        iconWidth = helper.dpToPx(18.0f);
        iconGap = helper.dpToPx(8.0f);
    }

    float textWidth = fontMetrics().horizontalAdvance(text());
    float contentWidth = hPadding * 2 + iconWidth + iconGap + textWidth;

    // 加上阴影边距
    QMarginsF margin = shadowMargin();
    float totalWidth = contentWidth + margin.left() + margin.right();
    float totalHeight = contentHeight + margin.top() + margin.bottom();

    return QSize(int(std::ceil(totalWidth)), int(std::ceil(totalHeight)));
}

QSize Button::minimumSizeHint() const {
    // Minimum width respects padding and some text
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // 内容区域高度
    float contentHeight = helper.dpToPx(40.0f);
    float hPadding = helper.dpToPx(24.0f);
    float iconWidth = leadingIcon_.isNull() ? 0 : helper.dpToPx(18.0f);
    float iconGap = leadingIcon_.isNull() ? 0 : helper.dpToPx(8.0f);

    // Minimum text width (approximately 3 characters)
    float minTextWidth = fontMetrics().horizontalAdvance("MMM");
    float contentWidth = hPadding * 2 + iconWidth + iconGap + minTextWidth;

    // 加上阴影边距
    QMarginsF margin = shadowMargin();
    float totalWidth = contentWidth + margin.left() + margin.right();
    float totalHeight = contentHeight + margin.top() + margin.bottom();

    return QSize(int(std::ceil(totalWidth)), int(std::ceil(totalHeight)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
inline CFColor fallbackOnPrimary() {
    return CFColor(255, 255, 255);
} // White
inline CFColor fallbackPrimaryContainer() {
    return CFColor(234, 221, 255);
}
inline CFColor fallbackOnPrimaryContainer() {
    return CFColor(29, 25, 67);
}
inline CFColor fallbackSecondary() {
    return CFColor(101, 163, 207);
} // Light Blue
inline CFColor fallbackSecondaryContainer() {
    return CFColor(179, 218, 255);
}
inline CFColor fallbackOnSecondaryContainer() {
    return CFColor(0, 46, 73);
}
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
} // Outline
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
} // Surface
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
} // namespace

CFColor Button::containerColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        switch (variant_) {
            case ButtonVariant::Filled:
                return CFColor(colorScheme.queryColor(PRIMARY));
            case ButtonVariant::Tonal:
                return CFColor(colorScheme.queryColor(SECONDARY_CONTAINER));
            case ButtonVariant::Outlined:
            case ButtonVariant::Text:
                return CFColor(colorScheme.queryColor(SURFACE));
            case ButtonVariant::Elevated:
                return CFColor(colorScheme.queryColor(SURFACE));
        }
    } catch (...) {
        // Fallback if theme access fails
    }

    switch (variant_) {
        case ButtonVariant::Filled:
            return fallbackPrimary();
        case ButtonVariant::Tonal:
            return fallbackSecondaryContainer();
        case ButtonVariant::Outlined:
        case ButtonVariant::Text:
        case ButtonVariant::Elevated:
            return fallbackSurface();
    }
    return fallbackSurface();
}

CFColor Button::labelColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        switch (variant_) {
            case ButtonVariant::Filled:
                return CFColor(colorScheme.queryColor(ON_PRIMARY));
            case ButtonVariant::Tonal:
                return CFColor(colorScheme.queryColor(ON_SECONDARY_CONTAINER));
            case ButtonVariant::Outlined:
            case ButtonVariant::Text:
            case ButtonVariant::Elevated:
                return CFColor(colorScheme.queryColor(PRIMARY));
        }
    } catch (...) {
        // Fallback if theme access fails
    }

    switch (variant_) {
        case ButtonVariant::Filled:
            return fallbackOnPrimary();
        case ButtonVariant::Tonal:
            return fallbackOnPrimaryContainer();
        case ButtonVariant::Outlined:
        case ButtonVariant::Text:
        case ButtonVariant::Elevated:
            return fallbackPrimary();
    }
    return fallbackOnSurface();
}

CFColor Button::stateLayerColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        // State layer color is the same as label color for all variants
        return labelColor();
    } catch (...) {
        return labelColor();
    }
}

CFColor Button::outlineColor() const {
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

float Button::cornerRadius() const {
    // Full rounded corners (50% of height)
    return height() / 2.0f;
}

QFont Button::labelFont() const {
    auto* app = Application::instance();
    if (!app) {
        // Fallback to system font with reasonable size
        QFont font = QPushButton::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Medium);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("labelLarge");
    } catch (...) {
        QFont font = QPushButton::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Medium);
        return font;
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void Button::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 计算按压偏移
    float pressOffset = 0.0f;
    if (m_pressEffectEnabled && m_material.elevation()) {
        pressOffset = m_material.elevation()->pressOffset();
    }

    // Calculate content area (inset to make room for shadow)
    QMarginsF margin = shadowMargin();
    QRectF contentRect =
        QRectF(rect()).adjusted(margin.left(), margin.top(), -margin.right(), -margin.bottom());

    // 应用按压偏移 - 整体向下平移，而不是压缩顶部
    contentRect.translate(0, pressOffset);

    // Create shape path (full rounded corners) using content area
    QPainterPath shape = roundedRect(contentRect, cornerRadius());

    // Step 1: Draw shadow (Elevated variant only) - uses contentRect
    drawShadow(p, contentRect, shape);

    // Step 2: Draw background
    drawBackground(p, shape);

    // Step 3: Draw state layer
    drawStateLayer(p, shape);

    // Step 4: Draw ripple
    drawRipple(p, shape);

    // Step 5: Draw outline (Outlined variant only)
    if (variant_ == ButtonVariant::Outlined) {
        drawOutline(p, shape);
    }

    // Step 6: Draw content (icon + text)
    drawContent(p, contentRect);

    // Step 7: Draw focus indicator
    drawFocusIndicator(p, shape);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

// Calculate shadow margin (extra space needed for shadow)
QMarginsF Button::shadowMargin() const {
    if (!m_material.elevation() || m_material.elevation()->elevation() <= 0) {
        return QMarginsF(0, 0, 0, 0);
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    // 根据 elevation 级别动态计算边距
    // Level 2: blur=4dp, offset=2dp, 最大偏移约 3dp
    // 预留边距 = offset + blur/2，更精确的阴影空间
    int level = m_material.elevation()->elevation();
    float margin = helper.dpToPx(2.0f + level * 1.5f); // level 2: 约 5dp
    return QMarginsF(margin, margin, margin, margin);
}

void Button::drawShadow(QPainter& p, const QRectF& contentRect, const QPainterPath& shape) {
    if (m_material.elevation() && m_material.elevation()->elevation() > 0) {
        m_material.elevation()->paintShadow(&p, shape);
    }
}

void Button::drawBackground(QPainter& p, const QPainterPath& shape) {
    CFColor bg = containerColor();

    // Handle disabled state
    if (!isEnabled()) {
        QColor color = bg.native_color();
        color.setAlphaF(0.38f); // 38% opacity for disabled
        p.fillPath(shape, color);
        return;
    }

    // Text variant has transparent background
    if (variant_ == ButtonVariant::Text) {
        return;
    }

    p.fillPath(shape, bg.native_color());
}

void Button::drawStateLayer(QPainter& p, const QPainterPath& shape) {
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

void Button::drawRipple(QPainter& p, const QPainterPath& shape) {
    if (m_material.ripple()) {
        // Set ripple color based on label color (content color)
        m_material.ripple()->setColor(labelColor());
        m_material.ripple()->paint(&p, shape);
    }
}

void Button::drawOutline(QPainter& p, const QPainterPath& shape) {
    // 使用 1px inset（而不是 dp）让 outline 刚好在边缘内侧
    // QPen 描边是居中对齐的，所以 0.5px 在内，0.5px 在外
    float inset = 0.5f;

    QColor color = outlineColor().native_color();
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.save();

    // Create inset path for outline
    QRectF shapeBounds = shape.boundingRect();
    QRectF insetRect = shapeBounds.adjusted(inset, inset, -inset, -inset);
    float adjustedRadius = std::max(0.0f, cornerRadius() - inset);
    QPainterPath insetShape = roundedRect(insetRect, adjustedRadius);

    QPen pen(color, 1.0);  // 1px width
    pen.setCosmetic(true); // Keep consistent 1px width across DPI
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(insetShape);

    p.restore();
}

void Button::drawContent(QPainter& p, const QRectF& contentRect) {
    CFColor textColor = labelColor();
    if (!isEnabled()) {
        QColor color = textColor.native_color();
        color.setAlphaF(0.38f);
        p.setPen(color);
    } else {
        p.setPen(textColor.native_color());
    }

    // Use button's font
    QFont font = labelFont();
    p.setFont(font);

    // Use contentRect passed in (already inset for shadow margin)
    QRectF textArea = contentRect;
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float hPadding = helper.dpToPx(24.0f);
    textArea.adjust(hPadding, 0, -hPadding, 0);

    // Calculate icon and text positions
    float iconWidth = 0;
    float iconGap = 0;

    if (!leadingIcon_.isNull()) {
        iconWidth = helper.dpToPx(18.0f);
        iconGap = helper.dpToPx(8.0f);
    }

    float textWidth = fontMetrics().horizontalAdvance(text());
    float totalContentWidth = iconWidth + iconGap + textWidth;
    float startX = textArea.left() + (textArea.width() - totalContentWidth) / 2.0f;
    float centerY = textArea.center().y();

    // Draw icon
    if (!leadingIcon_.isNull()) {
        float iconSize = helper.dpToPx(18.0f);
        QRectF iconRect(startX, centerY - iconSize / 2, iconSize, iconSize);
        leadingIcon_.paint(&p, iconRect.toRect());
        startX += iconWidth + iconGap;
    }

    // Draw text
    QRectF textRect(startX, textArea.top(), textWidth, textArea.height());
    p.drawText(textRect, Qt::AlignCenter, text());
}

void Button::drawFocusIndicator(QPainter& p, const QPainterPath& shape) {
    if (m_material.focusIndicator()) {
        m_material.focusIndicator()->paint(&p, shape, labelColor());
    }
}

} // namespace cf::ui::widget::material
