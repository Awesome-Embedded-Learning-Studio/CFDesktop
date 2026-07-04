/**
 * @file    display_backend_helper.h
 * @brief   Platform factory API for creating display server backends.
 *
 * Provides functions to retrieve the native platform factory for creating
 * display server backend instances.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "../components/IDisplayServerBackend.h"
#include <functional>

namespace cf::desktop::platform {

/**
 * @brief  Platform factory API for display server backend creation and destruction.
 *
 * Encapsulates function pointers for creating and releasing display server
 * backend instances, enabling dynamic dispatch to platform implementations.
 *
 * @ingroup none
 */
struct DisplayBackendFactoryAPI {
    /// Function type for creating backend instances.
    using CreateFunc = std::function<IDisplayServerBackend*()>;

    /// Function type for releasing backend instances.
    using ReleaseFunc = std::function<void(IDisplayServerBackend*)>;

    /// Function pointer for creating backend instances.
    CreateFunc creator_func;

    /// Function pointer for releasing backend instances.
    ReleaseFunc release_func;
};

/**
 * @brief  Returns the native platform's display backend factory API.
 *
 * @return DisplayBackendFactoryAPI struct containing creation and release functions.
 * @throws None
 * @note   The returned API may have empty functions if no backend is available.
 * @since   0.1
 * @ingroup none
 */
CF_DESKTOP_EXPORT DisplayBackendFactoryAPI native_display() noexcept;

} // namespace cf::desktop::platform
