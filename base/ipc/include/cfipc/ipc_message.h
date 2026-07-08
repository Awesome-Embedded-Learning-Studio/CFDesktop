/**
 * @file    ipc_message.h
 * @brief   A single IPC message: a type tag plus a JSON payload.
 *
 * Wire format is one compact JSON object per line, newline-terminated:
 * @code
 * {"type":"raise","payload":{...}}
 * @endcode
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QString>
#include <optional>

namespace cf::ipc {

/**
 * @brief  One inter-process message.
 *
 * Composed of a type tag (selects the handler) and an arbitrary JSON
 * payload. Serializes to a single compact JSON line for transport.
 *
 * @note   None
 * @warning None
 * @since  0.19.0
 * @ingroup ipc
 */
struct IPCMessage {
    /// @brief Handler selector (e.g. "raise", "notify").
    QString type;

    /// @brief Arbitrary JSON body; may be empty.
    QJsonObject payload;

    /**
     * @brief  Serializes the message to a compact JSON line.
     *
     * @return     UTF-8 JSON bytes (no trailing newline).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    QByteArray toJson() const;

    /**
     * @brief  Parses one JSON line into a message.
     *
     * @param[in]  line  Raw bytes received (trailing newline tolerated).
     * @return     The parsed message, or std::nullopt if the line is not a
     *             valid object or has an empty type.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    ipc
     */
    static std::optional<IPCMessage> fromJson(const QByteArray& line);
};

} // namespace cf::ipc
