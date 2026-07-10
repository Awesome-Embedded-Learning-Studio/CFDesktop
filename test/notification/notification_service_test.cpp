/**
 * @file    notification_service_test.cpp
 * @brief   Unit tests for NotificationService.
 *
 * Covers posting / dismissal / clear-all, newest-first ordering, signal
 * emission, and Do-Not-Disturb banner suppression backed by the real
 * ConfigStore (initialized with a mock path provider). Signal capture uses
 * direct same-thread lambda connections, so no event loop is needed.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#include "notification_service.h"

#include "cfconfig.hpp"
#include "mock_path_provider.h"

#include <QObject>
#include <QString>

#include <gtest/gtest.h>

using cf::desktop::desktop_component::Notification;
using cf::desktop::desktop_component::NotificationService;

namespace {
/// Counts signal emissions via a direct (same-thread) connection. The QObject
/// member is the connection context; its lifetime tracks the counter.
struct SignalCounter {
    QObject ctx;
    int posted{0};
    int dismissed{0};
    int cleared{0};
    int dnd{0};
    bool last_suppressed{false};
    Notification last_posted;

    explicit SignalCounter(NotificationService& svc) {
        QObject::connect(&svc, &NotificationService::notificationPosted, &ctx,
                         [&](const Notification& n, bool suppressed) {
                             last_posted = n;
                             last_suppressed = suppressed;
                             ++posted;
                         });
        QObject::connect(&svc, &NotificationService::notificationDismissed, &ctx,
                         [&](const QString&) { ++dismissed; });
        QObject::connect(&svc, &NotificationService::allCleared, &ctx, [&]() { ++cleared; });
        QObject::connect(&svc, &NotificationService::dndChanged, &ctx, [&](bool) { ++dnd; });
    }
};

Notification make(QString title) {
    Notification n;
    n.title = std::move(title);
    n.message = "body";
    n.app_id = "org.cf.test";
    return n;
}
} // namespace

class NotificationServiceTest : public ::testing::Test {
  protected:
    static bool config_initialized_;

    void SetUp() override {
        if (!config_initialized_) {
            auto provider = std::make_shared<cf::config::test::MockPathProvider>();
            cf::config::ConfigStore::instance().initialize(provider);
            config_initialized_ = true;
        }
        // Clean slate per test: drop notifications + reset DND.
        NotificationService::instance().clearAll();
        NotificationService::instance().setDndEnabled(false);
    }
};

bool NotificationServiceTest::config_initialized_ = false;

TEST_F(NotificationServiceTest, PostAddsToListNewestFirst) {
    auto& svc = NotificationService::instance();
    svc.post(make("A"));
    svc.post(make("B"));
    const auto all = svc.all();
    ASSERT_EQ(all.size(), 2);
    EXPECT_EQ(all[0].title.toStdString(), "B"); // newest first
    EXPECT_EQ(all[1].title.toStdString(), "A");
}

TEST_F(NotificationServiceTest, PostGeneratesIdTimestampAndEmits) {
    auto& svc = NotificationService::instance();
    SignalCounter counter(svc);
    svc.post(make("Hi"));
    ASSERT_EQ(counter.posted, 1);
    EXPECT_FALSE(counter.last_posted.id.isEmpty());
    EXPECT_GT(counter.last_posted.timestamp, 0);
    EXPECT_FALSE(counter.last_suppressed); // DND off by default
}

TEST_F(NotificationServiceTest, DismissRemovesByIdAndEmits) {
    auto& svc = NotificationService::instance();
    SignalCounter counter(svc);
    svc.post(make("X"));
    const QString id = svc.all().first().id;
    EXPECT_TRUE(svc.dismiss(id));
    EXPECT_TRUE(svc.all().isEmpty());
    EXPECT_EQ(counter.dismissed, 1);
    EXPECT_FALSE(svc.dismiss("nope")); // already gone -> no-op
}

TEST_F(NotificationServiceTest, ClearAllEmitsAndReturnsCount) {
    auto& svc = NotificationService::instance();
    SignalCounter counter(svc);
    svc.post(make("1"));
    svc.post(make("2"));
    EXPECT_EQ(svc.clearAll(), 2);
    EXPECT_EQ(svc.count(), 0);
    EXPECT_EQ(counter.cleared, 1);
    EXPECT_EQ(svc.clearAll(), 0); // empty -> no emit, zero removed
}

TEST_F(NotificationServiceTest, DndSuppressesBannerButKeepsCenter) {
    auto& svc = NotificationService::instance();
    SignalCounter counter(svc);
    svc.setDndEnabled(true);
    ASSERT_EQ(counter.dnd, 1);
    EXPECT_TRUE(svc.isDndEnabled());

    svc.post(make("quiet"));
    EXPECT_EQ(counter.posted, 1);
    EXPECT_TRUE(counter.last_suppressed); // banner suppressed
    EXPECT_EQ(svc.count(), 1);            // but still recorded in the center
}

TEST_F(NotificationServiceTest, DndRoundTripsThroughConfigStore) {
    auto& svc = NotificationService::instance();
    svc.setDndEnabled(true);
    EXPECT_TRUE(svc.isDndEnabled());
    svc.setDndEnabled(false);
    EXPECT_FALSE(svc.isDndEnabled());
}
