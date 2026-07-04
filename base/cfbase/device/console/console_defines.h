#pragma once
#include <utility>

namespace cf::base::device::console {

/**
 * @file    console_defines.h
 * @brief   Platform-specific console type definitions and constants.
 *
 * Provides console handle and size type definitions used across the console
 * abstraction layer.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup device
 */

#ifdef CFDESKTOP_OS_WINDOWS
/// @brief Console handle type for Windows platforms. Unit: handle identifier.
using console_handle_t = int;
/// @brief Invalid console handle sentinel value. Unit: handle identifier.
static constexpr const console_handle_t INVALID_CONSOLE_HANDLE = -1;
#else
/// @brief Console handle type for Unix platforms. Unit: handle identifier.
using console_handle_t = int;
/// @brief Invalid console handle sentinel value. Unit: handle identifier.
static constexpr const console_handle_t INVALID_CONSOLE_HANDLE = -1;
#endif

/// @brief Console size type as (width, height) pair. Unit: characters.
using console_size_t = std::pair<int, int>;

} // namespace cf::base::device::console
