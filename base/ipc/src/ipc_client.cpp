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

namespace {
/// @brief Connects, writes raw bytes, disconnects. Best-effort, blocking.
/// @note  Yields Windows foreground privilege afterward so the running
///        instance may activate its window.
bool sendRaw(const QString& socket_path, const QByteArray& data) {
    QLocalSocket socket;
    socket.connectToServer(socket_path);
    if (!socket.waitForConnected(200)) {
        return false;
    }
    socket.write(data);
    socket.waitForBytesWritten(200);
    socket.disconnectFromServer();
    if (socket.state() != QLocalSocket::UnconnectedState) {
        socket.waitForDisconnected(200);
    }

#ifdef Q_OS_WIN
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    return true;
}
} // namespace

bool IPCClient::send(const QString& socket_path, const IPCMessage& msg) {
    return sendRaw(socket_path, msg.toJson() + '\n');
}

bool IPCClient::send(const QString& socket_path, const QString& type, const QJsonObject& payload) {
    return send(socket_path, IPCMessage{type, payload});
}

} // namespace cf::ipc
