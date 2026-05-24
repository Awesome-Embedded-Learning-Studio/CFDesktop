/**
 * @file    ui/widget/material/widget/treeview/treeview.h
 * @brief   Material Design 3 TreeView widget.
 *
 * Implements Material Design 3 tree view for hierarchical data display.
 * Features expand/collapse animations, tree connection lines, proper
 * indentation, and Material Design color tokens.
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
#include <QPersistentModelIndex>
#include <QTreeView>
#include <QWidget>

namespace cf::ui::widget::material {

class TreeViewItemDelegate;

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Tree item height mode.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
enum class TreeItemHeight {
    Compact, ///< 48dp height for dense content
    Standard ///< 56dp height for standard content
};

/**
 * @brief  Tree indentation style.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
enum class TreeIndentStyle {
    Material, ///< 56dp per level with guide lines
    Classic   ///< Traditional nested indentation
};

/**
 * @brief  Material Design 3 TreeView widget.
 *
 * @details Implements Material Design 3 tree view with hierarchical data
 *          display support. Features include:
 *          - Expand/collapse with animation support
 *          - Tree connection lines for visual hierarchy
 *          - Proper indentation per level
 *          - Ripple effects on item interaction
 *          - Material Design 3 color tokens
 *          - Focus indicators for keyboard navigation
 *
 *          Rendering is handled by TreeViewItemDelegate following Qt's
 *          Model/View architecture.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT TreeView : public QTreeView {
    Q_OBJECT
    Q_PROPERTY(TreeItemHeight itemHeight READ itemHeight WRITE setItemHeight)
    Q_PROPERTY(TreeIndentStyle indentStyle READ indentStyle WRITE setIndentStyle)
    Q_PROPERTY(bool showTreeLines READ showTreeLines WRITE setShowTreeLines)
    Q_PROPERTY(bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated)

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Defaults to Standard item height and Material indent style.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit TreeView(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Components are parented to this, Qt deletes them.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~TreeView() override;

    /**
     * @brief  Gets the item height mode.
     *
     * @return        Current item height mode.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    TreeItemHeight itemHeight() const;

    /**
     * @brief  Sets the item height mode.
     *
     * @param[in]     height Item height mode to use.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setItemHeight(TreeItemHeight height);

    /**
     * @brief  Gets the indentation style.
     *
     * @return        Current indentation style.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    TreeIndentStyle indentStyle() const;

    /**
     * @brief  Sets the indentation style.
     *
     * @param[in]     style Indentation style to use.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setIndentStyle(TreeIndentStyle style);

    /**
     * @brief  Gets whether tree connection lines are shown.
     *
     * @return        true if tree lines are visible, false otherwise.
     *
     * @throws        None
     * @note          Tree lines show hierarchy between parent and child items.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool showTreeLines() const;

    /**
     * @brief  Sets whether tree connection lines are shown.
     *
     * @param[in]     show true to show tree lines, false otherwise.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setShowTreeLines(bool show);

    /**
     * @brief  Gets whether root items are decorated.
     *
     * @return        true if root items show expand/collapse controls.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool rootIsDecorated() const;

    /**
     * @brief  Sets whether root items are decorated.
     *
     * @param[in]     decorated true to decorate root items, false otherwise.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setRootIsDecorated(bool decorated);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the tree view.
     *
     * @throws        None
     * @note          Based on content and minimum item dimensions.
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
     * @brief  Paints the tree view.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Draws background, then lets Qt handle tree rendering via delegate.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Handles viewport mouse move event for hover tracking.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Updates hovered index for delegate.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseMoveEvent(QMouseEvent* event) override;

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

    /**
     * @brief  Handles rows inserted event for layout update.
     *
     * @param[in]     parent Parent model index.
     * @param[in]     start Start row.
     * @param[in]     end End row.
     *
     * @throws        None
     * @note          Ensures proper layout after model changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void rowsInserted(const QModelIndex& parent, int start, int end) override;

    /**
     * @brief  Draws tree branch indicators.
     *
     * @param[in]     painter QPainter to render with.
     * @param[in]     rect Rectangle for the branch area.
     * @param[in]     index Model index for the item.
     *
     * @throws        None
     * @note          Empty implementation - delegate handles all expand/collapse icon rendering.
     *                This prevents Qt from drawing default branch indicators that would
     *                conflict with the custom delegate rendering.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void drawBranches(QPainter* painter, const QRect& rect,
                      const QModelIndex& index) const override;

  private:
    // Layout helpers
    float itemHeightValue() const;

    // Color access methods (for background and ripple)
    CFColor surfaceColor() const;
    CFColor onSurfaceColor() const;

    // Behavior components
    base::MaterialWidgetBase m_material;

    // Delegate for item rendering
    TreeViewItemDelegate* m_delegate;

    // Properties
    TreeItemHeight m_itemHeight;
    TreeIndentStyle m_indentStyle;
    bool m_showTreeLines;
    bool m_rootIsDecorated;

    // Per-index state tracking for proper hover/ripple
    QPersistentModelIndex m_hoveredIndex;
    QPersistentModelIndex m_pressedIndex;
    QPoint m_pressPosition;
    QRectF m_rippleClipRect; // Store the item rect where ripple started
};

} // namespace cf::ui::widget::material
