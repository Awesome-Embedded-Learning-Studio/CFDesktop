/**
 * @file    app_launcher_test.cpp
 * @brief   Unit tests for the AppLauncher popup.
 *
 * Verifies tile-grid construction, the show/hide lifecycle, ESC dismissal,
 * and that a tile click propagates appLaunched(). Runs headless via the
 * offscreen Qt platform plugin.
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
            // segfaults under the offscreen platform once popup windows were
            // shown, so let the OS reclaim it instead.
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
/// @brief popup() shows the launcher; ESC hides it.
TEST_F(AppLauncherTest, EscapeHidesPopup) {
    AppLauncher launcher;
    launcher.setApps(defaultApps());
    launcher.setWindowFlags(Qt::Window); // Avoid Qt::Popup windowing in headless tests.
    launcher.popup(QRect(0, 0, 1920, 1080));
    EXPECT_TRUE(launcher.isShowing());
    QTest::keyClick(&launcher, Qt::Key_Escape);
    EXPECT_FALSE(launcher.isShowing());
}

/// @brief Clicking a tile emits appLaunched() with that tile's app_id.
TEST_F(AppLauncherTest, TileClickEmitsAppLaunched) {
    AppLauncher launcher;
    const auto apps = defaultApps();
    launcher.setApps(apps);
    launcher.setWindowFlags(Qt::Window); // Avoid Qt::Popup windowing in headless tests.
    launcher.popup(QRect(0, 0, 1920, 1080));

    const QList<LauncherTile*> tiles = launcher.findChildren<LauncherTile*>();
    ASSERT_FALSE(tiles.isEmpty());
    QSignalSpy spy(&launcher, &AppLauncher::appLaunched);
    QTest::mouseClick(tiles.first(), Qt::LeftButton, {}, QPoint(48, 38));
    if (spy.count() == 0) {
        spy.wait(1000);
    }
    EXPECT_EQ(spy.count(), 1);
    if (spy.count() == 1) {
        EXPECT_EQ(spy.takeFirst().at(0).toString(), apps.first().app_id);
    }
}
#endif
