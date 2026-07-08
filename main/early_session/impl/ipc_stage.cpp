/**
 * @file    ipc_stage.cpp
 * @brief   Implementation of the IPC server initialization stage.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup early_session
 */

#include "ipc_stage.h"

#include "cfipc/ipc_server.h"

#include <QDir>
#include <QtGlobal>

namespace cf::desktop::early_stage {

namespace {
/// @brief Socket path shared with the single-instance lock and client.
/// @note  Sits next to the QLockFile so both live in $TMPDIR.
constexpr QLatin1StringView kIpcSocketName{"/cfdesktop-shell.ipc"};
} // namespace

IpcStage::BootResult IpcStage::run_session() {
    const QString path = QDir::tempPath() + kIpcSocketName;
    if (!cf::ipc::IPCServer::instance().start(path)) {
        // Non-critical: a failed listener just means second-instance raise
        // will not work; the lock itself still prevents double boot.
        qWarning("IpcStage: failed to listen on %s", qPrintable(path));
        return BootResult::FAILED;
    }
    qInfo("IpcStage: listening on %s", qPrintable(path));
    return BootResult::OK;
}

} // namespace cf::desktop::early_stage
