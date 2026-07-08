/**
 * @file    ipc_server.cpp
 * @brief   Implementation of the single-instance IPC server.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_server.h"

#include <QLocalSocket>

namespace cf::ipc {

IPCServer& IPCServer::instance() {
    static IPCServer server;
    return server;
}

IPCServer::IPCServer() {
    connect(&server_, &QLocalServer::newConnection, this, &IPCServer::onNewConnection);
}

bool IPCServer::start(const QString& socket_path) {
    // Reclaim a stale socket left behind by a crashed previous instance.
    // QLocalServer::removeServer unlinks the file iff nothing is listening.
    QLocalServer::removeServer(socket_path);
    return server_.listen(socket_path);
}

void IPCServer::stop() {
    server_.close();
}

void IPCServer::onNewConnection() {
    QLocalSocket* conn = server_.nextPendingConnection();
    if (conn == nullptr) {
        return;
    }
    // The server owns the connection for its lifetime; auto-delete on close.
    conn->setParent(this);
    connect(conn, &QLocalSocket::disconnected, conn, &QObject::deleteLater);
    connect(conn, &QLocalSocket::readyRead, this, [this, conn]() {
        while (conn->canReadLine()) {
            const QByteArray line = conn->readLine().trimmed();
            if (line == "raise") {
                emit raiseRequested();
            }
        }
    });
}

} // namespace cf::ipc
