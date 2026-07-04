/**
 * @file    shell_layer_helper.h
 * @brief   Platform factory API for creating shell layers and strategies.
 *
 * Provides functions to retrieve the native platform factory for creating
 * shell layer and shell layer strategy instances, enabling platform-specific
 * rendering backends (QWidget, Wayland, etc.).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-11
 * @version 0.1
 * @since   0.12
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "../components/IShellLayer.h"
#include "../components/IShellLayerStrategy.h"
#include <QWidget>
#include <functional>
#include <memory>

namespace cf::desktop::platform {

/**
 * @brief  Platform factory API for shell layer and strategy creation.
 *
 * Encapsulates function pointers for creating shell layers (with their
 * platform-specific rendering backend) and default shell layer strategies
 * (including wallpaper support).
 *
 * @ingroup none
 */
struct ShellLayerFactoryAPI {
    /// Function type for creating shell layer instances.
    using ShellCreator = std::function<IShellLayer*(QWidget* parent)>;

    /// Function type for releasing shell layer instances.
    using ShellReleaser = std::function<void(IShellLayer*)>;

    /// Function type for creating default shell layer strategies.
    using StrategyCreator = std::function<std::unique_ptr<IShellLayerStrategy>()>;

    /// Function pointer for creating shell layer instances.
    ShellCreator shell_creator;

    /// Function pointer for releasing shell layer instances.
    ShellReleaser shell_releaser;

    /// Function pointer for creating the default shell layer strategy.
    StrategyCreator strategy_creator;
};

/**
 * @brief  Returns the native platform's shell layer factory API.
 *
 * @return ShellLayerFactoryAPI struct containing creation and release functions.
 * @throws None
 * @note   The returned API may have empty functions if no backend is available.
 * @since   0.12
 * @ingroup none
 */
CF_DESKTOP_EXPORT ShellLayerFactoryAPI native_shell_layer() noexcept;

} // namespace cf::desktop::platform
