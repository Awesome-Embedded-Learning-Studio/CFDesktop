/**
 * @file    ui/widget/material/widget/comboBox/combobox.h
 * @brief   Material Design 3 ComboBox widget.
 *
 * Implements Material Design 3 combo box (dropdown) with filled and
 * outlined variants, animated dropdown arrow, and custom list styling.
 *
 * @author  CFDesktop Team
 * @date    2026-03-18
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "base/color.h"
#include "base/include/base/weak_ptr/weak_ptr.h"
#include "cfmaterial_animation_factory.h"
#include "export.h"
#include <QComboBox>
#include <QPropertyAnimation>
#include <QWidget>

namespace cf::ui::widget::material {

// Forward declarations
namespace base {
class StateMachine;
class RippleHelper;
class MdFocusIndicator;
} // namespace base

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 ComboBox widget.
 *
 * @details Implements Material Design 3 combo box (dropdown) with filled
 *          and outlined variants, animated dropdown arrow, and custom list styling.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT ComboBox : public QComboBox {
    Q_OBJECT

  public:
    /**
     * @brief  ComboBox visual variant.
     *
     * @since  0.1
     * @ingroup ui_widget_material_widget
     */
    enum class ComboBoxVariant {
        Filled,  ///< Filled background with border
        Outlined ///< Outlined border only
    };
    Q_ENUM(ComboBoxVariant)

    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Defaults to Filled variant.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit ComboBox(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~ComboBox() override;

    /**
     * @brief  Gets the combo box variant.
     *
     * @return        Current combo box variant.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ComboBoxVariant variant() const;

    /**
     * @brief  Sets the combo box variant.
     *
     * @param[in]     variant ComboBox variant to use.
     *
     * @throws        None
     * @note          Changing variant updates the visual appearance.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setVariant(ComboBoxVariant variant);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the combo box.
     *
     * @throws        None
     * @note          Based on Material Design specifications.
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
     * @brief  Paints the combo box.
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
     * @brief  Shows the popup list.
     *
     * @throws        None
     * @note          Override to apply custom styling to popup.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void showPopup() override;

    /**
     * @brief  Hides the popup list.
     *
     * @throws        None
     * @note          Override to reset arrow animation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void hidePopup() override;

  private:
    // Drawing helpers - Material Design paint pipeline
    QRectF fieldRect() const;
    QRectF textRect() const;
    QRectF arrowRect() const;
    void drawBackground(QPainter& p, const QPainterPath& shape);
    void drawOutline(QPainter& p, const QPainterPath& shape);
    void drawStateLayer(QPainter& p, const QPainterPath& shape);
    void drawRipple(QPainter& p, const QPainterPath& shape);
    void drawText(QPainter& p, const QRectF& rect);
    void drawArrow(QPainter& p, const QRectF& rect);
    void drawFocusIndicator(QPainter& p, const QPainterPath& shape);

    // Color access methods
    CFColor containerColor() const;
    CFColor labelColor() const;
    CFColor outlineColor() const;
    CFColor stateLayerColor() const;
    CFColor arrowColor() const;
    float cornerRadius() const;

    // Helper to get arrow rotation angle
    float arrowRotation() const;

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::RippleHelper* m_ripple;
    base::MdFocusIndicator* m_focusIndicator;

    // Variant
    ComboBoxVariant variant_;

    // Arrow rotation (0 = down, 180 = up)
    float m_arrowRotation = 0.0f;

    // Drawer animation
    QPropertyAnimation* m_popupAnimation = nullptr;
    QWidget* m_popupContainer = nullptr;
    int m_targetPopupHeight = 0;
};

} // namespace cf::ui::widget::material
