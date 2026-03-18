/**
 * @file combobox.cpp
 * @brief Material Design 3 ComboBox Implementation
 *
 * Implements a Material Design 3 combo box (dropdown) with filled and
 * outlined variants, animated dropdown arrow, and custom list styling.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "combobox.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/easing.h"
#include "base/geometry_helper.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/material/cfmaterial_property_animation.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/ripple_helper.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOptionComboBox>
#include <QTimer>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;

// ============================================================================
// Constants
// ============================================================================

namespace {
// Material Design 3 ComboBox specifications (in dp)
constexpr float FIELD_HEIGHT_DP = 56.0f;
constexpr float ARROW_SIZE_DP = 24.0f;
constexpr float ARROW_ICON_SIZE_DP = 12.0f;
constexpr float H_PADDING_DP = 16.0f;
constexpr float OUTLINE_WIDTH_DP = 1.0f;
constexpr float CORNER_RADIUS_DP = 4.0f;
constexpr float FOCUS_RING_MARGIN_DP = 4.0f;
constexpr float MIN_WIDTH_DP = 120.0f;
constexpr float DEFAULT_WIDTH_DP = 200.0f;
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

ComboBox::ComboBox(QWidget* parent) : QComboBox(parent), variant_(ComboBoxVariant::Filled) {
    // Set size policy
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // Set minimum contents length for proper sizing
    setMinimumContentsLength(1);

    // Get animation factory from Application
    m_animationFactory = cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(
        application_support::Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_ripple = new RippleHelper(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Set ripple mode to bounded
    m_ripple->setMode(RippleHelper::Mode::Bounded);

    // Connect repaint signals
    connect(m_ripple, &RippleHelper::repaintNeeded, this, QOverload<>::of(&ComboBox::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&ComboBox::update));

    // Initialize popup animation
    m_popupAnimation = new QPropertyAnimation(this);
    m_popupAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_popupAnimation->setDuration(250);

    // Set default cursor
    setCursor(Qt::PointingHandCursor);
}

ComboBox::~ComboBox() {
    // Components are parented to this, Qt will delete them automatically
}

ComboBox::ComboBoxVariant ComboBox::variant() const {
    return variant_;
}

void ComboBox::setVariant(ComboBoxVariant variant) {
    if (variant_ != variant) {
        variant_ = variant;
        update();
    }
}

// ============================================================================
// Event Handlers
// ============================================================================

void ComboBox::enterEvent(QEnterEvent* event) {
    QComboBox::enterEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverEnter();
    update();
}

void ComboBox::leaveEvent(QEvent* event) {
    QComboBox::leaveEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverLeave();
    if (m_ripple)
        m_ripple->onCancel();
    update();
}

void ComboBox::mousePressEvent(QMouseEvent* event) {
    QComboBox::mousePressEvent(event);
    if (m_stateMachine)
        m_stateMachine->onPress(event->pos());
    if (m_ripple)
        m_ripple->onPress(event->pos(), fieldRect());
    update();
}

void ComboBox::mouseReleaseEvent(QMouseEvent* event) {
    QComboBox::mouseReleaseEvent(event);
    if (m_stateMachine)
        m_stateMachine->onRelease();
    if (m_ripple)
        m_ripple->onRelease();
    update();
}

void ComboBox::focusInEvent(QFocusEvent* event) {
    QComboBox::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    update();
}

void ComboBox::focusOutEvent(QFocusEvent* event) {
    QComboBox::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    update();
}

void ComboBox::changeEvent(QEvent* event) {
    QComboBox::changeEvent(event);
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

void ComboBox::showPopup() {
    // Start arrow rotation animation
    if (m_animationFactory) {
        auto anim = m_animationFactory->createPropertyAnimation(
            &m_arrowRotation, 0.0f, 180.0f, 200, cf::ui::base::Easing::Type::Standard, this);
        if (anim) {
            // IMPORTANT: Update range to fix cached animation's stale from/to values
            if (auto* propAnim =
                    dynamic_cast<cf::ui::components::material::CFMaterialPropertyAnimation*>(
                        anim.Get())) {
                propAnim->setRange(0.0f, 180.0f);
            }
            anim->start();
        } else {
            m_arrowRotation = 180.0f;
        }
    } else {
        m_arrowRotation = 180.0f;
    }

    // Apply custom styling to the list view
    QListView* listView = qobject_cast<QListView*>(view());
    if (listView) {
        listView->setStyleSheet("QListView {"
                                "    background-color: palette(base);"
                                "    border: 1px solid palette(mid);"
                                "    padding: 4px;"
                                "}"
                                "QListView::item {"
                                "    padding: 12px 16px;"
                                "    min-height: 40px;"
                                "}"
                                "QListView::item:hover {"
                                "    background-color: palette(highlight);"
                                "}"
                                "QListView::item:selected {"
                                "    background-color: palette(highlight);"
                                "}");
    }

    // First show the popup normally
    QComboBox::showPopup();

    // Get the popup container
    if (QWidget* container = findChild<QWidget*>(QLatin1String("QComboBoxPrivateContainer"))) {
        m_popupContainer = container;

        // Store the target height
        m_targetPopupHeight = container->height();

        // Set initial height for drawer animation (start from 0)
        container->setFixedHeight(0);

        // Stop any existing animation
        if (m_popupAnimation) {
            m_popupAnimation->stop();
        }

        // Setup drawer animation - expand from 0 to target height
        m_popupAnimation->setTargetObject(container);
        m_popupAnimation->setPropertyName("geometry");
        m_popupAnimation->setStartValue(
            QRect(container->x(), container->y(), container->width(), 0));

        // Calculate target position (drawer should expand downward from combo box bottom)
        QRect targetRect = container->geometry();
        targetRect.setHeight(m_targetPopupHeight);
        m_popupAnimation->setEndValue(targetRect);

        m_popupAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void ComboBox::hidePopup() {
    // Stop any running popup animation
    if (m_popupAnimation && m_popupAnimation->state() == QPropertyAnimation::Running) {
        m_popupAnimation->stop();
    }

    // Reset arrow rotation animation
    if (m_animationFactory) {
        auto anim = m_animationFactory->createPropertyAnimation(
            &m_arrowRotation, m_arrowRotation, 0.0f, 150, cf::ui::base::Easing::Type::Standard,
            this);
        if (anim) {
            // IMPORTANT: Update range to fix cached animation's stale from/to values
            if (auto* propAnim =
                    dynamic_cast<cf::ui::components::material::CFMaterialPropertyAnimation*>(
                        anim.Get())) {
                propAnim->setRange(m_arrowRotation, 0.0f);
            }
            anim->start();
        } else {
            m_arrowRotation = 0.0f;
        }
    } else {
        m_arrowRotation = 0.0f;
    }

    QComboBox::hidePopup();
    m_popupContainer = nullptr;
}

// ============================================================================
// Size Hints
// ============================================================================

QSize ComboBox::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design 3 combo box specifications:
    // - Field height: 56dp
    // - Horizontal padding: 16dp
    // - Arrow size: 24dp
    // - Minimum width: based on text content

    float hPadding = helper.dpToPx(H_PADDING_DP);
    float arrowWidth = helper.dpToPx(ARROW_SIZE_DP);
    float textWidth =
        fontMetrics().horizontalAdvance(currentText()) + hPadding * 2 + arrowWidth + 20;

    float height = helper.dpToPx(FIELD_HEIGHT_DP);
    float minWidth = helper.dpToPx(DEFAULT_WIDTH_DP);

    return QSize(int(std::ceil(qMax(textWidth, minWidth))), int(std::ceil(height)));
}

QSize ComboBox::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float height = helper.dpToPx(FIELD_HEIGHT_DP);
    float minWidth = helper.dpToPx(MIN_WIDTH_DP);

    return QSize(int(std::ceil(minWidth)), int(std::ceil(height)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164); // Purple 700
}
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31); // On Surface
}
inline CFColor fallbackSurface() {
    return CFColor(232, 226, 232); // Surface
}
inline CFColor fallbackOutline() {
    return CFColor(120, 124, 132); // Outline
}
inline CFColor fallbackOnPrimaryContainer() {
    return CFColor(233, 227, 235); // On Primary Container
}
inline CFColor fallbackPrimaryContainer() {
    return CFColor(225, 218, 237); // Primary Container
}
} // namespace

CFColor ComboBox::containerColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return variant_ == ComboBoxVariant::Filled ? fallbackPrimaryContainer() : fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        if (variant_ == ComboBoxVariant::Filled) {
            return CFColor(colorScheme.queryColor(PRIMARY_CONTAINER));
        }
        return CFColor(colorScheme.queryColor(SURFACE));
    } catch (...) {
        return variant_ == ComboBoxVariant::Filled ? fallbackPrimaryContainer() : fallbackSurface();
    }
}

CFColor ComboBox::labelColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return variant_ == ComboBoxVariant::Filled ? fallbackOnPrimaryContainer()
                                                   : fallbackOnSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();

        if (variant_ == ComboBoxVariant::Filled) {
            return CFColor(colorScheme.queryColor(ON_PRIMARY_CONTAINER));
        }
        return CFColor(colorScheme.queryColor(ON_SURFACE));
    } catch (...) {
        return variant_ == ComboBoxVariant::Filled ? fallbackOnPrimaryContainer()
                                                   : fallbackOnSurface();
    }
}

CFColor ComboBox::outlineColor() const {
    auto* app = application_support::Application::instance();
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

CFColor ComboBox::stateLayerColor() const {
    return labelColor();
}

CFColor ComboBox::arrowColor() const {
    return labelColor();
}

float ComboBox::cornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(CORNER_RADIUS_DP);
}

float ComboBox::arrowRotation() const {
    return m_arrowRotation;
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF ComboBox::fieldRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float height = helper.dpToPx(FIELD_HEIGHT_DP);
    return QRectF(0, 0, width(), height);
}

QRectF ComboBox::textRect() const {
    QRectF field = fieldRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float hPadding = helper.dpToPx(H_PADDING_DP);
    float arrowWidth = helper.dpToPx(ARROW_SIZE_DP);

    float x = field.left() + hPadding;
    float y = field.top();
    float w = field.width() - 2 * hPadding - arrowWidth - hPadding;
    float h = field.height();

    return QRectF(x, y, w, h);
}

QRectF ComboBox::arrowRect() const {
    QRectF field = fieldRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float arrowSize = helper.dpToPx(ARROW_SIZE_DP);

    float x = field.right() - arrowSize - helper.dpToPx(H_PADDING_DP);
    float y = (field.height() - arrowSize) / 2.0f;

    return QRectF(x, y, arrowSize, arrowSize);
}

// ============================================================================
// Paint Event
// ============================================================================

void ComboBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF field = fieldRect();
    QPainterPath shape = roundedRect(field, cornerRadius());

    // Step 1: Draw background (for filled variant)
    if (variant_ == ComboBoxVariant::Filled) {
        drawBackground(p, shape);
    }

    // Step 2: Draw outline (for outlined variant or all)
    drawOutline(p, shape);

    // Step 3: Draw state layer
    drawStateLayer(p, shape);

    // Step 4: Draw ripple
    drawRipple(p, shape);

    // Step 5: Draw text
    drawText(p, textRect());

    // Step 6: Draw arrow
    drawArrow(p, arrowRect());

    // Step 7: Draw focus indicator
    drawFocusIndicator(p, shape);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void ComboBox::drawBackground(QPainter& p, const QPainterPath& shape) {
    if (variant_ != ComboBoxVariant::Filled) {
        return;
    }

    CFColor bgColor = containerColor();
    QColor color = bgColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.fillPath(shape, color);
}

void ComboBox::drawOutline(QPainter& p, const QPainterPath& shape) {
    CFColor oColor = outlineColor();
    QColor color = oColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    // For filled variant, outline is only visible when hovered/focused
    if (variant_ == ComboBoxVariant::Filled) {
        if (m_stateMachine && isEnabled()) {
            float opacity = m_stateMachine->stateLayerOpacity();
            if (opacity > 0.0f) {
                color.setAlphaF(opacity);
            } else {
                return; // No outline when not interacting
            }
        } else {
            return;
        }
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float inset = helper.dpToPx(OUTLINE_WIDTH_DP) / 2.0f;
    QPainterPath insetShape =
        roundedRect(fieldRect().adjusted(inset, inset, -inset, -inset), cornerRadius() - inset);

    QPen pen(color, helper.dpToPx(OUTLINE_WIDTH_DP));
    pen.setCosmetic(false);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(insetShape);
}

void ComboBox::drawStateLayer(QPainter& p, const QPainterPath& shape) {
    if (!m_stateMachine || !isEnabled()) {
        return;
    }

    float opacity = m_stateMachine->stateLayerOpacity();
    if (opacity <= 0.0f) {
        return;
    }

    CFColor stateColor = stateLayerColor();
    QColor stateQColor = stateColor.native_color();
    stateQColor.setAlphaF(opacity * 0.08f); // State layer has reduced opacity

    p.fillPath(shape, stateQColor);
}

void ComboBox::drawRipple(QPainter& p, const QPainterPath& shape) {
    if (m_ripple) {
        // Update ripple color based on current state
        m_ripple->setColor(stateLayerColor());

        m_ripple->paint(&p, shape);
    }
}

void ComboBox::drawText(QPainter& p, const QRectF& rect) {
    CFColor textColor = labelColor();
    QColor color = textColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.setPen(color);
    p.setFont(font());

    // Draw text vertically centered, left aligned
    QRectF textBounds = rect.adjusted(0, 2, 0, -2);
    p.drawText(textBounds, Qt::AlignLeft | Qt::AlignVCenter, currentText());
}

void ComboBox::drawArrow(QPainter& p, const QRectF& rect) {
    CFColor arrowColor = this->arrowColor();
    QColor color = arrowColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }
    QPen pen(color, 2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);

    // Draw dropdown arrow (downward pointing triangle)
    float cx = rect.center().x();
    float cy = rect.center().y();
    float size = 5.0f; // Arrow size

    // Apply rotation transformation
    p.save();
    p.translate(cx, cy);
    p.rotate(arrowRotation());

    // Draw arrow as three connected lines
    QPointF points[3] = {QPointF(-size, -size / 2.0f), QPointF(0, size / 2.0f),
                         QPointF(size, -size / 2.0f)};

    for (int i = 0; i < 2; ++i) {
        p.drawLine(points[i], points[i + 1]);
    }

    p.restore();
}

void ComboBox::drawFocusIndicator(QPainter& p, const QPainterPath& shape) {
    if (m_focusIndicator) {
        // Expand rect slightly for focus ring
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(FOCUS_RING_MARGIN_DP);
        QRectF field = fieldRect();
        QRectF focusRect = field.adjusted(-margin, -margin, margin, margin);

        QPainterPath focusShape = roundedRect(focusRect, cornerRadius() + margin);
        m_focusIndicator->paint(&p, focusShape, containerColor());
    }
}

} // namespace cf::ui::widget::material
