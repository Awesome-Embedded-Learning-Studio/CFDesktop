/**
 * @file    ipc_registry_test.cpp
 * @brief   Unit tests for IPCMessageRegistry dispatch.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_message_registry.h"

#include <gtest/gtest.h>

TEST(IPCMessageRegistry, DispatchesToRegisteredHandler) {
    auto& reg = cf::ipc::IPCMessageRegistry::instance();
    int hits = 0;
    QJsonObject captured;
    reg.registerHandler("unit_test_type", [&](const QJsonObject& p) {
        captured = p;
        ++hits;
    });

    cf::ipc::IPCMessage msg;
    msg.type = "unit_test_type";
    msg.payload["v"] = 7;

    EXPECT_TRUE(reg.dispatch(msg));
    EXPECT_EQ(hits, 1);
    EXPECT_EQ(captured.value("v").toInt(), 7);
}

TEST(IPCMessageRegistry, ReturnsFalseForUnknownType) {
    cf::ipc::IPCMessage msg;
    msg.type = "definitely_unregistered_type_xyz";
    EXPECT_FALSE(cf::ipc::IPCMessageRegistry::instance().dispatch(msg));
}
