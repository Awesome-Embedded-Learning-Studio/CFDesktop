/**
 * @file    crash_handler_stage.cpp
 * @brief   Implementation of the crash handler initialization stage.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup early_session
 */

#include "crash_handler_stage.h"

#include "cfcrash/crash_handler.h"
#include "early_handle/early_handle.h"

#include <QCoreApplication>
#include <QLatin1String>
#include <QString>
#include <QtGlobal>

namespace cf::desktop::early_stage {

namespace {
/// @brief Crash reports live next to the executable, alongside the logger.
/// @note  Matches app_runtime_dir() == QCoreApplication::applicationDirPath().
constexpr QLatin1StringView kCrashesSubdir{"/crashes"};
} // namespace

CrashHandlerStage::BootResult CrashHandlerStage::run_session() {
    const QString dir = QCoreApplication::applicationDirPath() + kCrashesSubdir;
    // Hand this run's logger path to install() so the handler records it in
    // each .pending; finalize() then tails the crashed run's own log.
    const QString logger_path = EarlyHandle::instance().early_settings().get_boot_logger_path();
    if (!cf::crash::CrashHandler::instance().install(dir.toStdString(),
                                                     logger_path.toStdString())) {
        // Non-critical: handler arming is best-effort and platform-dependent.
        qWarning("CrashHandlerStage: signal handlers not armed on this platform");
    }

    // Fold any .pending left by a previous crashed run into a finalized .json.
    const auto finalized =
        cf::crash::CrashHandler::instance().finalizePendingReports(logger_path.toStdString());
    if (finalized > 0) {
        qInfo("CrashHandlerStage: finalized %lu crash report(s) from previous run",
              static_cast<unsigned long>(finalized));
    }
    return BootResult::OK;
}

} // namespace cf::desktop::early_stage
