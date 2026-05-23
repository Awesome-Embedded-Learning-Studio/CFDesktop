/**
 * @file treeview.cpp
 * @brief Material Design 3 TreeView Implementation
 *
 * Implements a Material Design 3 tree view with hierarchical data display.
 * Features expand/collapse animations, tree connection lines, proper
 * indentation, and Material Design color tokens.
 *
 * Rendering is delegated to TreeViewItemDelegate following Qt's Model/View
 * architecture. This class handles events, state management, and coordinates
 * between the view and delegate.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "treeview.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "private/treeviewdelegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Fallback Colors
// ============================================================================

namespace {
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
}
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
}
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

TreeView::TreeView(QWidget* parent)
    : QTreeView(parent), m_material(this,
                                    MaterialWidgetBase::Config{
                                        .useRipple = true,
                                        .useElevation = false,
                                        .useFocusIndicator = true,
                                    }),
      m_itemHeight(TreeItemHeight::Standard), m_indentStyle(TreeIndentStyle::Material),
      m_showTreeLines(true), m_rootIsDecorated(true) {
    // Initialize and set the delegate for item rendering
    m_delegate = new TreeViewItemDelegate(this);
    setItemDelegate(m_delegate);

    // Configure tree view for Material appearance
    setHeaderHidden(true);
    // Disable Qt's default branch decoration to avoid duplicate arrow rendering
    // Our delegate handles drawing expand/collapse icons
    // IMPORTANT: Call parent class method directly to bypass our override
    QTreeView::setRootIsDecorated(false);
    setIndentation(static_cast<int>(std::ceil(m_delegate->indentPerLevel())));
    setUniformRowHeights(true);

    // Expose root decorated state to delegate via property
    setProperty("_q_rootIsDecorated", m_rootIsDecorated);

    // Disable default frame
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);

    // Enable mouse tracking for hover effects
    viewport()->setMouseTracking(true);

    // Set viewport margins for padding
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float vPadding = helper.dpToPx(8.0f);
    setViewportMargins(0, static_cast<int>(vPadding), 0, static_cast<int>(vPadding));

    // Set item height through delegate
    setItemHeight(m_itemHeight);
}

TreeView::~TreeView() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

TreeItemHeight TreeView::itemHeight() const {
    return m_itemHeight;
}

void TreeView::setItemHeight(TreeItemHeight height) {
    if (m_itemHeight != height) {
        m_itemHeight = height;
        // Update delegate with new height
        if (m_delegate) {
            m_delegate->setItemHeight(height);
        }
        setUniformRowHeights(true);
        doItemsLayout();
        update();
    }
}

TreeIndentStyle TreeView::indentStyle() const {
    return m_indentStyle;
}

void TreeView::setIndentStyle(TreeIndentStyle style) {
    if (m_indentStyle != style) {
        m_indentStyle = style;
        if (m_delegate) {
            m_delegate->setIndentStyle(style);
        }
        setIndentation(static_cast<int>(std::ceil(m_delegate->indentPerLevel())));
        update();
    }
}

bool TreeView::showTreeLines() const {
    return m_showTreeLines;
}

void TreeView::setShowTreeLines(bool show) {
    if (m_showTreeLines != show) {
        m_showTreeLines = show;
        if (m_delegate) {
            m_delegate->setShowTreeLines(show);
        }
        update();
    }
}

bool TreeView::rootIsDecorated() const {
    return m_rootIsDecorated;
}

void TreeView::setRootIsDecorated(bool decorated) {
    if (m_rootIsDecorated != decorated) {
        m_rootIsDecorated = decorated;
        // Always keep Qt's default disabled - our delegate handles drawing icons
        // This must be called to prevent Qt from reserving space for branch indicators
        QTreeView::setRootIsDecorated(false);
        // Expose state to delegate via property
        setProperty("_q_rootIsDecorated", m_rootIsDecorated);
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize TreeView::sizeHint() const {
    // Minimum visible area: show at least 3 items
    float itemH = itemHeightValue();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float minWidth = helper.dpToPx(200.0f);
    float minHeight = itemH * 3.0f;

    return QSize(static_cast<int>(std::ceil(minWidth)), static_cast<int>(std::ceil(minHeight)));
}

QSize TreeView::minimumSizeHint() const {
    // Show at least 2 items
    float itemH = itemHeightValue();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float minWidth = helper.dpToPx(150.0f);
    float minHeight = itemH * 2.0f;

    return QSize(static_cast<int>(std::ceil(minWidth)), static_cast<int>(std::ceil(minHeight)));
}

// ============================================================================
// Layout Helpers
// ============================================================================

float TreeView::itemHeightValue() const {
    return m_delegate ? m_delegate->itemHeightValue() : 56.0f;
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor TreeView::surfaceColor() const {
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

CFColor TreeView::onSurfaceColor() const {
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

// ============================================================================
// Event Handlers
// ============================================================================

void TreeView::enterEvent(QEnterEvent* event) {
    QTreeView::enterEvent(event);
    m_material.onEnterEvent();
    // Initialize hovered index on enter
    QPoint localPos = viewport()->mapFromGlobal(QCursor::pos());
    m_hoveredIndex = indexAt(localPos);
    setProperty("_q_hoveredIndex", QVariant::fromValue(m_hoveredIndex));
    update();
}

void TreeView::leaveEvent(QEvent* event) {
    QTreeView::leaveEvent(event);
    m_material.onLeaveEvent();
    // Clear hovered index
    if (m_hoveredIndex.isValid()) {
        QPersistentModelIndex oldIndex = m_hoveredIndex;
        m_hoveredIndex = QPersistentModelIndex();
        setProperty("_q_hoveredIndex", QVariant());
        update(oldIndex);
    }
}

void TreeView::mouseMoveEvent(QMouseEvent* event) {
    QTreeView::mouseMoveEvent(event);

    // Map to viewport coordinates
    QPoint viewportPos = viewport()->mapFromParent(event->pos());

    // Update hovered index
    QModelIndex newIndex = indexAt(viewportPos);
    if (newIndex != m_hoveredIndex) {
        // Store old index for update
        QPersistentModelIndex oldIndex = m_hoveredIndex;
        m_hoveredIndex = newIndex;

        // Set property for delegate to access
        setProperty("_q_hoveredIndex", QVariant::fromValue(m_hoveredIndex));

        // Update both old and new regions
        if (oldIndex.isValid()) {
            update(oldIndex);
        }
        if (m_hoveredIndex.isValid()) {
            update(m_hoveredIndex);
        }
    }
}

void TreeView::mousePressEvent(QMouseEvent* event) {
    // Map to viewport coordinates for hit testing
    QPoint viewportPos = viewport()->mapFromParent(event->pos());
    QModelIndex index = indexAt(viewportPos);

    // Check if click is on expand/collapse icon
    if (index.isValid() && model() && model()->hasChildren(index)) {
        // Replicate the delegate's expand icon position calculation
        QRect itemRect = visualRect(index); // viewport coordinates
        float hPadding = m_delegate->horizontalPaddingValue();
        float iconSz = m_delegate->iconSizeValue();
        float indent = m_delegate->indentPerLevel();

        // Calculate depth by walking up the model hierarchy
        int depth = 0;
        QModelIndex current = index;
        while (current.parent().isValid()) {
            current = current.parent();
            depth++;
        }

        // Calculate expand icon rect (must match delegate's paint logic)
        QRectF expandRect(hPadding + (indent * depth), itemRect.center().y() - iconSz / 2.0f,
                          iconSz, iconSz);

        if (expandRect.contains(viewportPos)) {
            // Click is on expand/collapse icon - manually toggle expansion
            if (isExpanded(index)) {
                collapse(index);
            } else {
                expand(index);
            }

            // Still handle ripple/state effects
            if (m_material.stateMachine()) {
                m_material.stateMachine()->onPress(event->pos());
            }
            if (m_material.ripple()) {
                // Use viewport coordinates for ripple (painter draws on viewport)
                m_material.ripple()->onPress(viewportPos, QRectF(itemRect));
                // Store clip rect for ripple animation
                m_rippleClipRect = QRectF(itemRect);
            }
            m_pressedIndex = index;
            update();
            return; // Don't call base class - avoid Qt's incorrect hit detection
        }
    }

    // Non-icon area: use normal flow (selection, ripple, etc.)
    QTreeView::mousePressEvent(event);

    if (m_material.stateMachine()) {
        m_material.stateMachine()->onPress(event->pos());
    }

    // Track pressed index for ripple
    QModelIndex pressedIndex = indexAt(viewportPos);
    if (pressedIndex.isValid()) {
        m_pressedIndex = pressedIndex;
        m_pressPosition = event->pos();

        if (m_material.ripple()) {
            QRect itemRect = visualRect(pressedIndex);
            // Use viewport coordinates for ripple (painter draws on viewport)
            m_material.ripple()->onPress(viewportPos, QRectF(itemRect));
            // Store clip rect for ripple animation (so it continues after release)
            m_rippleClipRect = QRectF(itemRect);
        }
    }
    update();
}

void TreeView::mouseReleaseEvent(QMouseEvent* event) {
    QTreeView::mouseReleaseEvent(event);

    if (m_material.stateMachine()) {
        m_material.stateMachine()->onRelease();
    }
    if (m_material.ripple()) {
        m_material.ripple()->onRelease();
    }
    m_pressedIndex = QPersistentModelIndex();
    update();
}

void TreeView::focusInEvent(QFocusEvent* event) {
    QTreeView::focusInEvent(event);
    m_material.onFocusIn();
}

void TreeView::focusOutEvent(QFocusEvent* event) {
    QTreeView::focusOutEvent(event);
    m_material.onFocusOut();
}

void TreeView::changeEvent(QEvent* event) {
    QTreeView::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

void TreeView::rowsInserted(const QModelIndex& parent, int start, int end) {
    QTreeView::rowsInserted(parent, start, end);
    // Ensure layout is updated after model changes
    doItemsLayout();
}

// ============================================================================
// Draw Branches Override
// ============================================================================

void TreeView::drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const {
    // Completely disable Qt's default branch drawing
    // Our delegate handles all expand/collapse icon rendering
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    Q_UNUSED(index);
}

// ============================================================================
// Paint Event
// ============================================================================

void TreeView::paintEvent(QPaintEvent* event) {
    // Draw viewport background
    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(viewport()->rect(), surfaceColor().native_color());

    // Let Qt handle tree rendering via delegate
    // This calls the delegate's paint() method for each visible item
    QTreeView::paintEvent(event);

    // Draw ripple if active (overlay on top of items)
    // RippleHelper manages its own state - always try to paint
    if (m_material.ripple() && !m_rippleClipRect.isEmpty()) {
        m_material.ripple()->setColor(onSurfaceColor());
        QPainterPath clipPath;
        clipPath.addRect(m_rippleClipRect);
        p.save();
        p.setClipPath(clipPath);
        m_material.ripple()->paint(&p, clipPath);
        p.restore();

        // Clear clip rect when ripple is no longer active
        if (!m_material.ripple()->hasActiveRipple()) {
            m_rippleClipRect = QRectF();
        }
    }

    // Draw focus indicator on current index if focused
    if (m_material.focusIndicator() && hasFocus()) {
        QModelIndex current = currentIndex();
        if (current.isValid()) {
            QRect itemRect = visualRect(current);
            if (itemRect.intersects(viewport()->rect())) {
                QPainterPath shape;
                shape.addRect(itemRect);
                m_material.focusIndicator()->paint(&p, shape, onSurfaceColor());
            }
        }
    }
}

} // namespace cf::ui::widget::material
