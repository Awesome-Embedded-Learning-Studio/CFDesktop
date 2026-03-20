#pragma once
#include "../console_defines.h"
#include "base/policy_chain/policy_chain.hpp"
#include <memory>

namespace cf::base::device::console {
static constexpr console::console_size_t INVALID_ONE{-1, -1};
/**
 * @file    console_platform.h
 * @brief   Platform-specific console policy and support functions.
 *
 * Provides factory functions for creating platform-specific console policies
 * and querying console capabilities.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup device
 */

using cf::PolicyChain;

/**
 * @brief  Creates the platform-specific console size policy chain.
 *
 * Constructs and returns a policy chain for querying console size based on
 * the current platform (Windows or Unix).
 *
 * @return  Unique pointer to the configured policy chain.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup device
 */
std::unique_ptr<PolicyChain<console_size_t>> get_platform_size_policy();

/**
 * @brief  Checks whether the platform supports console operations.
 *
 * Determines if the current platform supports console functionality.
 *
 * @return  True if console is supported, false otherwise.
 * @throws  None
 * @note    None
 * @warning None
 * @since   N/A
 * @ingroup device
 */
bool platform_console_support();

} // namespace cf::base::device::console
