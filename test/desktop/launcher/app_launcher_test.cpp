/**
 * @file    app_launcher_test.cpp
 * @brief   Unit tests for the AppLauncher popup.
 *
 * Verifies tile-grid construction and the initial hidden state, plus (when
 * Qt6::Test is available) that a tile click propagates appLaunched(). Tests
 * deliberately avoid showing any top-level window or spinning an event loop:
 * under several CI offscreen platforms showing a window / running the event
 * loop blocks, which would hang the suite. Runs headless via the offscreen Qt
 * platform plugin.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "app_entry.h"
#include "app_launcher.h"
#include "launcher_tile.h"

#include <gtest/gtest.h>

#include <QApplication>
#include <QCoreApplication>
#include <QList>
#include <QRect>
#include <QString>
#include <QWidget>

#ifdef QT_TEST_AVAILABLE
#    include <QSignalSpy>
#    include <QtTest/QTest>
#endif

using cf::desktop::desktop_component::AppLauncher;
using cf::desktop::desktop_component::defaultApps;
using cf::desktop::desktop_component::LauncherTile;

namespace {
/// @brief Ensures exactly one offscreen QApplication exists for the suite.
class AppLauncherTest : public ::testing::Test {
  protected:
    static void SetUpTestSuite() {
        qputenv("QT_QPA_PLATFORM", "offscreen");
        if (QCoreApplication::instance() == nullptr) {
            static int argc = 1;
            static char arg0[] = "app_launcher_test";
            static char* argv[] = {arg0, nullptr};
            // Intentionally leaked: destroying QApplication at process exit
            // segfaults under some offscreen platforms once windows were shown,
            // so let the OS reclaim it instead.
            new QApplication(argc, argv);
        }
    }
};
} // namespace

/// @brief setApps() builds one tile per AppEntry.
TEST_F(AppLauncherTest, SetAppsBuildsExpectedTileCount) {
    AppLauncher launcher;
    const auto apps = defaultApps();
    launcher.setApps(apps);
    const QList<LauncherTile*> tiles = launcher.findChildren<LauncherTile*>();
    EXPECT_EQ(tiles.size(), apps.size());
}

/// @brief A freshly constructed launcher is hidden.
TEST_F(AppLauncherTest, StartsHidden) {
    AppLauncher launcher;
    EXPECT_FALSE(launcher.isShowing());
}

#ifdef QT_TEST_AVAILABLE
/// @brief Clicking a tile emits appLaunched() with that tile's app_id. No
///        window is shown and no event loop is spun, so this stays headless-safe.
TEST_F(AppLauncherTest, TileClickEmitsAppLaunched) {
    AppLauncher launcher;
    const auto apps = defaultApps();
    launcher.setApps(apps);
    const QList<LauncherTile*> tiles = launcher.findChildren<LauncherTile*>();
    ASSERT_FALSE(tiles.isEmpty());
    QSignalSpy spy(&launcher, &AppLauncher::appLaunched);
    QTest::mouseClick(tiles.first(), Qt::LeftButton, {}, QPoint(48, 38));
    EXPECT_EQ(spy.count(), 1);
    if (spy.count() == 1) {
        EXPECT_EQ(spy.takeFirst().at(0).toString(), apps.first().app_id);
    }
}
#endif
