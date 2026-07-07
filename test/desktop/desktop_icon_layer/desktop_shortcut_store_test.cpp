/**
 * @file    test/desktop/desktop_icon_layer/desktop_shortcut_store_test.cpp
 * @brief   GoogleTest unit tests for DesktopShortcutStore.
 *
 * Covers JSON serialize/deserialize round-trip, malformed/empty input, empty
 * app_id filtering, and seedFrom() grid layout + cap. load()/save() (real
 * ConfigStore IO) are intentionally not covered here to avoid polluting the
 * host config; the serialization layer they delegate to is fully tested.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "desktop_icon_layer/desktop_shortcut.h"
#include "desktop_icon_layer/desktop_shortcut_store.h"

#include <QByteArray>
#include <QString>

#include <gtest/gtest.h>

using cf::desktop::desktop_component::AppEntry;
using cf::desktop::desktop_component::DesktopShortcut;
using cf::desktop::desktop_component::DesktopShortcutStore;

TEST(DesktopShortcutStore, SerializeDeserializeRoundTrip) {
    QList<DesktopShortcut> in;
    in.append({QStringLiteral("about"), 0, 0});
    in.append({QStringLiteral("calculator"), 1, 0});
    in.append({QStringLiteral("firefox"), 0, 1});
    const QByteArray bytes = DesktopShortcutStore::serialize(in);
    const QList<DesktopShortcut> out = DesktopShortcutStore::deserialize(bytes);
    ASSERT_EQ(out.size(), in.size());
    for (int i = 0; i < in.size(); ++i) {
        EXPECT_EQ(out[i].app_id.toStdString(), in[i].app_id.toStdString());
        EXPECT_EQ(out[i].col, in[i].col);
        EXPECT_EQ(out[i].row, in[i].row);
    }
}

TEST(DesktopShortcutStore, DeserializeEmptyReturnsEmpty) {
    EXPECT_TRUE(DesktopShortcutStore::deserialize(QByteArray()).isEmpty());
}

TEST(DesktopShortcutStore, DeserializeMalformedReturnsEmpty) {
    EXPECT_TRUE(DesktopShortcutStore::deserialize(QByteArray("not json{")).isEmpty());
}

TEST(DesktopShortcutStore, DeserializeSkipsEmptyAppId) {
    const QByteArray bytes = R"([{"app_id":"","col":0,"row":0},{"app_id":"x","col":1,"row":0}])";
    const auto out = DesktopShortcutStore::deserialize(bytes);
    ASSERT_EQ(out.size(), 1);
    EXPECT_EQ(out[0].app_id.toStdString(), "x");
}

TEST(DesktopShortcutStore, SeedFromLaysOutInGridOrder) {
    QList<AppEntry> apps;
    for (int i = 0; i < 5; ++i) {
        AppEntry e;
        e.app_id = QStringLiteral("app%1").arg(i);
        apps.append(e);
    }
    const auto seed = DesktopShortcutStore::seedFrom(apps);
    ASSERT_EQ(seed.size(), 5);
    // kSeedCols = 8, so the first 8 land in row 0, left-to-right.
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(seed[i].app_id.toStdString(), "app" + std::to_string(i));
        EXPECT_EQ(seed[i].col, i);
        EXPECT_EQ(seed[i].row, 0);
    }
}

TEST(DesktopShortcutStore, SeedFromWrapsToSecondRow) {
    QList<AppEntry> apps;
    for (int i = 0; i < 10; ++i) {
        AppEntry e;
        e.app_id = QStringLiteral("a%1").arg(i);
        apps.append(e);
    }
    const auto seed = DesktopShortcutStore::seedFrom(apps);
    ASSERT_EQ(seed.size(), 10);
    // Index 8 wraps to col 0, row 1.
    EXPECT_EQ(seed[8].col, 0);
    EXPECT_EQ(seed[8].row, 1);
}

TEST(DesktopShortcutStore, SeedFromCapsAtLimit) {
    QList<AppEntry> apps;
    for (int i = 0; i < 100; ++i) {
        AppEntry e;
        e.app_id = QStringLiteral("a%1").arg(i);
        apps.append(e);
    }
    EXPECT_EQ(DesktopShortcutStore::seedFrom(apps).size(), 12); // kSeedCount
}
