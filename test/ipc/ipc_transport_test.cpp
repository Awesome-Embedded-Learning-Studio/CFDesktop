/**
 * @file    ipc_transport_test.cpp
 * @brief   Integration test: IPCClient send -> IPCServer -> registry dispatch.
 *
 * Spins a QEventLoop so the server's async readyRead can deliver the
 * message the blocking client just wrote. Uses a custom main() to provide
 * the QCoreApplication that QLocalSocket requires.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup ipc
 */

#include "cfipc/ipc_client.h"
#include "cfipc/ipc_message_registry.h"
#include "cfipc/ipc_server.h"

#include <QCoreApplication>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonObject>
#include <QTimer>

#include <atomic>

#include <gtest/gtest.h>

namespace {
/// @brief Builds a unique socket path per test to avoid collisions.
QString uniquePath() {
    static std::atomic<int> counter{0};
    return QDir::tempPath() + "/cfdesktop-ipc-test-" +
           QString::number(QCoreApplication::applicationPid()) + "-" +
           QString::number(counter.fetch_add(1)) + ".ipc";
}
} // namespace

class IPCTransportTest : public ::testing::Test {
  protected:
    void SetUp() override {
        path_ = uniquePath();
        ASSERT_TRUE(cf::ipc::IPCServer::instance().start(path_));
    }
    void TearDown() override {
        cf::ipc::IPCServer::instance().stop();
        QFile::remove(path_);
    }
    QString path_;
};

TEST_F(IPCTransportTest, ClientMessageDispatchesOnServer) {
    QJsonObject received;
    QEventLoop loop;
    cf::ipc::IPCMessageRegistry::instance().registerHandler("transport_test",
                                                            [&](const QJsonObject& p) {
                                                                received = p;
                                                                loop.quit();
                                                            });
    // Timeout fallback so the test cannot hang if delivery fails.
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);

    QJsonObject payload;
    payload["hello"] = "world";

    // Send from inside the event loop. On Windows named pipes the server must
    // be spinning its loop to accept the connection and emit readyRead before
    // the one-shot client disconnects; sending before loop.exec() drops the
    // message there. (Production always has the server loop running.)
    bool sent = false;
    QTimer::singleShot(
        0, [&]() { sent = cf::ipc::IPCClient::send(path_, "transport_test", payload); });

    loop.exec();
    ASSERT_TRUE(sent);
    EXPECT_EQ(received.value("hello").toString().toStdString(), "world");
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
