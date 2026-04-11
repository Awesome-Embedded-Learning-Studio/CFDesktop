/**
 * @file    ui/base/math_helper.h
 * @brief   Mathematical utility functions for UI animations and transitions.
 *
 * Provides interpolation, clamping, remapping, cubic Bezier curves,
 * spring physics simulation, and angle interpolation functions for UI components.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-02-23
 * @version 0.1
 * @since   0.1
 * @ingroup ui
 */

#pragma once
#include "export.h"
#include <utility>

namespace cf::ui {

/**
 * @brief Mathematical utilities for UI animations and transitions.
 *
 * Provides functions for value interpolation, clamping, remapping,
 * cubic Bezier easing curves, spring physics simulation, and angle
 * interpolation with boundary handling.
 *
 * @note    None of these functions perform thread synchronization.
 * @ingroup ui
 */
namespace math {

/**
 * @brief  Performs linear interpolation between two values.
 *
 * Computes the linear interpolation between `a` and `b` using parameter `t`.
 * When `t` is 0, returns `a`; when `t` is 1, returns `b`. Values outside
 * [0, 1] produce extrapolated results.
 *
 * @param[in] a  Start value.
 * @param[in] b  End value.
 * @param[in] t  Interpolation parameter. Valid range: [0.0, 1.0] for normal
 *               interpolation; values outside produce extrapolation.
 * @return        Interpolated value between `a` and `b`.
 * @throws       None
 * @note         Commonly used for smooth transitions and animations.
 * @warning      No bounds checking on `t`; extrapolation occurs for t < 0 or t > 1.
 * @since        0.1
 * @ingroup      ui
 */
CF_UI_EXPORT float lerp(float a, float b, float t);

/**
 * @brief  Clamps a value to the specified range.
 *
 * Constrains `value` to the inclusive range [`min`, `max`]. If `value`
 * is less than `min`, returns `min`; if greater than `max`, returns `max`.
 *
 * @param[in] value  Value to clamp.
 * @param[in] min    Minimum bound (inclusive).
 * @param[in] max    Maximum bound (inclusive).
 * @return           Clamped value in range [`min`, `max`].
 * @throws          None
 * @note            This function does not validate that min <= max.
 * @warning         Behavior is undefined if `min` > `max`.
 * @since           0.1
 * @ingroup         ui
 */
CF_UI_EXPORT float clamp(float value, float min, float max);

/**
 * @brief  Remaps a value from one range to another.
 *
 * Transforms `value` from the input range [`inMin`, `inMax`] to the
 * output range [`outMin`, `outMax`]. Preserves relative position within
 * each range.
 *
 * @param[in] value  Input value to remap.
 * @param[in] inMin  Input range minimum.
 * @param[in] inMax  Input range maximum.
 * @param[in] outMin Output range minimum.
 * @param[in] outMax Output range maximum.
 * @return           Remapped value in output range.
 * @throws          None
 * @note            If `inMin == inMax`, returns `outMax` when value >= inMin,
 *                  otherwise returns `outMin`.
 * @warning         Behavior is undefined if `inMin == inMax` and `value` is NaN.
 * @since           0.1
 * @ingroup         ui
 */
CF_UI_EXPORT float remap(float value, float inMin, float inMax, float outMin, float outMax);

/**
 * @brief  Evaluates a cubic Bezier easing curve.
 *
 * Computes the y-value at parameter `t` for a cubic Bezier curve with
 * fixed endpoints P0=(0,0) and P3=(1,1), and control points P1=(x1,y1)
 * and P2=(x2,y2). Used for custom easing functions in animations.
 *
 * @param[in] x1  First control point X coordinate. Valid range: [0.0, 1.0].
 * @param[in] y1  First control point Y coordinate. No range restriction.
 * @param[in] x2  Second control point X coordinate. Valid range: [0.0, 1.0].
 * @param[in] y2  Second control point Y coordinate. No range restriction.
 * @param[in] t   Curve parameter. Valid range: [0.0, 1.0].
 * @return        Y-value along the Bezier curve at parameter `t`.
 * @throws       None
 * @note         This is the explicit formula for y(t), not a solver for x(t).
 *               For animation easing, typically assumes x(t) ≈ t.
 * @warning      Values of `t` outside [0.0, 1.0] produce extrapolated results.
 * @since        0.1
 * @ingroup      ui
 */
CF_UI_EXPORT float cubicBezier(float x1, float y1, float x2, float y2, float t);

/**
 * @brief  Performs a single step of spring physics simulation.
 *
 * Simulates one time step of a damped spring using semi-implicit Euler
 * integration. Returns the new position and velocity after applying
 * spring forces toward the target.
 *
 * @param[in] position  Current position.
 * @param[in] velocity  Current velocity.
 * @param[in] target    Target position the spring pulls toward.
 * @param[in] stiffness Spring stiffness coefficient. Higher values create
 *                      stronger, faster springs. Typical range: 100-500.
 * @param[in] damping   Spring damping coefficient. Lower values create more
 *                      oscillation. Typical range: 5-30.
 * @param[in] dt        Time step in seconds. Typical value: 1/60 (60 FPS).
 * @return              Pair containing [new_position, new_velocity].
 * @throws             None
 * @note                Stability requires `dt` to be small relative to
 *                      stiffness. Large `stiffness` * `dt` may cause instability.
 * @warning             Negative `stiffness` creates an unstable (repulsive) spring.
 * @since               0.1
 * @ingroup             ui
 */
CF_UI_EXPORT std::pair<float, float> springStep(float position, float velocity, float target,
                                                float stiffness, float damping, float dt);

/**
 * @brief  Performs linear interpolation between two angles.
 *
 * Interpolates between angles `a` and `b` using parameter `t`, handling
 * the wraparound at 0°/360°. Always takes the shortest path around the
 * circle, whether clockwise or counter-clockwise.
 *
 * @param[in] a  Start angle in degrees. No range restriction.
 * @param[in] b  End angle in degrees. No range restriction.
 * @param[in] t  Interpolation parameter. Valid range: [0.0, 1.0].
 * @return        Interpolated angle in degrees.
 * @throws       None
 * @note         The result is normalized to [0, 360) range.
 * @warning      None
 * @since        0.1
 * @ingroup      ui
 */
CF_UI_EXPORT float lerpAngle(float a, float b, float t);

} // namespace math

} // namespace cf::ui
