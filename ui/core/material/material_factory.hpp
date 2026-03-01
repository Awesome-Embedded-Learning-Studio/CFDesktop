/**
 * @file material_factory.hpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Color Scheme Factory Functions
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include <QByteArray>

#include "../../export.h"
#include "base/expected/expected.hpp"
#include "cfmaterial_fonttype.h"
#include "cfmaterial_motion.h"
#include "cfmaterial_radius_scale.h"
#include "cfmaterial_scheme.h"

namespace cf::ui::core::material {

// =============================================================================
// Error Types
// =============================================================================

/**
 * @brief  Error type for material scheme operations.
 *
 * @since  0.1
 * @ingroup ui_core
 */
struct MaterialSchemeError {
    /**
     * @brief  Error kind enumeration.
     *
     * @since  0.1
     */
    enum class Kind {
        InvalidJson,        ///< JSON parsing failed
        MissingColor,       ///< Required color not found in JSON
        InvalidColorFormat, ///< Color value format is invalid
        GenerationFailed    ///< Dynamic color generation failed
    } kind = Kind::InvalidJson;

    /**
     * @brief  Human-readable error message.
     *
     * @since  0.1
     */
    std::string message;

    /**
     * @brief  Default constructor.
     *
     * @since  0.1
     */
    MaterialSchemeError() = default;

    /**
     * @brief  Construct from kind and message.
     *
     * @param  k Error kind.
     * @param  msg Error message.
     *
     * @since  0.1
     */
    MaterialSchemeError(Kind k, std::string msg) : kind(k), message(std::move(msg)) {}
};

/**
 * @brief  Result type for factory functions.
 *
 * @since  0.1
 */
using Result = cf::expected<MaterialColorScheme, MaterialSchemeError>;

// =============================================================================
// Factory Functions
// =============================================================================

/**
 * @brief  Creates Material Design 3 default light theme.
 *
 * Uses the official Material Design 3 baseline purple color scheme.
 *
 * @return     MaterialColorScheme with light theme colors.
 *
 * @since      0.1
 * @ingroup    ui_core
 *
 * @code
 * auto lightScheme = cf::ui::core::material::light();
 * QColor primary = lightScheme.queryExpectedColor("md.primary");
 * @endcode
 */
CF_UI_EXPORT MaterialColorScheme light();

/**
 * @brief  Creates Material Design 3 default dark theme.
 *
 * Uses the official Material Design 3 baseline purple color scheme
 * adapted for dark mode.
 *
 * @return     MaterialColorScheme with dark theme colors.
 *
 * @since      0.1
 * @ingroup    ui_core
 */
CF_UI_EXPORT MaterialColorScheme dark();

/**
 * @brief  Creates color scheme from Material Theme Builder JSON.
 *
 * Supports the Material Theme Builder export format with the following
 * structure:
 *
 * @code
 * {
 *   "schemes": {
 *     "light": { "primary": "#6750A4", ... },
 *     "dark": { "primary": "#D0BCFF", ... }
 *   }
 * }
 * @endcode
 *
 * Also supports direct color values:
 *
 * @code
 * {
 *   "primary": "#6750A4",
 *   "onPrimary": "#FFFFFF",
 *   ...
 * }
 * @endcode
 *
 * @param[in]  json   JSON export from Material Theme Builder.
 * @param[in]  isDark Whether to use light or dark scheme from JSON (default: false).
 *
 * @return     MaterialColorScheme parsed from JSON, or error.
 *
 * @since      0.1
 * @ingroup    ui_core
 */
CF_UI_EXPORT Result fromJson(const QByteArray& json, bool isDark = false);

/**
 * @brief  Creates dynamic color scheme from a key color.
 *
 * Implements Material You dynamic color generation using the
 * HCT color space and tonal palette algorithm.
 *
 * @param[in]  keyColor Source color for generating the scheme.
 * @param[in]  isDark   Whether to generate a dark scheme (default: false).
 *
 * @return     MaterialColorScheme generated from the key color.
 *
 * @since      0.1
 * @ingroup    ui_core
 */
CF_UI_EXPORT MaterialColorScheme fromKeyColor(cf::ui::base::CFColor keyColor, bool isDark = false);

/**
 * @brief  Converts a MaterialColorScheme to JSON format.
 *
 * Exports the scheme in Material Theme Builder compatible format.
 *
 * @param[in]  scheme The color scheme to export.
 *
 * @return     JSON representation of the color scheme.
 *
 * @since      0.1
 * @ingroup    ui_core
 */
CF_UI_EXPORT QByteArray toJson(const MaterialColorScheme& scheme);

// =============================================================================
// Typography Factory Functions
// =============================================================================

/**
 * @brief  Creates default Material Design 3 typography scale.
 *
 * Uses system default sans-serif font with MD3 specified sizes and weights.
 *
 * @return     MaterialTypography with default configuration.
 *
 * @since      0.1
 * @ingroup    ui_core
 *
 * @code
 * auto typography = cf::ui::core::material::defaultTypography();
 * QFont titleFont = typography.queryTargetFont("md.typography.titleLarge");
 * @endcode
 */
CF_UI_EXPORT MaterialTypography defaultTypography();

// =============================================================================
// Radius Scale Factory Functions
// =============================================================================

/**
 * @brief Creates default Material Design 3 radius scale.
 *
 * Uses Material Design 3 specified corner radius values.
 *
 * @return     MaterialRadiusScale with default configuration.
 *
 * @since      0.1
 * @ingroup    ui_core
 *
 * @code
 * auto radiusScale = cf::ui::core::material::defaultRadiusScale();
 * float smallRadius = radiusScale.queryRadiusScale("md.shape.cornerSmall"); // 8.0f
 * @endcode
 */
CF_UI_EXPORT MaterialRadiusScale defaultRadiusScale();

// =============================================================================
// Motion Factory Functions
// =============================================================================

/**
 * @brief Creates default Material Design 3 motion scheme.
 *
 * Uses Material Design 3 specified duration and easing values for animations.
 *
 * @return     MaterialMotionScheme with default configuration.
 *
 * @since      0.1
 * @ingroup    ui_core
 *
 * @code
 * auto motionScheme = cf::ui::core::material::motion();
 * int duration = motionScheme.queryDuration("shortEnter"); // 200
 * auto spec = motionScheme.getMotionSpec("mediumExit");
 * @endcode
 */
CF_UI_EXPORT MaterialMotionScheme motion();

} // namespace cf::ui::core::material
