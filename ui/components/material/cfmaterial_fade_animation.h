/**
 * @file    cfmaterial_fade_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Fade Animation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements a fade animation that follows Material Design 3 motion
 * specifications. Animates the opacity of a widget from a start value
 * to an end value using the specified timing and easing curve.
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
#include <QGraphicsOpacityEffect>
#include <QWidget>

namespace cf::ui::components::material {

/**
 * @brief  Material Design 3 Fade Animation.
 *
 * @details Animates the opacity of a widget from a start value to an end
 *          value following Material Design 3 motion specifications.
 *
 *          The animation uses:
 *          - Duration from the MotionSpec token
 *          - Easing curve from the MotionSpec token
 *          - Optional delay before starting
 *
 *          Opacity is applied via QGraphicsOpacityEffect for widgets
 *          that support it.
 *
 * @note    The target widget must remain valid during animation.
 * @warning Changing the target widget during animation may cause issues.
 * @since   0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Create a fade-in animation
 * auto& motionSpec = theme.motion_spec();
 * auto fadeAnim = std::make_unique<CFMaterialFadeAnimation>(
 *     cf::WeakPtr<cf::ui::core::IMotionSpec>(&motionSpec),
 *     this);
 * fadeAnim->setRange(0.0f, 1.0f);  // Fade from transparent to opaque
 * fadeAnim->setTargetWidget(myWidget);
 * fadeAnim->start();
 *
 * // Connect to progress signal
 * connect(fadeAnim.get(), &ICFAbstractAnimation::progressChanged,
 *         this, [](float progress) {
 *     qDebug() << "Fade progress:" << progress;
 * });
 * @endcode
 */
class CF_UI_EXPORT CFMaterialFadeAnimation : public ICFTimingAnimation {
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
    explicit CFMaterialFadeAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @since 0.1
     */
    ~CFMaterialFadeAnimation() override;

    // Non-copyable, non-movable
    CFMaterialFadeAnimation(const CFMaterialFadeAnimation&) = delete;
    CFMaterialFadeAnimation& operator=(const CFMaterialFadeAnimation&) = delete;
    CFMaterialFadeAnimation(CFMaterialFadeAnimation&&) = delete;
    CFMaterialFadeAnimation& operator=(CFMaterialFadeAnimation&&) = delete;

    // =========================================================================
    // ICFAbstractAnimation Interface
    // =========================================================================

    /**
     * @brief  Start the fade animation.
     *
     * @param[in]     dir Direction to play (Forward = fade in, Backward = fade out).
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
     * @since 0.1
     */
    void pause() override;

    /**
     * @brief  Stop the animation and reset to initial state.
     *
     * @since 0.1
     */
    void stop() override;

    /**
     * @brief  Reverse the animation direction.
     *
     * @since 0.1
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
     * @brief  Get the current opacity value.
     *
     * @return Current opacity (0.0 = transparent, 1.0 = opaque).
     *
     * @since 0.1
     */
    float currentValue() const override { return currentOpacity_; }

    // =========================================================================
    // Fade-Specific Methods
    // =========================================================================

    /**
     * @brief  Set the target widget for the fade animation.
     *
     * @details The widget's opacity is animated. The widget must
     *          remain valid for the duration of the animation.
     *
     * @param[in]     widget Target widget (may be nullptr).
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     *
     * @code
     * fadeAnim->setTargetWidget(myLabel);
     * @endcode
     */
    void setTargetWidget(QWidget* widget);

    /**
     * @brief  Get the target widget.
     *
     * @return Target widget, or nullptr if not set.
     *
     * @since 0.1
     */
    QWidget* targetWidget() const { return targetWidget_; }

    /**
     * @brief  Get a weak pointer to this animation.
     *
     * @return WeakPtr that can be used to safely access this animation.
     *
     * @since 0.1
     */
    cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() override { return weak_factory_.GetWeakPtr(); }

  private:
    /// Current opacity value (0.0 to 1.0)
    float currentOpacity_ = 1.0f;

    /// Target widget to animate
    QWidget* targetWidget_ = nullptr;

    /// Graphics effect for opacity (created/owned by this animation)
    QGraphicsOpacityEffect* opacityEffect_ = nullptr;

    /// Whether the animation owns the opacity effect
    bool ownsOpacityEffect_ = false;

    /// Duration in milliseconds (from motion spec)
    int durationMs_ = 200;

    /// Delay in milliseconds before starting
    int delayMs_ = 0;

    /// Elapsed time (including delay)
    int elapsedTime_ = 0;

    /**
     * @brief  Apply the current opacity to the target widget.
     *
     * @param  opacity Opacity value (0.0 to 1.0).
     *
     * @since 0.1
     */
    void applyOpacity(float opacity);

    /**
     * @brief  Ensure the target widget has an opacity effect.
     *
     * @since 0.1
     */
    void ensureOpacityEffect();

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
