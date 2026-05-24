/**
 * @file checkbox.cpp
 * @brief Material Design 3 CheckBox Implementation
 *
 * Implements a Material Design 3 checkbox with three states: unchecked, checked,
 * and indeterminate. Supports ripple effects, state layers, and focus indicators.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "checkbox.h"
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
// Constructor / Destructor
// ============================================================================

CheckBox::CheckBox(QWidget* parent)
    : QCheckBox(parent), m_material(this, MaterialWidgetBase::Config{.useElevation = false}) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    if (checkState() == Qt::Checked) {
        m_checkAnimationProgress = 1.0f;
        m_material.stateMachine()->onCheckedChanged(true);
    } else if (checkState() == Qt::PartiallyChecked) {
        m_checkAnimationProgress = 1.0f;
    }

    setCursor(Qt::PointingHandCursor);
}

CheckBox::CheckBox(const QString& text, QWidget* parent) : CheckBox(parent) {
    setText(text);
}

void CheckBox::setChecked(bool checked) {
    if (isChecked() == checked) {
        return;
    }
    QCheckBox::setChecked(checked);
    updateAnimationProgress(checked ? 1.0f : 0.0f, checked);
}

void CheckBox::setCheckState(Qt::CheckState state) {
    if (checkState() == state) {
        return;
    }
    QCheckBox::setCheckState(state);

    float progress = 0.0f;
    bool isCheckedState = false;
    switch (state) {
        case Qt::Unchecked:
            progress = 0.0f;
            isCheckedState = false;
            break;
        case Qt::PartiallyChecked:
            progress = 1.0f; // Full progress for complete indeterminate mark
            isCheckedState = false;
            break;
        case Qt::Checked:
            progress = 1.0f;
            isCheckedState = true;
            break;
    }
    updateAnimationProgress(progress, isCheckedState);
}

void CheckBox::updateAnimationProgress(float progress, bool checked) {
    m_checkAnimationProgress = progress;
    if (m_material.stateMachine()) {
        m_material.stateMachine()->onCheckedChanged(checked);
    }
    update();
}

void CheckBox::startCheckMarkAnimation(float target) {
    float fromValue = m_checkAnimationProgress;

    auto factory = cf::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
        application_support::Application::animationFactory());

    if (!factory) {
        m_checkAnimationProgress = target;
        update();
        return;
    }

    auto anim =
        factory->createPropertyAnimation(&m_checkAnimationProgress, fromValue, target, 300,
                                         cf::ui::base::Easing::Type::EmphasizedDecelerate, this);

    if (anim) {
        if (auto* propAnim = dynamic_cast<CFMaterialPropertyAnimation*>(anim.Get())) {
            propAnim->setRange(fromValue, target);
        }
        anim->start();
    } else {
        m_checkAnimationProgress = target;
        update();
    }
}

CheckBox::~CheckBox() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void CheckBox::enterEvent(QEnterEvent* event) {
    QCheckBox::enterEvent(event);
    m_material.onEnterEvent();
}

void CheckBox::leaveEvent(QEvent* event) {
    QCheckBox::leaveEvent(event);
    m_material.onLeaveEvent();
}

void CheckBox::mousePressEvent(QMouseEvent* event) {
    QCheckBox::mousePressEvent(event);
    m_material.onMousePress(event->pos(), checkboxRect());
}

void CheckBox::mouseReleaseEvent(QMouseEvent* event) {
    QCheckBox::mouseReleaseEvent(event);
    m_material.onMouseRelease();
}

void CheckBox::focusInEvent(QFocusEvent* event) {
    QCheckBox::focusInEvent(event);
    m_material.onFocusIn();
}

void CheckBox::focusOutEvent(QFocusEvent* event) {
    QCheckBox::focusOutEvent(event);
    m_material.onFocusOut();
}

void CheckBox::changeEvent(QEvent* event) {
    QCheckBox::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

void CheckBox::nextCheckState() {
    Qt::CheckState oldState = checkState();
    QCheckBox::nextCheckState();
    Qt::CheckState newState = checkState();

    float newTarget = 0.0f;
    bool checked = false;

    switch (newState) {
        case Qt::Unchecked:
            newTarget = 0.0f;
            checked = false;
            break;
        case Qt::PartiallyChecked:
            newTarget = 1.0f;
            checked = false;
            break;
        case Qt::Checked:
            newTarget = 1.0f;
            checked = true;
            break;
    }

    if (m_material.stateMachine()) {
        m_material.stateMachine()->onCheckedChanged(checked);
    }

    startCheckMarkAnimation(newTarget);
}

bool CheckBox::hitButton(const QPoint& pos) const {
    return rect().contains(pos);
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

bool CheckBox::hasError() const {
    return m_error;
}

void CheckBox::setError(bool error) {
    if (m_error != error) {
        m_error = error;
        emit errorChanged(error);
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize CheckBox::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float leftPadding = helper.dpToPx(12.0f);
    float rightPadding = helper.dpToPx(12.0f);
    float boxSize = helper.dpToPx(18.0f);
    float spacing = helper.dpToPx(12.0f);
    float textWidth = text().isEmpty() ? 0.0f : fontMetrics().horizontalAdvance(text());

    float width = leftPadding + boxSize + spacing + textWidth + rightPadding;
    float minWidth = helper.dpToPx(48.0f);
    width = std::max(width, minWidth);

    float height = helper.dpToPx(48.0f); // Fixed height for touch target

    return QSize(int(std::ceil(width)), int(std::ceil(height)));
}

QSize CheckBox::minimumSizeHint() const {
    return sizeHint();
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164);
} // Purple 700
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
} // On Surface
inline CFColor fallbackError() {
    return CFColor(186, 26, 26);
} // Error
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132);
} // Outline
} // namespace

CFColor CheckBox::checkmarkColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return m_error ? fallbackError() : fallbackPrimary();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        if (m_error) {
            return CFColor(colorScheme.queryColor(ERROR));
        }

        if (isChecked() || checkState() == Qt::PartiallyChecked) {
            return CFColor(colorScheme.queryColor(PRIMARY));
        }
        return CFColor(colorScheme.queryColor(ON_SURFACE));
    } catch (...) {
        return m_error ? fallbackError() : fallbackPrimary();
    }
}

CFColor CheckBox::markDrawColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return CFColor(255, 255, 255);
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_PRIMARY));
    } catch (...) {
        return CFColor(255, 255, 255);
    }
}

CFColor CheckBox::borderColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return m_error ? fallbackError() : fallbackOutline();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        if (m_error) {
            return CFColor(colorScheme.queryColor(ERROR));
        }

        if (isChecked() || checkState() == Qt::PartiallyChecked) {
            return CFColor(colorScheme.queryColor(PRIMARY));
        }

        return CFColor(colorScheme.queryColor(OUTLINE));
    } catch (...) {
        return m_error ? fallbackError() : fallbackOutline();
    }
}

CFColor CheckBox::backgroundColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return CFColor(Qt::transparent);
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        if (isChecked() || checkState() == Qt::PartiallyChecked) {
            return CFColor(colorScheme.queryColor(PRIMARY));
        }
        return CFColor(Qt::transparent);
    } catch (...) {
        return CFColor(Qt::transparent);
    }
}

CFColor CheckBox::stateLayerColor() const {
    return checkmarkColor();
}

float CheckBox::cornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(2.0f);
}

float CheckBox::checkboxSize() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(18.0f);
}

float CheckBox::strokeWidth() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(2.0f);
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF CheckBox::checkboxRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float boxSize = checkboxSize();
    float y = (height() - boxSize) / 2.0f;
    float x = helper.dpToPx(12.0f);

    return QRectF(x, y, boxSize, boxSize);
}

QRectF CheckBox::textRect() const {
    QRectF box = checkboxRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float spacing = helper.dpToPx(12.0f);
    float x = box.right() + spacing;
    float y = 0;
    float w = width() - x;
    float h = height();

    return QRectF(x, y, w, h);
}

// ============================================================================
// Paint Event
// ============================================================================

void CheckBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF box = checkboxRect();

    // Step 1: Draw background (for checked/indeterminate)
    drawBackground(p, box);

    // Step 2: Draw border
    drawBorder(p, box);

    // Step 3: Draw check mark or indeterminate mark
    Qt::CheckState state = checkState();
    if (state == Qt::PartiallyChecked) {
        drawIndeterminateMark(p, box);
    } else if (state == Qt::Checked) {
        drawCheckMark(p, box);
    }

    // Step 4: Draw ripple
    drawRipple(p, box);

    // Step 5: Draw text
    if (!text().isEmpty()) {
        drawText(p, textRect());
    }

    // Step 6: Draw focus indicator
    drawFocusIndicator(p, box);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void CheckBox::drawBackground(QPainter& p, const QRectF& rect) {
    if (checkState() == Qt::Unchecked) {
        return;
    }

    CFColor bgColor = backgroundColor();
    QColor color = bgColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    QPainterPath shape = roundedRect(rect, cornerRadius());
    p.fillPath(shape, color);
}

void CheckBox::drawBorder(QPainter& p, const QRectF& rect) {
    CFColor bColor = borderColor();
    QColor color = bColor.native_color();

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

    p.save();

    float inset = strokeWidth() / 2.0f;
    QRectF insetRect = rect.adjusted(inset, inset, -inset, -inset);
    float adjustedRadius = std::max(0.0f, cornerRadius() - inset);
    QPainterPath shape = roundedRect(insetRect, adjustedRadius);

    QPen pen(color, strokeWidth());
    pen.setCosmetic(false);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(shape);

    p.restore();
}

void CheckBox::drawCheckMark(QPainter& p, const QRectF& rect) {
    CFColor cmColor = markDrawColor();
    QColor color = cmColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.save();
    QPen pen(color, strokeWidth() * 0.6f);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);

    float w = rect.width();
    float h = rect.height();

    float left = rect.left() + w * 0.25f;
    float bottom = rect.top() + h * 0.55f;

    float centerX = rect.left() + w * 0.45f;
    float centerY = rect.top() + h * 0.70f;

    float right = rect.left() + w * 0.75f;
    float top = rect.top() + h * 0.35f;

    float progress = m_checkAnimationProgress;

    if (progress > 0.0f) {
        float segment1Progress = std::min(progress * 2.0f, 1.0f);
        float currentX = left + (centerX - left) * segment1Progress;
        float currentY = bottom + (centerY - bottom) * segment1Progress;
        p.drawLine(QPointF(left, bottom), QPointF(currentX, currentY));
    }

    if (progress > 0.5f) {
        float segment2Progress = (progress - 0.5f) * 2.0f;
        float currentX = centerX + (right - centerX) * segment2Progress;
        float currentY = centerY + (top - centerY) * segment2Progress;
        p.drawLine(QPointF(centerX, centerY), QPointF(currentX, currentY));
    }

    p.restore();
}

void CheckBox::drawIndeterminateMark(QPainter& p, const QRectF& rect) {
    CFColor cmColor = markDrawColor();
    QColor color = cmColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.save();
    QPen pen(color, strokeWidth() * 0.8f);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);

    float w = rect.width();
    float h = rect.height();

    float margin = w * 0.25f;
    float y = rect.top() + h / 2.0f;
    float x1 = rect.left() + margin;
    float x2 = rect.right() - margin;

    float progress = m_checkAnimationProgress;

    if (progress > 0.0f) {
        float currentX = x1 + (x2 - x1) * progress;
        p.drawLine(QPointF(x1, y), QPointF(currentX, y));
    }

    p.restore();
}

void CheckBox::drawRipple(QPainter& p, const QRectF& rect) {
    if (m_material.ripple()) {
        m_material.ripple()->setColor(stateLayerColor());

        QPainterPath clipPath = roundedRect(rect, cornerRadius());
        m_material.ripple()->paint(&p, clipPath);
    }
}

void CheckBox::drawText(QPainter& p, const QRectF& rect) {
    CFColor textColor = checkmarkColor();

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

void CheckBox::drawFocusIndicator(QPainter& p, const QRectF& rect) {
    if (m_material.focusIndicator()) {
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(4.0f);
        QRectF focusRect = rect.adjusted(-margin, -margin, margin, margin);

        QPainterPath shape = roundedRect(focusRect, cornerRadius() + margin);
        m_material.focusIndicator()->paint(&p, shape, checkmarkColor());
    }
}

} // namespace cf::ui::widget::material
