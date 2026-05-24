/**
 * @file    ui/widget/material/widget/doublespinbox/doublespinbox.h
 * @brief   Material Design 3 DoubleSpinBox widget.
 *
 * Implements Material Design 3 double spin box with support for floating-point
 * input, increment/decrement buttons, outline style, focus indicator, and
 * state layer effects.
 *
 * @author  CFDesktop Team
 * @date    2026-03-18
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include <QMarginsF>
#include <QResizeEvent>

#include "base/color.h"
#include "export.h"
#include "widget/material/base/material_widget_base.h"
#include <QDoubleSpinBox>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 DoubleSpinBox widget.
 *
 * @details Implements Material Design 3 double spin box with support for
 *          floating-point input, increment/decrement buttons, outline style,
 *          focus indicator, and state layer effects following Material Design
 *          3 specifications.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT DoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit DoubleSpinBox(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~DoubleSpinBox() override;

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the spin box.
     *
     * @throws        None
     * @note          Based on content width, button width, and padding.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

    /**
     * @brief  Gets the minimum recommended size.
     *
     * @return        Minimum recommended size.
     *
     * @throws        None
     * @note          Ensures touch target size requirements.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

  protected:
    /**
     * @brief  Paints the spin box.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Implements 7-step Material Design paint pipeline.
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
     * @brief  Handles mouse press event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Triggers ripple and press state for button area.
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
     * @note          Tracks hover state over button areas.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void mouseMoveEvent(QMouseEvent* event) override;

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
     * @brief  Handles widget resize event.
     *
     * @param[in]     event Resize event.
     *
     * @throws        None
     * @note          Constrains internal lineEdit to text area only.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void resizeEvent(QResizeEvent* event) override;

  private:
    // Drawing helpers - 7-step paint pipeline
    void drawBackground(QPainter& p, const QPainterPath& shape);
    void drawStateLayer(QPainter& p, const QPainterPath& shape);
    void drawRipple(QPainter& p, const QPainterPath& shape);
    void drawOutline(QPainter& p, const QPainterPath& shape);
    void drawText(QPainter& p, const QRectF& textRect);
    void drawButtons(QPainter& p, const QRectF& buttonRect);
    void drawFocusIndicator(QPainter& p, const QPainterPath& shape);

    // Layout helpers
    QRectF contentRect() const;
    QRectF textRect() const;
    QRectF incrementButtonRect() const;
    QRectF decrementButtonRect() const;
    QPainterPath shapePath() const;

    // Button helpers
    bool isOverIncrementButton(const QPoint& pos) const;
    bool isOverDecrementButton(const QPoint& pos) const;
    bool isOverButtons(const QPoint& pos) const;
    void updateButtonHoverState(const QPoint& pos);

    // Update lineEdit text color based on current state
    void updateTextColor();

    // Color access methods
    CFColor containerColor() const;
    CFColor textColor() const;
    CFColor stateLayerColor() const;
    CFColor outlineColor() const;
    CFColor focusOutlineColor() const;
    CFColor buttonIconColor() const;
    float cornerRadius() const;
    QFont textFont() const;

    // Behavior components
    base::MaterialWidgetBase m_material;

    // Internal state
    bool m_hoveringIncrementButton;
    bool m_hoveringDecrementButton;
    bool m_pressingIncrementButton;
    bool m_pressingDecrementButton;
};

} // namespace cf::ui::widget::material
