/**
 * @file cfmaterial_motion_token_literals.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Motion Token String Literals
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Compile-time string constants for Material Design 3 motion tokens.
 * These literals are used throughout the motion system to ensure
 * consistency and avoid string duplication.
 *
 * @ingroup ui_core
 */
#pragma once

#include <cstdint>

namespace cf::ui::core::token::literals {

// =============================================================================
// Motion Duration Token Literals
// =============================================================================

/**
 * @brief Short enter motion duration token.
 *
 * Duration: 200ms
 * Used for: Small elements entering the screen
 */
inline constexpr const char MOTION_SHORT_ENTER_DURATION[] = "md.motion.shortEnter.duration";

/**
 * @brief Short exit motion duration token.
 *
 * Duration: 150ms
 * Used for: Small elements exiting the screen
 */
inline constexpr const char MOTION_SHORT_EXIT_DURATION[] = "md.motion.shortExit.duration";

/**
 * @brief Medium enter motion duration token.
 *
 * Duration: 300ms
 * Used for: Medium-sized elements entering the screen
 */
inline constexpr const char MOTION_MEDIUM_ENTER_DURATION[] = "md.motion.mediumEnter.duration";

/**
 * @brief Medium exit motion duration token.
 *
 * Duration: 250ms
 * Used for: Medium-sized elements exiting the screen
 */
inline constexpr const char MOTION_MEDIUM_EXIT_DURATION[] = "md.motion.mediumExit.duration";

/**
 * @brief Long enter motion duration token.
 *
 * Duration: 450ms
 * Used for: Large elements entering the screen
 */
inline constexpr const char MOTION_LONG_ENTER_DURATION[] = "md.motion.longEnter.duration";

/**
 * @brief Long exit motion duration token.
 *
 * Duration: 400ms
 * Used for: Large elements exiting the screen
 */
inline constexpr const char MOTION_LONG_EXIT_DURATION[] = "md.motion.longExit.duration";

/**
 * @brief State change motion duration token.
 *
 * Duration: 200ms
 * Used for: State layer animations
 */
inline constexpr const char MOTION_STATE_CHANGE_DURATION[] = "md.motion.stateChange.duration";

/**
 * @brief Ripple expand motion duration token.
 *
 * Duration: 400ms
 * Used for: Ripple effect expansion
 */
inline constexpr const char MOTION_RIPPLE_EXPAND_DURATION[] = "md.motion.rippleExpand.duration";

/**
 * @brief Ripple fade motion duration token.
 *
 * Duration: 150ms
 * Used for: Ripple effect fade out
 */
inline constexpr const char MOTION_RIPPLE_FADE_DURATION[] = "md.motion.rippleFade.duration";

// =============================================================================
// Motion Easing Token Literals
// =============================================================================

/**
 * @brief Short enter motion easing token.
 *
 * Easing: EmphasizedDecelerate
 */
inline constexpr const char MOTION_SHORT_ENTER_EASING[] = "md.motion.shortEnter.easing";

/**
 * @brief Short exit motion easing token.
 *
 * Easing: EmphasizedAccelerate
 */
inline constexpr const char MOTION_SHORT_EXIT_EASING[] = "md.motion.shortExit.easing";

/**
 * @brief Medium enter motion easing token.
 *
 * Easing: EmphasizedDecelerate
 */
inline constexpr const char MOTION_MEDIUM_ENTER_EASING[] = "md.motion.mediumEnter.easing";

/**
 * @brief Medium exit motion easing token.
 *
 * Easing: EmphasizedAccelerate
 */
inline constexpr const char MOTION_MEDIUM_EXIT_EASING[] = "md.motion.mediumExit.easing";

/**
 * @brief Long enter motion easing token.
 *
 * Easing: Emphasized
 */
inline constexpr const char MOTION_LONG_ENTER_EASING[] = "md.motion.longEnter.easing";

/**
 * @brief Long exit motion easing token.
 *
 * Easing: Emphasized
 */
inline constexpr const char MOTION_LONG_EXIT_EASING[] = "md.motion.longExit.easing";

/**
 * @brief State change motion easing token.
 *
 * Easing: Standard
 */
inline constexpr const char MOTION_STATE_CHANGE_EASING[] = "md.motion.stateChange.easing";

/**
 * @brief Ripple expand motion easing token.
 *
 * Easing: Standard
 */
inline constexpr const char MOTION_RIPPLE_EXPAND_EASING[] = "md.motion.rippleExpand.easing";

/**
 * @brief Ripple fade motion easing token.
 *
 * Easing: Linear
 */
inline constexpr const char MOTION_RIPPLE_FADE_EASING[] = "md.motion.rippleFade.easing";

// =============================================================================
// All Motion Tokens Array (for iteration)
// =============================================================================

/**
 * @brief Array containing all Material Design 3 motion token literals.
 *
 * Ordered by category: Durations (9), Easing (9).
 */
inline constexpr const char* const ALL_MOTION_TOKENS[] = {
    // Durations
    MOTION_SHORT_ENTER_DURATION, MOTION_SHORT_EXIT_DURATION, MOTION_MEDIUM_ENTER_DURATION,
    MOTION_MEDIUM_EXIT_DURATION, MOTION_LONG_ENTER_DURATION, MOTION_LONG_EXIT_DURATION,
    MOTION_STATE_CHANGE_DURATION, MOTION_RIPPLE_EXPAND_DURATION, MOTION_RIPPLE_FADE_DURATION,
    // Easing
    MOTION_SHORT_ENTER_EASING, MOTION_SHORT_EXIT_EASING, MOTION_MEDIUM_ENTER_EASING,
    MOTION_MEDIUM_EXIT_EASING, MOTION_LONG_ENTER_EASING, MOTION_LONG_EXIT_EASING,
    MOTION_STATE_CHANGE_EASING, MOTION_RIPPLE_EXPAND_EASING, MOTION_RIPPLE_FADE_EASING};

/**
 * @brief Total count of Material Design 3 motion tokens.
 */
inline constexpr size_t MOTION_TOKEN_COUNT = 18;

} // namespace cf::ui::core::token::literals
