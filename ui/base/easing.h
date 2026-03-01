/**
 * @file ui/base/easing.h
 * @brief Easing curves and spring animation presets.
 *
 * Provides convenient access to predefined easing curves and spring
 * physics presets for smooth animations following Material Design motion
 * principles.
 *
 * @author Charliechen114514
 * @date 2026-02-23
 * @version 0.1
 * @since 0.1
 * @ingroup ui
 */
#pragma once
#include <QEasingCurve>

namespace cf::ui::base {
/**
 * @brief Easing curves and spring presets for animations.
 *
 * Provides convenient access to predefined easing curves following
 * Material Design motion principles.
 *
 * @ingroup ui
 */
namespace Easing {

/**
 * @brief Predefined easing curve types.
 *
 * Defines standard easing curves used for animations following
 * Material Design motion specifications.
 *
 * @ingroup ui
 */
enum class Type {
    Emphasized,           ///< Emphasized easing with acceleration and deceleration.
    EmphasizedDecelerate, ///< Emphasized deceleration easing.
    EmphasizedAccelerate, ///< Emphasized acceleration easing.
    Standard,             ///< Standard easing with acceleration and deceleration.
    StandardDecelerate,   ///< Standard deceleration easing.
    StandardAccelerate,   ///< Standard acceleration easing.
    Linear                ///< Linear easing (no acceleration).
};

/**
 * @brief Converts an easing type to a QEasingCurve.
 *
 * @param[in] t The easing type to convert.
 * @return QEasingCurve corresponding to the specified type.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
QEasingCurve fromEasingType(const Type t);

/**
 * @brief Creates a custom cubic bezier easing curve.
 *
 * @param[in] x1 First control point X coordinate (0.0 to 1.0).
 * @param[in] y1 First control point Y coordinate.
 * @param[in] x2 Second control point X coordinate (0.0 to 1.0).
 * @param[in] y2 Second control point Y coordinate.
 * @return Custom QEasingCurve with the specified control points.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
QEasingCurve custom(float x1, float y1, float x2, float y2);

/**
 * @brief Spring physics preset for animations.
 *
 * Defines stiffness and damping parameters for spring-based
 * animations.
 *
 * @ingroup ui
 */
struct SpringPreset {
    float stiffness; ///< Spring stiffness coefficient.
    float damping;   ///< Spring damping coefficient.
};

/**
 * @brief Returns a gentle spring preset.
 *
 * @return SpringPreset with gentle stiffness and damping.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
SpringPreset springGentle();

/**
 * @brief Returns a bouncy spring preset.
 *
 * @return SpringPreset with high bounciness.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
SpringPreset springBouncy();

/**
 * @brief Returns a stiff spring preset.
 *
 * @return SpringPreset with high stiffness.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
SpringPreset springStiff();

} // namespace Easing

} // namespace cf::ui::base
