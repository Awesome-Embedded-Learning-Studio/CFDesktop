/**
 * @file    cfmaterial_scale_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Scale Animation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements a scale animation that follows Material Design 3 motion
 * specifications. Animates the size of a widget from a start scale
 * to an end scale using the specified timing and easing curve.
 *
 * This animation integrates with the MaterialMotionScheme to obtain
 * duration and easing values based on the motion token (e.g., "md.motion.shortEnter").
 *
 * @ingroup ui_components_material
 */
#pragma once

#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include "components/timing_animation.h"
#include "core/motion_spec.h"
#include "export.h"
#include <QTransform>
#include <QWidget>

namespace cf::ui::components::material {

/**
 * @brief  Material Design 3 Scale Animation.
 *
 * @details Animates the scale of a widget from a start value to an end
 *          value following Material Design 3 motion specifications.
 *
 *          The animation uses:
 *          - Duration from the MotionSpec token
 *          - Easing curve from the MotionSpec token
 *          - Optional delay before starting
 *
 *          Scale is applied via QWidget::setGeometry() with adjusted
 *          size and position to maintain the widget's center point.
 *
 * @note    The target widget must remain valid during animation.
 * @warning Changing the target widget during animation may cause issues.
 * @since   0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Create a scale-up animation
 * auto& motionSpec = theme.motion_spec();
 * auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(
 *     cf::WeakPtr<cf::ui::core::IMotionSpec>(&motionSpec),
 *     this);
 * scaleAnim->setRange(0.8f, 1.0f);  // Scale from 80% to 100%
 * scaleAnim->setTargetWidget(myWidget);
 * scaleAnim->start();
 * @endcode
 */
class CF_UI_EXPORT CFMaterialScaleAnimation : public ICFTimingAnimation {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor with motion spec.
     *
     * @param[in]     spec Raw pointer to the motion spec for timing/easing.
     *                    Must remain valid for the lifetime of this animation.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    explicit CFMaterialScaleAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @since 0.1
     */
    ~CFMaterialScaleAnimation() override;

    // Non-copyable, non-movable
    CFMaterialScaleAnimation(const CFMaterialScaleAnimation&) = delete;
    CFMaterialScaleAnimation& operator=(const CFMaterialScaleAnimation&) = delete;
    CFMaterialScaleAnimation(CFMaterialScaleAnimation&&) = delete;
    CFMaterialScaleAnimation& operator=(CFMaterialScaleAnimation&&) = delete;

    // =========================================================================
    // ICFAbstractAnimation Interface
    // =========================================================================

    /**
     * @brief  Start the scale animation.
     *
     * @param[in]     dir Direction to play.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void start(Direction dir = Direction::Forward) override;

    /**
     * @brief  Pause the animation.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void pause() override;

    /**
     * @brief  Stop the animation and reset to initial state.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void stop() override;

    /**
     * @brief  Reverse the animation direction.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void reverse() override;

    /**
     * @brief  Update animation state.
     *
     * @param[in]     dt Time elapsed since last tick (milliseconds).
     *
     * @return        true if animation is still running, false if finished.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    bool tick(int dt) override;

    // =========================================================================
    // ICFTimingAnimation Interface
    // =========================================================================

    /**
     * @brief  Get the current scale value.
     *
     * @return Current scale (1.0 = normal size).
     *
     * @since 0.1
     */
    float currentValue() const override { return currentScale_; }

    // =========================================================================
    // Scale-Specific Methods
    // =========================================================================

    /**
     * @brief  Set the target widget for the scale animation.
     *
     * @param[in]     widget Target widget (may be nullptr).
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void setTargetWidget(QWidget* widget);

    /**
     * @brief  Get the target widget.
     *
     * @return        Target widget, or nullptr if not set.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    QWidget* targetWidget() const { return targetWidget_; }

    /**
     * @brief  Set whether to scale from center.
     *
     * @param[in]     center If true, widget scales from its center point.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void setScaleFromCenter(bool center) { scaleFromCenter_ = center; }

    /**
     * @brief  Check if scaling from center.
     *
     * @return true if scaling from center.
     *
     * @since 0.1
     */
    bool scaleFromCenter() const { return scaleFromCenter_; }

    /**
     * @brief  Get a weak pointer to this animation.
     *
     * @return WeakPtr that can be used to safely access this animation.
     *
     * @since 0.1
     */
    cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() override { return weak_factory_.GetWeakPtr(); }

  private:
    /// Current scale value (1.0 = normal size)
    float currentScale_ = 1.0f;

    /// Target widget to animate
    QWidget* targetWidget_ = nullptr;

    /// Original geometry of the widget (stored at animation start)
    QRect originalGeometry_;

    /// Whether to scale from the center point
    bool scaleFromCenter_ = true;

    /// Duration in milliseconds
    int durationMs_ = 200;

    /// Delay in milliseconds before starting
    int delayMs_ = 0;

    /// Elapsed time (including delay)
    int elapsedTime_ = 0;

    /**
     * @brief  Apply the current scale to the target widget.
     *
     * @param  scale Scale value (1.0 = normal size).
     *
     * @since 0.1
     */
    void applyScale(float scale);

    /**
     * @brief  Calculate the geometry for a given scale value.
     *
     * @param  scale Scale value.
     *
     * @return QRect representing the scaled geometry.
     *
     * @since 0.1
     */
    QRect calculateScaledGeometry(float scale) const;

    /**
     * @brief  Calculate eased progress based on timing function.
     *
     * @param  linearProgress Linear progress (0.0 to 1.0).
     *
     * @return Eased progress (0.0 to 1.0).
     *
     * @since 0.1
     */
    float calculateEasedProgress(float linearProgress) const;

    /// WeakPtrFactory for creating weak pointers to this animation
    /// Must be the last member to ensure it's destroyed first
    cf::WeakPtrFactory<ICFAbstractAnimation> weak_factory_{this};
};

} // namespace cf::ui::components::material
