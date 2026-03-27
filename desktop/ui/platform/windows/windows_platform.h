/**
 * @file    desktop/ui/platform/windows/windows_platform.h
 * @brief   Platform-specific factory API for Windows desktop environments.
 *
 * Provides the native implementation entry point for creating desktop property
 * strategies on Windows platforms.
 *
 * @author  N/A
 * @date    2026-03-27
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "platform_helper.h"

namespace cf::desktop::platform_strategy {

/**
 * @brief  Returns the native platform factory API implementation for Windows.
 *
 * Provides access to the platform-specific factory for creating desktop
 * property strategies on Windows systems.
 *
 * @return  The platform factory API structure.
 * @throws  None
 * @note    None
 * @warning None
 * @since   0.1
 * @ingroup none
 */
CF_DESKTOP_EXPORT PlatformFactoryAPI native_impl();

} // namespace cf::desktop::platform_strategy
