/**
 * @file app_launch_service.cpp
 * @brief External application launch implementation.
 *
 * Splits the command string with QProcess::splitCommand (so quoted arguments
 * and URLs survive), then starts the program detached. The shell is bypassed
 * on purpose: a detached process is a direct child, which keeps launches
 * predictable and avoids an extra shell process.
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-16
 * @version 0.2
 * @since 0.19
 * @ingroup components
 */

#include "app_launch_service.h"

#include "cflog.h"

#include <QProcess>
#include <QStringList>

namespace cf::desktop::desktop_component {

namespace {
constexpr const char* kTag = "AppLaunchService";
}

aex::expected<qint64, AppLaunchError> AppLaunchService::launch(const QString& exec_command) {
    if (exec_command.trimmed().isEmpty()) {
        cf::log::warningftag(kTag, "launch skipped: empty exec_command");
        return aex::unexpected(AppLaunchError::Empty);
    }

    const QStringList parts = QProcess::splitCommand(exec_command);
    if (parts.isEmpty()) {
        cf::log::warningftag(kTag, "launch skipped: unparsable command '{}'",
                             exec_command.toStdString());
        return aex::unexpected(AppLaunchError::Unparsable);
    }

    const QString program = parts.first();
    const QStringList args = parts.mid(1);

    qint64 pid = 0;
    if (QProcess::startDetached(program, args, QString{}, &pid)) {
        cf::log::infoftag(kTag, "launched '{}' (pid {})", exec_command.toStdString(), pid);
        return pid;
    }

    cf::log::errorftag(kTag, "failed to launch '{}'", exec_command.toStdString());
    return aex::unexpected(AppLaunchError::Failed);
}

} // namespace cf::desktop::desktop_component
