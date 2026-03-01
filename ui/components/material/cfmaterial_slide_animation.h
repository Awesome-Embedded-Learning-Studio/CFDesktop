/**
 * @file    cfmaterial_slide_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Slide Animation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements a slide animation that follows Material Design 3 motion
 * specifications. Animates the position of a widget from a start offset
 * to an end offset using the specified timing and easing curve.
 *
 * This animation integrates with the MaterialMotionScheme to obtain
 * duration and easing values based on the motion token (e.g., "md.motion.mediumEnter").
 *
 * @ingroup ui_components_material
 */
#pragma once

#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include "components/timing_animation.h"
#include "core/motion_spec.h"
#include "export.h"
#include <QPoint>
#include <QWidget>

namespace cf::ui::components::material {

/**
 * @brief  Slide direction enumeration.
 *
 * @details Defines the four cardinal directions for slide animations.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
enum class SlideDirection {
    Up,   ///< Slide upward (widget moves from bottom to top)
    Down, ///< Slide downward (widget moves from top to bottom)
    Left, ///< Slide leftward (widget moves from right to left)
    Right ///< Slide rightward (widget moves from left to right)
};

/**
 * @brief  Material Design 3 Slide Animation.
 *
 * @details Animates the position of a widget from a start offset to an
 *          end offset following Material Design 3 motion specifications.
 *
 *          The animation uses:
 *          - Duration from the MotionSpec token
 *          - Easing curve from the MotionSpec token
 *          - Optional delay before starting
 *
 *          Position is applied via widget movement using setGeometry()
 *          or pos(). The original position is stored and restored after
 *          animation completion.
 *
 * @note    The target widget must remain valid during animation.
 * @warning Changing the target widget during animation may cause issues.
 * @since   0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Create a slide-up animation
 * auto& motionSpec = theme.motion_spec();
 * auto slideAnim = std::make_unique<CFMaterialSlideAnimation>(
 *     cf::WeakPtr<cf::ui::core::IMotionSpec>(&motionSpec),
 *     SlideDirection::Up,
 *     this);
 * slideAnim->setDistance(100.0f);  // Slide 100 pixels
 * slideAnim->setTargetWidget(myWidget);
 * slideAnim->start();
 * @endcode
 */
class CF_UI_EXPORT CFMaterialSlideAnimation : public ICFTimingAnimation {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor with motion spec and direction.
     *
     * @param[in]     spec Raw pointer to the motion spec for timing/easing.
     *                    Must remain valid for the lifetime of this animation.
     * @param[in]     direction Direction of the slide animation.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    explicit CFMaterialSlideAnimation(cf::ui::core::IMotionSpec* spec, SlideDirection direction,
                                      QObject* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    ~CFMaterialSlideAnimation() override;

    // Non-copyable, non-movable
    CFMaterialSlideAnimation(const CFMaterialSlideAnimation&) = delete;
    CFMaterialSlideAnimation& operator=(const CFMaterialSlideAnimation&) = delete;
    CFMaterialSlideAnimation(CFMaterialSlideAnimation&&) = delete;
    CFMaterialSlideAnimation& operator=(CFMaterialSlideAnimation&&) = delete;

    // =========================================================================
    // ICFAbstractAnimation Interface
    // =========================================================================

    /**
     * @brief  Start the slide animation.
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
     * @brief  Get the current offset value.
     *
     * @return Current offset in pixels.
     *
     * @since 0.1
     */
    float currentValue() const override { return currentOffset_; }

    // =========================================================================
    // Slide-Specific Methods
    // =========================================================================

    /**
     * @brief  Set the target widget for the slide animation.
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
     * @brief  Set the slide distance in pixels.
     *
     * @param[in]     distance Distance to slide in pixels.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void setDistance(float distance) { distance_ = distance; }

    /**
     * @brief  Get the slide distance.
     *
     * @return        Distance in pixels.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    float distance() const { return distance_; }

    /**
     * @brief  Set the slide direction.
     *
     * @param[in]     direction Direction to slide.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void setDirection(SlideDirection direction) { direction_ = direction; }

    /**
     * @brief  Get the slide direction.
     *
     * @return Current slide direction.
     *
     * @since 0.1
     */
    SlideDirection direction() const { return direction_; }

    /**
     * @brief  Get a weak pointer to this animation.
     *
     * @return WeakPtr that can be used to safely access this animation.
     *
     * @since 0.1
     */
    cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() override { return weak_factory_.GetWeakPtr(); }

  private:
    /// Current offset value in pixels
    float currentOffset_ = 0.0f;

    /// Target widget to animate
    QWidget* targetWidget_ = nullptr;

    /// Original position of the widget (stored at animation start)
    QPoint originalPosition_;

    /// Direction of the slide
    SlideDirection direction_ = SlideDirection::Up;

    /// Distance to slide in pixels
    float distance_ = 100.0f;

    /// Duration in milliseconds
    int durationMs_ = 300;

    /// Delay in milliseconds before starting
    int delayMs_ = 0;

    /// Elapsed time (including delay)
    int elapsedTime_ = 0;

    /**
     * @brief  Apply the current offset to the target widget.
     *
     * @param  offset Offset value in pixels.
     *
     * @since 0.1
     */
    void applyOffset(float offset);

    /**
     * @brief  Calculate the offset point for a given offset value.
     *
     * @param  offset Offset distance in pixels.
     *
     * @return QPoint representing the offset.
     *
     * @since 0.1
     */
    QPoint calculateOffsetPoint(float offset) const;

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
