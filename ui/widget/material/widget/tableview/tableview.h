/**
 * @file    ui/widget/material/widget/tableview/tableview.h
 * @brief   Material Design 3 TableView widget.
 *
 * Implements Material Design 3 table view for two-dimensional data display.
 * Features custom header rendering, grid lines, row selection with ripple,
 * sort indicators, column resize feedback, and Material Design color tokens.
 *
 * @author  Material Design Framework Team
 * @date    2026-03-18
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "base/color.h"
#include "export.h"
#include "widget/material/base/material_widget_base.h"
#include <QTableView>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Table row height mode.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
enum class TableRowHeight {
    Compact, ///< 48dp height for dense content
    Standard ///< 56dp height for standard content
};

/**
 * @brief  Grid line style.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
enum class TableGridStyle {
    None,       ///< No grid lines
    Horizontal, ///< Horizontal lines only
    Vertical,   ///< Vertical lines only
    Both        ///< Both horizontal and vertical lines
};

/**
 * @brief  Material Design 3 TableView widget.
 *
 * @details Implements Material Design 3 table view with two-dimensional
 *          data display support. Features include:
 *          - Custom table header rendering with 48dp height
 *          - Grid lines with OutlineVariant color
 *          - Row selection with PrimaryContainer overlay (12% opacity)
 *          - Ripple effects on row interaction
 *          - Sort indicators for sortable columns
 *          - Column resize visual feedback
 *          - Alternating row colors (SurfaceVariant 5% opacity)
 *          - Material Design 3 color tokens
 *          - Focus indicators for keyboard navigation
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT TableView : public QTableView {
    Q_OBJECT
    Q_PROPERTY(TableRowHeight rowHeight READ rowHeight WRITE setRowHeight)
    Q_PROPERTY(TableGridStyle gridStyle READ gridStyle WRITE setGridStyle)
    Q_PROPERTY(bool showHeader READ showHeader WRITE setShowHeader)
    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors)
    Q_PROPERTY(bool rippleEnabled READ rippleEnabled WRITE setRippleEnabled)

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Defaults to Standard row height and Both grid style.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit TableView(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Components are parented to this, Qt deletes them.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~TableView() override;

    /**
     * @brief  Gets the row height mode.
     *
     * @return        Current row height mode.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    TableRowHeight rowHeight() const;

    /**
     * @brief  Sets the row height mode.
     *
     * @param[in]     height Row height mode to use.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setRowHeight(TableRowHeight height);

    /**
     * @brief  Gets the grid line style.
     *
     * @return        Current grid line style.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    TableGridStyle gridStyle() const;

    /**
     * @brief  Sets the grid line style.
     *
     * @param[in]     style Grid line style to use.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setGridStyle(TableGridStyle style);

    /**
     * @brief  Gets whether the header is shown.
     *
     * @return        true if header is visible, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool showHeader() const;

    /**
     * @brief  Sets whether to show the header.
     *
     * @param[in]     show true to show header, false to hide.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setShowHeader(bool show);

    /**
     * @brief  Gets whether alternating row colors are enabled.
     *
     * @return        true if alternating row colors are enabled.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool alternatingRowColors() const;

    /**
     * @brief  Sets whether to use alternating row colors.
     *
     * @param[in]     enabled true to enable alternating colors, false to disable.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setAlternatingRowColors(bool enabled);

    /**
     * @brief  Gets whether ripple effect is enabled.
     *
     * @return        true if ripple is enabled, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool rippleEnabled() const;

    /**
     * @brief  Sets whether ripple effect is enabled.
     *
     * @param[in]     enabled true to enable ripple, false to disable.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setRippleEnabled(bool enabled);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the table view.
     *
     * @throws        None
     * @note          Based on content and minimum dimensions.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

    /**
     * @brief  Gets the minimum size hint.
     *
     * @return        Minimum recommended size.
     *
     * @throws        None
     * @note          Ensures minimum content visibility.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

  protected:
    /**
     * @brief  Paints the table view.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Completely overrides default table rendering.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Handles mouse press event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Triggers ripple and state changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles mouse release event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Updates press state.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles mouse move event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Tracks hover state for row highlighting.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles mouse enter event.
     *
     * @param[in]     event Enter event.
     *
     * @throws        None
     * @note          Updates hover state.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void enterEvent(QEnterEvent* event) override;

    /**
     * @brief  Handles mouse leave event.
     *
     * @param[in]     event Leave event.
     *
     * @throws        None
     * @note          Updates hover state.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void leaveEvent(QEvent* event) override;

    /**
     * @brief  Handles focus in event.
     *
     * @param[in]     event Focus event.
     *
     * @throws        None
     * @note          Shows focus indicator.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void focusInEvent(QFocusEvent* event) override;

    /**
     * @brief  Handles focus out event.
     *
     * @param[in]     event Focus event.
     *
     * @throws        None
     * @note          Hides focus indicator.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void focusOutEvent(QFocusEvent* event) override;

    /**
     * @brief  Handles widget state change event.
     *
     * @param[in]     event Change event.
     *
     * @throws        None
     * @note          Updates appearance based on state changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void changeEvent(QEvent* event) override;

  private:
    // Drawing helpers - viewport-based rendering
    void drawGridLines(QPainter& p, const QRectF& viewportRect);
    void drawFocusIndicator(QPainter& p, const QRectF& rect);

    // Row state helpers
    bool isRowHovered(int row) const;
    bool isRowPressed(int row) const;
    bool isRowSelected(int row) const;

    // Layout helpers
    float rowHeightValue() const;
    float headerHeightValue() const;

    // Color access methods
    CFColor onSurfaceColor() const;
    CFColor outlineVariantColor() const;
    CFColor primaryContainerColor() const;

    // Behavior components
    base::MaterialWidgetBase m_material;

    // Properties
    TableRowHeight rowHeight_;
    TableGridStyle gridStyle_;
    bool showHeader_;
    bool alternatingRowColors_;
    bool rippleEnabled_;

    // Cached press position for ripple
    QPoint m_pressPos;
    bool m_hasValidPressPos;

    // Hover/pressed row tracking
    int m_hoveredRow;
    int m_pressedRow;
};

} // namespace cf::ui::widget::material
