/**
 * @file cfmaterial_token_literals.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Color Token String Literals
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Compile-time string constants for Material Design 3 color tokens.
 * These literals are used throughout the color system to ensure
 * consistency and avoid string duplication.
 *
 * @ingroup ui_core
 */
#pragma once

#include <cstdint>

namespace cf::ui::core::token::literals {

// =============================================================================
// Primary Color Token Literals
// =============================================================================

/**
 * @brief Primary color token string literal.
 *
 * Used as the main branding color for key components.
 */
inline constexpr const char PRIMARY[] = "md.primary";

/**
 * @brief On-primary color token string literal.
 *
 * Text and icons drawn on top of primary color.
 */
inline constexpr const char ON_PRIMARY[] = "md.onPrimary";

/**
 * @brief Primary container color token string literal.
 *
 * Tonal surface variant containing primary-colored content.
 */
inline constexpr const char PRIMARY_CONTAINER[] = "md.primaryContainer";

/**
 * @brief On-primary-container color token string literal.
 *
 * Text and icons drawn on top of primary container color.
 */
inline constexpr const char ON_PRIMARY_CONTAINER[] = "md.onPrimaryContainer";

// =============================================================================
// Secondary Color Token Literals
// =============================================================================

/**
 * @brief Secondary color token string literal.
 *
 * Alternative to primary color for less prominent components.
 */
inline constexpr const char SECONDARY[] = "md.secondary";

/**
 * @brief On-secondary color token string literal.
 *
 * Text and icons drawn on top of secondary color.
 */
inline constexpr const char ON_SECONDARY[] = "md.onSecondary";

/**
 * @brief Secondary container color token string literal.
 *
 * Tonal surface variant containing secondary-colored content.
 */
inline constexpr const char SECONDARY_CONTAINER[] = "md.secondaryContainer";

/**
 * @brief On-secondary-container color token string literal.
 *
 * Text and icons drawn on top of secondary container color.
 */
inline constexpr const char ON_SECONDARY_CONTAINER[] = "md.onSecondaryContainer";

// =============================================================================
// Tertiary Color Token Literals
// =============================================================================

/**
 * @brief Tertiary color token string literal.
 *
 * Used for contrasting accents and balance in the UI.
 */
inline constexpr const char TERTIARY[] = "md.tertiary";

/**
 * @brief On-tertiary color token string literal.
 *
 * Text and icons drawn on top of tertiary color.
 */
inline constexpr const char ON_TERTIARY[] = "md.onTertiary";

/**
 * @brief Tertiary container color token string literal.
 *
 * Tonal surface variant containing tertiary-colored content.
 */
inline constexpr const char TERTIARY_CONTAINER[] = "md.tertiaryContainer";

/**
 * @brief On-tertiary-container color token string literal.
 *
 * Text and icons drawn on top of tertiary container color.
 */
inline constexpr const char ON_TERTIARY_CONTAINER[] = "md.onTertiaryContainer";

// =============================================================================
// Error Color Token Literals
// =============================================================================

/**
 * @brief Error color token string literal.
 *
 * Used for error states and destructive actions.
 */
inline constexpr const char ERROR[] = "md.error";

/**
 * @brief On-error color token string literal.
 *
 * Text and icons drawn on top of error color.
 */
inline constexpr const char ON_ERROR[] = "md.onError";

/**
 * @brief Error container color token string literal.
 *
 * Tonal surface variant containing error-colored content.
 */
inline constexpr const char ERROR_CONTAINER[] = "md.errorContainer";

/**
 * @brief On-error-container color token string literal.
 *
 * Text and icons drawn on top of error container color.
 */
inline constexpr const char ON_ERROR_CONTAINER[] = "md.onErrorContainer";

// =============================================================================
// Surface Color Token Literals
// =============================================================================

/**
 * @brief Background color token string literal.
 *
 * The base color of the application background.
 */
inline constexpr const char BACKGROUND[] = "md.background";

/**
 * @brief On-background color token string literal.
 *
 * Text and icons drawn on top of background color.
 */
inline constexpr const char ON_BACKGROUND[] = "md.onBackground";

/**
 * @brief Surface color token string literal.
 *
 * Color for surfaces such as cards, sheets, and menus.
 */
inline constexpr const char SURFACE[] = "md.surface";

/**
 * @brief On-surface color token string literal.
 *
 * Text and icons drawn on top of surface color.
 */
inline constexpr const char ON_SURFACE[] = "md.onSurface";

/**
 * @brief Surface variant color token string literal.
 *
 * Variant of surface color for subtle differentiation.
 */
inline constexpr const char SURFACE_VARIANT[] = "md.surfaceVariant";

/**
 * @brief On-surface-variant color token string literal.
 *
 * Text and icons drawn on top of surface variant color.
 */
inline constexpr const char ON_SURFACE_VARIANT[] = "md.onSurfaceVariant";

/**
 * @brief Outline color token string literal.
 *
 * Color for borders and outlines.
 */
inline constexpr const char OUTLINE[] = "md.outline";

/**
 * @brief Outline variant color token string literal.
 *
 * Variant of outline color for subtle differentiation.
 */
inline constexpr const char OUTLINE_VARIANT[] = "md.outlineVariant";

// =============================================================================
// Utility Color Token Literals
// =============================================================================

/**
 * @brief Shadow color token string literal.
 *
 * Color for drop shadows.
 */
inline constexpr const char SHADOW[] = "md.shadow";

/**
 * @brief Scrim color token string literal.
 *
 * Color for overlay scrim.
 */
inline constexpr const char SCRIM[] = "md.scrim";

/**
 * @brief Inverse surface color token string literal.
 *
 * Inverted surface color for special UI scenarios.
 */
inline constexpr const char INVERSE_SURFACE[] = "md.inverseSurface";

/**
 * @brief Inverse-on-surface color token string literal.
 *
 * Text and icons drawn on top of inverse surface color.
 */
inline constexpr const char INVERSE_ON_SURFACE[] = "md.inverseOnSurface";

/**
 * @brief Inverse primary color token string literal.
 *
 * Inverted primary color for special UI scenarios.
 */
inline constexpr const char INVERSE_PRIMARY[] = "md.inversePrimary";

// =============================================================================
// All Tokens Array (for iteration)
// =============================================================================

/**
 * @brief Array containing all 26 Material Design 3 color token literals.
 *
 * Ordered by group: Primary (4), Secondary (4), Tertiary (4),
 * Error (4), Surface (8), Utility (5).
 */
inline constexpr const char* const ALL_TOKENS[] = {
    // Primary
    PRIMARY, ON_PRIMARY, PRIMARY_CONTAINER, ON_PRIMARY_CONTAINER,
    // Secondary
    SECONDARY, ON_SECONDARY, SECONDARY_CONTAINER, ON_SECONDARY_CONTAINER,
    // Tertiary
    TERTIARY, ON_TERTIARY, TERTIARY_CONTAINER, ON_TERTIARY_CONTAINER,
    // Error
    ERROR, ON_ERROR, ERROR_CONTAINER, ON_ERROR_CONTAINER,
    // Surface
    BACKGROUND, ON_BACKGROUND, SURFACE, ON_SURFACE, SURFACE_VARIANT, ON_SURFACE_VARIANT, OUTLINE,
    OUTLINE_VARIANT,
    // Utility
    SHADOW, SCRIM, INVERSE_SURFACE, INVERSE_ON_SURFACE, INVERSE_PRIMARY};

/**
 * @brief Total count of Material Design 3 color tokens.
 */
inline constexpr size_t TOKEN_COUNT = 26;

} // namespace cf::ui::core::token::literals
