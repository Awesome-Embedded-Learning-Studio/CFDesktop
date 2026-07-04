/**
 * @file    linux_wsl_platform.h
 * @brief   Platform-specific factory API for Linux/WSL desktop environments.
 *
 * Provides the native implementation entry point for creating desktop property
 * strategies on Linux/WSL platforms.
 *
 * @author  CFDesktop Team
 * @date    2026-03-27
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup platform_wsl
 */

#pragma once
#include "platform_helper.h"

namespace cf::desktop::platform_strategy {

/**
 * @brief  Returns the native platform factory API implementation.
 *
 * Provides access to the platform-specific factory for creating desktop
 * property strategies on Linux/WSL systems.
 *
 * @return  The platform factory API structure.
 * @throws  None
 * @note    None
 * @warning None
 * @since   0.13.0
 * @ingroup platform_wsl
 */
PlatformFactoryAPI native_impl();

} // namespace cf::desktop::platform_strategy
