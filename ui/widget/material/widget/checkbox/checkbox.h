/**
 * @file    ui/widget/material/widget/checkbox/checkbox.h
 * @brief   Material Design 3 CheckBox widget.
 *
 * Implements Material Design 3 checkbox with support for unchecked, checked,
 * and indeterminate states. Includes ripple effects, state layers, and focus
 * indicators following Material Design 3 specifications.
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
#include <QCheckBox>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 CheckBox widget.
 *
 * @details Implements Material Design 3 checkbox with support for unchecked,
 *          checked, and indeterminate states. Includes ripple effects, state
 *          layers, and focus indicators following Material Design 3
 *          specifications.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT CheckBox : public QCheckBox {
    Q_OBJECT
    Q_PROPERTY(bool error READ hasError WRITE setError NOTIFY errorChanged)

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
    explicit CheckBox(QWidget* parent = nullptr);

    /**
     * @brief  Constructor with text.
     *
     * @param[in]     text CheckBox text label.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit CheckBox(const QString& text, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~CheckBox() override;

    /**
     * @brief  Sets the checked state.
     *
     * @param[in]     checked true to check, false to uncheck.
     *
     * @throws        None
     * @note          Updates animation progress for correct rendering.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setChecked(bool checked);

    /**
     * @brief  Sets the check state.
     *
     * @param[in]     state The check state to set.
     *
     * @throws        None
     * @note          Updates animation progress for correct rendering.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setCheckState(Qt::CheckState state);

    /**
     * @brief  Gets whether the checkbox is in error state.
     *
     * @return        true if error state is active, false otherwise.
     *
     * @throws        None
     * @note          Error state affects the border color.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool hasError() const;

    /**
     * @brief  Sets the error state.
     *
     * @param[in]     error true to set error state, false to clear.
     *
     * @throws        None
     * @note          Error state uses error color for the border.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setError(bool error);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the checkbox.
     *
     * @throws        None
     * @note          Based on icon size, text, and spacing.
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

  signals:
    /**
     * @brief  Signal emitted when error state changes.
     *
     * @param[in]     error The new error state.
     *
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void errorChanged(bool error);

  protected:
    /**
     * @brief  Paints the checkbox.
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
     * @param[in]     event State change event.
     *
     * @throws        None
     * @note          Triggers check mark animation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void nextCheckState() override;

    /**
     * @brief  Determines if a point is within the clickable button area.
     *
     * @param[in]     pos The point to check, in widget coordinates.
     * @return        true if the point is within the clickable area, false otherwise.
     *
     * @throws        None
     * @note          Overrides QAbstractButton behavior to make entire widget clickable.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool hitButton(const QPoint& pos) const override;

  private:
    // Drawing helpers - Material Design paint pipeline
    QRectF checkboxRect() const;
    QRectF textRect() const;
    void drawBackground(QPainter& p, const QRectF& rect);
    void drawBorder(QPainter& p, const QRectF& rect);
    void drawCheckMark(QPainter& p, const QRectF& rect);
    void drawIndeterminateMark(QPainter& p, const QRectF& rect);
    void drawRipple(QPainter& p, const QRectF& rect);
    void drawText(QPainter& p, const QRectF& rect);
    void drawFocusIndicator(QPainter& p, const QRectF& rect);

    // Animation helper
    void updateAnimationProgress(float progress, bool checked);
    void startCheckMarkAnimation(float target);

    // Color access methods
    CFColor checkmarkColor() const;
    CFColor markDrawColor() const; // Color for check/indeterminate mark on background
    CFColor borderColor() const;
    CFColor backgroundColor() const;
    CFColor stateLayerColor() const;
    float cornerRadius() const;

    // Helper to get checkbox size in pixels
    float checkboxSize() const;
    float strokeWidth() const;

    // Behavior components
    base::MaterialWidgetBase m_material;

    // Check mark animation progress (0.0 to 1.0)
    float m_checkAnimationProgress = 0.0f;

    // Error state
    bool m_error = false;
};

} // namespace cf::ui::widget::material
