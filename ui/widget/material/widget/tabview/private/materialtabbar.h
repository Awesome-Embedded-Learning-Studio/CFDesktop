/**
 * @file materialtabbar.h
 * @brief Material Design 3 TabBar - Internal Implementation
 *
 * Internal TabBar implementation for TabView. Not exposed in public API.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @ingroup ui_widget_material
 */

#pragma once

#include <QApplication>
#include <QPainter>
#include <QPointer>
#include <QSet>
#include <QTabBar>

#include "base/device_pixel.h"

#include "base/include/base/weak_ptr/weak_ptr.h"
#include "material/widget/button/button.h"
#include "widget/material/base/material_widget_base.h"

namespace cf::ui::widget::material {

// Forward declarations
class TabView;

using CFColor = base::CFColor;
/**
 * @brief Internal Material TabBar implementation.
 *
 * Not exported - used only by TabView internally.
 *
 * @since 0.1
 * @ingroup ui_widget_material_widget
 */
class MaterialTabBar : public QTabBar {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent Parent TabView widget.
     *
     * @throws        None
     * @note          Initializes with default Material Design styling.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit MaterialTabBar(TabView* parent);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Components are parented to this; Qt deletes them.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~MaterialTabBar() override;

    /**
     * @brief  Gets the tab height.
     *
     * @return        Tab height in device-independent pixels.
     *
     * @throws        None
     * @note          Default follows Material Design 3 specifications.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    int tabHeightDp() const { return m_tabHeightDp; }

    /**
     * @brief  Sets the tab height.
     *
     * @param[in]     height Tab height in device-independent pixels.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setTabHeightDp(int height);

    /**
     * @brief  Gets the minimum tab width.
     *
     * @return        Minimum tab width in device-independent pixels.
     *
     * @throws        None
     * @note          Default follows Material Design 3 specifications.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    int tabMinWidthDp() const { return m_tabMinWidthDp; }

    /**
     * @brief  Sets the minimum tab width.
     *
     * @param[in]     width Minimum tab width in device-independent pixels.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setTabMinWidthDp(int width);

    /**
     * @brief  Gets whether the selection indicator is shown.
     *
     * @return        true if indicator is visible, false otherwise.
     *
     * @throws        None
     * @note          The indicator slides to the selected tab.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool showIndicator() const { return m_showIndicator; }

    /**
     * @brief  Sets whether the selection indicator is shown.
     *
     * @param[in]     show true to show indicator, false to hide.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setShowIndicator(bool show);

    /**
     * @brief  Sets whether a tab can be closed.
     *
     * @param[in]     index Tab index.
     * @param[in]     closeable true to make tab closeable, false otherwise.
     *
     * @throws        None
     * @note          Closeable tabs display a close button.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setTabCloseable(int index, bool closeable);

    /**
     * @brief  Gets whether a tab can be closed.
     *
     * @param[in]     index Tab index.
     * @return        true if tab is closeable, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool isTabCloseable(int index) const;

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the tab bar.
     *
     * @throws        None
     * @note          Calculates based on tab height and content.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    inline QSize sizeHint() const override {
        ::cf::ui::base::device::CanvasUnitHelper helper(qApp->devicePixelRatio());
        float height = helper.dpToPx(static_cast<float>(m_tabHeightDp));
        QSize baseSize = QTabBar::sizeHint();
        return QSize(baseSize.width(), static_cast<int>(std::ceil(height)));
    }

  signals:
    /**
     * @brief  Signal emitted when a tab close button is clicked.
     *
     * @param[in]     index Index of the tab to close.
     *
     * @note          Connect to this signal to handle tab closure.
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void tabCloseRequested(int index);

  protected:
    /**
     * @brief  Gets the size hint for a specific tab.
     *
     * @param[in]     index Tab index.
     * @return        Size hint for the tab.
     *
     * @throws        None
     * @note          Considers minimum width and content size.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize tabSizeHint(int index) const override;

    /**
     * @brief  Paints the tab bar.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Draws tabs and selection indicator.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

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
     * @brief  Handles mouse move event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Tracks hover for close buttons.
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
     * @note          Detects clicks on close buttons.
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
    /**
     * @brief  Draws a single tab.
     *
     * @param[in]     p QPainter to render with.
     * @param[in]     index Tab index to draw.
     *
     * @throws        None
     * @note          Draws tab background, text, and close button if applicable.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void drawTab(QPainter& p, int index);
    void drawTabStateLayer(QPainter& p, const QRect& tabRect, int index);
    void drawTabContent(QPainter& p, const QRect& tabRect, int index);
    void drawCloseIcon(QPainter& p, const QRect& closeRect, int index);
    void drawIndicator(QPainter& p);

    void animateIndicatorTo(int index);
    void updateIndicatorPosition();

    QRect closeButtonRect(int index) const;
    bool isOverCloseButton(int index, const QPoint& pos) const;
    void updateCloseButtonHover(const QPoint& pos);

    // Color access
    CFColor backgroundColor() const;
    CFColor tabTextColor() const;
    CFColor selectedTabTextColor() const;
    CFColor indicatorColor() const;
    CFColor stateLayerColor() const;
    CFColor focusIndicatorColor() const;
    QFont tabFont() const;

    TabView* m_tabView;
    int m_tabHeightDp;
    int m_tabMinWidthDp;
    bool m_showIndicator;

    float m_indicatorPosition;
    float m_indicatorTargetPosition;
    int m_lastIndex;
    int m_hoveredIndex;
    int m_pressedIndex;
    int m_closeButtonHoveredIndex; // Index of tab whose close button is hovered

    QSet<int> m_closeableTabs; // Set of closeable tab indices

    cf::WeakPtr<components::ICFAbstractAnimation> m_indicatorAnimation;
    base::MaterialWidgetBase m_material;
};

} // namespace cf::ui::widget::material
