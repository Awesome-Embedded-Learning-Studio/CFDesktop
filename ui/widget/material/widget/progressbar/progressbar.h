/**
 * @file    ui/widget/material/widget/progressBar/progressbar.h
 * @brief   Material Design 3 ProgressBar widget.
 *
 * Implements Material Design 3 progress bar with determinate and
 * indeterminate modes. Includes smooth animations, state layer,
 * and focus indicators following Material Design 3 specifications.
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
#include <QProgressBar>
#include <QWidget>

namespace cf::ui::widget::material {

// Forward declarations
namespace base {
class StateMachine;
class MdFocusIndicator;
} // namespace base

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 ProgressBar widget.
 *
 * @details Implements Material Design 3 progress bar with determinate and
 *          indeterminate modes. Includes smooth animations, state layer,
 *          and focus indicators following Material Design 3 specifications.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT ProgressBar : public QProgressBar {
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
    explicit ProgressBar(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~ProgressBar() override;

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the progress bar.
     *
     * @throws        None
     * @note          Based on Material Design 4dp height with padding.
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
     * @brief  Paints the progress bar.
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
    void drawBackground(QPainter& p, const QRectF& rect);
    void drawFill(QPainter& p, const QRectF& trackRect);
    void drawIndeterminate(QPainter& p, const QRectF& trackRect);
    void drawText(QPainter& p, const QRectF& rect);
    void drawFocusIndicator(QPainter& p, const QRectF& rect);

    // Color access methods
    CFColor trackColor() const;
    CFColor fillColor() const;
    CFColor textColor() const;
    CFColor stateLayerColor() const;
    float cornerRadius() const;
    float trackHeight() const;

    // Indeterminate animation
    void startIndeterminateAnimation();
    void stopIndeterminateAnimation();
    void updateIndeterminatePosition();

    // Behavior components
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::StateMachine* m_stateMachine;
    base::MdFocusIndicator* m_focusIndicator;

    // Indeterminate animation state (0.0 to 1.0)
    float m_indeterminatePosition = 0.0f;
    bool m_indeterminateAnimating = false;
};

} // namespace cf::ui::widget::material
