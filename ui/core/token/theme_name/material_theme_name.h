/**
 * @file    ui/core/token/theme_name/material_theme_name.h
 * @brief   Literal constants for Material theme names.
 *
 * Provides string literal constants for identifying Material Design
 * themes in the token system.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once

namespace cf::ui::core::token::literals {

/// @brief Material theme default name identifier.
inline constexpr const char MATERIAL_THEME_NAME[] = "theme.md.material";

/// @brief Material theme light variant name identifier.
inline constexpr const char MATERIAL_THEME_LIGHT[] = "theme.material.light";

/// @brief Material theme dark variant name identifier.
inline constexpr const char MATERIAL_THEME_DARK[] = "theme.material.dark";

} // namespace cf::ui::core::token::literals
