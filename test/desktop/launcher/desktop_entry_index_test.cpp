/**
 * @file    test/desktop/launcher/desktop_entry_index_test.cpp
 * @brief   GoogleTest unit tests for DesktopEntryIndex.
 *
 * Covers: empty/missing dir, valid Application parse (Exec %-fields stripped,
 * icon/exec resolved), skipping NoDisplay, skipping non-Application types.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "launcher/desktop_entry_index.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using cf::desktop::desktop_component::DesktopEntryIndex;
using cf::desktop::desktop_component::LaunchKind;

namespace {
/// Writes a .desktop file body to <dir>/<name>.
void writeDesktop(const QString& dir, const QString& name, const QString& body) {
    QFile f(dir + QLatin1Char('/') + name);
    f.open(QIODevice::WriteOnly);
    f.write(body.toUtf8());
}
} // namespace

TEST(DesktopEntryIndex, EmptyDirReturnsEmpty) {
    QTemporaryDir tmp;
    EXPECT_TRUE(DesktopEntryIndex::indexFrom(tmp.path()).isEmpty());
}

TEST(DesktopEntryIndex, NonexistentDirReturnsEmpty) {
    EXPECT_TRUE(
        DesktopEntryIndex::indexFrom(QStringLiteral("/nonexistent/cfdesktop/desktop")).isEmpty());
}

TEST(DesktopEntryIndex, ParsesApplicationEntry) {
    QTemporaryDir tmp;
    writeDesktop(
        tmp.path(), QStringLiteral("firefox.desktop"),
        "[Desktop Entry]\nType=Application\nName=Firefox\nExec=firefox %u\nIcon=firefox\n");
    const auto apps = DesktopEntryIndex::indexFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_EQ(apps[0].app_id.toStdString(), "firefox");
    EXPECT_EQ(apps[0].display_name.toStdString(), "Firefox");
    EXPECT_EQ(apps[0].exec_command.toStdString(), "firefox"); // %u stripped
    EXPECT_EQ(apps[0].icon_path.toStdString(), "firefox");
    EXPECT_EQ(apps[0].launch_kind, LaunchKind::DetachedProcess);
}

TEST(DesktopEntryIndex, SkipsNoDisplay) {
    QTemporaryDir tmp;
    writeDesktop(tmp.path(), QStringLiteral("hidden.desktop"),
                 "[Desktop Entry]\nType=Application\nName=Hidden\nExec=hidden\nNoDisplay=true\n");
    EXPECT_TRUE(DesktopEntryIndex::indexFrom(tmp.path()).isEmpty());
}

TEST(DesktopEntryIndex, SkipsNonApplication) {
    QTemporaryDir tmp;
    writeDesktop(tmp.path(), QStringLiteral("link.desktop"),
                 "[Desktop Entry]\nType=Link\nName=Link\nURL=http://example.com\n");
    EXPECT_TRUE(DesktopEntryIndex::indexFrom(tmp.path()).isEmpty());
}

TEST(DesktopEntryIndex, CleansMultipleExecFields) {
    QTemporaryDir tmp;
    writeDesktop(tmp.path(), QStringLiteral("code.desktop"),
                 "[Desktop Entry]\nType=Application\nName=Code\nExec=code --foo %F\n");
    const auto apps = DesktopEntryIndex::indexFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_EQ(apps[0].exec_command.toStdString(), "code --foo"); // %F stripped, --foo kept
}

TEST(DesktopEntryIndex, FallsBackToBasenameWhenNameMissing) {
    QTemporaryDir tmp;
    writeDesktop(tmp.path(), QStringLiteral("ghost.desktop"),
                 "[Desktop Entry]\nType=Application\nExec=ghost\n"); // no Name=
    const auto apps = DesktopEntryIndex::indexFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_EQ(apps[0].app_id.toStdString(), "ghost");
    EXPECT_EQ(apps[0].display_name.toStdString(), "ghost"); // falls back to basename
}
