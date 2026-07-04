/**
 * @file    console_win_impl.h
 * @brief   Windows-specific console implementation functions.
 *
 * Provides console size detection and color support capability queries for
 * Windows platforms.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup device
 */

#pragma once

#include <utility>
namespace cf::base::device::impl::Win {

/**
 * @brief  Retrieves the current console size on Windows.
 *
 * Queries the Windows console for its current dimensions expressed as
 * character cells.
 *
 * @return  Pair of (width, height) in character units.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup device
 */
std::pair<int, int> console_size();

/**
 * @brief  Checks whether the Windows console supports colorful output.
 *
 * Determines if the console supports color escape sequences or ANSI color
 * codes on Windows platforms.
 *
 * @return  True if colorful output is supported, false otherwise.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup device
 */
bool support_colorful();

} // namespace cf::base::device::impl::Win
