/**
 * @file    ipc_message_registry.cpp
 * @brief   Implementation of the IPC message handler registry.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_message_registry.h"

namespace cf::ipc {

IPCMessageRegistry& IPCMessageRegistry::instance() {
    static IPCMessageRegistry registry;
    return registry;
}

void IPCMessageRegistry::registerHandler(const QString& type, Handler handler) {
    handlers_[type] = std::move(handler);
}

bool IPCMessageRegistry::dispatch(const IPCMessage& msg) const {
    const auto it = handlers_.constFind(msg.type);
    if (it == handlers_.cend()) {
        return false;
    }
    it.value()(msg.payload);
    return true;
}

} // namespace cf::ipc
