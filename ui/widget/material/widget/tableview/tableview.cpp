/**
 * @file tableview.cpp
 * @brief Material Design 3 TableView Implementation
 *
 * Implements a Material Design 3 table view with custom header rendering,
 * grid lines, row selection effects, and Material Design color tokens.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "tableview.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "cfmaterial_animation_factory.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "widget/material/base/focus_ring.h"
#include "widget/material/base/ripple_helper.h"
#include "widget/material/base/state_machine.h"

#include <QApplication>
#include <QFontMetrics>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::components;
using namespace cf::ui::components::material;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::material::base;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Constants
// ============================================================================

namespace {
constexpr float kHeaderHeightDp = 48.0f;
constexpr float kRowHeightCompactDp = 48.0f;
constexpr float kRowHeightStandardDp = 56.0f;
constexpr float kSelectedRowAlpha = 0.12f; // 12% opacity
constexpr float kHoverAlpha = 0.08f;       // 8% opacity

// Fallback colors
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
}
inline CFColor fallbackOutlineVariant() {
    return CFColor(188, 188, 188);
}
inline CFColor fallbackPrimaryContainer() {
    return CFColor(234, 221, 255);
}
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

TableView::TableView(QWidget* parent)
    : QTableView(parent), rowHeight_(TableRowHeight::Standard),
      gridStyle_(TableGridStyle::Horizontal), showHeader_(true), alternatingRowColors_(false),
      rippleEnabled_(true), m_hasValidPressPos(false), m_hoveredRow(-1), m_pressedRow(-1) {
    // Get animation factory from Application
    m_animationFactory =
        cf::WeakPtr<CFMaterialAnimationFactory>::DynamicCast(Application::animationFactory());

    // Initialize behavior components
    m_stateMachine = new StateMachine(m_animationFactory, this);
    m_ripple = new RippleHelper(m_animationFactory, this);
    m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);

    // Set ripple mode to bounded for table cells
    m_ripple->setMode(RippleHelper::Mode::Bounded);

    // Connect repaint signals
    connect(m_ripple, &RippleHelper::repaintNeeded, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));

    // Configure default QTableView properties for Material rendering
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);

    // Disable default Qt styling - we handle all rendering
    setStyleSheet("QTableView { border: none; background: transparent; }");

    // Configure header
    auto* header = horizontalHeader();
    if (header) {
        header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        header->setStretchLastSection(false);
        header->setSectionsMovable(true);
        header->setSectionsClickable(true);
        header->setSortIndicatorShown(true);
    }

    auto* vHeader = verticalHeader();
    if (vHeader) {
        vHeader->setVisible(false);
        vHeader->setDefaultSectionSize(static_cast<int>(rowHeightValue()));
    }

    // Enable alternating row colors by default
    QTableView::setAlternatingRowColors(alternatingRowColors_);
}

TableView::~TableView() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

TableRowHeight TableView::rowHeight() const {
    return rowHeight_;
}

void TableView::setRowHeight(TableRowHeight height) {
    if (rowHeight_ != height) {
        rowHeight_ = height;
        auto* vHeader = verticalHeader();
        if (vHeader) {
            vHeader->setDefaultSectionSize(static_cast<int>(rowHeightValue()));
        }
        updateGeometry();
        update();
    }
}

TableGridStyle TableView::gridStyle() const {
    return gridStyle_;
}

void TableView::setGridStyle(TableGridStyle style) {
    if (gridStyle_ != style) {
        gridStyle_ = style;
        update();
    }
}

bool TableView::showHeader() const {
    return showHeader_;
}

void TableView::setShowHeader(bool show) {
    if (showHeader_ != show) {
        showHeader_ = show;
        auto* header = horizontalHeader();
        if (header) {
            header->setVisible(show);
        }
        updateGeometry();
        update();
    }
}

bool TableView::alternatingRowColors() const {
    return alternatingRowColors_;
}

void TableView::setAlternatingRowColors(bool enabled) {
    if (alternatingRowColors_ != enabled) {
        alternatingRowColors_ = enabled;
        // Also set the base class property for internal state tracking
        QTableView::setAlternatingRowColors(enabled);
        update();
    }
}

bool TableView::rippleEnabled() const {
    return rippleEnabled_;
}

void TableView::setRippleEnabled(bool enabled) {
    if (rippleEnabled_ != enabled) {
        rippleEnabled_ = enabled;
        if (!enabled && m_ripple) {
            m_ripple->onCancel();
        }
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize TableView::sizeHint() const {
    // Calculate size based on content
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    int rowCount = model() ? model()->rowCount() : 0;
    int columnCount = model() ? model()->columnCount() : 0;

    float width = 0;
    auto* hHeader = horizontalHeader();
    if (hHeader && columnCount > 0) {
        for (int i = 0; i < columnCount; ++i) {
            width += hHeader->sectionSize(i);
        }
    }
    if (width < helper.dpToPx(200.0f)) {
        width = helper.dpToPx(200.0f);
    }

    float height = 0;
    if (showHeader_ && hHeader) {
        height += headerHeightValue();
    }
    if (rowCount > 0) {
        height += rowCount * rowHeightValue();
    } else {
        // Show at least 5 rows for empty tables
        height += 5 * rowHeightValue();
    }

    return QSize(int(std::ceil(width)), int(std::ceil(height)));
}

QSize TableView::minimumSizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Minimum width for 2 columns
    float minWidth = helper.dpToPx(200.0f);

    // Minimum height for header + 3 rows
    float minHeight = headerHeightValue() + 3 * rowHeightValue();

    return QSize(int(std::ceil(minWidth)), int(std::ceil(minHeight)));
}

// ============================================================================
// Event Handlers
// ============================================================================

void TableView::paintEvent(QPaintEvent* event) {
    // First, let Qt draw the base table content (cells, selection, etc.)
    // This uses the item delegate to render each cell
    QTableView::paintEvent(event);

    // Then, overlay our Material Design custom rendering on the viewport
    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing);

    QRectF viewportRect = viewport()->rect();

    // Draw hover state overlay (semi-transparent)
    if (model() && m_hoveredRow >= 0) {
        QRect rowRect = visualRect(model()->index(m_hoveredRow, 0));
        if (rowRect.intersects(viewportRect.toRect())) {
            rowRect.setLeft(0);
            rowRect.setWidth(viewportRect.width());
            CFColor hoverColor = onSurfaceColor();
            QColor color = hoverColor.native_color();
            color.setAlphaF(kHoverAlpha);
            p.fillRect(rowRect, color);
        }
    }

    // Draw ripple effects
    if (rippleEnabled_ && m_ripple && (m_pressedRow >= 0 || m_hoveredRow >= 0)) {
        int row = (m_pressedRow >= 0) ? m_pressedRow : m_hoveredRow;
        if (row >= 0) {
            QRect rowRect = visualRect(model()->index(row, 0));
            rowRect.setLeft(0);
            rowRect.setWidth(viewportRect.width());
            QPainterPath clipPath;
            clipPath.addRect(rowRect);
            m_ripple->setColor(onSurfaceColor());
            m_ripple->paint(&p, clipPath);
        }
    }

    // Draw grid lines
    if (gridStyle_ != TableGridStyle::None) {
        drawGridLines(p, viewportRect);
    }

    // Draw focus indicator around the actual content area
    if (hasFocus() && m_focusIndicator && model()) {
        // Calculate actual content bounds
        QRectF contentRect = viewportRect;

        // Get the actual row and column count
        int rowCount = model()->rowCount();
        int columnCount = model()->columnCount();

        if (rowCount > 0 && columnCount > 0) {
            // Get bottom-right corner of the last visible cell
            QModelIndex lastIndex = model()->index(rowCount - 1, columnCount - 1);
            QRect lastCellRect = visualRect(lastIndex);
            if (lastCellRect.isValid()) {
                qreal contentWidth = static_cast<qreal>(lastCellRect.right() + 1);
                qreal contentHeight = static_cast<qreal>(lastCellRect.bottom() + 1);
                contentRect.setWidth(qMin(contentWidth, viewportRect.width()));
                contentRect.setHeight(qMin(contentHeight, viewportRect.height()));
            }
        }

        drawFocusIndicator(p, contentRect);
    }
}

void TableView::mousePressEvent(QMouseEvent* event) {
    QTableView::mousePressEvent(event);

    if (event->button() == Qt::LeftButton) {
        m_pressPos = event->pos();
        m_hasValidPressPos = true;

        // Convert to viewport coordinates: event->pos() is in view coords, indexAt() needs viewport
        // coords
        QPoint viewportPos = viewport()->mapFrom(this, event->pos());
        QModelIndex index = indexAt(viewportPos);
        int row = index.row();
        if (row >= 0) {
            m_pressedRow = row;
            if (m_stateMachine) {
                m_stateMachine->onPress(event->pos());
            }
            if (m_ripple && rippleEnabled_) {
                QRect rowRect = visualRect(model()->index(row, 0));
                rowRect.setLeft(0);
                rowRect.setWidth(viewport()->width());
                // Ripple expects viewport coordinates since rowRect is in viewport coords
                m_ripple->onPress(viewportPos, rowRect);
            }
            update();
        }
    }
}

void TableView::mouseReleaseEvent(QMouseEvent* event) {
    QTableView::mouseReleaseEvent(event);

    if (m_pressedRow >= 0) {
        if (m_stateMachine) {
            m_stateMachine->onRelease();
        }
        if (m_ripple && rippleEnabled_) {
            m_ripple->onRelease();
        }
        m_pressedRow = -1;
        m_hasValidPressPos = false;
        update();
    }
}

void TableView::mouseMoveEvent(QMouseEvent* event) {
    QTableView::mouseMoveEvent(event);

    // Convert to viewport coordinates: event->pos() is in view coords, indexAt() needs viewport
    // coords
    QPoint viewportPos = viewport()->mapFrom(this, event->pos());
    QModelIndex index = indexAt(viewportPos);
    int row = index.row();

    if (m_hoveredRow != row) {
        m_hoveredRow = row;
        if (row >= 0 && m_stateMachine) {
            m_stateMachine->onHoverEnter();
        }
        update();
    }
}

void TableView::enterEvent(QEnterEvent* event) {
    QTableView::enterEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onHoverEnter();
    }
    update();
}

void TableView::leaveEvent(QEvent* event) {
    QTableView::leaveEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onHoverLeave();
    }
    if (m_ripple) {
        m_ripple->onCancel();
    }
    m_hoveredRow = -1;
    m_pressedRow = -1;
    update();
}

void TableView::focusInEvent(QFocusEvent* event) {
    QTableView::focusInEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onFocusIn();
    }
    if (m_focusIndicator) {
        m_focusIndicator->onFocusIn();
    }
    update();
}

void TableView::focusOutEvent(QFocusEvent* event) {
    QTableView::focusOutEvent(event);
    if (m_stateMachine) {
        m_stateMachine->onFocusOut();
    }
    if (m_focusIndicator) {
        m_focusIndicator->onFocusOut();
    }
    update();
}

void TableView::changeEvent(QEvent* event) {
    QTableView::changeEvent(event);
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
// Drawing Helpers
// ============================================================================

void TableView::drawGridLines(QPainter& p, const QRectF& viewportRect) {
    if (!model())
        return;

    p.save();

    CFColor gridColor = outlineVariantColor();
    QPen pen(gridColor.native_color(), 1.0);
    pen.setCosmetic(true);
    p.setPen(pen);

    // Draw vertical lines
    if (gridStyle_ == TableGridStyle::Vertical || gridStyle_ == TableGridStyle::Both) {
        int columnCount = model()->columnCount();
        for (int col = 0; col < columnCount; ++col) {
            if (!isColumnHidden(col)) {
                float x = columnViewportPosition(col) + columnWidth(col);
                p.drawLine(QPointF(x, viewportRect.top()), QPointF(x, viewportRect.bottom()));
            }
        }
    }

    // Draw horizontal lines - use actual row positions from visualRect
    if (gridStyle_ == TableGridStyle::Horizontal || gridStyle_ == TableGridStyle::Both) {
        int rowCount = model()->rowCount();
        for (int row = 0; row <= rowCount; ++row) {
            if (row < rowCount && !isRowHidden(row)) {
                QRect rowRect = visualRect(model()->index(row, 0));
                // Draw line at bottom of each row
                float y = rowRect.bottom();
                p.drawLine(QPointF(viewportRect.left(), y), QPointF(viewportRect.right(), y));
            } else if (row == rowCount) {
                // Draw line after the last visible row
                // Find the last visible row and draw below it
                for (int r = rowCount - 1; r >= 0; --r) {
                    if (!isRowHidden(r)) {
                        QRect rowRect = visualRect(model()->index(r, 0));
                        float y = rowRect.bottom();
                        p.drawLine(QPointF(viewportRect.left(), y),
                                   QPointF(viewportRect.right(), y));
                        break;
                    }
                }
            }
        }
    }

    p.restore();
}

void TableView::drawFocusIndicator(QPainter& p, const QRectF& rect) {
    if (m_focusIndicator) {
        // Create focus indicator path (inset to account for both ring width and spacing)
        // Ring width: 3dp, Additional inset: 3dp = Total 6dp
        QPainterPath path;
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float inset = helper.dpToPx(2.0f);
        QRectF focusRect = rect.adjusted(-inset, -inset, inset, inset);
        path.addRoundedRect(focusRect, 4, 4);

        m_focusIndicator->paint(&p, path, primaryContainerColor());
    }
}

// ============================================================================
// Row State Helpers
// ============================================================================

bool TableView::isRowHovered(int row) const {
    return m_hoveredRow == row;
}

bool TableView::isRowPressed(int row) const {
    return m_pressedRow == row;
}

bool TableView::isRowSelected(int row) const {
    if (!selectionModel() || !model()) {
        return false;
    }
    QModelIndex index = model()->index(row, 0);
    return selectionModel()->isSelected(index);
}

// ============================================================================
// Layout Helpers
// ============================================================================

float TableView::rowHeightValue() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    switch (rowHeight_) {
        case TableRowHeight::Compact:
            return helper.dpToPx(kRowHeightCompactDp);
        case TableRowHeight::Standard:
            return helper.dpToPx(kRowHeightStandardDp);
    }
    return helper.dpToPx(kRowHeightStandardDp);
}

float TableView::headerHeightValue() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(kHeaderHeightDp);
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor TableView::onSurfaceColor() const {
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

CFColor TableView::outlineVariantColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOutlineVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(OUTLINE_VARIANT));
    } catch (...) {
        return fallbackOutlineVariant();
    }
}

CFColor TableView::primaryContainerColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackPrimaryContainer();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(PRIMARY_CONTAINER));
    } catch (...) {
        return fallbackPrimaryContainer();
    }
}

} // namespace cf::ui::widget::material
