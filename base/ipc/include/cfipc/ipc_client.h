/**
 * @file    ipc_client.h
 * @brief   One-shot IPC client for signaling the running shell instance.
 *
 * Provides a blocking send() that connects to the running shell's
 * IPCServer socket, writes one line-based message, and disconnects.
 * Used by a second shell instance (which failed to acquire the single-
 * instance lock) to ask the running one to raise itself before exiting.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#pragma once

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
     * @brief  Sends a single message token to the running shell.
     *
     * Connects to the socket at socket_path, writes the token followed by
     * a newline, then disconnects. On Windows, yields foreground privilege
     * afterward so the running instance can come to the front.
     *
     * @param[in]  socket_path  Path of the running shell's IPC socket.
     * @param[in]  message      Single token to send (no embedded newline).
     * @return     True if the message was written; false on connect failure
     *             (no running instance listening).
     * @throws     None
     * @note       Blocks up to ~200 ms per phase (connect / write).
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static bool send(const QString& socket_path, const QString& message);
};

} // namespace cf::ipc
