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

    // Line-based JSON framing: one IPCMessage per newline. Drain on every
    // readyRead, on disconnect, and once up front. The disconnect + up-front
    // drains matter on Windows named pipes: a one-shot client that writes and
    // closes immediately can deliver the final bytes without a separate
    // readyRead, and bytes may already be buffered before this slot is wired.
    auto drain = [conn]() {
        while (conn->canReadLine()) {
            const auto msg = IPCMessage::fromJson(conn->readLine());
            if (msg.has_value()) {
                IPCMessageRegistry::instance().dispatch(*msg);
            }
        }
    };
    connect(conn, &QLocalSocket::readyRead, this, drain);
    connect(conn, &QLocalSocket::disconnected, this, [conn, drain]() {
        // A one-shot peer can deliver final bytes without a readyRead (notably
        // on Windows named pipes); force Qt to pull them before draining.
        conn->waitForReadyRead(100);
        drain();
        conn->deleteLater();
    });
    // Bytes may have arrived (and the peer gone) before this slot was wired;
    // pull and drain now.
    conn->waitForReadyRead(100);
    drain();
}

} // namespace cf::ipc
