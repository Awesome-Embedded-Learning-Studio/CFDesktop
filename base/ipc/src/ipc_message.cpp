/**
 * @file    ipc_message.cpp
 * @brief   Implementation of IPCMessage serialization.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_message.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace cf::ipc {

QByteArray IPCMessage::toJson() const {
    QJsonObject obj;
    obj["type"] = type;
    obj["payload"] = payload;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<IPCMessage> IPCMessage::fromJson(const QByteArray& line) {
    const QJsonDocument doc = QJsonDocument::fromJson(line.trimmed());
    if (!doc.isObject()) {
        return std::nullopt;
    }
    const QJsonObject obj = doc.object();
    IPCMessage msg;
    msg.type = obj.value("type").toString();
    msg.payload = obj.value("payload").toObject();
    if (msg.type.isEmpty()) {
        return std::nullopt;
    }
    return msg;
}

} // namespace cf::ipc
