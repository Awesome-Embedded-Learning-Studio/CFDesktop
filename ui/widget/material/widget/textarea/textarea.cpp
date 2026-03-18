/**
 * @file textarea.cpp
 * @brief Material Design 3 TextArea Implementation
 *
 * Implements a Material Design 3 text area with filled and outlined variants.
 * Supports floating labels, character counter, helper/error text, and
 * multi-line text input with auto-resize support.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "textarea.h"
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
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTextBlock>
#include <QTextCursor>

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

TextArea::TextArea(TextAreaVariant variant, QWidget* parent)
    : QTextEdit(parent), m_variant(variant), m_showCharacterCounter(false), m_maxLength(0),
      m_minLines(1), m_maxLines(0), m_isFloating(false), m_hasError(false),
      m_floatingProgress(0.0f), m_updatingGeometry(false) {

    // Disable native frame
    setFrameStyle(QFrame::NoFrame);
    // Set viewport margins for custom drawing space above text
    setViewportMargins(0, 0, 0, 0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&TextArea::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&TextArea::update));

    // Connect text change signal
    connect(this, &QTextEdit::textChanged, this, &TextArea::textChanged);

    // Set default font
    setFont(inputFont());

    // Set cursor
    setCursor(Qt::IBeamCursor);

    // Disable tab focus for proper multi-line editing
    setTabChangesFocus(false);

    // Configure viewport for proper background rendering
    // (background appears below text, overlays are drawn in paintEvent)
    if (viewport()) {
        viewport()->setAutoFillBackground(true);
        QPalette pal = viewport()->palette();
        pal.setColor(QPalette::Base, containerColor().native_color());
        viewport()->setPalette(pal);
    }
}

TextArea::TextArea(const QString& text, TextAreaVariant variant, QWidget* parent)
    : TextArea(variant, parent) {
    setText(text);
}

TextArea::~TextArea() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void TextArea::mousePressEvent(QMouseEvent* event) {
    QTextEdit::mousePressEvent(event);
    if (m_stateMachine)
        m_stateMachine->onPress(event->pos());
    if (m_ripple)
        m_ripple->onPress(event->pos(), rect());
    update();
}

void TextArea::mouseReleaseEvent(QMouseEvent* event) {
    QTextEdit::mouseReleaseEvent(event);
    if (m_stateMachine)
        m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    update();
}

void TextArea::focusInEvent(QFocusEvent* event) {
    QTextEdit::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    updateFloatingState(true);
    update();
}

void TextArea::focusOutEvent(QFocusEvent* event) {
    QTextEdit::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    updateFloatingState(!toPlainText().isEmpty());
    update();
}

void TextArea::changeEvent(QEvent* event) {
    QTextEdit::changeEvent(event);
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

void TextArea::resizeEvent(QResizeEvent* event) {
    QTextEdit::resizeEvent(event);
    update();
}

void TextArea::textChanged() {
    // Enforce max length
    if (m_maxLength > 0) {
        QString text = toPlainText();
        if (text.length() > m_maxLength) {
            // Truncate to max length
            text = text.left(m_maxLength);
            m_updatingGeometry = true;
            setPlainText(text);
            m_updatingGeometry = false;

            // Move cursor to end
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }
    }

    // Update floating state based on content
    updateFloatingState(hasFocus() || !toPlainText().isEmpty());

    // Update character counter visibility
    if (m_showCharacterCounter) {
        update();
    }

    // Auto-resize based on line count
    if (!m_updatingGeometry) {
        updateGeometryForLines();
    }
}

void TextArea::keyPressEvent(QKeyEvent* event) {
    // Block Enter key if maxLines is set and already at limit
    if (m_maxLines > 0 && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        if (document()->blockCount() >= m_maxLines) {
            // Ignore the Enter key
            return;
        }
    }

    QTextEdit::keyPressEvent(event);
}

void TextArea::paintEvent(QPaintEvent* event) {
    // 1) First let base class render text content to viewport
    QTextEdit::paintEvent(event);

    // 2) Then draw overlays (label, outline, ripple, focus, helper, counter) on viewport
    QWidget* vp = viewport();
    if (!vp) {
        return; // extreme edge case guard
    }

    QPainter p(vp);
    if (!p.isActive()) {
        qWarning() << "TextArea::paintEvent: painter not active on viewport()";
        return;
    }
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF field = fieldRect();
    const QRectF helper = helperTextRect();

    // Draw order: outline/ripple/focus/helper/counter (overlay)
    drawOutline(p, field);

    // Ensure ripple/focus routines check painter active internally
    if (p.isActive()) {
        drawRipple(p, field);
        drawFocusIndicator(p, field);
    }

    drawHelperText(p, helper);
    if (m_showCharacterCounter)
        drawCharacterCounter(p, helper);

    // p destructor will end painting
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

TextArea::TextAreaVariant TextArea::variant() const {
    return m_variant;
}

void TextArea::setVariant(TextAreaVariant variant) {
    if (m_variant != variant) {
        m_variant = variant;
        updateGeometry();
        update();
    }
}

QString TextArea::label() const {
    return m_label;
}

void TextArea::setLabel(const QString& label) {
    if (m_label != label) {
        m_label = label;
        updateGeometry();
        update();
    }
}

QString TextArea::helperText() const {
    return m_helperText;
}

void TextArea::setHelperText(const QString& text) {
    if (m_helperText != text) {
        m_helperText = text;
        updateGeometry();
        update();
    }
}

QString TextArea::errorText() const {
    return m_errorText;
}

void TextArea::setErrorText(const QString& text) {
    if (m_errorText != text) {
        m_errorText = text;
        m_hasError = !text.isEmpty();
        updateGeometry();
        update();
    }
}

bool TextArea::isFloating() const {
    return m_isFloating;
}

bool TextArea::showCharacterCounter() const {
    return m_showCharacterCounter;
}

void TextArea::setShowCharacterCounter(bool show) {
    if (m_showCharacterCounter != show) {
        m_showCharacterCounter = show;
        updateGeometry();
        update();
    }
}

int TextArea::maxLength() const {
    return m_maxLength;
}

void TextArea::setMaxLength(int length) {
    if (m_maxLength != length) {
        m_maxLength = length;
        update();
    }
}

int TextArea::minLines() const {
    return m_minLines;
}

void TextArea::setMinLines(int lines) {
    if (m_minLines != lines && lines > 0) {
        m_minLines = lines;
        updateGeometry();
        update();
    }
}

int TextArea::maxLines() const {
    return m_maxLines;
}

void TextArea::setMaxLines(int lines) {
    if (m_maxLines != lines && lines >= 0) {
        m_maxLines = lines;
        updateGeometry();
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize TextArea::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Calculate line height
    QFont f = inputFont();
    QFontMetricsF fm(f);
    float lineHeight = fm.height() + helper.dpToPx(8.0f); // Line spacing

    // Calculate height based on line count
    int lineCount = qMax(m_minLines, document()->blockCount());
    if (m_maxLines > 0) {
        lineCount = qMin(lineCount, m_maxLines);
    }

    float contentHeight = lineHeight * lineCount;
    float labelHeight = m_label.isEmpty() ? 0 : helper.dpToPx(16.0f);
    float vPadding = helper.dpToPx(16.0f); // Top and bottom padding
    float fieldHeight = contentHeight + vPadding * 2;

    // Add space for floating label (it overlays content)
    float totalHeight = fieldHeight;

    // Add helper text height
    float helperHeight = m_helperText.isEmpty() && m_errorText.isEmpty() ? 0 : helper.dpToPx(16.0f);
    totalHeight += helperHeight;

    // Minimum width for touch target
    float minWidth = helper.dpToPx(280.0f);
    float contentWidth = helper.dpToPx(280.0f); // Default width

    return QSize(int(std::ceil(contentWidth)), int(std::ceil(totalHeight)));
}

QSize TextArea::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QFont f = inputFont();
    QFontMetricsF fm(f);
    float lineHeight = fm.height() + helper.dpToPx(8.0f);

    float contentHeight = lineHeight * m_minLines;
    float vPadding = helper.dpToPx(16.0f);
    float fieldHeight = contentHeight + vPadding * 2;

    float helperHeight = m_helperText.isEmpty() && m_errorText.isEmpty() ? 0 : helper.dpToPx(16.0f);
    float totalHeight = fieldHeight + helperHeight;

    // Minimum width for usability
    float minWidth = helper.dpToPx(200.0f);

    return QSize(int(std::ceil(minWidth)), int(std::ceil(totalHeight)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor TextArea::containerColor() const {
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

CFColor TextArea::onContainerColor() const {
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

CFColor TextArea::labelColor() const {
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

CFColor TextArea::inputTextColor() const {
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

CFColor TextArea::outlineColor() const {
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

CFColor TextArea::focusOutlineColor() const {
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

CFColor TextArea::errorColor() const {
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

CFColor TextArea::helperTextColor() const {
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

float TextArea::cornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    // Small corner radius (4dp)
    return helper.dpToPx(4.0f);
}

QFont TextArea::inputFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QTextEdit::font();
        font.setPixelSize(16);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QTextEdit::font();
        font.setPixelSize(16);
        return font;
    }
}

QFont TextArea::labelFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QTextEdit::font();
        font.setPixelSize(16);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QTextEdit::font();
        font.setPixelSize(16);
        return font;
    }
}

QFont TextArea::helperFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QTextEdit::font();
        font.setPixelSize(12);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodySmall");
    } catch (...) {
        QFont font = QTextEdit::font();
        font.setPixelSize(12);
        return font;
    }
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF TextArea::fieldRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Calculate height based on content
    QFont f = inputFont();
    QFontMetricsF fm(f);
    float lineHeight = fm.height() + helper.dpToPx(8.0f);

    int lineCount = qMax(m_minLines, document()->blockCount());
    if (m_maxLines > 0) {
        lineCount = qMin(lineCount, m_maxLines);
    }

    float contentHeight = lineHeight * lineCount;
    float vPadding = helper.dpToPx(16.0f);
    float fieldHeight = contentHeight + vPadding * 2;

    return QRectF(0, 0, width(), fieldHeight);
}

QRectF TextArea::textRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float hPadding = helper.dpToPx(16.0f);

    // Account for floating label
    float topMargin = helper.dpToPx(16.0f);
    if (m_isFloating && !m_label.isEmpty()) {
        topMargin += helper.dpToPx(8.0f);
    }

    float left = hPadding;
    float top = topMargin;
    float availableWidth = field.width() - hPadding * 2;
    float availableHeight = field.height() - topMargin - helper.dpToPx(16.0f);

    return QRectF(left, top, availableWidth, availableHeight);
}

QRectF TextArea::helperTextRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QRectF field = fieldRect();
    float helperHeight = helper.dpToPx(16.0f);
    float top = field.bottom();

    return QRectF(field.left(), top, field.width(), helperHeight);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void TextArea::drawBackground(QPainter& p, const QRectF& fieldRect) {
    if (m_variant == TextAreaVariant::Outlined) {
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

void TextArea::drawOutline(QPainter& p, const QRectF& fieldRect) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float radius = cornerRadius();

    if (m_variant == TextAreaVariant::Filled) {
        // Filled variant: draw bottom border only
        float lineWidth = helper.dpToPx(1.0f);

        // Use active width when focused
        float activeWidth = helper.dpToPx(2.0f);
        float currentWidth = hasFocus() ? activeWidth : lineWidth;

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
        // Outlined variant: draw rounded rectangle
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

void TextArea::drawHelperText(QPainter& p, const QRectF& helperRect) {
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

void TextArea::drawCharacterCounter(QPainter& p, const QRectF& helperRect) {
    if (!m_showCharacterCounter) {
        return;
    }

    p.save();

    QFont f = helperFont();
    p.setFont(f);

    QString counterText = QString("%1/%2").arg(toPlainText().length()).arg(m_maxLength);
    QFontMetricsF fm(f);
    float textWidth = fm.horizontalAdvance(counterText);

    QColor textColor = helperTextColor().native_color();
    if (toPlainText().length() > m_maxLength) {
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

void TextArea::drawFocusIndicator(QPainter& p, const QRectF& fieldRect) {
    if (!p.isActive())
        return; // guard
    if (m_focusIndicator && hasFocus()) {
        QPainterPath shape = roundedRect(fieldRect, cornerRadius());
        m_focusIndicator->paint(&p, shape, focusOutlineColor());
    }
}

void TextArea::drawRipple(QPainter& p, const QRectF& fieldRect) {
    if (!p.isActive())
        return; // guard
    if (m_ripple) {
        QPainterPath shape;
        if (m_variant == TextAreaVariant::Outlined) {
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

void TextArea::updateFloatingState(bool shouldFloat) {
    if (m_isFloating != shouldFloat) {
        m_isFloating = shouldFloat;
        emit floatingChanged(m_isFloating);
    }
    animateFloatingTo(shouldFloat);
}

void TextArea::animateFloatingTo(bool floating) {
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

    // Create property animation for floating progress (same approach as TextField)
    auto anim = m_animationFactory->createPropertyAnimation(
        &m_floatingProgress, m_floatingProgress, target, 200,
        cf::ui::base::Easing::Type::EmphasizedDecelerate, this);

    if (anim) {
        anim->start();
    } else {
        m_floatingProgress = target;
        update();
    }
}

void TextArea::updateGeometryForLines() {
    // Calculate new height based on content
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    QFont f = inputFont();
    QFontMetricsF fm(f);
    float lineHeight = fm.height() + helper.dpToPx(8.0f);

    // Calculate line count
    int lineCount = qMax(m_minLines, document()->blockCount());
    if (m_maxLines > 0) {
        lineCount = qMin(lineCount, m_maxLines);
    }

    float contentHeight = lineHeight * lineCount;
    float vPadding = helper.dpToPx(16.0f);
    float fieldHeight = contentHeight + vPadding * 2;

    // Add helper text height
    float helperHeight = m_helperText.isEmpty() && m_errorText.isEmpty() ? 0 : helper.dpToPx(16.0f);
    float totalHeight = fieldHeight + helperHeight;

    // Set the new height
    int newHeight = int(std::ceil(totalHeight));
    if (height() != newHeight) {
        setFixedHeight(newHeight);
    }
}

} // namespace cf::ui::widget::material
