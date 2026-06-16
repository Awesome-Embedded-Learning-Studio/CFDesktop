/**
 * @file    app_launch_service.h
 * @brief   Launches external applications via QProcess.
 *
 * AppLaunchService detaches an external process for a given command string,
 * splitting it into program + arguments so commands such as "xdg-open ." or
 * "xdg-open https://example.com" resolve correctly. launch() returns the
 * process id on success, or an AppLaunchError on failure; failures are logged
 * and never silently swallowed.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-16
 * @version 0.2
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include "base/expected/expected.hpp"

#include <QString>

namespace cf::desktop::desktop_component {

/**
 * @brief  Reason an application launch did not produce a process.
 *
 * @ingroup components
 */
enum class AppLaunchError {
    Empty,      ///< Command was empty or only whitespace.
    Unparsable, ///< Command could not be split into program + arguments.
    Failed      ///< The process could not be started.
};

/**
 * @brief  Stateless service that launches external applications.
 *
 * @ingroup components
 */
class AppLaunchService {
  public:
    /// @brief Deleted; AppLaunchService is static-only.
    AppLaunchService() = delete;

    /**
     * @brief  Detaches an external process for a command string.
     *
     * Splits the command into a program and its arguments, then starts it
     * detached so CFDesktop is not the parent and the launched app outlives
     * the shell.
     *
     * @param[in] exec_command  The command string (program plus arguments).
     *
     * @return The started process id, or an AppLaunchError. Empty or
     *         unparsable commands and launch failures are logged before the
     *         error is returned; they are never hidden.
     *
     * @throws None
     * @note   Failures log a warning or error naming the command.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    static cf::expected<qint64, AppLaunchError> launch(const QString& exec_command);
};

} // namespace cf::desktop::desktop_component
