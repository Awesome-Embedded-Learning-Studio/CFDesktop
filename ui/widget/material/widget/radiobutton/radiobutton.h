/**
 * @file    ui/widget/material/widget/radiobutton/radiobutton.h
 * @brief   Material Design 3 RadioButton widget.
 *
 * Implements Material Design 3 radio button with circular selection area,
 * inner circle scale animation, ripple effects, and focus indicators.
 * Supports mutual exclusion through QButtonGroup integration.
 *
 * @author  CFDesktop Team
 * @date    2026-03-01
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "base/color.h"
#include "export.h"
#include "widget/material/base/material_widget_base.h"
#include <QRadioButton>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 RadioButton widget.
 *
 * @details Implements Material Design 3 radio button with circular selection
 *          area, inner circle scale animation, ripple effects, and focus
 *          indicators. Supports mutual exclusion through QButtonGroup
 *          integration.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT RadioButton : public QRadioButton {
    Q_OBJECT
    Q_PROPERTY(bool error READ hasError WRITE setError DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(bool pressEffectEnabled READ pressEffectEnabled WRITE setPressEffectEnabled)

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
    explicit RadioButton(QWidget* parent = nullptr);

    /**
     * @brief  Constructor with text.
     *
     * @param[in]     text Button text label.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit RadioButton(const QString& text, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~RadioButton() override;

    /**
     * @brief  Gets whether the radio button is in error state.
     *
     * @return        true if in error state, false otherwise.
     *
     * @throws        None
     * @note          Error state displays with error color theme.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool hasError() const;

    /**
     * @brief  Sets the error state.
     *
     * @param[in]     error true to set error state, false otherwise.
     *
     * @throws        None
     * @note          Error state displays with error color theme.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setError(bool error);

    /**
     * @brief  Gets whether press effect is enabled.
     *
     * @return        true if press effect is enabled, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool pressEffectEnabled() const;

    /**
     * @brief  Sets whether press effect is enabled.
     *
     * @param[in]     enabled true to enable press effect, false to disable.
     *
     * @throws        None
     * @note          Press effect includes ripple animation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setPressEffectEnabled(bool enabled);

    /**
     * @brief  Sets the checked state of the radio button.
     *
     * @param[in]     checked true to check, false to uncheck.
     *
     * @throws        None
     * @note          Override to sync inner circle scale with checked state.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setChecked(bool checked);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the radio button.
     *
     * @throws        None
     * @note          Based on text and touch target requirements.
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
     * @note          Ensures touch target size requirements (48dp).
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

    /**
     * @brief  Handles hit test for mouse events.
     *
     * @param[in]     pos Mouse position.
     *
     * @return        true if the position is within the clickable area.
     *
     * @throws        None
     * @note          Entire widget area is clickable.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool hitButton(const QPoint& pos) const override;

  protected:
    /**
     * @brief  Paints the radio button.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Implements Material Design paint pipeline.
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
     * @note          Triggers ripple and press state.
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

    /**
     * @brief  Handles check state change event.
     *
     * @param[in]     checked Check state.
     *
     * @throws        None
     * @note          Triggers inner circle scale animation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void nextCheckState() override;

  private:
    // Drawing helpers - Material Design paint pipeline
    void drawStateLayer(QPainter& p, const QRectF& radioRect);
    void drawRipple(QPainter& p, const QRectF& radioRect);
    void drawOuterRing(QPainter& p, const QRectF& radioRect);
    void drawInnerCircle(QPainter& p, const QRectF& radioRect);
    void drawText(QPainter& p, const QRectF& textRect);
    void drawFocusIndicator(QPainter& p, const QRectF& radioRect);

    // Color access methods
    CFColor radioColor() const;
    CFColor onRadioColor() const;
    CFColor stateLayerColor() const;
    CFColor errorColor() const;
    QFont labelFont() const;

    // Layout calculations
    QRectF calculateRadioRect() const;
    QRectF calculateTextRect(const QRectF& radioRect) const;

    // Animation helpers
    void startInnerCircleAnimation(bool checked);

    // Behavior components
    base::MaterialWidgetBase m_material;

    // Animation state
    float m_innerCircleScale = 0.0f;

    // Properties
    bool m_hasError = false;
    bool m_pressEffectEnabled = true;
};

} // namespace cf::ui::widget::material
