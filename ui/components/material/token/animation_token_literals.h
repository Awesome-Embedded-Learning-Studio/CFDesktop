/**
 * @file    animation_token_literals.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Animation Token String Literals
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Compile-time string constants for Material Design 3 animation tokens.
 * These literals are used throughout the animation system to ensure
 * consistency and avoid string duplication.
 *
 * Animation tokens follow the naming pattern: "md.animation.<type><direction>"
 * where type is "fade", "slide", "scale", etc. and direction is "In", "Out".
 *
 * @ingroup ui_components_material
 */
#pragma once

#include <cstdint>

namespace cf::ui::components::material::token_literals {

// =============================================================================
// Animation Token Literals - Fade Animations
// =============================================================================

/**
 * @brief  Fade-in animation token.
 *
 * @details Opacity animation from 0 to 1. Uses shortEnter timing by default.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_FADE_IN[] = "md.animation.fadeIn";

/**
 * @brief  Fade-out animation token.
 *
 * @details Opacity animation from 1 to 0. Uses shortExit timing by default.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_FADE_OUT[] = "md.animation.fadeOut";

// =============================================================================
// Animation Token Literals - Slide Animations
// =============================================================================

/**
 * @brief  Slide-up animation token.
 *
 * @details Position animation sliding upward. Uses mediumEnter timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_SLIDE_UP[] = "md.animation.slideUp";

/**
 * @brief  Slide-down animation token.
 *
 * @details Position animation sliding downward. Uses mediumExit timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_SLIDE_DOWN[] = "md.animation.slideDown";

/**
 * @brief  Slide-left animation token.
 *
 * @details Position animation sliding left. Uses mediumEnter timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_SLIDE_LEFT[] = "md.animation.slideLeft";

/**
 * @brief  Slide-right animation token.
 *
 * @details Position animation sliding right. Uses mediumEnter timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_SLIDE_RIGHT[] = "md.animation.slideRight";

// =============================================================================
// Animation Token Literals - Scale Animations
// =============================================================================

/**
 * @brief  Scale-up animation token.
 *
 * @details Size animation growing from smaller to normal. Uses shortEnter timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_SCALE_UP[] = "md.animation.scaleUp";

/**
 * @brief  Scale-down animation token.
 *
 * @details Size animation shrinking from normal to smaller. Uses shortExit timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_SCALE_DOWN[] = "md.animation.scaleDown";

// =============================================================================
// Animation Token Literals - Rotate Animations
// =============================================================================

/**
 * @brief  Rotate-in animation token.
 *
 * @details Rotation animation for entering elements. Uses mediumEnter timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_ROTATE_IN[] = "md.animation.rotateIn";

/**
 * @brief  Rotate-out animation token.
 *
 * @details Rotation animation for exiting elements. Uses mediumExit timing.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char ANIMATION_ROTATE_OUT[] = "md.animation.rotateOut";

// =============================================================================
// All Animation Tokens Array (for iteration)
// =============================================================================

/**
 * @brief  Array containing all Material Design 3 animation token literals.
 *
 * @details Ordered by category: Fade (2), Slide (4), Scale (2), Rotate (2).
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr const char* const ALL_ANIMATION_TOKENS[] = {
    // Fade
    ANIMATION_FADE_IN, ANIMATION_FADE_OUT,
    // Slide
    ANIMATION_SLIDE_UP, ANIMATION_SLIDE_DOWN, ANIMATION_SLIDE_LEFT, ANIMATION_SLIDE_RIGHT,
    // Scale
    ANIMATION_SCALE_UP, ANIMATION_SCALE_DOWN,
    // Rotate
    ANIMATION_ROTATE_IN, ANIMATION_ROTATE_OUT};

/**
 * @brief  Total count of Material Design 3 animation tokens.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr size_t ANIMATION_TOKEN_COUNT = 10;

} // namespace cf::ui::components::material::token_literals
