/**
 * @file    spring_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Spring-based Animation Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the spring-based animation interface that uses spring physics
 * for natural motion. This provides animations that follow spring dynamics
 * rather than fixed timing curves.
 *
 * @ingroup ui_components
 */
#pragma once

#include "animation.h"
#include "base/easing.h"
#include <QObject>

namespace cf::ui::components {

/**
 * @brief  Spring-based animation interface.
 *
 * @details Base class for animations that use spring physics for
 *          natural, bouncy motion following Material Design 3.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFSpringAnimation : public ICFAbstractAnimation {
    Q_OBJECT
  public:
    /**
     * @brief  Constructor with spring easing preset.
     *
     * @param[in] easing Spring preset for physics parameters.
     * @param[in] parent QObject parent.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    ICFSpringAnimation(const base::Easing::SpringPreset& easing, QObject* parent = nullptr)
        : ICFAbstractAnimation(parent) {
        easing_ = easing;
    }

    /**
     * @brief  Sets the target value for the spring animation.
     *
     * @param[in] target Target value to animate toward.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    virtual void setTarget(float target) { m_target = target; }

    /**
     * @brief  Sets the initial velocity for the spring animation.
     *
     * @param[in] velocity Initial velocity value.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    virtual void setInitialVelocity(float velocity) { m_velocity = velocity; }

    /**
     * @brief  Gets the current animated value.
     *
     * @return        Current animated value.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    virtual float currentValue() const = 0;

    /**
     * @brief  Updates the spring animation state.
     *
     * @details Uses springStep for physics-based interpolation.
     *
     * @param[in] dt Time elapsed since last tick (milliseconds).
     *
     * @return        true if animation continues, false if finished.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    bool tick(int dt) override; // Using springStep

  protected:
    /// Spring easing preset containing physics parameters (stiffness, damping).
    base::Easing::SpringPreset easing_;

    /// Current position value in the spring animation.
    float m_position = 0.0f;

    /// Current velocity value in the spring animation.
    float m_velocity = 0.0f;

    /// Target value for the spring animation to settle at.
    float m_target = 1.0f;
};

} // namespace cf::ui::components
