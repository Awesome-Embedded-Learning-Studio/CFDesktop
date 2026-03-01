/**
 * @file cfmaterial_radius_scale_literals.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Radius Scale Token String Literals
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Compile-time string constants for Material Design 3 radius scale tokens.
 * These literals are used throughout the shape system to ensure
 * consistency and avoid string duplication.
 *
 * @ingroup ui_core
 */
#pragma once

#include <cstddef>

namespace cf::ui::core::token::literals {

// =============================================================================
// Corner Radius Token Literals
// =============================================================================

/**
 * @brief Corner None token string literal.
 *
 * Value: 0dp - No corner radius.
 */
inline constexpr const char CORNER_NONE[] = "md.shape.cornerNone";

/**
 * @brief Corner Extra Small token string literal.
 *
 * Value: 4dp - Used for chips, small cards.
 */
inline constexpr const char CORNER_EXTRA_SMALL[] = "md.shape.cornerExtraSmall";

/**
 * @brief Corner Small token string literal.
 *
 * Value: 8dp - Used for text fields, checkboxes.
 */
inline constexpr const char CORNER_SMALL[] = "md.shape.cornerSmall";

/**
 * @brief Corner Medium token string literal.
 *
 * Value: 12dp - Used for cards.
 */
inline constexpr const char CORNER_MEDIUM[] = "md.shape.cornerMedium";

/**
 * @brief Corner Large token string literal.
 *
 * Value: 16dp - Used for alert dialogs.
 */
inline constexpr const char CORNER_LARGE[] = "md.shape.cornerLarge";

/**
 * @brief Corner Extra Large token string literal.
 *
 * Value: 28dp - Used for FAB, modals.
 */
inline constexpr const char CORNER_EXTRA_LARGE[] = "md.shape.cornerExtraLarge";

/**
 * @brief Corner Extra Extra Large token string literal.
 *
 * Value: 32dp - Used for drawers.
 */
inline constexpr const char CORNER_EXTRA_EXTRA_LARGE[] = "md.shape.cornerExtraExtraLarge";

// =============================================================================
// All Tokens Array (for iteration)
// =============================================================================

/**
 * @brief Array containing all 7 Material Design 3 radius scale token literals.
 */
inline constexpr const char* const ALL_RADIUS_TOKENS[] = {
    CORNER_NONE,        CORNER_EXTRA_SMALL,      CORNER_SMALL, CORNER_MEDIUM, CORNER_LARGE,
    CORNER_EXTRA_LARGE, CORNER_EXTRA_EXTRA_LARGE};

/**
 * @brief Total count of Material Design 3 radius scale tokens.
 */
inline constexpr size_t RADIUS_TOKEN_COUNT = 7;

} // namespace cf::ui::core::token::literals
