/**
 * @file    cfmaterial_property_animation.h
 * @brief   Material Design 3 Property Animation
 * @version 0.1
 * @date    2026-03-01
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements a property animation that follows Material Design 3 motion
 * specifications. Animates a float property from a start value to an end value
 * using the specified duration and easing curve.
 *
 * This animation is designed for simple property animations like:
 * - Floating label progress (0.0 to 1.0)
 * - Scale animations
 * - Opacity transitions
 * - Any float property animation
 *
 * @ingroup ui_components_material
 */
#pragma once

#include "base/easing.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include "components/timing_animation.h"
#include "core/motion_spec.h"
#include "export.h"
#include <QObject>

namespace cf::ui::components::material {

/**
 * @brief  Material Design 3 Property Animation.
 *
 * @details Animates a float property from a start value to an end value
 *          following Material Design 3 motion specifications.
 *
 *          The animation uses:
 *          - Duration specified in milliseconds
 *          - Easing curve for smooth motion
 *          - Direct property update via pointer
 *
 * @note    The property pointer must remain valid during animation.
 * @warning None
 * @since   0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Create a property animation
 * float scale = 0.0f;
 * auto propAnim = std::make_unique<CFMaterialPropertyAnimation>(
 *     &scale, 0.0f, 1.0f, 200, Easing::Type::EmphasizedDecelerate, this);
 *
 * // Connect to finished signal
 * connect(propAnim.get(), &CFMaterialPropertyAnimation::finished,
 *         this, [&scale]() {
 *     qDebug() << "Final scale:" << scale;
 * });
 *
 * // Start the animation
 * propAnim->start();
 * @endcode
 */
class CF_UI_EXPORT CFMaterialPropertyAnimation : public ICFAbstractAnimation {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor with property pointer.
     *
     * @param[in]     value Pointer to the float property to animate.
     *                     Must remain valid for the lifetime of this animation.
     * @param[in]     from Start value of the animation.
     * @param[in]     to End value of the animation.
     * @param[in]     durationMs Duration of the animation in milliseconds.
     * @param[in]     easing Easing type for the animation.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          The value pointer must remain valid for the lifetime
     *                of this animation.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    CFMaterialPropertyAnimation(
        float* value, float from, float to, int durationMs,
        base::Easing::Type easing = base::Easing::Type::EmphasizedDecelerate,
        QObject* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @since 0.1
     */
    ~CFMaterialPropertyAnimation() override;

    // Non-copyable, non-movable
    CFMaterialPropertyAnimation(const CFMaterialPropertyAnimation&) = delete;
    CFMaterialPropertyAnimation& operator=(const CFMaterialPropertyAnimation&) = delete;
    CFMaterialPropertyAnimation(CFMaterialPropertyAnimation&&) = delete;
    CFMaterialPropertyAnimation& operator=(CFMaterialPropertyAnimation&&) = delete;

    // =========================================================================
    // ICFAbstractAnimation Interface
    // =========================================================================

    /**
     * @brief  Start the animation.
     *
     * @param[in] dir Direction to play the animation.
     *
     * @since 0.1
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
     * @brief  Update animation state by delta time.
     *
     * @param[in] dt Delta time in milliseconds.
     *
     * @return     true if animation is still running, false if finished.
     *
     * @since      0.1
     */
    bool tick(int dt) override;

    /**
     * @brief  Gets a weak pointer to this animation.
     *
     * @return     Weak pointer to this animation instance.
     *
     * @throws     None
     * @note       Provides safe access to potentially destroyed animation.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components_material
     */
    cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() override { return weak_factory_.GetWeakPtr(); }

    // =========================================================================
    // Property-Specific Methods
    // =========================================================================

    /**
     * @brief  Set the target widget for repaint notifications.
     *
     * @details The widget receives update() calls during animation
     *          to trigger repaints.
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
     * @brief  Get the current value.
     *
     * @return Current animated value.
     *
     * @since 0.1
     */
    float currentValue() const { return m_value ? *m_value : 0.0f; }

    /**
     * @brief  Set the value range for the animation.
     *
     * @details Allows reusing the same animation object with different
     *          start/end values. Call before start() to update the range.
     *
     * @param[in] from Start value of the animation.
     * @param[in] to End value of the animation.
     *
     * @since 0.1
     */
    void setRange(float from, float to) {
        m_from = from;
        m_to = to;
    }

  private slots:
    /**
     * @brief  Internal timer tick handler.
     *
     * @since 0.1
     */
    void onTimerTick();

  private:
    /// Pointer to the float property being animated
    float* m_value;

    /// Start value
    float m_from;

    /// End value
    float m_to;

    /// Duration in milliseconds
    int m_durationMs;

    /// Easing type
    base::Easing::Type m_easing;

    /// Elapsed time in milliseconds
    int m_elapsed;

    /// Target widget for repaint notifications
    QWidget* m_targetWidget;

    /// Internal timer
    QTimer* m_timer;

    /// Target FPS for this animation (default 60.0f)
    float m_targetFps = 60.0f;

    /**
     * @brief  Calculate the timer interval based on target FPS.
     *
     * @return Timer interval in milliseconds.
     */
    int calculateInterval() const { return static_cast<int>(1000.0f / m_targetFps); }

    /// WeakPtrFactory for creating weak pointers to this animation
    /// Must be the last member to ensure it's destroyed first
    cf::WeakPtrFactory<ICFAbstractAnimation> weak_factory_{this};
};

} // namespace cf::ui::components::material
