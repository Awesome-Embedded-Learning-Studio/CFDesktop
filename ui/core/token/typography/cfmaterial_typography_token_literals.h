/**
 * @file cfmaterial_typography_token_literals.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Typography Token String Literals
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Compile-time string constants for Material Design 3 typography tokens.
 * These literals are used throughout the typography system to ensure
 * consistency and avoid string duplication.
 *
 * @ingroup ui_core
 */
#pragma once

#include <cstdint>

namespace cf::ui::core::token::literals {

// =============================================================================
// Display Typography Token Literals
// =============================================================================

/**
 * @brief Display Large typography token string literal.
 *
 * Used for hero content, size: 57sp, weight: 400, line-height: 64sp.
 */
inline constexpr const char TYPOGRAPHY_DISPLAY_LARGE[] = "md.typography.displayLarge";

/**
 * @brief Display Medium typography token string literal.
 *
 * Used for hero content, size: 45sp, weight: 400, line-height: 52sp.
 */
inline constexpr const char TYPOGRAPHY_DISPLAY_MEDIUM[] = "md.typography.displayMedium";

/**
 * @brief Display Small typography token string literal.
 *
 * Used for hero content, size: 36sp, weight: 400, line-height: 44sp.
 */
inline constexpr const char TYPOGRAPHY_DISPLAY_SMALL[] = "md.typography.displaySmall";

// =============================================================================
// Headline Typography Token Literals
// =============================================================================

/**
 * @brief Headline Large typography token string literal.
 *
 * Used for app bar important text, size: 32sp, weight: 400, line-height: 40sp.
 */
inline constexpr const char TYPOGRAPHY_HEADLINE_LARGE[] = "md.typography.headlineLarge";

/**
 * @brief Headline Medium typography token string literal.
 *
 * Used for app bar important text, size: 28sp, weight: 400, line-height: 36sp.
 */
inline constexpr const char TYPOGRAPHY_HEADLINE_MEDIUM[] = "md.typography.headlineMedium";

/**
 * @brief Headline Small typography token string literal.
 *
 * Used for app bar important text, size: 24sp, weight: 400, line-height: 32sp.
 */
inline constexpr const char TYPOGRAPHY_HEADLINE_SMALL[] = "md.typography.headlineSmall";

// =============================================================================
// Title Typography Token Literals
// =============================================================================

/**
 * @brief Title Large typography token string literal.
 *
 * Used for section headings, size: 22sp, weight: 500, line-height: 28sp.
 */
inline constexpr const char TYPOGRAPHY_TITLE_LARGE[] = "md.typography.titleLarge";

/**
 * @brief Title Medium typography token string literal.
 *
 * Used for section headings, size: 16sp, weight: 500, line-height: 24sp.
 */
inline constexpr const char TYPOGRAPHY_TITLE_MEDIUM[] = "md.typography.titleMedium";

/**
 * @brief Title Small typography token string literal.
 *
 * Used for section headings, size: 14sp, weight: 500, line-height: 20sp.
 */
inline constexpr const char TYPOGRAPHY_TITLE_SMALL[] = "md.typography.titleSmall";

// =============================================================================
// Body Typography Token Literals
// =============================================================================

/**
 * @brief Body Large typography token string literal.
 *
 * Used for main content, size: 16sp, weight: 400, line-height: 24sp.
 */
inline constexpr const char TYPOGRAPHY_BODY_LARGE[] = "md.typography.bodyLarge";

/**
 * @brief Body Medium typography token string literal.
 *
 * Used for main content, size: 14sp, weight: 400, line-height: 20sp.
 */
inline constexpr const char TYPOGRAPHY_BODY_MEDIUM[] = "md.typography.bodyMedium";

/**
 * @brief Body Small typography token string literal.
 *
 * Used for main content, size: 12sp, weight: 400, line-height: 16sp.
 */
inline constexpr const char TYPOGRAPHY_BODY_SMALL[] = "md.typography.bodySmall";

// =============================================================================
// Label Typography Token Literals
// =============================================================================

/**
 * @brief Label Large typography token string literal.
 *
 * Used for secondary information, size: 14sp, weight: 500, line-height: 20sp.
 */
inline constexpr const char TYPOGRAPHY_LABEL_LARGE[] = "md.typography.labelLarge";

/**
 * @brief Label Medium typography token string literal.
 *
 * Used for secondary information, size: 12sp, weight: 500, line-height: 16sp.
 */
inline constexpr const char TYPOGRAPHY_LABEL_MEDIUM[] = "md.typography.labelMedium";

/**
 * @brief Label Small typography token string literal.
 *
 * Used for secondary information, size: 11sp, weight: 500, line-height: 16sp.
 */
inline constexpr const char TYPOGRAPHY_LABEL_SMALL[] = "md.typography.labelSmall";

// =============================================================================
// Line Height Token Literals
// =============================================================================

/**
 * @brief Display Large line-height token string literal.
 *
 * Value: 64sp
 */
inline constexpr const char LINEHEIGHT_DISPLAY_LARGE[] = "md.lineHeight.displayLarge";

/**
 * @brief Display Medium line-height token string literal.
 *
 * Value: 52sp
 */
inline constexpr const char LINEHEIGHT_DISPLAY_MEDIUM[] = "md.lineHeight.displayMedium";

/**
 * @brief Display Small line-height token string literal.
 *
 * Value: 44sp
 */
inline constexpr const char LINEHEIGHT_DISPLAY_SMALL[] = "md.lineHeight.displaySmall";

/**
 * @brief Headline Large line-height token string literal.
 *
 * Value: 40sp
 */
inline constexpr const char LINEHEIGHT_HEADLINE_LARGE[] = "md.lineHeight.headlineLarge";

/**
 * @brief Headline Medium line-height token string literal.
 *
 * Value: 36sp
 */
inline constexpr const char LINEHEIGHT_HEADLINE_MEDIUM[] = "md.lineHeight.headlineMedium";

/**
 * @brief Headline Small line-height token string literal.
 *
 * Value: 32sp
 */
inline constexpr const char LINEHEIGHT_HEADLINE_SMALL[] = "md.lineHeight.headlineSmall";

/**
 * @brief Title Large line-height token string literal.
 *
 * Value: 28sp
 */
inline constexpr const char LINEHEIGHT_TITLE_LARGE[] = "md.lineHeight.titleLarge";

/**
 * @brief Title Medium line-height token string literal.
 *
 * Value: 24sp
 */
inline constexpr const char LINEHEIGHT_TITLE_MEDIUM[] = "md.lineHeight.titleMedium";

/**
 * @brief Title Small line-height token string literal.
 *
 * Value: 20sp
 */
inline constexpr const char LINEHEIGHT_TITLE_SMALL[] = "md.lineHeight.titleSmall";

/**
 * @brief Body Large line-height token string literal.
 *
 * Value: 24sp
 */
inline constexpr const char LINEHEIGHT_BODY_LARGE[] = "md.lineHeight.bodyLarge";

/**
 * @brief Body Medium line-height token string literal.
 *
 * Value: 20sp
 */
inline constexpr const char LINEHEIGHT_BODY_MEDIUM[] = "md.lineHeight.bodyMedium";

/**
 * @brief Body Small line-height token string literal.
 *
 * Value: 16sp
 */
inline constexpr const char LINEHEIGHT_BODY_SMALL[] = "md.lineHeight.bodySmall";

/**
 * @brief Label Large line-height token string literal.
 *
 * Value: 20sp
 */
inline constexpr const char LINEHEIGHT_LABEL_LARGE[] = "md.lineHeight.labelLarge";

/**
 * @brief Label Medium line-height token string literal.
 *
 * Value: 16sp
 */
inline constexpr const char LINEHEIGHT_LABEL_MEDIUM[] = "md.lineHeight.labelMedium";

/**
 * @brief Label Small line-height token string literal.
 *
 * Value: 16sp
 */
inline constexpr const char LINEHEIGHT_LABEL_SMALL[] = "md.lineHeight.labelSmall";

// =============================================================================
// All Tokens Array (for iteration)
// =============================================================================

/**
 * @brief Array containing all 15 Material Design 3 typography token literals.
 */
inline constexpr const char* const ALL_TYPOGRAPHY_TOKENS[] = {
    // Display
    TYPOGRAPHY_DISPLAY_LARGE, TYPOGRAPHY_DISPLAY_MEDIUM, TYPOGRAPHY_DISPLAY_SMALL,
    // Headline
    TYPOGRAPHY_HEADLINE_LARGE, TYPOGRAPHY_HEADLINE_MEDIUM, TYPOGRAPHY_HEADLINE_SMALL,
    // Title
    TYPOGRAPHY_TITLE_LARGE, TYPOGRAPHY_TITLE_MEDIUM, TYPOGRAPHY_TITLE_SMALL,
    // Body
    TYPOGRAPHY_BODY_LARGE, TYPOGRAPHY_BODY_MEDIUM, TYPOGRAPHY_BODY_SMALL,
    // Label
    TYPOGRAPHY_LABEL_LARGE, TYPOGRAPHY_LABEL_MEDIUM, TYPOGRAPHY_LABEL_SMALL};

/**
 * @brief Array containing all 15 Material Design 3 line-height token literals.
 */
inline constexpr const char* const ALL_LINEHEIGHT_TOKENS[] = {
    // Display
    LINEHEIGHT_DISPLAY_LARGE, LINEHEIGHT_DISPLAY_MEDIUM, LINEHEIGHT_DISPLAY_SMALL,
    // Headline
    LINEHEIGHT_HEADLINE_LARGE, LINEHEIGHT_HEADLINE_MEDIUM, LINEHEIGHT_HEADLINE_SMALL,
    // Title
    LINEHEIGHT_TITLE_LARGE, LINEHEIGHT_TITLE_MEDIUM, LINEHEIGHT_TITLE_SMALL,
    // Body
    LINEHEIGHT_BODY_LARGE, LINEHEIGHT_BODY_MEDIUM, LINEHEIGHT_BODY_SMALL,
    // Label
    LINEHEIGHT_LABEL_LARGE, LINEHEIGHT_LABEL_MEDIUM, LINEHEIGHT_LABEL_SMALL};

/**
 * @brief Total count of Material Design 3 typography tokens.
 */
inline constexpr size_t TYPOGRAPHY_TOKEN_COUNT = 15;

/**
 * @brief Total count of Material Design 3 line-height tokens.
 */
inline constexpr size_t LINEHEIGHT_TOKEN_COUNT = 15;

} // namespace cf::ui::core::token::literals
