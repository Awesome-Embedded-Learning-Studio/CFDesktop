/**
 * @file    ui/widget/material/widget/switch/switch.h
 * @brief   Material Design 3 Switch widget.
 *
 * Implements Material Design 3 switch (toggle) with animated thumb position,
 * track color transitions, and state layers. Provides an alternative to
 * checkboxes for binary on/off settings.
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
#include <QCheckBox>
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
 * @brief  Material Design 3 Switch widget.
 *
 * @details Implements Material Design 3 switch (toggle) with animated thumb
 *          position, track color transitions, and state layers. Provides an
 *          alternative to checkboxes for binary on/off settings.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT Switch : public QCheckBox {
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
    explicit Switch(QWidget* parent = nullptr);

    /**
     * @brief  Constructor with text.
     *
     * @param[in]     text Switch text label.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit Switch(const QString& text, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~Switch() override;

    /**
     * @brief  Sets the checked state.
     *
     * @param[in]     checked true to check, false to uncheck.
     *
     * @throws        None
     * @note          Updates thumb position animation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setChecked(bool checked);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the switch.
     *
     * @throws        None
     * @note          Based on Material Design 52x32dp switch track.
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
     * @brief  Paints the switch.
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
     * @note          Triggers thumb position animation.
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
     * @note          Makes entire widget clickable.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool hitButton(const QPoint& pos) const override;

  private:
    // Drawing helpers - Material Design paint pipeline
    QRectF trackRect() const;
    QRectF thumbRect() const;
    QRectF textRect() const;
    void drawTrack(QPainter& p, const QRectF& rect);
    void drawThumb(QPainter& p, const QRectF& rect);
    void drawRipple(QPainter& p, const QRectF& rect);
    void drawText(QPainter& p, const QRectF& rect);
    void drawFocusIndicator(QPainter& p, const QRectF& rect);

    // Animation helper
    void startThumbPositionAnimation(float target);

    // Color access methods
    CFColor trackColor() const;
    CFColor thumbColor() const;
    CFColor thumbIconColor() const;
    CFColor stateLayerColor() const;
    CFColor labelColor() const;
    float trackCornerRadius() const;
    float thumbRadius() const;

    // Helper to get dimensions in pixels
    float trackWidth() const;
    float trackHeight() const;
    float thumbDiameter() const;

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::RippleHelper* m_ripple;
    base::MdElevationController* m_elevation;
    base::MdFocusIndicator* m_focusIndicator;

    // Thumb position animation progress (0.0 = unchecked/left, 1.0 = checked/right)
    float m_thumbPosition = 0.0f;
    // Guard flag: true when in nextCheckState(), prevents setChecked from
    // snapping position so animation can handle the transition smoothly
    bool m_inNextCheckState = false;
};

} // namespace cf::ui::widget::material
