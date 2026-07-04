/**
 * @file    console_unix_impl.h
 * @brief   Unix-specific console implementation functions.
 *
 * Provides console size detection and color support capability queries for
 * Unix-like platforms including fallback mechanisms for terminal detection.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup device
 */

#pragma once
#include "../console_defines.h"
#include "console_platform.h"

namespace cf::base::device::impl::Unix {

/// @brief Invalid console size sentinel value. Unit: character cells.
static constexpr console::console_size_t INVALID_ONE{-1, -1};

/**
 * @brief  Retrieves the current console size on Unix platforms.
 *
 * Queries the Unix terminal for its current dimensions expressed as
 * character cells.
 *
 * @return  Console size as (width, height) pair in character units.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup device
 */
console::console_size_t console_size();

/**
 * @brief  Retrieves console size using fallback method V1.
 *
 * Provides an alternative detection mechanism for terminals where the
 * primary size detection method fails.
 *
 * @return  Console size as (width, height) pair in character units.
 * @throws  None
 * @note    This is a fallback method used when primary detection fails.
 * @warning None
 * @since   N/A
 * @ingroup device
 */
console::console_size_t console_size_fallback_v1();

/**
 * @brief  Checks whether the Unix terminal supports colorful output.
 *
 * Determines if the terminal supports color escape sequences or ANSI color
 * codes on Unix-like platforms.
 *
 * @return  True if colorful output is supported, false otherwise.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup device
 */
bool support_colorful();

} // namespace cf::base::device::impl::Unix