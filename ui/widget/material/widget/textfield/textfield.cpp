/**
 * @file textfield.cpp
 * @brief Material Design 3 TextField Implementation
 *
 * Implements a Material Design 3 text field with filled and outlined variants.
 * Supports floating labels, prefix/suffix icons, character counter, helper/error
 * text, and password mode.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "textfield.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
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
#include <QStyle>

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
inline CFColor fallbackError() {
    return CFColor(186, 26, 26);
} // Error
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

TextField::TextField(TextFieldVariant variant, QWidget* parent)
    : QLineEdit(parent), m_variant(variant), m_showCharacterCounter(false), m_maxLength(0),
      m_isFloating(false), m_hasError(false), m_floatingProgress(0.0f), m_outlineWidth(1.0f),
      m_hoveringClearButton(false), m_pressingClearButton(false) {

    // Disable native frame and background
    setFrame(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setTextMargins(0, 0, 0, 0);

    // Get animation factory from Application
    m_animationFactory =
        cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_ripple = new RippleHelper(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Set ripple mode
    m_ripple->setMode(RippleHelper::Mode::Bounded);

    // Connect repaint signals
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&TextField::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&TextField::update));

    // Connect text change signal
    connect(this, &QLineEdit::textChanged, this, &TextField::textChanged);

    // Set default font
    setFont(inputFont());

    // Set cursor
    setCursor(Qt::IBeamCursor);
}

TextField::TextField(const QString& text, TextFieldVariant variant, QWidget* parent)
    : TextField(variant, parent) {
    setText(text);
}

TextField::~TextField() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void TextField::mousePressEvent(QMouseEvent* event) {
    // Check if clear button was clicked
    QRectF clearRect = clearButtonRect();
    if (!text().isEmpty() && clearRect.contains(event->pos())) {
        m_pressingClearButton = true;
        update();
        return;
    }

    QLineEdit::mousePressEvent(event);
    if (m_stateMachine)
        m_stateMachine->onPress(event->pos());
    if (m_ripple)
        m_ripple->onPress(event->pos(), rect());
    update();
}

void TextField::mouseReleaseEvent(QMouseEvent* event) {
    // Check if clear button was released
    if (m_pressingClearButton) {
        m_pressingClearButton = false;
        QRectF clearRect = clearButtonRect();
        if (clearRect.contains(event->pos())) {
            clear();
        }
        update();
        return;
    }

    QLineEdit::mouseReleaseEvent(event);
    if (m_stateMachine)
        m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    update();
}

void TextField::focusInEvent(QFocusEvent* event) {
    QLineEdit::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    updateFloatingState(true);
    update();
}

void TextField::focusOutEvent(QFocusEvent* event) {
    QLineEdit::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    updateFloatingState(!text().isEmpty());
    update();
}

void TextField::changeEvent(QEvent* event) {
    QLineEdit::changeEvent(event);
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

void TextField::resizeEvent(QResizeEvent* event) {
    QLineEdit::resizeEvent(event);
    update();
}

void TextField::textChanged(const QString& text) {
    // Update floating state based on content
    updateFloatingState(hasFocus() || !text.isEmpty());

    // Update character counter visibility
    if (m_showCharacterCounter) {
        update();
    }
}

void TextField::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Calculate layout rectangles
    QRectF field = fieldRect();
    QRectF helper = helperTextRect();

    // Step 1: Draw background (Filled variant only)
    drawBackground(p, field);

    // Step 2: Draw outline (Outlined variant and Filled active state)
    drawOutline(p, field);

    // Step 3: Draw label (floating or resting)
    drawLabel(p, field);

    // Step 4: Draw prefix icon
    drawPrefixIcon(p, textRect());

    // Step 5: Draw text content
    drawText(p, textRect());

    // Step 6: Draw suffix icon
    drawSuffixIcon(p, textRect());

    // Step 7: Draw clear button
    drawClearButton(p, textRect());

    // Step 8: Draw ripple
    drawRipple(p, field);

    // Step 9: Draw focus indicator
    drawFocusIndicator(p, field);

    // Step 10: Draw helper/error text
    drawHelperText(p, helper);

    // Step 11: Draw character counter
    if (m_showCharacterCounter) {
        drawCharacterCounter(p, helper);
    }
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

TextField::TextFieldVariant TextField::variant() const {
    return m_variant;
}

void TextField::setVariant(TextFieldVariant variant) {
    if (m_variant != variant) {
        m_variant = variant;
        updateGeometry();
        update();
    }
}

QString TextField::label() const {
    return m_label;
}

void TextField::setLabel(const QString& label) {
    if (m_label != label) {
        m_label = label;
        updateGeometry();
        update();
    }
}

QString TextField::helperText() const {
    return m_helperText;
}

void TextField::setHelperText(const QString& text) {
    if (m_helperText != text) {
        m_helperText = text;
        updateGeometry();
        update();
    }
}

QString TextField::errorText() const {
    return m_errorText;
}

void TextField::setErrorText(const QString& text) {
    if (m_errorText != text) {
        m_errorText = text;
        m_hasError = !text.isEmpty();
        updateGeometry();
        update();
    }
}

bool TextField::isFloating() const {
    return m_isFloating;
}

QIcon TextField::prefixIcon() const {
    return m_prefixIcon;
}

void TextField::setPrefixIcon(const QIcon& icon) {
    if (m_prefixIcon.cacheKey() != icon.cacheKey()) {
        m_prefixIcon = icon;
        updateGeometry();
        update();
    }
}

QIcon TextField::suffixIcon() const {
    return m_suffixIcon;
}

void TextField::setSuffixIcon(const QIcon& icon) {
    if (m_suffixIcon.cacheKey() != icon.cacheKey()) {
        m_suffixIcon = icon;
        updateGeometry();
        update();
    }
}

bool TextField::showCharacterCounter() const {
    return m_showCharacterCounter;
}

void TextField::setShowCharacterCounter(bool show) {
    if (m_showCharacterCounter != show) {
        m_showCharacterCounter = show;
        updateGeometry();
        update();
    }
}

int TextField::maxLength() const {
    return m_maxLength;
}

void TextField::setMaxLength(int length) {
    if (m_maxLength != length) {
        m_maxLength = length;
        QLineEdit::setMaxLength(length > 0 ? length : 32767);
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize TextField::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design text field specifications:
    // - Height: 56dp (filled), 56dp (outlined)
    // - Label height: 16dp
    // - Text height: 24dp
    // - Helper text height: 16dp
    float fieldHeight = helper.dpToPx(56.0f);
    float helperHeight = m_helperText.isEmpty() && m_errorText.isEmpty() ? 0 : helper.dpToPx(16.0f);
    float totalHeight = fieldHeight + helperHeight;

    // Minimum width for touch target
    float minWidth = helper.dpToPx(280.0f);
    float contentWidth = helper.dpToPx(280.0f); // Default width

    return QSize(int(std::ceil(contentWidth)), int(std::ceil(totalHeight)));
}

QSize TextField::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float fieldHeight = helper.dpToPx(56.0f);
    float helperHeight = m_helperText.isEmpty() && m_errorText.isEmpty() ? 0 : helper.dpToPx(16.0f);
    float totalHeight = fieldHeight + helperHeight;

    // Minimum width for usability
    float minWidth = helper.dpToPx(200.0f);

    return QSize(int(std::ceil(minWidth)), int(std::ceil(totalHeight)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor TextField::containerColor() const {
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

CFColor TextField::onContainerColor() const {
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

CFColor TextField::labelColor() const {
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

CFColor TextField::inputTextColor() const {
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

CFColor TextField::outlineColor() const {
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

CFColor TextField::focusOutlineColor() const {
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

CFColor TextField::errorColor() const {
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

CFColor TextField::helperTextColor() const {
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

float TextField::cornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    // Small corner radius (4dp)
    return helper.dpToPx(4.0f);
}

QFont TextField::inputFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QLineEdit::font();
        font.setPixelSize(16);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QLineEdit::font();
        font.setPixelSize(16);
        return font;
    }
}

QFont TextField::labelFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QLineEdit::font();
        font.setPixelSize(16);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QLineEdit::font();
        font.setPixelSize(16);
        return font;
    }
}

QFont TextField::helperFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QLineEdit::font();
        font.setPixelSize(12);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodySmall");
    } catch (...) {
        QFont font = QLineEdit::font();
        font.setPixelSize(12);
        return font;
    }
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF TextField::fieldRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float fieldHeight = helper.dpToPx(56.0f);
    return QRectF(0, 0, width(), fieldHeight);
}

QRectF TextField::textRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float hPadding = helper.dpToPx(16.0f);

    // Account for prefix icon
    float prefixWidth = 0;
    if (!m_prefixIcon.isNull()) {
        prefixWidth = helper.dpToPx(24.0f) + helper.dpToPx(12.0f); // icon + gap
    }

    // Account for suffix icon
    float suffixWidth = 0;
    if (!m_suffixIcon.isNull()) {
        suffixWidth = helper.dpToPx(24.0f) + helper.dpToPx(12.0f); // icon + gap
    }

    // Account for clear button
    float clearWidth = 0;
    if (!text().isEmpty()) {
        clearWidth = helper.dpToPx(24.0f) + helper.dpToPx(8.0f); // button + gap
    }

    // For floating label, adjust top margin
    float topMargin = m_isFloating ? helper.dpToPx(16.0f) : helper.dpToPx(28.0f);

    float left = hPadding + prefixWidth;
    float top = topMargin;
    float availableWidth = field.width() - hPadding * 2 - prefixWidth - suffixWidth - clearWidth;
    float height = helper.dpToPx(24.0f); // Text line height

    return QRectF(left, top, availableWidth, height);
}

QRectF TextField::helperTextRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float helperHeight = helper.dpToPx(16.0f);
    float top = field.bottom();

    return QRectF(field.left(), top, field.width(), helperHeight);
}

QRectF TextField::prefixIconRect() const {
    if (m_prefixIcon.isNull()) {
        return QRectF();
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float hPadding = helper.dpToPx(16.0f);
    float iconSize = helper.dpToPx(24.0f);
    float centerY = field.center().y();

    return QRectF(hPadding, centerY - iconSize / 2, iconSize, iconSize);
}

QRectF TextField::suffixIconRect() const {
    if (m_suffixIcon.isNull()) {
        return QRectF();
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float hPadding = helper.dpToPx(16.0f);
    float iconSize = helper.dpToPx(24.0f);
    float gap = helper.dpToPx(12.0f);
    float centerY = field.center().y();

    // Account for clear button
    float clearOffset = 0;
    if (!text().isEmpty()) {
        clearOffset = helper.dpToPx(24.0f) + helper.dpToPx(8.0f);
    }

    float right = field.width() - hPadding - clearOffset;

    return QRectF(right - iconSize, centerY - iconSize / 2, iconSize, iconSize);
}

QRectF TextField::clearButtonRect() const {
    if (text().isEmpty()) {
        return QRectF();
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float hPadding = helper.dpToPx(16.0f);
    float iconSize = helper.dpToPx(24.0f);
    float centerY = field.center().y();

    float right = field.width() - hPadding;

    return QRectF(right - iconSize, centerY - iconSize / 2, iconSize, iconSize);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void TextField::drawBackground(QPainter& p, const QRectF& fieldRect) {
    if (m_variant == TextFieldVariant::Outlined) {
        return; // Outlined variant has no background
    }

    // Filled variant background
    CFColor bg = containerColor();
    if (!isEnabled()) {
        QColor color = bg.native_color();
        color.setAlphaF(0.38f);
        p.fillRect(fieldRect, color);
        return;
    }

    p.fillRect(fieldRect, bg.native_color());
}

void TextField::drawOutline(QPainter& p, const QRectF& fieldRect) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float radius = cornerRadius();

    if (m_variant == TextFieldVariant::Filled) {
        // Filled variant: draw bottom line only
        float lineWidth = helper.dpToPx(1.0f);

        // Use active width when focused or floating
        float activeWidth = helper.dpToPx(2.0f);
        float currentWidth = lineWidth;
        if (hasFocus() || m_isFloating) {
            currentWidth = lineWidth + (activeWidth - lineWidth) * m_floatingProgress;
        }

        // Determine color
        QColor color;
        if (m_hasError) {
            color = errorColor().native_color();
        } else if (hasFocus()) {
            color = focusOutlineColor().native_color();
        } else {
            color = outlineColor().native_color();
        }

        if (!isEnabled()) {
            color.setAlphaF(0.38f);
        }

        // Draw bottom line
        p.fillRect(QRectF(fieldRect.left(), fieldRect.bottom() - currentWidth, fieldRect.width(),
                          currentWidth),
                   color);
    } else {
        // Outlined variant: draw rounded rectangle with gap for label
        QPainterPath shape = roundedRect(fieldRect, radius);

        // Determine color
        QColor color;
        if (m_hasError) {
            color = errorColor().native_color();
        } else if (hasFocus()) {
            color = focusOutlineColor().native_color();
        } else {
            color = outlineColor().native_color();
        }

        if (!isEnabled()) {
            color.setAlphaF(0.38f);
        }

        // Calculate outline width
        float baseWidth = helper.dpToPx(1.0f);
        float activeWidth = helper.dpToPx(2.0f);
        float currentWidth = hasFocus() ? activeWidth : baseWidth;

        QPen pen(color, currentWidth);
        pen.setCosmetic(true);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawPath(shape);
    }
}

void TextField::drawLabel(QPainter& p, const QRectF& fieldRect) {
    if (m_label.isEmpty()) {
        return;
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QFont labelF = labelFont();
    QFontMetricsF fm(labelF);

    // Calculate label position based on floating state
    float floatingScale = 0.75f;             // 75% of normal size
    float floatingY = helper.dpToPx(12.0f);  // Top position when floating
    float restingY = fieldRect.center().y(); // Center when resting

    // Interpolate between resting and floating positions
    float currentY = restingY + (floatingY - restingY) * m_floatingProgress;
    float currentScale = 1.0f - (1.0f - floatingScale) * m_floatingProgress;

    // Calculate horizontal position
    float leftMargin = helper.dpToPx(16.0f);

    // For outlined variant with focus, add horizontal padding
    if (m_variant == TextFieldVariant::Outlined && hasFocus()) {
        leftMargin += helper.dpToPx(4.0f);
    }

    // Account for prefix icon
    if (!m_prefixIcon.isNull()) {
        leftMargin += helper.dpToPx(24.0f) + helper.dpToPx(12.0f);
    }

    // For filled variant, create background gap when floating
    if (m_variant == TextFieldVariant::Filled && m_floatingProgress > 0.5f) {
        float bgWidth = fm.horizontalAdvance(m_label) * currentScale + helper.dpToPx(8.0f);
        QRectF bgRect(leftMargin - helper.dpToPx(4.0f),
                      floatingY - fm.height() * currentScale / 2 - helper.dpToPx(2.0f), bgWidth,
                      fm.height() * currentScale + helper.dpToPx(4.0f));
        p.fillRect(bgRect, containerColor().native_color());
    }

    p.save();

    // Set font and color
    // Get the actual font height since pixelSize() may return 0 if pointSize is used
    float baseFontSize = labelF.pixelSize();
    if (baseFontSize <= 0) {
        baseFontSize = fm.height();
    }
    int scaledSize = static_cast<int>(baseFontSize * currentScale);
    if (scaledSize < 1) {
        scaledSize = 1; // Ensure minimum valid size
    }
    labelF.setPixelSize(scaledSize);
    p.setFont(labelF);

    QColor labelColor;
    if (m_hasError) {
        labelColor = errorColor().native_color();
    } else if (hasFocus() || m_isFloating) {
        labelColor = focusOutlineColor().native_color();
    } else {
        labelColor = this->labelColor().native_color();
    }

    if (!isEnabled()) {
        labelColor.setAlphaF(0.38f);
    }

    p.setPen(labelColor);

    // Draw label
    QRectF textRect(leftMargin, currentY - fm.height() * currentScale / 2,
                    fieldRect.width() - leftMargin * 2, fm.height() * currentScale);
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_label);

    p.restore();
}

void TextField::drawText(QPainter& p, const QRectF& textRect) {
    // Use QLineEdit's default text rendering but clip to our text rect
    p.save();
    p.setClipRect(textRect);

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QFont f = inputFont();
    p.setFont(f);

    QColor textColor = inputTextColor().native_color();
    if (!isEnabled()) {
        textColor.setAlphaF(0.38f);
    }
    p.setPen(textColor);

    // Get display text
    QString displayText = text();
    if (displayText.isEmpty() && placeholderText().isEmpty() == false) {
        // Show placeholder
        QColor placeholderColor = onContainerColor().native_color();
        placeholderColor.setAlphaF(0.6f);
        p.setPen(placeholderColor);
        displayText = placeholderText();
    }

    // Draw text
    QRectF adjustedRect = textRect;
    adjustedRect.setHeight(QFontMetricsF(f).height());

    // Handle password mode
    if (echoMode() == Password) {
        displayText = QString(displayText.length(), QChar(0x2022)); // Bullet character
    }

    p.drawText(adjustedRect, Qt::AlignLeft | Qt::AlignTop, displayText);

    // Draw cursor
    if (hasFocus() && cursorPosition() >= 0) {
        QFontMetricsF fm(f);
        int cursorPos = qBound(0, cursorPosition(), displayText.length());
        float cursorX = textRect.left() + fm.horizontalAdvance(displayText.left(cursorPos));

        float cursorWidth = helper.dpToPx(1.0f);
        QRectF cursorRect(cursorX, adjustedRect.top(), cursorWidth, adjustedRect.height());

        p.fillRect(cursorRect, focusOutlineColor().native_color());
    }

    p.restore();
}

void TextField::drawPrefixIcon(QPainter& p, const QRectF& textRect) {
    if (m_prefixIcon.isNull()) {
        return;
    }

    QRectF iconRect = prefixIconRect();
    if (iconRect.isEmpty()) {
        return;
    }

    QColor iconColor = onContainerColor().native_color();
    if (!isEnabled()) {
        iconColor.setAlphaF(0.38f);
    }

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    m_prefixIcon.paint(&p, iconRect.toRect(), Qt::AlignCenter, mode);
}

void TextField::drawSuffixIcon(QPainter& p, const QRectF& textRect) {
    if (m_suffixIcon.isNull()) {
        return;
    }

    QRectF iconRect = suffixIconRect();
    if (iconRect.isEmpty()) {
        return;
    }

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    m_suffixIcon.paint(&p, iconRect.toRect(), Qt::AlignCenter, mode);
}

void TextField::drawClearButton(QPainter& p, const QRectF& textRect) {
    Q_UNUSED(textRect)
    if (text().isEmpty()) {
        return;
    }

    QRectF iconRect = clearButtonRect();
    if (iconRect.isEmpty()) {
        return;
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());

    p.save();

    // Draw clear "X" icon
    QColor iconColor = onContainerColor().native_color();
    if (m_hoveringClearButton || m_pressingClearButton) {
        iconColor = iconColor.lighter(120);
    }
    if (!isEnabled()) {
        iconColor.setAlphaF(0.38f);
    }

    p.setPen(QPen(iconColor, helper.dpToPx(2.0f)));
    p.setRenderHint(QPainter::Antialiasing);

    float cx = iconRect.center().x();
    float cy = iconRect.center().y();
    float size = helper.dpToPx(18.0f);
    float half = size / 2;

    p.drawLine(QPointF(cx - half, cy - half), QPointF(cx + half, cy + half));
    p.drawLine(QPointF(cx + half, cy - half), QPointF(cx - half, cy + half));

    p.restore();
}

void TextField::drawHelperText(QPainter& p, const QRectF& helperRect) {
    QString text = m_hasError ? m_errorText : m_helperText;
    if (text.isEmpty()) {
        return;
    }

    p.save();

    QFont f = helperFont();
    p.setFont(f);

    QColor textColor = m_hasError ? errorColor().native_color() : helperTextColor().native_color();
    if (!isEnabled()) {
        textColor.setAlphaF(0.38f);
    }
    p.setPen(textColor);

    p.drawText(helperRect, Qt::AlignLeft | Qt::AlignVCenter, text);

    p.restore();
}

void TextField::drawCharacterCounter(QPainter& p, const QRectF& helperRect) {
    if (!m_showCharacterCounter) {
        return;
    }

    p.save();

    QFont f = helperFont();
    p.setFont(f);

    QString counterText = QString("%1/%2").arg(text().length()).arg(m_maxLength);
    QFontMetricsF fm(f);
    float textWidth = fm.horizontalAdvance(counterText);

    QColor textColor = helperTextColor().native_color();
    if (text().length() > m_maxLength) {
        textColor = errorColor().native_color();
    }
    if (!isEnabled()) {
        textColor.setAlphaF(0.38f);
    }
    p.setPen(textColor);

    QRectF counterRect(helperRect.right() - textWidth, helperRect.top(), textWidth,
                       helperRect.height());
    p.drawText(counterRect, Qt::AlignRight | Qt::AlignVCenter, counterText);

    p.restore();
}

void TextField::drawFocusIndicator(QPainter& p, const QRectF& fieldRect) {
    if (m_focusIndicator && hasFocus()) {
        QPainterPath shape = roundedRect(fieldRect, cornerRadius());
        m_focusIndicator->paint(&p, shape, focusOutlineColor());
    }
}

void TextField::drawRipple(QPainter& p, const QRectF& fieldRect) {
    if (m_ripple) {
        QPainterPath shape;
        if (m_variant == TextFieldVariant::Outlined) {
            shape = roundedRect(fieldRect, cornerRadius());
        } else {
            // For filled variant, clip to background
            shape.addRect(fieldRect);
        }
        m_ripple->paint(&p, shape);
    }
}

// ============================================================================
// Animation Helpers
// ============================================================================

void TextField::updateFloatingState(bool shouldFloat) {
    if (m_isFloating != shouldFloat) {
        m_isFloating = shouldFloat;
        emit floatingChanged(m_isFloating);
    }
    animateFloatingTo(shouldFloat);
}

void TextField::animateFloatingTo(bool floating) {
    float target = floating ? 1.0f : 0.0f;

    // Skip animation if already at the target state
    if (qFuzzyCompare(m_floatingProgress, target)) {
        return;
    }

    if (!m_animationFactory) {
        m_floatingProgress = target;
        update();
        return;
    }

    // Create property animation for floating progress
    auto anim = m_animationFactory->createPropertyAnimation(
        &m_floatingProgress, m_floatingProgress, target, 200,
        cf::ui::base::Easing::Type::EmphasizedDecelerate, this);

    if (anim) {
        // IMPORTANT: Update range to fix cached animation's stale from/to values
        if (auto* propAnim =
                dynamic_cast<cf::ui::components::material::CFMaterialPropertyAnimation*>(
                    anim.Get())) {
            propAnim->setRange(m_floatingProgress, target);
        }
        anim->start();
    } else {
        m_floatingProgress = target;
        update();
    }
}

} // namespace cf::ui::widget::material
