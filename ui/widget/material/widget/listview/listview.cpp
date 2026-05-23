/**
 * @file listview.cpp
 * @brief Material Design 3 ListView Implementation
 *
 * Implements a Material Design 3 list view with support for single-line,
 * two-line, and three-line list items. Supports ripple effects, state layers,
 * separators, and focus indicators.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

// Force PCH rebuild: Added internal delegate for item size control
#include "listview.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyledItemDelegate>

namespace cf::ui::widget::material {

// ============================================================================
// Private Delegate for Item Size Control
// ============================================================================

/**
 * @brief Internal delegate for controlling item sizes in ListView.
 *
 * This delegate provides custom size hints for list items based on the
 * configured ItemHeight setting. The default delegate would otherwise use
 * QFontMetrics-based sizing which doesn't respect our Material Design
 * specifications.
 *
 * @note This class is defined in the .cpp file and is not part of the
 *       public API. It is only used internally by ListView.
 */
class ListViewDelegate : public QStyledItemDelegate {
  public:
    explicit ListViewDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent), m_itemHeightPx(56.0f) {}

    void setItemHeight(float heightPx) { m_itemHeightPx = heightPx; }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        Q_UNUSED(option);
        Q_UNUSED(index);

        // Return fixed height based on ItemHeight setting
        // Width is left to the view's discretion (suggested: 200dp)
        return QSize(-1, static_cast<int>(m_itemHeightPx));
    }

  private:
    float m_itemHeightPx;
};

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

ListView::ListView(QWidget* parent)
    : QListView(parent), m_material(this,
                                    MaterialWidgetBase::Config{
                                        .useRipple = true,
                                        .useElevation = false,
                                        .useFocusIndicator = true,
                                    }),
      m_itemHeight(ItemHeight::SingleLine), m_showSeparator(true), m_rippleEnabled(true),
      m_hoveredIndex(-1), m_pressedIndex(-1), m_delegate(nullptr) {
    // Initialize and set delegate for item size control
    m_delegate = new ListViewDelegate(this);
    setItemDelegate(m_delegate);

    // Configure viewport for mouse tracking
    viewport()->setMouseTracking(true);

    // Set default selection mode
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set item height
    setItemHeight(m_itemHeight);

    // Disable default styling
    setAttribute(Qt::WA_StyledBackground, false);
}

ListView::~ListView() {
    // Components are parented to this, Qt will delete them automatically
}

// ============================================================================
// Event Handlers
// ============================================================================

void ListView::enterEvent(QEnterEvent* event) {
    QListView::enterEvent(event);
    m_material.onEnterEvent();
}

void ListView::leaveEvent(QEvent* event) {
    QListView::leaveEvent(event);
    m_material.onLeaveEvent();
    m_hoveredIndex = -1;
    update();
}

void ListView::mousePressEvent(QMouseEvent* event) {
    QListView::mousePressEvent(event);

    // Map viewport position to model index
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_rippleEnabled) {
        m_pressedIndex = index.row();
        m_pressPosition = event->pos();

        m_material.onMousePress(event->pos(), visualItemRect(index));
    }
    update();
}

void ListView::mouseReleaseEvent(QMouseEvent* event) {
    QListView::mouseReleaseEvent(event);

    if (m_pressedIndex >= 0) {
        m_material.onMouseRelease();
        m_pressedIndex = -1;
    }
    update();
}

void ListView::focusInEvent(QFocusEvent* event) {
    QListView::focusInEvent(event);
    m_material.onFocusIn();
}

void ListView::focusOutEvent(QFocusEvent* event) {
    QListView::focusOutEvent(event);
    m_material.onFocusOut();
}

void ListView::changeEvent(QEvent* event) {
    QListView::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        m_material.onEnabledChange(isEnabled());
    }
}

void ListView::resizeEvent(QResizeEvent* event) {
    QListView::resizeEvent(event);
    update();
}

// ============================================================================
// Property Getters/Setters
// ============================================================================

int ListView::itemHeight() const {
    return static_cast<int>(m_itemHeight);
}

void ListView::setItemHeight(ItemHeight height) {
    if (m_itemHeight != height) {
        m_itemHeight = height;
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        float itemHeightPx = helper.dpToPx(static_cast<float>(itemHeight()));

        // Update delegate with new height
        if (m_delegate) {
            m_delegate->setItemHeight(itemHeightPx);
        }

        setUniformItemSizes(true);
        // Force update of item size through delegate size hint
        updateGeometry();
        update();
    }
}

void ListView::setItemHeightInt(int height) {
    // Convert int to ItemHeight enum
    if (height == static_cast<int>(ItemHeight::SingleLine) ||
        height == static_cast<int>(ItemHeight::TwoLine) ||
        height == static_cast<int>(ItemHeight::ThreeLine)) {
        setItemHeight(static_cast<ItemHeight>(height));
    }
}

bool ListView::showSeparator() const {
    return m_showSeparator;
}

void ListView::setShowSeparator(bool show) {
    if (m_showSeparator != show) {
        m_showSeparator = show;
        update();
    }
}

bool ListView::rippleEnabled() const {
    return m_rippleEnabled;
}

void ListView::setRippleEnabled(bool enabled) {
    if (m_rippleEnabled != enabled) {
        m_rippleEnabled = enabled;
        update();
    }
}

// ============================================================================
// Size Hints
// ============================================================================

QSize ListView::sizeHint() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float itemHeightPx = helper.dpToPx(static_cast<float>(itemHeight()));
    int itemCount = model() ? model()->rowCount() : 0;

    // Default width: 200dp
    float defaultWidth = helper.dpToPx(200.0f);

    // Minimum height: at least one item row
    float totalHeight = itemHeightPx * (itemCount > 0 ? itemCount : 1);

    return QSize(static_cast<int>(defaultWidth), static_cast<int>(totalHeight));
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
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(73, 69, 79);
} // On Surface Variant
inline CFColor fallbackSurfaceVariant() {
    return CFColor(231, 224, 236);
} // Surface Variant
inline CFColor fallbackOutlineVariant() {
    return CFColor(187, 186, 189);
} // Outline Variant
} // namespace

CFColor ListView::backgroundColor() const {
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

CFColor ListView::textColor() const {
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

CFColor ListView::secondaryTextColor() const {
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

CFColor ListView::selectedBackgroundColor() const {
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

CFColor ListView::selectedTextColor() const {
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

CFColor ListView::separatorColor() const {
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

CFColor ListView::stateLayerColor() const {
    // State layer uses onSurface color with varying opacity
    return textColor();
}

float ListView::cornerRadius() const {
    // ListView items use small rounded corners (8dp)
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(8.0f);
}

QFont ListView::itemFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QListView::font();
        font.setPixelSize(16);
        font.setWeight(QFont::Normal);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyLarge");
    } catch (...) {
        QFont font = QListView::font();
        font.setPixelSize(16);
        font.setWeight(QFont::Normal);
        return font;
    }
}

QFont ListView::secondaryFont() const {
    auto* app = Application::instance();
    if (!app) {
        QFont font = QListView::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Normal);
        return font;
    }

    try {
        const auto& theme = app->currentTheme();
        auto& fontType = theme.font_type();
        return fontType.queryTargetFont("bodyMedium");
    } catch (...) {
        QFont font = QListView::font();
        font.setPixelSize(14);
        font.setWeight(QFont::Normal);
        return font;
    }
}

// ============================================================================
// Item State Helpers
// ============================================================================

bool ListView::isIndexHovered(int index) const {
    return m_hoveredIndex == index && isEnabled();
}

bool ListView::isIndexPressed(int index) const {
    return m_pressedIndex == index && isEnabled();
}

bool ListView::isIndexSelected(int index) const {
    if (!selectionModel()) {
        return false;
    }
    QModelIndex modelIndex = model()->index(index, 0);
    return selectionModel()->isSelected(modelIndex);
}

QRectF ListView::visualItemRect(const QModelIndex& index) const {
    if (!index.isValid()) {
        return QRectF();
    }
    QRect rect = visualRect(index);
    return QRectF(rect);
}

// ============================================================================
// Paint Event
// ============================================================================

void ListView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing);

    QRectF viewportRect = QRectF(viewport()->rect());

    // Draw viewport background
    drawViewportBackground(p, viewportRect);

    // Update hovered index
    QPoint mousePos = viewport()->mapFromGlobal(QCursor::pos());
    QModelIndex hoveredIndex = indexAt(mousePos);
    m_hoveredIndex = hoveredIndex.isValid() ? hoveredIndex.row() : -1;

    // Get visible items
    QModelIndex topLeft = indexAt(viewportRect.topLeft().toPoint());
    QModelIndex bottomRight = indexAt(viewportRect.bottomRight().toPoint());

    if (!topLeft.isValid()) {
        return;
    }

    if (!bottomRight.isValid()) {
        bottomRight = model()->index(model()->rowCount() - 1, 0);
    }

    // Draw each visible item
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        QModelIndex index = model()->index(row, 0);
        QRectF itemRect = visualItemRect(index);

        // Skip if item is outside viewport
        if (!itemRect.intersects(viewportRect)) {
            continue;
        }

        // Draw item background (for selected state)
        drawItemBackground(p, itemRect, row);

        // Draw state layer (hover/pressed)
        drawItemStateLayer(p, itemRect, row);

        // Draw ripple effect
        drawItemRipple(p, itemRect, row);

        // Draw item content (text, icons)
        drawItemContent(p, itemRect, row);

        // Draw separator (except for last item)
        if (m_showSeparator && row < model()->rowCount() - 1) {
            drawSeparator(p, itemRect);
        }

        // Draw focus indicator if this is the current index
        if (hasFocus() && currentIndex().row() == row) {
            drawFocusIndicator(p, itemRect, row);
        }
    }
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void ListView::drawViewportBackground(QPainter& p, const QRectF& viewportRect) {
    CFColor bg = backgroundColor();
    p.fillRect(viewportRect, bg.native_color());
}

void ListView::drawItemBackground(QPainter& p, const QRectF& itemRect, int index) {
    if (isIndexSelected(index)) {
        CFColor selectedBg = selectedBackgroundColor();
        QPainterPath shape = roundedRect(itemRect, 0.0f); // No rounding for item background
        p.fillPath(shape, selectedBg.native_color());
    }
}

void ListView::drawItemStateLayer(QPainter& p, const QRectF& itemRect, int index) {
    if (!isEnabled() || !m_material.stateMachine()) {
        return;
    }

    // Only draw state layer for hovered or pressed items
    if (!isIndexHovered(index) && !isIndexPressed(index)) {
        return;
    }

    float opacity = m_material.stateMachine()->stateLayerOpacity();
    if (opacity <= 0.0f) {
        return;
    }

    CFColor stateColor = stateLayerColor();
    QColor color = stateColor.native_color();
    color.setAlphaF(color.alphaF() * opacity);

    QPainterPath shape = roundedRect(itemRect, 0.0f);
    p.fillPath(shape, color);
}

void ListView::drawItemRipple(QPainter& p, const QRectF& itemRect, int index) {
    if (!m_rippleEnabled || !m_material.ripple()) {
        return;
    }

    m_material.ripple()->setColor(textColor());

    // Create a clip path for the item
    QPainterPath clipPath;
    clipPath.addRect(itemRect);

    p.save();
    p.setClipPath(clipPath);
    m_material.ripple()->paint(&p, clipPath);
    p.restore();
}

void ListView::drawItemContent(QPainter& p, const QRectF& itemRect, int index) {
    QModelIndex modelIndex = model()->index(index, 0);
    if (!modelIndex.isValid()) {
        return;
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float horizontalPadding = helper.dpToPx(16.0f);
    float iconSize = helper.dpToPx(24.0f);
    float iconSpacing = helper.dpToPx(16.0f);

    QRectF contentRect = itemRect.adjusted(horizontalPadding, 0, -horizontalPadding, 0);

    // Determine text color based on state
    CFColor itemTextColor;
    if (isIndexSelected(index)) {
        itemTextColor = selectedTextColor();
    } else {
        itemTextColor = textColor();
    }

    // Handle disabled state
    if (!isEnabled()) {
        QColor color = itemTextColor.native_color();
        color.setAlphaF(0.38f);
        p.setPen(color);
    } else {
        p.setPen(itemTextColor.native_color());
    }

    // Check for icon/decoration
    QVariant decorationVariant = modelIndex.data(Qt::DecorationRole);
    QIcon icon;
    if (!decorationVariant.isNull() && decorationVariant.canConvert<QIcon>()) {
        icon = decorationVariant.value<QIcon>();
    }

    // Get text
    QString text = modelIndex.data(Qt::DisplayRole).toString();

    // Calculate layout
    float currentX = contentRect.left();
    float centerY = contentRect.center().y();

    // Draw icon if present
    if (!icon.isNull()) {
        QRectF iconRect(currentX, centerY - iconSize / 2, iconSize, iconSize);
        QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        icon.paint(&p, iconRect.toRect(), Qt::AlignCenter, mode);
        currentX += iconSize + iconSpacing;
    }

    // Draw text
    QFont font = itemFont();
    p.setFont(font);

    QRectF textRect(currentX, contentRect.top(), contentRect.right() - currentX,
                    contentRect.height());

    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    p.drawText(textRect, alignment, text);

    // Draw secondary text if available (for two-line items)
    QVariant secondaryVariant = modelIndex.data(Qt::UserRole + 1);
    if (secondaryVariant.isValid()) {
        QString secondaryText = secondaryVariant.toString();
        QFont secFont = secondaryFont();
        p.setFont(secFont);

        CFColor secColor = secondaryTextColor();
        if (!isEnabled()) {
            QColor color = secColor.native_color();
            color.setAlphaF(0.38f);
            p.setPen(color);
        } else {
            p.setPen(secColor.native_color());
        }

        // Position secondary text below primary text
        QFontMetrics primaryMetrics(font);
        float primaryHeight = primaryMetrics.boundingRect(text).height();
        float secondaryTop = textRect.top() + primaryHeight + helper.dpToPx(4.0f);

        QRectF secondaryRect(textRect.left(), secondaryTop, textRect.width(),
                             textRect.bottom() - secondaryTop);
        p.drawText(secondaryRect, alignment, secondaryText);
    }
}

void ListView::drawSeparator(QPainter& p, const QRectF& itemRect) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float separatorHeight = helper.dpToPx(1.0f);
    float horizontalPadding = helper.dpToPx(16.0f);

    CFColor sepColor = separatorColor();
    QColor color = sepColor.native_color();
    color.setAlphaF(color.alphaF() * 0.12f); // 12% opacity for separators

    p.setPen(QPen(color, separatorHeight));
    p.setBrush(Qt::NoBrush);

    float y = itemRect.bottom() - separatorHeight / 2;
    float left = itemRect.left() + horizontalPadding;
    float right = itemRect.right() - horizontalPadding;

    p.drawLine(QPointF(left, y), QPointF(right, y));
}

void ListView::drawFocusIndicator(QPainter& p, const QRectF& itemRect, int index) {
    if (!m_material.focusIndicator()) {
        return;
    }

    // Create a path for the focus indicator around the item
    QPainterPath shape;
    shape.addRect(itemRect);

    // Use the text color as the focus indicator color
    m_material.focusIndicator()->paint(&p, shape, textColor());
}

} // namespace cf::ui::widget::material
