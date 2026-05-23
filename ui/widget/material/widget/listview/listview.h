/**
 * @file    ui/widget/material/widget/listview/listview.h
 * @brief   Material Design 3 ListView widget.
 *
 * Implements Material Design 3 list view with support for single-line,
 * two-line, and three-line list items. Includes ripple effects, state
 * layers, separators, and focus indicators.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include <QIcon>
#include <QListView>
#include <QWidget>

#include "base/color.h"
#include "export.h"
#include "widget/material/base/material_widget_base.h"

namespace cf::ui::widget::material {

class ListViewDelegate;

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 ListView widget.
 *
 * @details Implements Material Design 3 list view with support for single-line,
 *          two-line, and three-line list items. Includes ripple effects, state
 *          layers, separators, and focus indicators.
 *
 * @since  N/A
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT ListView : public QListView {
    Q_OBJECT
    Q_PROPERTY(int itemHeight READ itemHeight WRITE setItemHeightInt)
    Q_PROPERTY(bool showSeparator READ showSeparator WRITE setShowSeparator)
    Q_PROPERTY(bool rippleEnabled READ rippleEnabled WRITE setRippleEnabled)

  public:
    /**
     * @brief  List item height specification.
     *
     * @since  N/A
     * @ingroup ui_widget_material_widget
     */
    enum class ItemHeight {
        SingleLine = 56, ///< 56dp for single-line items
        TwoLine = 72,    ///< 72dp for two-line items
        ThreeLine = 88   ///< 88dp for three-line items
    };
    Q_ENUM(ItemHeight);

    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    explicit ListView(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    ~ListView() override;

    /**
     * @brief  Gets the item height in dp.
     *
     * @return        Item height in device-independent pixels.
     *
     * @throws        None
     * @note          Returns the integer value of ItemHeight enum.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    int itemHeight() const;

    /**
     * @brief  Sets the item height.
     *
     * @param[in]     height Item height type.
     *
     * @throws        None
     * @note          Updates the uniform item size.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setItemHeight(ItemHeight height);

    /**
     * @brief  Sets the item height (int version for Q_PROPERTY).
     *
     * @param[in]     height Item height value.
     *
     * @throws        None
     * @note          Converts int to ItemHeight enum.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setItemHeightInt(int height);

    /**
     * @brief  Gets whether separators are shown.
     *
     * @return        true if separators are shown, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    bool showSeparator() const;

    /**
     * @brief  Sets whether to show separators.
     *
     * @param[in]     show true to show separators, false to hide.
     *
     * @throws        None
     * @note          Separators are drawn between items.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setShowSeparator(bool show);

    /**
     * @brief  Gets whether ripple effect is enabled.
     *
     * @return        true if ripple is enabled, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
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
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setRippleEnabled(bool enabled);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the list view.
     *
     * @throws        None
     * @note          Based on item count and item height.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

  protected:
    /**
     * @brief  Paints the list view.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Implements Material Design paint pipeline for viewport.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Handles viewport mouse press event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Triggers ripple and updates state.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles viewport mouse release event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Updates state.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief  Handles viewport mouse enter event.
     *
     * @param[in]     event Enter event.
     *
     * @throws        None
     * @note          Updates hover state.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void enterEvent(QEnterEvent* event) override;

    /**
     * @brief  Handles viewport mouse leave event.
     *
     * @param[in]     event Leave event.
     *
     * @throws        None
     * @note          Updates hover state.
     * @warning       None
     * @since         N/A
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
     * @since         N/A
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
     * @since         N/A
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
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void changeEvent(QEvent* event) override;

    /**
     * @brief  Handles viewport resize event.
     *
     * @param[in]     event Resize event.
     *
     * @throws        None
     * @note          Updates layout.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void resizeEvent(QResizeEvent* event) override;

  private:
    // Drawing helpers - Material Design paint pipeline
    void drawViewportBackground(QPainter& p, const QRectF& viewportRect);
    void drawItemBackground(QPainter& p, const QRectF& itemRect, int index);
    void drawItemStateLayer(QPainter& p, const QRectF& itemRect, int index);
    void drawItemRipple(QPainter& p, const QRectF& itemRect, int index);
    void drawItemContent(QPainter& p, const QRectF& itemRect, int index);
    void drawSeparator(QPainter& p, const QRectF& itemRect);
    void drawFocusIndicator(QPainter& p, const QRectF& itemRect, int index);

    // Item state helpers
    bool isIndexHovered(int index) const;
    bool isIndexPressed(int index) const;
    bool isIndexSelected(int index) const;
    QRectF visualItemRect(const QModelIndex& index) const;

    // Color access methods
    CFColor backgroundColor() const;
    CFColor textColor() const;
    CFColor secondaryTextColor() const;
    CFColor selectedBackgroundColor() const;
    CFColor selectedTextColor() const;
    CFColor separatorColor() const;
    CFColor stateLayerColor() const;
    float cornerRadius() const;
    QFont itemFont() const;
    QFont secondaryFont() const;

    // Behavior components
    base::MaterialWidgetBase m_material;

    // Item properties
    ItemHeight m_itemHeight;
    bool m_showSeparator;
    bool m_rippleEnabled;

    // Hover state tracking
    int m_hoveredIndex;
    int m_pressedIndex;
    QPoint m_pressPosition;

    // Delegate for item size control (defined in .cpp)
    ListViewDelegate* m_delegate;
};

} // namespace cf::ui::widget::material
