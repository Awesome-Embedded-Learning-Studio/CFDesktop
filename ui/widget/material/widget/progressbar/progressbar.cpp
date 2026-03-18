/**
 * @file progressbar.cpp
 * @brief Material Design 3 ProgressBar Implementation
 *
 * Implements a Material Design 3 progress bar with determinate and
 * indeterminate modes. Supports smooth animations, state layers,
 * and focus indicators.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "progressbar.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/easing.h"
#include "base/geometry_helper.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/material/cfmaterial_property_animation.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>

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
// Material Design 3 ProgressBar specifications (in dp)
constexpr float TRACK_HEIGHT_DP = 8.0f;
constexpr float TEXT_TO_TRACK_GAP_DP = 4.0f;
constexpr float FOCUS_RING_MARGIN_DP = 4.0f;
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

ProgressBar::ProgressBar(QWidget* parent) : QProgressBar(parent) {
    // Set size policy
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Get animation factory from Application
    m_animationFactory = cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(
        application_support::Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Connect repaint signals
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            QOverload<>::of(&ProgressBar::update));

    // Start indeterminate animation if in indeterminate mode
    if (minimum() == 0 && maximum() == 0) {
        startIndeterminateAnimation();
    }
}

ProgressBar::~ProgressBar() {
    stopIndeterminateAnimation();
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void ProgressBar::enterEvent(QEnterEvent* event) {
    QProgressBar::enterEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverEnter();
    update();
}

void ProgressBar::leaveEvent(QEvent* event) {
    QProgressBar::leaveEvent(event);
    if (m_stateMachine)
        m_stateMachine->onHoverLeave();
    update();
}

void ProgressBar::focusInEvent(QFocusEvent* event) {
    QProgressBar::focusInEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusIn();
    if (m_focusIndicator)
        m_focusIndicator->onFocusIn();
    update();
}

void ProgressBar::focusOutEvent(QFocusEvent* event) {
    QProgressBar::focusOutEvent(event);
    if (m_stateMachine)
        m_stateMachine->onFocusOut();
    if (m_focusIndicator)
        m_focusIndicator->onFocusOut();
    update();
}

void ProgressBar::changeEvent(QEvent* event) {
    QProgressBar::changeEvent(event);
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

// ============================================================================
// Size Hints
// ============================================================================

QSize ProgressBar::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material Design 3 progress bar specifications:
    // - Track height: 4dp
    // - Touch target: 48dp (for accessibility)
    // - Minimum width: 280dp

    float minHeight = helper.dpToPx(48.0f); // Touch target
    float minWidth = helper.dpToPx(280.0f);

    return QSize(int(std::ceil(minWidth)), int(std::ceil(minHeight)));
}

QSize ProgressBar::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    float minHeight = helper.dpToPx(48.0f);
    float minWidth = helper.dpToPx(200.0f);

    return QSize(int(std::ceil(minWidth)), int(std::ceil(minHeight)));
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackSurface() {
    return CFColor(232, 226, 232); // Surface container
}
inline CFColor fallbackPrimary() {
    return CFColor(103, 80, 164); // Purple 700
}
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31); // On Surface
}
} // namespace

CFColor ProgressBar::trackColor() const {
    auto* app = application_support::Application::instance();
    if (!app) {
        return fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        // Use SURFACE_VARIANT for track background (subtle contrast from main surface)
        return CFColor(colorScheme.queryColor(SURFACE_VARIANT));
    } catch (...) {
        return fallbackSurface();
    }
}

CFColor ProgressBar::fillColor() const {
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

CFColor ProgressBar::textColor() const {
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

CFColor ProgressBar::stateLayerColor() const {
    return fillColor();
}

float ProgressBar::cornerRadius() const {
    // Progress bar uses full rounding (pill shape)
    return trackHeight() / 2.0f;
}

float ProgressBar::trackHeight() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(TRACK_HEIGHT_DP);
}

// ============================================================================
// Layout Helpers
// ============================================================================

QRectF ProgressBar::trackRect() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float h = trackHeight();
    float y = (height() - h) / 2.0f;
    return QRectF(0, y, width(), h);
}

// ============================================================================
// Indeterminate Animation
// ============================================================================

void ProgressBar::startIndeterminateAnimation() {
    if (!m_animationFactory || m_indeterminateAnimating) {
        return;
    }

    m_indeterminateAnimating = true;

    // Create a looping animation for indeterminate mode
    // Using a property animation on m_indeterminatePosition
    auto anim = m_animationFactory->createPropertyAnimation(
        &m_indeterminatePosition, 0.0f, 1.0f, 1500, cf::ui::base::Easing::Type::Linear, this);

    if (anim) {
        // IMPORTANT: Update range to fix cached animation's stale from/to values
        if (auto* propAnim =
                dynamic_cast<cf::ui::components::material::CFMaterialPropertyAnimation*>(
                    anim.Get())) {
            propAnim->setRange(0.0f, 1.0f);
        }
        // Connect to finished signal to restart the animation
        connect(anim.Get(), &ICFAbstractAnimation::finished, this, [this]() {
            m_indeterminatePosition = 0.0f;
            startIndeterminateAnimation();
        });
        anim->start();
    }
}

void ProgressBar::stopIndeterminateAnimation() {
    m_indeterminateAnimating = false;
    m_indeterminatePosition = 0.0f;
}

void ProgressBar::updateIndeterminatePosition() {
    // This is called by the animation system
    update();
}

// ============================================================================
// Paint Event
// ============================================================================

void ProgressBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF track = trackRect();

    // Step 1: Draw background track
    drawBackground(p, track);

    // Step 2: Draw progress fill
    if (minimum() == 0 && maximum() == 0) {
        drawIndeterminate(p, track);
    } else {
        drawFill(p, track);
    }

    // Step 3: Draw text (if visible)
    if (isTextVisible()) {
        drawText(p, rect());
    }

    // Step 4: Draw focus indicator
    drawFocusIndicator(p, track);
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void ProgressBar::drawBackground(QPainter& p, const QRectF& rect) {
    CFColor bgColor = trackColor();
    QColor color = bgColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    QPainterPath shape = roundedRect(rect, cornerRadius());
    p.fillPath(shape, color);
}

void ProgressBar::drawFill(QPainter& p, const QRectF& trackRect) {
    CFColor fillColor = this->fillColor();
    QColor color = fillColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    // Handle state layer overlay
    if (m_stateMachine && isEnabled()) {
        float opacity = m_stateMachine->stateLayerOpacity();
        if (opacity > 0.0f) {
            CFColor stateColor = stateLayerColor();
            QColor stateQColor = stateColor.native_color();
            stateQColor.setAlphaF(opacity);

            // Blend state color with fill color
            int r = int(color.red() * (1.0f - opacity) + stateQColor.red() * opacity);
            int g = int(color.green() * (1.0f - opacity) + stateQColor.green() * opacity);
            int b = int(color.blue() * (1.0f - opacity) + stateQColor.blue() * opacity);
            color = QColor(r, g, b, color.alpha());
        }
    }

    // Calculate fill width based on value
    int minVal = minimum();
    int maxVal = maximum();
    int curVal = value();

    if (maxVal > minVal) {
        double ratio = static_cast<double>(curVal - minVal) / (maxVal - minVal);
        double fillWidth = trackRect.width() * ratio;

        QRectF fillRect = trackRect;
        fillRect.setWidth(fillWidth);

        QPainterPath shape = roundedRect(fillRect, cornerRadius());
        p.fillPath(shape, color);
    }
}

void ProgressBar::drawIndeterminate(QPainter& p, const QRectF& trackRect) {
    CFColor fillColor = this->fillColor();
    QColor color = fillColor.native_color();

    // Handle disabled state
    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    // Indeterminate animation: a segment that moves across the track
    // The segment is 1/3 of the track width
    double segmentWidth = trackRect.width() / 3.0;

    // Calculate position based on animation progress
    // The segment moves from -segmentWidth to trackRect.width()
    double xPos = -segmentWidth + (trackRect.width() + segmentWidth) * m_indeterminatePosition;

    QRectF fillRect = trackRect;
    fillRect.setX(xPos);
    fillRect.setWidth(segmentWidth);

    // Clip to track bounds
    QPainterPath clipPath = roundedRect(trackRect, cornerRadius());
    p.setClipPath(clipPath);

    QPainterPath shape = roundedRect(fillRect, cornerRadius());
    p.fillPath(shape, color);

    p.setClipping(false);
}

void ProgressBar::drawText(QPainter& p, const QRectF& rect) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    CFColor tColor = textColor();
    QColor color = tColor.native_color();

    if (!isEnabled()) {
        color.setAlphaF(0.38f);
    }

    p.setPen(color);
    p.setFont(font());

    // Draw text centered above the track
    QString text = this->text();
    if (text.isEmpty()) {
        int minVal = minimum();
        int maxVal = maximum();
        int curVal = value();

        if (maxVal > minVal) {
            int percentage = static_cast<int>((curVal - minVal) * 100 / (maxVal - minVal));
            text = QString("%1%").arg(percentage);
        }
    }

    QRectF textRect = rect.adjusted(0, 0, 0, -trackHeight() - helper.dpToPx(TEXT_TO_TRACK_GAP_DP));
    p.drawText(textRect, Qt::AlignCenter, text);
}

void ProgressBar::drawFocusIndicator(QPainter& p, const QRectF& rect) {
    if (m_focusIndicator) {
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float margin = helper.dpToPx(FOCUS_RING_MARGIN_DP);
        QRectF focusRect = rect.adjusted(-margin, -margin, margin, margin);

        QPainterPath shape = roundedRect(focusRect, cornerRadius() + margin);
        m_focusIndicator->paint(&p, shape, fillColor());
    }
}

} // namespace cf::ui::widget::material
