/**
 * @file    ipc_client.cpp
 * @brief   Implementation of the one-shot IPC client.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_client.h"

#include <QLocalSocket>

#ifdef Q_OS_WIN
#    include <windows.h>
#endif

namespace cf::ipc {

bool IPCClient::send(const QString& socket_path, const QString& message) {
    QLocalSocket socket;
    socket.connectToServer(socket_path);
    if (!socket.waitForConnected(200)) {
        // No running instance listening (or it crashed mid-handshake).
        return false;
    }
    socket.write((message + '\n').toUtf8());
    socket.waitForBytesWritten(200);
    socket.disconnectFromServer();
    if (socket.state() != QLocalSocket::UnconnectedState) {
        socket.waitForDisconnected(200);
    }

#ifdef Q_OS_WIN
    // Yield foreground privilege so the running instance may activate its
    // window; otherwise SetForegroundWindow is blocked by the foreground lock.
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    return true;
}

} // namespace cf::ipc
