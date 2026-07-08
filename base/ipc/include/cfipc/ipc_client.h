/**
 * @file    ipc_client.h
 * @brief   One-shot IPC client for sending a message to the running shell.
 *
 * Provides blocking send() overloads that connect to the running shell's
 * IPCServer socket, write one JSON message, and disconnect. Used by a
 * second shell instance (which failed to acquire the single-instance
 * lock) to ask the running one to raise itself before exiting.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#pragma once

#include "cfipc/ipc_message.h"

#include <QJsonObject>
#include <QObject>
#include <QString>

namespace cf::ipc {

/**
 * @brief  Static one-shot IPC client.
 *
 * All methods are blocking and need no event loop, so they are safe to
 * call directly from main() before QApplication::exec().
 *
 * @note   None
 * @warning None
 * @since  0.19.0
 * @ingroup ipc
 */
class IPCClient {
  public:
    /**
     * @brief  Sends a fully-formed message to the running shell.
     *
     * @param[in]  socket_path  Path of the running shell's IPC socket.
     * @param[in]  msg          The message to serialize and send.
     * @return     True if the message was written; false on connect failure.
     * @throws     None
     * @note       Blocks up to ~200 ms per phase (connect / write).
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static bool send(const QString& socket_path, const IPCMessage& msg);

    /**
     * @brief  Convenience overload: builds a message from type + payload.
     *
     * @param[in]  socket_path  Path of the running shell's IPC socket.
     * @param[in]  type         Message type tag.
     * @param[in]  payload      Optional JSON payload (defaults to empty).
     * @return     True if the message was written; false on connect failure.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static bool send(const QString& socket_path, const QString& type,
                     const QJsonObject& payload = {});
};

} // namespace cf::ipc
