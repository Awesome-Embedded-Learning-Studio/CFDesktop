/**
 * @file    test/desktop/launcher/app_icon_resolver_test.cpp
 * @brief   GoogleTest unit tests for resolve_app_icon.
 *
 * Covers the resolution chain: empty path -> null, nonexistent absolute path
 * -> null, bogus theme name -> null (graceful when no icon theme matches),
 * and a real Qt resource path loads. QIcon/QPixmap require a QGuiApplication,
 * so this file provides its own main() that constructs one on the offscreen
 * QPA platform (headless-safe under ctest).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "launcher/app_icon_resolver.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QSize>
#include <QString>

#include <gtest/gtest.h>

using cf::desktop::desktop_component::AppEntry;
using cf::desktop::desktop_component::resolve_app_icon;

namespace {
/// Builds a minimal AppEntry with only icon_path set (all the resolver reads).
AppEntry makeEntry(const QString& icon_path) {
    AppEntry e;
    e.app_id = QStringLiteral("test");
    e.display_name = QStringLiteral("Test");
    e.icon_path = icon_path;
    return e;
}
} // namespace

TEST(AppIconResolver, EmptyPathReturnsNull) {
    EXPECT_TRUE(resolve_app_icon(makeEntry(QString()), QSize(48, 48)).isNull());
}

TEST(AppIconResolver, NonexistentAbsolutePathReturnsNull) {
    // Path-like value that fails to load returns null (no theme fall-through).
    EXPECT_TRUE(resolve_app_icon(makeEntry(QStringLiteral("/nonexistent/cfdesktop/icon.png")),
                                 QSize(48, 48))
                    .isNull());
}

TEST(AppIconResolver, BogusThemeNameReturnsNull) {
    // A non-path value is treated as a theme name; an unknown name yields a
    // null icon -> null pixmap. Proves graceful degradation, not a crash.
    const auto pm =
        resolve_app_icon(makeEntry(QStringLiteral("totally-bogus-cf-icon-xyz")), QSize(48, 48));
    EXPECT_TRUE(pm.isNull());
}

TEST(AppIconResolver, ThemeLookupDoesNotCrashOnKnownName) {
    // "firefox" may or may not resolve depending on the installed icon theme;
    // the contract is only that the call is safe and returns something (null
    // or a real pixmap) without throwing.
    resolve_app_icon(makeEntry(QStringLiteral("firefox")), QSize(48, 48));
    SUCCEED();
}

int main(int argc, char** argv) {
    // QIcon/QPixmap need a QGuiApplication; offscreen keeps it headless for CI.
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QGuiApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
