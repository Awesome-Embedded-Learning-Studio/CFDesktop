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

#include "cfipc/ipc_message.h"
#include "cfipc/ipc_message_registry.h"

#include <QJsonObject>
#include <QLocalSocket>

namespace cf::ipc {

IPCServer& IPCServer::instance() {
    static IPCServer server;
    return server;
}

IPCServer::IPCServer() {
    connect(&server_, &QLocalServer::newConnection, this, &IPCServer::onNewConnection);
    // Route the "raise" message type to our signal; other types are owned
    // by whoever registered them in the registry.
    IPCMessageRegistry::instance().registerHandler(
        "raise", [this](const QJsonObject&) { emit raiseRequested(); });
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
    connect(conn, &QLocalSocket::readyRead, this, [conn]() {
        // Line-based JSON framing: one IPCMessage per newline.
        while (conn->canReadLine()) {
            const auto msg = IPCMessage::fromJson(conn->readLine());
            if (msg.has_value()) {
                IPCMessageRegistry::instance().dispatch(*msg);
            }
        }
    });
}

} // namespace cf::ipc
