/**
 * @file    animation_token_mapping.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Animation Token to MotionSpec Mapping
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the mapping between animation tokens and their corresponding
 * MotionSpec configurations. This enables the factory to resolve a
 * token like "md.animation.fadeIn" to a complete AnimationDescriptor
 * with animation type, motion spec, property, and default values.
 *
 * The mapping follows Material Design 3 motion principles:
 * - Fade: 200ms enter, 150ms exit
 * - Slide: 300ms enter, 250ms exit
 * - Scale: 200ms enter, 150ms exit
 * - Rotate: 300ms enter, 250ms exit
 *
 * @ingroup ui_components_material
 */
#pragma once

#include "animation_token_literals.h"
#include <cstring>

namespace cf::ui::components::material::token_literals {

// =============================================================================
// Animation Token → MotionSpec Mapping Structure
// =============================================================================

/**
 * @brief  Animation token mapping entry.
 *
 * @details Defines how an animation token maps to a concrete animation
 *          configuration. This includes:
 *          - The animation token string
 *          - The animation type (fade, slide, scale, etc.)
 *          - The motion spec token for timing/easing
 *          - The property to animate
 *          - Default start and end values
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
struct AnimationTokenMapping {
    /// The animation token (e.g., "md.animation.fadeIn")
    const char* animationToken;

    /// Animation type: "fade", "slide", "scale", "rotate"
    const char* animationType;

    /// Motion spec token: e.g., "md.motion.shortEnter"
    const char* motionToken;

    /// Property to animate: "opacity", "positionX", "positionY", "scale"
    const char* property;

    /// Default start value for the animation
    float defaultFrom;

    /// Default end value for the animation
    float defaultTo;

    /**
     * @brief  Check if this mapping matches a given token.
     *
     * @param[in]     token The token string to compare.
     *
     * @return        true if the animationToken matches, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    bool matches(const char* token) const {
        return animationToken && token &&
               std::strcmp(animationToken, token) == 0;
    }
};

// =============================================================================
// Animation Token Mapping Table
// =============================================================================

/**
 * @brief  Complete animation token mapping table.
 *
 * @details Defines all predefined animation tokens and their
 *          corresponding configurations. This table is used by
 *          MaterialAnimationFactory to resolve tokens to descriptors.
 *
 *          The table is ordered by animation category for easier lookup:
 *          - Fade animations (2 entries)
 *          - Slide animations (4 entries)
 *          - Scale animations (2 entries)
 *          - Rotate animations (2 entries)
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr AnimationTokenMapping TOKEN_MAPPINGS[] = {
    // =========================================================================
    // Fade Animations
    // =========================================================================
    {
        ANIMATION_FADE_IN,     // "md.animation.fadeIn"
        "fade",                // Animation type
        "md.motion.shortEnter",  // Motion spec (200ms, EmphasizedDecelerate)
        "opacity",             // Property to animate
        0.0f,                  // Start value (transparent)
        1.0f                   // End value (fully visible)
    },
    {
        ANIMATION_FADE_OUT,    // "md.animation.fadeOut"
        "fade",                // Animation type
        "md.motion.shortExit", // Motion spec (150ms, EmphasizedAccelerate)
        "opacity",             // Property to animate
        1.0f,                  // Start value (fully visible)
        0.0f                   // End value (transparent)
    },

    // =========================================================================
    // Slide Animations
    // =========================================================================
    {
        ANIMATION_SLIDE_UP,    // "md.animation.slideUp"
        "slide",               // Animation type
        "md.motion.mediumEnter", // Motion spec (300ms, EmphasizedDecelerate)
        "positionY",           // Property to animate
        100.0f,                // Start value (offset downward)
        0.0f                   // End value (normal position)
    },
    {
        ANIMATION_SLIDE_DOWN,  // "md.animation.slideDown"
        "slide",               // Animation type
        "md.motion.mediumExit", // Motion spec (250ms, EmphasizedAccelerate)
        "positionY",           // Property to animate
        0.0f,                  // Start value (normal position)
        100.0f                 // End value (offset downward)
    },
    {
        ANIMATION_SLIDE_LEFT,  // "md.animation.slideLeft"
        "slide",               // Animation type
        "md.motion.mediumEnter", // Motion spec (300ms, EmphasizedDecelerate)
        "positionX",           // Property to animate
        100.0f,                // Start value (offset right)
        0.0f                   // End value (normal position)
    },
    {
        ANIMATION_SLIDE_RIGHT, // "md.animation.slideRight"
        "slide",               // Animation type
        "md.motion.mediumEnter", // Motion spec (300ms, EmphasizedDecelerate)
        "positionX",           // Property to animate
        -100.0f,               // Start value (offset left)
        0.0f                   // End value (normal position)
    },

    // =========================================================================
    // Scale Animations
    // =========================================================================
    {
        ANIMATION_SCALE_UP,    // "md.animation.scaleUp"
        "scale",               // Animation type
        "md.motion.shortEnter", // Motion spec (200ms, EmphasizedDecelerate)
        "scale",               // Property to animate
        0.8f,                  // Start value (smaller)
        1.0f                   // End value (normal size)
    },
    {
        ANIMATION_SCALE_DOWN,  // "md.animation.scaleDown"
        "scale",               // Animation type
        "md.motion.shortExit", // Motion spec (150ms, EmphasizedAccelerate)
        "scale",               // Property to animate
        1.0f,                  // Start value (normal size)
        0.8f                   // End value (smaller)
    },

    // =========================================================================
    // Rotate Animations
    // =========================================================================
    {
        ANIMATION_ROTATE_IN,   // "md.animation.rotateIn"
        "rotate",              // Animation type
        "md.motion.mediumEnter", // Motion spec (300ms, EmphasizedDecelerate)
        "rotation",            // Property to animate
        -45.0f,                // Start value (rotated counter-clockwise)
        0.0f                   // End value (normal rotation)
    },
    {
        ANIMATION_ROTATE_OUT,  // "md.animation.rotateOut"
        "rotate",              // Animation type
        "md.motion.mediumExit", // Motion spec (250ms, EmphasizedAccelerate)
        "rotation",            // Property to animate
        0.0f,                  // Start value (normal rotation)
        45.0f                  // End value (rotated clockwise)
    },
};

/**
 * @brief  Total number of animation token mappings.
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
inline constexpr size_t TOKEN_MAPPING_COUNT =
    sizeof(TOKEN_MAPPINGS) / sizeof(AnimationTokenMapping);

// =============================================================================
// Token Resolution Functions
// =============================================================================

/**
 * @brief  Find a mapping entry by animation token.
 *
 * @details Performs a linear search through the mapping table.
 *          Returns nullptr if the token is not found.
 *
 * @param  token The animation token to look up.
 *
 * @return Pointer to the matching mapping entry, or nullptr if not found.
 *
 * @throws     None
 * @note       Linear search is acceptable for small tables (< 100 entries).
 * @warning    The returned pointer is valid only as long as TOKEN_MAPPINGS
 *             is accessible (meaning: program lifetime).
 * @since      0.1
 * @ingroup    ui_components_material
 *
 * @code
 * const auto* mapping = findTokenMapping("md.animation.fadeIn");
 * if (mapping) {
 *     // mapping->animationType == "fade"
 *     // mapping->motionToken == "md.motion.shortEnter"
 *     // mapping->property == "opacity"
 *     // mapping->defaultFrom == 0.0f
 *     // mapping->defaultTo == 1.0f
 * }
 * @endcode
 */
inline const AnimationTokenMapping* findTokenMapping(const char* token) {
    for (size_t i = 0; i < TOKEN_MAPPING_COUNT; ++i) {
        if (TOKEN_MAPPINGS[i].matches(token)) {
            return &TOKEN_MAPPINGS[i];
        }
    }
    return nullptr;
}

/**
 * @brief  Check if an animation token exists in the mapping table.
 *
 * @param[in]     token The animation token to check.
 *
 * @return        true if the token is found, false otherwise.
 *
 * @throws        None
 * @note          None
 * @warning       None
 * @since         0.1
 * @ingroup       ui_components_material
 */
inline bool hasTokenMapping(const char* token) {
    return findTokenMapping(token) != nullptr;
}

} // namespace cf::ui::components::material::token_literals
