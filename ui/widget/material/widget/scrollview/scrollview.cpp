/**
 * @file scrollview.cpp
 * @brief Material Design 3 ScrollView Implementation
 *
 * Implements a Material Design 3 scroll area with custom scrollbars,
 * fade effects, and theme integration.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "scrollview.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "cfmaterial_animation_factory.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimerEvent>
#include <QWheelEvent>
#include <qtypes.h>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Internal ScrollbarOverlay Widget
// ============================================================================

/**
 * @brief Internal overlay widget for drawing custom scrollbars.
 * Defined in .cpp to keep implementation details hidden.
 * This overlay is transparent to mouse events - it only draws scrollbars.
 * Scrollbar interaction is handled by ScrollView's eventFilter.
 */
class ScrollbarOverlay : public QWidget {
  public:
    ScrollbarOverlay(QWidget* viewportParent, ScrollView* scrollView)
        : QWidget(viewportParent), m_scrollView(scrollView) {
        // Completely transparent to mouse events - let events pass through to content
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setAttribute(Qt::WA_NoSystemBackground);
    }

    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        if (!m_scrollView)
            return;

        m_scrollView->updateWidthAnimation();
        m_scrollView->updateOpacityAnimation();

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // Check both policy and maximum for vertical scrollbar
        auto vPolicy = m_scrollView->verticalScrollBarPolicy();
        bool shouldShowVertical =
            (vPolicy == Qt::ScrollBarAlwaysOn) ||
            (vPolicy != Qt::ScrollBarAlwaysOff && m_scrollView->verticalScrollBar()->maximum() > 0);

        // Check both policy and maximum for horizontal scrollbar
        auto hPolicy = m_scrollView->horizontalScrollBarPolicy();
        bool shouldShowHorizontal = (hPolicy == Qt::ScrollBarAlwaysOn) ||
                                    (hPolicy != Qt::ScrollBarAlwaysOff &&
                                     m_scrollView->horizontalScrollBar()->maximum() > 0);

        if (shouldShowVertical) {
            m_scrollView->drawVerticalScrollbar(p);
        }
        if (shouldShowHorizontal) {
            m_scrollView->drawHorizontalScrollbar(p);
        }
    }

    ScrollView* scrollView() const { return m_scrollView; }

  private:
    ScrollView* m_scrollView;
};

// ============================================================================
// Constructor / Destructor
// ============================================================================

ScrollView::ScrollView(QWidget* parent) : QScrollArea(parent) {
    // Get animation factory from Application
    m_animationFactory =
        aex::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    // Use AsNeeded as default policy for custom scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Explicitly hide native scrollbars
    horizontalScrollBar()->hide();
    verticalScrollBar()->hide();

    // Configure scroll step size
    horizontalScrollBar()->setSingleStep(60);
    verticalScrollBar()->setSingleStep(60);

    // Reserve space for custom scrollbars at right and bottom
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float scrollbarSpace = helper.dpToPx(20.0f);
    setViewportMargins(0, 0, int(scrollbarSpace), int(scrollbarSpace));

    // Enable mouse tracking on ScrollView
    setMouseTracking(true);
    viewport()->setMouseTracking(true);

    // Create overlay widget as child of viewport so it sits above
    // the scrolling content and receives mouse events correctly.
    m_overlay = new ScrollbarOverlay(viewport(), this);
    m_overlay->setStyleSheet("background: rgba(255,0,0,0.06);");
    m_overlay->setGeometry(viewport()->rect());
    m_overlay->raise();
    m_overlay->show();

    // Make sure we know when the viewport resizes/moves so we keep overlay aligned
    viewport()->installEventFilter(this);

    // Start animation timer for smooth transitions
    m_animationTimer.start();

    // Initialize width
    m_currentWidth = helper.dpToPx(12.0f);
    m_targetWidth = m_currentWidth;
}

ScrollView::~ScrollView() {
    stopFadeTimer();
}

// ============================================================================
// Event Handlers
// ============================================================================

void ScrollView::enterEvent(QEnterEvent* event) {
    QScrollArea::enterEvent(event);
    m_isHovering = true;
    showScrollbars();
}

void ScrollView::leaveEvent(QEvent* event) {
    QScrollArea::leaveEvent(event);
    m_isHovering = false;
    if (m_scrollbarFadeEnabled) {
        startFadeTimer();
    }
}

void ScrollView::resizeEvent(QResizeEvent* event) {
    QScrollArea::resizeEvent(event);
    if (m_overlay) {
        // Keep overlay exactly over the viewport
        m_overlay->setGeometry(viewport()->rect());
        m_overlay->raise();
    }
}

void ScrollView::scrollContentsBy(int dx, int dy) {
    QScrollArea::scrollContentsBy(dx, dy);

    // Mark as scrolling and show scrollbars
    m_isScrolling = (dx != 0 || dy != 0);
    showScrollbars();

    // Restart fade timer
    if (m_scrollbarFadeEnabled) {
        startFadeTimer();
    }
}

void ScrollView::changeEvent(QEvent* event) {
    QScrollArea::changeEvent(event);
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) {
        if (m_overlay)
            m_overlay->update(); // Redraw with new theme colors
    }
}

void ScrollView::mousePressEvent(QMouseEvent* event) {
    QScrollArea::mousePressEvent(event);
}

void ScrollView::mouseMoveEvent(QMouseEvent* event) {
    QScrollArea::mouseMoveEvent(event);
}

void ScrollView::mouseReleaseEvent(QMouseEvent* event) {
    QScrollArea::mouseReleaseEvent(event);
}

int ScrollView::deltaPosToScroll(int delta, float maxThumbTravel, int range) {
    if (maxThumbTravel <= 0 || range <= 0)
        return 0;
    return int(delta * range / maxThumbTravel);
}

void ScrollView::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_fadeTimerId) {
        stopFadeTimer();
        hideScrollbars();
    }
    QScrollArea::timerEvent(event);
}

bool ScrollView::eventFilter(QObject* obj, QEvent* ev) {
    if (obj == viewport()) {
        if (ev->type() == QEvent::Resize || ev->type() == QEvent::Move) {
            if (m_overlay) {
                m_overlay->setGeometry(viewport()->rect());
                m_overlay->raise();
                m_overlay->update();
            }
            return false;
        }

        // Handle mouse events for scrollbar interaction
        switch (ev->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove: {
                auto* mouseEvent = static_cast<QMouseEvent*>(ev);
                // Map to viewport coordinates (viewport is parent of overlay)
                QPoint pos = mouseEvent->pos();

                // Check if over scrollbar
                bool overScrollbar =
                    isPointOverVerticalThumb(pos) || isPointOverHorizontalThumb(pos) ||
                    isPointOverVerticalTrack(pos) || isPointOverHorizontalTrack(pos);

                // Always handle mouse events when dragging, otherwise only when over scrollbar
                if (m_isDraggingThumb || overScrollbar) {
                    handleScrollbarMouseEvent(mouseEvent, pos);
                    return true; // Eat the event
                }

                // Not over scrollbar - reset hover state if needed
                if (ev->type() == QEvent::MouseMove) {
                    bool wasHoveringVertical =
                        m_isHoveringVerticalThumb || m_isHoveringVerticalTrack;
                    bool wasHoveringHorizontal =
                        m_isHoveringHorizontalThumb || m_isHoveringHorizontalTrack;

                    m_isHoveringVerticalThumb = false;
                    m_isHoveringHorizontalThumb = false;
                    m_isHoveringVerticalTrack = false;
                    m_isHoveringHorizontalTrack = false;

                    if (wasHoveringVertical || wasHoveringHorizontal) {
                        setScrollbarState(ScrollbarState::Idle);
                        viewport()->setCursor(Qt::ArrowCursor);
                    }
                }
                break;
            }
            case QEvent::Leave: {
                // End drag if active
                if (m_isDraggingThumb) {
                    m_isDraggingThumb = false;
                    setScrollbarState(ScrollbarState::Idle);
                    viewport()->setCursor(Qt::ArrowCursor);
                }
                // Reset hover state
                bool wasHovering = m_isHoveringVerticalThumb || m_isHoveringHorizontalThumb ||
                                   m_isHoveringVerticalTrack || m_isHoveringHorizontalTrack;
                if (wasHovering) {
                    m_isHoveringVerticalThumb = false;
                    m_isHoveringHorizontalThumb = false;
                    m_isHoveringVerticalTrack = false;
                    m_isHoveringHorizontalTrack = false;
                    setScrollbarState(ScrollbarState::Idle);
                }
                break;
            }
            default:
                break;
        }
    }
    return QScrollArea::eventFilter(obj, ev);
}

// ============================================================================
// Scrollbar Mouse Event Handler
// ============================================================================

void ScrollView::handleScrollbarMouseEvent(QMouseEvent* event, const QPoint& pos) {
    switch (event->type()) {
        case QEvent::MouseButtonPress: {
            if (event->button() == Qt::LeftButton) {
                if (isPointOverVerticalThumb(pos)) {
                    m_isDraggingThumb = true;
                    m_dragOrientation = Qt::Vertical;
                    m_dragScrollValue = verticalScrollBar()->value();
                    m_dragStartPosition = pos.y();
                    setScrollbarState(ScrollbarState::Dragged);
                    return;
                }
                if (isPointOverHorizontalThumb(pos)) {
                    m_isDraggingThumb = true;
                    m_dragOrientation = Qt::Horizontal;
                    m_dragScrollValue = horizontalScrollBar()->value();
                    m_dragStartPosition = pos.x();
                    setScrollbarState(ScrollbarState::Dragged);
                    return;
                }
                if (isPointOverVerticalTrack(pos)) {
                    QRectF thumbRect = verticalScrollbarThumbRect();
                    if (pos.y() < thumbRect.top()) {
                        verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
                    } else if (pos.y() > thumbRect.bottom()) {
                        verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
                    }
                    return;
                }
                if (isPointOverHorizontalTrack(pos)) {
                    QRectF thumbRect = horizontalScrollbarThumbRect();
                    if (pos.x() < thumbRect.left()) {
                        horizontalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
                    } else if (pos.x() > thumbRect.right()) {
                        horizontalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
                    }
                    return;
                }
            }
            break;
        }
        case QEvent::MouseMove: {
            bool needsUpdate = false;

            if (m_isDraggingThumb) {
                if (m_dragOrientation == Qt::Vertical) {
                    int deltaY = pos.y() - m_dragStartPosition;
                    QRectF thumbRect = verticalScrollbarThumbRect();
                    QRectF trackRect = verticalScrollbarTrackRect();

                    float maxThumbTravel = trackRect.height() - thumbRect.height();
                    if (maxThumbTravel > 0) {
                        int range = verticalScrollBar()->maximum() - verticalScrollBar()->minimum();
                        int newValue = m_dragScrollValue +
                                       int(deltaPosToScroll(deltaY, maxThumbTravel, range));
                        verticalScrollBar()->setValue(newValue);
                    }
                } else {
                    int deltaX = pos.x() - m_dragStartPosition;
                    QRectF thumbRect = horizontalScrollbarThumbRect();
                    QRectF trackRect = horizontalScrollbarTrackRect();

                    float maxThumbTravel = trackRect.width() - thumbRect.width();
                    if (maxThumbTravel > 0) {
                        int range =
                            horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum();
                        int newValue = m_dragScrollValue +
                                       int(deltaPosToScroll(deltaX, maxThumbTravel, range));
                        horizontalScrollBar()->setValue(newValue);
                    }
                }
                return;
            }

            // Update hover state
            bool wasHoveringVertical = m_isHoveringVerticalThumb || m_isHoveringVerticalTrack;
            bool wasHoveringHorizontal = m_isHoveringHorizontalThumb || m_isHoveringHorizontalTrack;

            m_isHoveringVerticalThumb = isPointOverVerticalThumb(pos);
            m_isHoveringHorizontalThumb = isPointOverHorizontalThumb(pos);
            m_isHoveringVerticalTrack = isPointOverVerticalTrack(pos);
            m_isHoveringHorizontalTrack = isPointOverHorizontalTrack(pos);

            bool isHoveringVertical = m_isHoveringVerticalThumb || m_isHoveringVerticalTrack;
            bool isHoveringHorizontal = m_isHoveringHorizontalThumb || m_isHoveringHorizontalTrack;

            if ((wasHoveringVertical != isHoveringVertical) ||
                (wasHoveringHorizontal != isHoveringHorizontal)) {
                needsUpdate = true;
            }

            if (m_isHoveringVerticalThumb || m_isHoveringHorizontalThumb) {
                viewport()->setCursor(Qt::PointingHandCursor);
            } else {
                viewport()->setCursor(Qt::ArrowCursor);
            }

            if (m_scrollbarHoverExpansion) {
                CanvasUnitHelper helper(qApp->devicePixelRatio());
                if (isHoveringVertical || isHoveringHorizontal) {
                    m_targetWidth = helper.dpToPx(16.0f);
                } else {
                    m_targetWidth = helper.dpToPx(12.0f);
                }
                needsUpdate = true;
            }

            if (needsUpdate) {
                updateWidthAnimation();
            }
            break;
        }
        case QEvent::MouseButtonRelease: {
            if (m_isDraggingThumb && event->button() == Qt::LeftButton) {
                m_isDraggingThumb = false;

                // Update hover state based on current mouse position
                m_isHoveringVerticalThumb = isPointOverVerticalThumb(pos);
                m_isHoveringHorizontalThumb = isPointOverHorizontalThumb(pos);
                m_isHoveringVerticalTrack = isPointOverVerticalTrack(pos);
                m_isHoveringHorizontalTrack = isPointOverHorizontalTrack(pos);

                bool isOver = m_isHoveringVerticalThumb || m_isHoveringHorizontalThumb ||
                              m_isHoveringVerticalTrack || m_isHoveringHorizontalTrack;

                if (isOver) {
                    setScrollbarState(ScrollbarState::Hovered);
                    viewport()->setCursor(Qt::PointingHandCursor);
                } else {
                    setScrollbarState(ScrollbarState::Idle);
                    viewport()->setCursor(Qt::ArrowCursor);
                }

                // Ensure animation update happens after state change
                updateWidthAnimation();
                if (m_overlay)
                    m_overlay->update();
            }
            break;
        }
        default:
            break;
    }
}

// ============================================================================
// Paint Event
// ============================================================================

void ScrollView::paintEvent(QPaintEvent* event) {
    // Base class draws viewport contents
    QScrollArea::paintEvent(event);
    // Scrollbars are drawn by the overlay widget
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void ScrollView::drawVerticalScrollbar(QPainter& p) {
    QRectF trackRect = verticalScrollbarTrackRect();
    drawScrollbarTrack(p, trackRect, Qt::Vertical);

    QRectF thumbRect = verticalScrollbarThumbRect();
    if (!thumbRect.isEmpty()) {
        drawScrollbarThumb(p, thumbRect, Qt::Vertical);
    }
}

void ScrollView::drawHorizontalScrollbar(QPainter& p) {
    QRectF trackRect = horizontalScrollbarTrackRect();
    drawScrollbarTrack(p, trackRect, Qt::Horizontal);

    QRectF thumbRect = horizontalScrollbarThumbRect();
    if (!thumbRect.isEmpty()) {
        drawScrollbarThumb(p, thumbRect, Qt::Horizontal);
    }
}

void ScrollView::drawScrollbarTrack(QPainter& p, const QRectF& trackRect,
                                    Qt::Orientation orientation) {
    Q_UNUSED(orientation)

    CFColor trackColor = scrollbarTrackColor();
    QColor color = trackColor.native_color();
    color.setAlphaF(color.alphaF() * 0.5f); // Semi-transparent

    QPainterPath path = roundedRect(trackRect, scrollBarCornerRadius());
    p.fillPath(path, color);
}

void ScrollView::drawScrollbarThumb(QPainter& p, const QRectF& thumbRect,
                                    Qt::Orientation orientation) {
    Q_UNUSED(orientation)

    if (thumbRect.isEmpty())
        return;

    // Get current state
    ScrollbarState state = scrollbarState();
    float opacity = m_scrollbarOpacity; // Use animated opacity value, not the binary function

    // Draw state layer for hovered/dragged states
    if (state == ScrollbarState::Hovered || state == ScrollbarState::Dragged) {
        float stateLayerOpacity = (state == ScrollbarState::Dragged) ? 0.12f : 0.08f;
        drawStateLayer(p, thumbRect, scrollbarStateLayerColor().native_color(), stateLayerOpacity);
    }

    // Draw main thumb with Material Design colors
    CFColor thumbColor = scrollbarThumbColor();
    QColor color = thumbColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);

    QPainterPath path = roundedRect(thumbRect, scrollBarCornerRadius());
    p.fillPath(path, color);

    // Draw subtle inner highlight for dragged state
    if (state == ScrollbarState::Dragged) {
        QColor highlightColor(255, 255, 255, 30);
        QPainterPath highlightPath =
            roundedRect(thumbRect.adjusted(1, 1, -1, -1), scrollBarCornerRadius() * 0.8f);
        p.fillPath(highlightPath, highlightColor);
    }
}

// ============================================================================
// Scrollbar Geometry
// ============================================================================

QRectF ScrollView::verticalScrollbarTrackRect() const {
    float w = scrollBarWidth();
    float h = scrollBarWidth(); // Same width for horizontal scrollbar
    // Anchor to right edge of viewport, leave space for horizontal scrollbar at bottom
    return QRectF(viewport()->width() - w, 0, w, viewport()->height() - h);
}

QRectF ScrollView::verticalScrollbarThumbRect() const {
    QScrollBar* scrollbar = verticalScrollBar();
    if (!scrollbar || scrollbar->maximum() <= 0) {
        return QRectF();
    }

    QRectF trackRect = verticalScrollbarTrackRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Calculate thumb position based on scroll range
    int range = scrollbar->maximum() - scrollbar->minimum();
    if (range <= 0) {
        return QRectF();
    }

    float value = scrollbar->value() - scrollbar->minimum();
    float pageRatio = float(scrollbar->pageStep()) / float(range + scrollbar->pageStep());

    float thumbHeight = std::max(qreal(helper.dpToPx(24.0f)), trackRect.height() * pageRatio);
    float maxThumbY = trackRect.height() - thumbHeight;
    float thumbY = trackRect.top() + (value / range) * maxThumbY;

    return QRectF(trackRect.left(), thumbY, trackRect.width(), thumbHeight);
}

QRectF ScrollView::horizontalScrollbarTrackRect() const {
    float w = scrollBarWidth();
    float h = scrollBarWidth();
    // Anchor to bottom edge of viewport, leave space for vertical scrollbar at right
    return QRectF(0, viewport()->height() - h, viewport()->width() - w, h);
}

QRectF ScrollView::horizontalScrollbarThumbRect() const {
    QScrollBar* scrollbar = horizontalScrollBar();
    if (!scrollbar || scrollbar->maximum() <= 0) {
        return QRectF();
    }

    QRectF trackRect = horizontalScrollbarTrackRect();
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Calculate thumb position based on scroll range
    int range = scrollbar->maximum() - scrollbar->minimum();
    if (range <= 0) {
        return QRectF();
    }

    float value = scrollbar->value() - scrollbar->minimum();
    float pageRatio = float(scrollbar->pageStep()) / float(range + scrollbar->pageStep());

    float thumbWidth = std::max(qreal(helper.dpToPx(24.0f)), trackRect.width() * pageRatio);
    float maxThumbX = trackRect.width() - thumbWidth;
    float thumbX = trackRect.left() + (value / range) * maxThumbX;

    return QRectF(thumbX, trackRect.top(), thumbWidth, trackRect.height());
}

// ============================================================================
// Color Access Methods
// ============================================================================

namespace {
// Fallback colors when theme is not available
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
} // Surface
inline CFColor fallbackSurfaceVariant() {
    return CFColor(231, 224, 236);
} // Surface Variant
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(49, 48, 51);
} // On Surface Variant
} // namespace

CFColor ScrollView::backgroundColor() const {
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

CFColor ScrollView::scrollbarTrackColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackSurfaceVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(SURFACE_VARIANT));
    } catch (...) {
        return fallbackSurfaceVariant();
    }
}

CFColor ScrollView::scrollbarThumbColor() const {
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

// ============================================================================
// Unit Conversion Helpers
// ============================================================================

float ScrollView::scrollBarWidth() const {
    // Use current animated width for smooth transitions
    return m_currentWidth;
}

float ScrollView::scrollBarMargin() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(4.0f); // 4dp margin
}

float ScrollView::scrollBarCornerRadius() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(8.0f); // 8dp corner radius
}

float ScrollView::scrollBarOpacity() const {
    // Return the animated opacity value for any external consumers
    return m_scrollbarOpacity;
}

// ============================================================================
// Fade Effect Helpers
// ============================================================================

void ScrollView::showScrollbars() {
    // Set target opacity and let animation handle the transition
    m_targetOpacity = 1.0f;
    if (m_overlay)
        m_overlay->update();
}

void ScrollView::hideScrollbars() {
    // Set target opacity and let animation handle the transition
    if (!m_isHovering && !m_isScrolling) {
        m_targetOpacity = 0.4f;
        if (m_overlay)
            m_overlay->update();
    }
}

void ScrollView::startFadeTimer() {
    stopFadeTimer();
    m_fadeTimerId = startTimer(m_scrollbarFadeDelay);
}

void ScrollView::stopFadeTimer() {
    if (m_fadeTimerId != 0) {
        killTimer(m_fadeTimerId);
        m_fadeTimerId = 0;
    }
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

bool ScrollView::scrollbarFadeEnabled() const {
    return m_scrollbarFadeEnabled;
}

void ScrollView::setScrollbarFadeEnabled(bool enabled) {
    if (m_scrollbarFadeEnabled != enabled) {
        m_scrollbarFadeEnabled = enabled;
        if (!enabled) {
            stopFadeTimer();
            showScrollbars();
        }
        if (m_overlay)
            m_overlay->update();
    }
}

int ScrollView::scrollbarFadeDelay() const {
    return m_scrollbarFadeDelay;
}

void ScrollView::setScrollbarFadeDelay(int delay) {
    if (m_scrollbarFadeDelay != delay && delay >= 0) {
        m_scrollbarFadeDelay = delay;
    }
}

// ============================================================================
// Size Hint
// ============================================================================

QSize ScrollView::sizeHint() const {
    QSize hint = QScrollArea::sizeHint();

    // Add space for scrollbars
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float scrollbarWidth = scrollBarWidth() + scrollBarMargin() * 2;

    // Only add scrollbar width if we expect scrollbars to be visible
    if (verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff) {
        hint.rwidth() += int(std::ceil(scrollbarWidth));
    }
    if (horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff) {
        hint.rheight() += int(std::ceil(scrollbarWidth));
    }

    return hint;
}

// ============================================================================
// New Material Design 3 Methods
// ============================================================================

void ScrollView::drawStateLayer(QPainter& p, const QRectF& rect, const QColor& color,
                                float opacity) {
    if (rect.isEmpty() || opacity <= 0.0f)
        return;

    QColor stateColor = color;
    stateColor.setAlphaF(opacity);

    QPainterPath path = roundedRect(rect, scrollBarCornerRadius());
    p.fillPath(path, stateColor);
}

CFColor ScrollView::scrollbarStateLayerColor() const {
    // Use primary color for state layer, or fall back to on surface variant
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurfaceVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(PRIMARY));
    } catch (...) {
        return fallbackOnSurfaceVariant();
    }
}

float ScrollView::scrollBarExpandedWidth() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(16.0f); // 16dp when hovered per Material Design
}

float ScrollView::currentScrollBarWidth() const {
    return m_currentWidth;
}

ScrollbarState ScrollView::scrollbarState() const {
    if (m_isDraggingThumb) {
        return ScrollbarState::Dragged;
    }
    if (m_isHoveringVerticalThumb || m_isHoveringHorizontalThumb || m_isHoveringVerticalTrack ||
        m_isHoveringHorizontalTrack) {
        return ScrollbarState::Hovered;
    }
    return ScrollbarState::Idle;
}

void ScrollView::setScrollbarState(ScrollbarState state) {
    // Update opacity target based on state
    switch (state) {
        case ScrollbarState::Idle:
            m_targetOpacity = 0.4f;
            if (m_scrollbarHoverExpansion) {
                CanvasUnitHelper helper(qApp->devicePixelRatio());
                m_targetWidth = helper.dpToPx(12.0f);
            }
            break;
        case ScrollbarState::Hovered:
        case ScrollbarState::Dragged:
            m_targetOpacity = 1.0f;
            if (m_scrollbarHoverExpansion) {
                CanvasUnitHelper helper(qApp->devicePixelRatio());
                m_targetWidth = helper.dpToPx(16.0f);
            }
            break;
    }
    if (m_overlay)
        m_overlay->update();
}

bool ScrollView::isPointOverVerticalThumb(const QPoint& pos) const {
    if (verticalScrollBar()->maximum() <= 0) {
        return false;
    }
    QRectF thumbRect = verticalScrollbarThumbRect();
    // Add some padding for easier interaction
    QRectF hitRect = thumbRect.adjusted(-4, -4, 4, 4);
    return hitRect.contains(pos);
}

bool ScrollView::isPointOverHorizontalThumb(const QPoint& pos) const {
    if (horizontalScrollBar()->maximum() <= 0) {
        return false;
    }
    QRectF thumbRect = horizontalScrollbarThumbRect();
    // Add some padding for easier interaction
    QRectF hitRect = thumbRect.adjusted(-4, -4, 4, 4);
    return hitRect.contains(pos);
}

bool ScrollView::isPointOverVerticalTrack(const QPoint& pos) const {
    if (verticalScrollBar()->maximum() <= 0) {
        return false;
    }
    QRectF trackRect = verticalScrollbarTrackRect();
    return trackRect.contains(pos);
}

bool ScrollView::isPointOverHorizontalTrack(const QPoint& pos) const {
    if (horizontalScrollBar()->maximum() <= 0) {
        return false;
    }
    QRectF trackRect = horizontalScrollbarTrackRect();
    return trackRect.contains(pos);
}

void ScrollView::updateWidthAnimation() {
    // Smoothly interpolate current width towards target
    float diff = m_targetWidth - m_currentWidth;
    if (std::abs(diff) > 0.5f) {
        m_currentWidth += diff * ANIMATION_SPEED_WIDTH;
        if (m_overlay)
            m_overlay->update();
    } else {
        m_currentWidth = m_targetWidth;
    }
}

void ScrollView::updateOpacityAnimation() {
    // Smoothly interpolate current opacity towards target
    float diff = m_targetOpacity - m_scrollbarOpacity;
    if (std::abs(diff) > 0.01f) {
        m_scrollbarOpacity += diff * ANIMATION_SPEED_OPACITY;
        if (m_overlay)
            m_overlay->update();
    } else {
        m_scrollbarOpacity = m_targetOpacity;
    }
}

bool ScrollView::scrollbarHoverExpansion() const {
    return m_scrollbarHoverExpansion;
}

void ScrollView::setScrollbarHoverExpansion(bool enabled) {
    if (m_scrollbarHoverExpansion != enabled) {
        m_scrollbarHoverExpansion = enabled;
        if (m_overlay)
            m_overlay->update();
    }
}

} // namespace cf::ui::widget::material
