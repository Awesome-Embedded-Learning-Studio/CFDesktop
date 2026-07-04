/**
 * @file    platform_helper.h
 * @brief   Platform factory API for creating desktop property strategies.
 *
 * Provides functions to retrieve the native platform factory for creating
 * and releasing desktop property strategy instances.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-26
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "IDesktopPropertyStrategy.h"
#include <functional>

namespace cf::desktop::platform_strategy {

/**
 * @brief  Platform factory API for strategy creation and destruction.
 *
 * Encapsulates function pointers for creating and releasing desktop property
 * strategy instances, enabling dynamic loading of platform implementations.
 *
 * @ingroup none
 */
struct PlatformFactoryAPI {
    /**
     * @brief  Function type for creating strategy instances.
     *
     * Takes a strategy type and returns a pointer to a new strategy instance.
     */
    using CreateFunc =
        std::function<IDesktopPropertyStrategy*(IDesktopPropertyStrategy::StrategyType)>;

    /**
     * @brief  Function type for releasing strategy instances.
     *
     * Takes a strategy pointer and handles its destruction.
     */
    using ReleaseFunc = std::function<void(IDesktopPropertyStrategy*)>;

    /**
     * @brief  Function pointer for creating strategy instances.
     *
     * Ownership: callable. May be empty if not initialized.
     */
    CreateFunc creator_func;

    /**
     * @brief  Function pointer for releasing strategy instances.
     *
     * Ownership: callable. May be empty if not initialized.
     */
    ReleaseFunc release_func;
};

/**
 * @brief  Returns the native platform factory API.
 *
 * Provides access to the locally compiled platform factory for creating
 * and releasing desktop property strategy instances.
 *
 * @return Platform factory API struct containing creation and release functions.
 * @throws None
 * @note   This function provides the locally linked implementation.
 * @warning The returned API must be initialized before use.
 * @since   0.1
 * @ingroup none
 */
CF_DESKTOP_EXPORT PlatformFactoryAPI native() noexcept;

// TODO: Load from DLL
// PlatformFactoryAPI* remote() noexcept;

} // namespace cf::desktop::platform_strategy
