/**
 * @file    ipc_message_test.cpp
 * @brief   Unit tests for IPCMessage JSON serialization.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_message.h"

#include <gtest/gtest.h>

TEST(IPCMessage, RoundTripsTypeAndPayload) {
    cf::ipc::IPCMessage msg;
    msg.type = "raise";
    msg.payload["x"] = 42;
    msg.payload["name"] = "test";

    const auto parsed = cf::ipc::IPCMessage::fromJson(msg.toJson());
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->type.toStdString(), "raise");
    EXPECT_EQ(parsed->payload.value("x").toInt(), 42);
    EXPECT_EQ(parsed->payload.value("name").toString().toStdString(), "test");
}

TEST(IPCMessage, RejectsEmptyType) {
    const QByteArray wire = R"({"type":"","payload":{}})";
    EXPECT_FALSE(cf::ipc::IPCMessage::fromJson(wire).has_value());
}

TEST(IPCMessage, RejectsMalformedJson) {
    EXPECT_FALSE(cf::ipc::IPCMessage::fromJson("not json").has_value());
    EXPECT_FALSE(cf::ipc::IPCMessage::fromJson("").has_value());
    EXPECT_FALSE(cf::ipc::IPCMessage::fromJson("[]").has_value()); // array, not object
}
