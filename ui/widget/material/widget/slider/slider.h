/**
 * @file    ui/widget/material/widget/slider/slider.h
 * @brief   Material Design 3 Slider widget.
 *
 * Implements Material Design 3 slider with horizontal/vertical orientations,
 * active/inactive track portions, thumb with elevation, tick marks, and
 * state layers.
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
#include <QSlider>
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
 * @brief  Material Design 3 Slider widget.
 *
 * @details Implements Material Design 3 slider with horizontal/vertical
 *          orientations, active/inactive track portions, thumb with elevation,
 *          tick marks, and state layers.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT Slider : public QSlider {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Defaults to horizontal orientation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit Slider(QWidget* parent = nullptr);

    /**
     * @brief  Constructor with orientation.
     *
     * @param[in]     orientation Qt::Horizontal or Qt::Vertical.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit Slider(Qt::Orientation orientation, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~Slider() override;

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the slider.
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
     * @brief  Paints the slider.
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
     * @brief  Handles mouse move event.
     *
     * @param[in]     event Mouse event.
     *
     * @throws        None
     * @note          Updates drag state.
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

  private:
    // Drawing helpers - Material Design paint pipeline
    QRectF trackRect() const;
    QRectF activeTrackRect() const;
    QRectF thumbRect() const;
    void drawInactiveTrack(QPainter& p, const QRectF& rect);
    void drawActiveTrack(QPainter& p, const QRectF& rect);
    void drawTickMarks(QPainter& p, const QRectF& trackRect);
    void drawThumb(QPainter& p, const QRectF& rect);
    void drawRipple(QPainter& p, const QRectF& rect);
    void drawFocusIndicator(QPainter& p, const QRectF& trackRect);

    // Color access methods
    CFColor activeTrackColor() const;
    CFColor inactiveTrackColor() const;
    CFColor thumbColor() const;
    CFColor stateLayerColor() const;
    float trackHeight() const;
    float thumbRadius() const;
    float thumbDiameter() const;

    // Helper to calculate thumb position
    float thumbPosition() const;

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::RippleHelper* m_ripple;
    base::MdElevationController* m_elevation;
    base::MdFocusIndicator* m_focusIndicator;

    // Track last press position for ripple
    QPoint m_lastPressPos;
};

} // namespace cf::ui::widget::material
