/**
 * @file    ipc_server.h
 * @brief   Single-instance IPC server for the desktop shell.
 *
 * Listens on a QLocalServer socket so a second shell instance can signal
 * the running one. Uses a line-based protocol: each message is a
 * newline-terminated token. The only token defined in this phase is
 * "raise", which asks the running shell to come to the foreground.
 *
 * The server is a process-wide singleton; the early-session boot chain
 * starts it, and the desktop entity connects to its signals.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#pragma once

#include <QLocalServer>
#include <QObject>
#include <QString>

namespace cf::ipc {

/**
 * @brief  Process-wide singleton IPC server.
 *
 * Owns a QLocalServer and emits signals in response to messages received
 * from peer (second-instance) clients. The shell connects to the signals
 * to react to activation requests.
 *
 * @note   Thread-affine: must be created and used on the main (Qt event
 *         loop) thread.
 * @warning None
 * @since  0.19.0
 * @ingroup ipc
 *
 * @code
 * auto& server = cf::ipc::IPCServer::instance();
 * server.start("/tmp/cfdesktop-shell.ipc");
 * QObject::connect(&server, &cf::ipc::IPCServer::raiseRequested,
 *                  [](){ /* bring shell to front *\/ });
 * @endcode
 */
class IPCServer : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief  Returns the process-wide singleton instance.
     *
     * @return     Reference to the singleton IPCServer.
     * @throws     None
     * @note       Created on first access; lives until process exit.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static IPCServer& instance();

    /**
     * @brief  Starts listening on the given socket path.
     *
     * Removes any stale socket file left by a crashed previous instance
     * before listening.
     *
     * @param[in]  socket_path  Filesystem path for the QLocalServer socket.
     * @return     True if listening started; false if the port is taken.
     * @throws     None
     * @note       Safe to call once; idempotent calls close and re-listen.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    bool start(const QString& socket_path);

    /**
     * @brief  Stops listening and closes the server socket.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    void stop();

  signals:
    /**
     * @brief  Emitted when a peer sends the "raise" token.
     *
     * The shell should bring its main window to the foreground.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    void raiseRequested();

  private slots:
    /**
     * @brief  Handles an incoming peer connection.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    void onNewConnection();

  private:
    /**
     * @brief  Constructs the IPC server.
     *
     * @throws     None
     * @note       Private; access via instance().
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    IPCServer();

    /// @brief The underlying Qt local server.
    QLocalServer server_;
};

} // namespace cf::ipc
