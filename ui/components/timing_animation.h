/**
 * @file    timing_animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Timing-based Animation Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the timing-based animation interface that uses motion spec
 * for duration and easing values. This is the base class for animations
 * that follow Material Design 3 motion specifications.
 *
 * @ingroup ui_components
 */
#pragma once

#include "animation.h"
#include "core/motion_spec.h"
#include <qobject.h>

namespace cf::ui::components {

// Forward declaration
namespace core {
struct IMotionSpec;
}

/**
 * @brief  Timing-based animation interface.
 *
 * @details Base class for animations that use MotionSpec for timing
 *          and easing configuration following Material Design 3.
 *
 *          NOTE: The motion_spec pointer must remain valid for the
 *          lifetime of this animation. This is guaranteed when the
 *          animation is created by CFMaterialAnimationFactory, which
 *          holds a reference to the theme that owns the motion spec.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFTimingAnimation : public ICFAbstractAnimation {
    Q_OBJECT
  public:
    /**
     * @brief  Constructor with motion specification.
     *
     * @param[in] spec Raw pointer to the motion spec for timing/easing.
     *                  Must remain valid for the lifetime of this animation.
     * @param[in] parent QObject parent.
     *
     * @throws     None
     * @note       The motion_spec pointer must remain valid for the lifetime
     *             of this animation.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    explicit ICFTimingAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent = nullptr);

    /**
     * @brief  Sets the value range for the animation.
     *
     * @param[in] from Start value of the animation.
     * @param[in] to End value of the animation.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    virtual void setRange(float from, float to) {
        m_from = from;
        m_to = to;
    }

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

  protected:
    cf::ui::core::IMotionSpec* motion_spec_ = nullptr;
    float m_from = 0.0f;
    float m_to = 1.0f;
    int m_elapsed = 0;
};

} // namespace cf::ui::components
