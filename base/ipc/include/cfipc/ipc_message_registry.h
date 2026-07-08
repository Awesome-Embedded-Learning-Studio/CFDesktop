/**
 * @file    ipc_message_registry.h
 * @brief   Type-routed dispatch for incoming IPC messages.
 *
 * Holds a map of message type to handler callable. The IPCServer looks
 * up each received message here and invokes the matching handler.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#pragma once

#include "cfipc/ipc_message.h"

#include <QHash>
#include <QJsonObject>
#include <QString>
#include <functional>

namespace cf::ipc {

/**
 * @brief  Process-wide registry of IPC message handlers.
 *
 * Callers register a handler per message type; the IPCServer dispatches
 * each incoming message by looking its type up here.
 *
 * @note   None
 * @warning None
 * @since  0.19.0
 * @ingroup ipc
 */
class IPCMessageRegistry {
  public:
    /// @brief Callable invoked for a matching message payload.
    using Handler = std::function<void(const QJsonObject& payload)>;

    /**
     * @brief  Returns the process-wide singleton instance.
     *
     * @return     Reference to the singleton registry.
     * @throws     None
     * @note       Created on first access.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static IPCMessageRegistry& instance();

    /**
     * @brief  Registers or replaces a handler for a message type.
     *
     * @param[in]  type     Message type to handle.
     * @param[in]  handler  Callable invoked when a matching message arrives.
     * @throws     None
     * @note       A later registration for the same type replaces the prior.
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    void registerHandler(const QString& type, Handler handler);

    /**
     * @brief  Routes a message to its registered handler.
     *
     * @param[in]  msg  The message to dispatch.
     * @return     True if a handler was found and invoked; false otherwise.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    bool dispatch(const IPCMessage& msg) const;

  private:
    IPCMessageRegistry() = default;

    /// @brief Map of message type to handler.
    QHash<QString, Handler> handlers_;
};

} // namespace cf::ipc
