/**
 * @file    ipc_notify_test.cpp
 * @brief   Unit tests for the "notify" IPC message routing.
 *
 * Verifies that dispatching a "notify" IPCMessage makes IPCServer emit
 * notifyReceived with the payload, and that "raise" routing still works
 * (backward compatibility). Signal capture uses a direct same-thread lambda
 * connection so no event loop is needed.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_message.h"
#include "cfipc/ipc_message_registry.h"
#include "cfipc/ipc_server.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

#include <gtest/gtest.h>

// A direct (same-thread) lambda connection fires synchronously on emit, so
// the counter is updated before the assertion runs -- no QSignalSpy/event
// loop machinery required inside a plain GTest binary.
TEST(IPCServerNotify, EmitsNotifyReceivedOnNotifyDispatch) {
    auto& server = cf::ipc::IPCServer::instance();
    int got = 0;
    QJsonObject captured;
    QObject receiver;
    QObject::connect(&server, &cf::ipc::IPCServer::notifyReceived, &receiver,
                     [&](const QJsonObject& payload) {
                         captured = payload;
                         ++got;
                     });

    cf::ipc::IPCMessage msg;
    msg.type = "notify";
    msg.payload["title"] = "Hello";
    msg.payload["message"] = "World";
    msg.payload["app_id"] = "org.cf.test";

    EXPECT_TRUE(cf::ipc::IPCMessageRegistry::instance().dispatch(msg));
    ASSERT_EQ(got, 1);
    EXPECT_EQ(captured.value("title").toString().toStdString(), "Hello");
    EXPECT_EQ(captured.value("app_id").toString().toStdString(), "org.cf.test");
}

TEST(IPCServerNotify, StillEmitsRaiseOnRaiseDispatch) {
    // Backward compatibility: adding "notify" must not disturb "raise".
    auto& server = cf::ipc::IPCServer::instance();
    int got = 0;
    QObject receiver;
    QObject::connect(&server, &cf::ipc::IPCServer::raiseRequested, &receiver, [&]() { ++got; });

    cf::ipc::IPCMessage msg;
    msg.type = "raise";
    EXPECT_TRUE(cf::ipc::IPCMessageRegistry::instance().dispatch(msg));
    EXPECT_EQ(got, 1);
}

TEST(IPCServerNotify, NotifyPayloadSurvivesWireRoundTrip) {
    // Round-trip through the JSON wire format before dispatching, so both the
    // parser and the routing are exercised.
    auto& server = cf::ipc::IPCServer::instance();
    QJsonObject captured;
    QObject receiver;
    QObject::connect(&server, &cf::ipc::IPCServer::notifyReceived, &receiver,
                     [&](const QJsonObject& payload) { captured = payload; });

    cf::ipc::IPCMessage original;
    original.type = "notify";
    original.payload["title"] = "Batch done";
    original.payload["actions"] = QJsonArray{"open", "dismiss"};
    const auto parsed = cf::ipc::IPCMessage::fromJson(original.toJson());
    ASSERT_TRUE(parsed.has_value());
    ASSERT_EQ(parsed->type.toStdString(), "notify");

    EXPECT_TRUE(cf::ipc::IPCMessageRegistry::instance().dispatch(*parsed));
    EXPECT_EQ(captured.value("title").toString().toStdString(), "Batch done");
    EXPECT_EQ(captured.value("actions").toArray().at(0).toString().toStdString(), "open");
}
