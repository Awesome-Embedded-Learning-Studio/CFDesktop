/**
 * @file ui/base/color_helper.h
 * @brief Color utility functions for blending, contrast, and tonal palettes.
 *
 * Provides helper functions for color manipulation including blending,
 * elevation overlays, contrast ratio calculation, and tonal palette generation.
 *
 * @author Charliechen114514
 * @date 2026-02-23
 * @version 0.1
 * @since 0.1
 * @ingroup ui
 */
#pragma once
#include "color.h"
#include "export.h"
#include <QList>

namespace cf::ui::base {

/**
 * @brief Blends two colors with a specified ratio.
 *
 * @param[in] base The base color to blend.
 * @param[in] overlay The overlay color to blend onto the base.
 * @param[in] ratio Blending ratio between 0.0 and 1.0.
 * @return Blended color result.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT CFColor blend(const CFColor& base, CFColor& overlay, float ratio);

/**
 * @brief Applies elevation overlay to a surface color.
 *
 * @param[in] surface The base surface color.
 * @param[in] primary The primary color for overlay.
 * @param[in] elevation Elevation level in dp units.
 * @return Color with elevation overlay applied.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT CFColor elevationOverlay(CFColor& surface, CFColor& primary, int elevation);

/**
 * @brief Calculates the contrast ratio between two colors.
 *
 * @param[in] a First color.
 * @param[in] b Second color.
 * @return Contrast ratio value (typically 1.0 to 21.0).
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT float contrastRatio(CFColor& a, CFColor& b);

/**
 * @brief Generates a tonal palette from a key color.
 *
 * @param[in] keyColor The base key color for palette generation.
 * @return List of generated tonal colors.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT QList<CFColor> tonalPalette(CFColor keyColor);

} // namespace cf::ui::base
