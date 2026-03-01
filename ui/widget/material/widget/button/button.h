/**
 * @file    ui/widget/material/widget/button/button.h
 * @brief   Material Design 3 Button widget.
 *
 * Implements Material Design 3 button with support for filled, tonal,
 * outlined, text, and elevated variants. Includes ripple effects, state
 * layers, elevation shadows, and focus indicators.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include <QMarginsF>

#include "base/color.h"
#include "base/include/base/weak_ptr/weak_ptr.h"
#include "cfmaterial_animation_factory.h"
#include "export.h"
#include <QIcon>
#include <QPushButton>
#include <QWidget>

namespace cf::ui::widget::material {

// Forward declarations
namespace base {
class StateMachine;
class RippleHelper;
class MdElevationController;
class MdFocusIndicator;
} // namespace base

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 Button widget.
 *
 * @details Implements Material Design 3 button with support for filled,
 *          tonal, outlined, text, and elevated variants. Includes ripple
 *          effects, state layers, elevation shadows, and focus indicators.
 *
 * @since  N/A
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT Button : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(ButtonVariant variant READ variant WRITE setVariant)
    Q_PROPERTY(int elevation READ elevation WRITE setElevation)
    Q_PROPERTY(bool pressEffectEnabled READ pressEffectEnabled WRITE setPressEffectEnabled)

  public:
    /**
     * @brief  Button visual variant.
     *
     * @since  N/A
     * @ingroup ui_widget_material_widget
     */
    enum class ButtonVariant { Filled, Tonal, Outlined, Text, Elevated };
    Q_ENUM(ButtonVariant);

    /**
     * @brief  Constructor with variant.
     *
     * @param[in]     variant Button visual variant.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    explicit Button(ButtonVariant variant = ButtonVariant::Filled, QWidget* parent = nullptr);

    /**
     * @brief  Constructor with text and variant.
     *
     * @param[in]     text Button text label.
     * @param[in]     variant Button visual variant.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    explicit Button(const QString& text, ButtonVariant variant = ButtonVariant::Filled,
                    QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    ~Button() override;

    /**
     * @brief  Gets the elevation level.
     *
     * @return        Elevation level (0-5).
     *
     * @throws        None
     * @note          Material Design defines 6 standard levels.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    int elevation() const;

    /**
     * @brief  Sets the elevation level.
     *
     * @param[in]     level Elevation level (0-5).
     *
     * @throws        None
     * @note          Affects shadow rendering.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setElevation(int level);

    /**
     * @brief  Sets the light source angle for shadow.
     *
     * @param[in]     degrees Angle in degrees (-90 to 90).
     *
     * @throws        None
     * @note          Default is 15 degrees (light from top-left).
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setLightSourceAngle(float degrees);

    /**
     * @brief  Gets the light source angle.
     *
     * @return        Light source angle in degrees.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    float lightSourceAngle() const;

    /**
     * @brief  Sets the leading icon.
     *
     * @param[in]     icon Icon to display before text.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setLeadingIcon(const QIcon& icon);

    /**
     * @brief  Redirects setIcon to setLeadingIcon for convenience.
     *
     * @param[in]     icon Icon to display before text.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setIcon(const QIcon& icon) { setLeadingIcon(icon); }

    /**
     * @brief  Gets the current icon.
     *
     * @return        Current icon.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    QIcon icon() const { return leadingIcon_; }

    /**
     * @brief  Gets whether press effect is enabled.
     *
     * @return        true if press effect is enabled, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    bool pressEffectEnabled() const;

    /**
     * @brief  Sets whether press effect is enabled.
     *
     * @param[in]     enabled true to enable press effect, false to disable.
     *
     * @throws        None
     * @note          Press effect includes elevation change and ripple.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setPressEffectEnabled(bool enabled);

    /**
     * @brief  Gets the button variant.
     *
     * @return        Current button variant.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    ButtonVariant variant() const;

    /**
     * @brief  Sets the button variant.
     *
     * @param[in]     variant Button variant to use.
     *
     * @throws        None
     * @note          Changing variant updates the visual appearance.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setVariant(ButtonVariant variant);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the button.
     *
     * @throws        None
     * @note          Based on text, icon, and padding.
     * @warning       None
     * @since         N/A
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
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

  protected:
    /**
     * @brief  Paints the button.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Implements 7-step Material Design paint pipeline.
     * @warning       None
     * @since         N/A
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
     * @since         N/A
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
     * @since         N/A
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
     * @since         N/A
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
     * @since         N/A
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

  private:
    // Drawing helpers - 7-step paint pipeline
    QMarginsF shadowMargin() const;
    void drawShadow(QPainter& p, const QRectF& contentRect, const QPainterPath& shape);
    void drawBackground(QPainter& p, const QPainterPath& shape);
    void drawStateLayer(QPainter& p, const QPainterPath& shape);
    void drawRipple(QPainter& p, const QPainterPath& shape);
    void drawOutline(QPainter& p, const QPainterPath& shape);
    void drawContent(QPainter& p, const QRectF& contentRect);
    void drawFocusIndicator(QPainter& p, const QPainterPath& shape);

    // Color access methods
    CFColor containerColor() const;
    CFColor labelColor() const;
    CFColor stateLayerColor() const;
    CFColor outlineColor() const;
    float cornerRadius() const;
    QFont labelFont() const;

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::RippleHelper* m_ripple;
    base::MdElevationController* m_elevation;
    base::MdFocusIndicator* m_focusIndicator;

    ButtonVariant variant_;
    QIcon leadingIcon_;
    bool m_pressEffectEnabled = true;
};

} // namespace cf::ui::widget::material
