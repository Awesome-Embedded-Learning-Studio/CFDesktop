/**
 * @file    test/desktop/launcher/app_discoverer_test.cpp
 * @brief   GoogleTest unit tests for AppDiscoverer.
 *
 * Covers: empty/missing apps dir, valid manifest parse (icon/exec resolved to
 * absolute), skipping subdirs without app.json, skipping manifests missing
 * required fields, empty icon handling.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "launcher/app_discoverer.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using cf::desktop::desktop_component::AppDiscoverer;

namespace {
/// Writes a manifest body to <apps_dir>/<id>/app.json.
void writeManifest(const QString& apps_dir, const QString& id, const QString& body) {
    QDir(apps_dir).mkpath(id);
    QFile f(apps_dir + QLatin1Char('/') + id + QStringLiteral("/app.json"));
    f.open(QIODevice::WriteOnly);
    f.write(body.toUtf8());
}
} // namespace

TEST(AppDiscoverer, EmptyDirReturnsEmpty) {
    QTemporaryDir tmp;
    EXPECT_TRUE(tmp.isValid());
    EXPECT_TRUE(AppDiscoverer::discoverFrom(tmp.path()).isEmpty());
}

TEST(AppDiscoverer, NonexistentDirReturnsEmpty) {
    EXPECT_TRUE(
        AppDiscoverer::discoverFrom(QStringLiteral("/nonexistent/cfdesktop/apps")).isEmpty());
}

TEST(AppDiscoverer, ParsesValidManifest) {
    QTemporaryDir tmp;
    writeManifest(tmp.path(), QStringLiteral("calc"),
                  R"({"app_id":"calc","display_name":"Calc","exec":"calc","icon":"icon.png"})");
    const auto apps = AppDiscoverer::discoverFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_EQ(apps[0].app_id.toStdString(), "calc");
    EXPECT_EQ(apps[0].display_name.toStdString(), "Calc");
    EXPECT_TRUE(apps[0].exec_command.endsWith(QStringLiteral("/calc/calc")));
    EXPECT_TRUE(apps[0].icon_path.endsWith(QStringLiteral("/calc/icon.png")));
}

TEST(AppDiscoverer, SkipsSubdirWithoutManifest) {
    QTemporaryDir tmp;
    writeManifest(tmp.path(), QStringLiteral("calc"), R"({"app_id":"calc","exec":"calc"})");
    QDir(tmp.path()).mkpath(QStringLiteral("no_manifest")); // subdir without app.json
    const auto apps = AppDiscoverer::discoverFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_EQ(apps[0].app_id.toStdString(), "calc");
}

TEST(AppDiscoverer, SkipsManifestMissingExec) {
    QTemporaryDir tmp;
    writeManifest(tmp.path(), QStringLiteral("bad"),
                  R"({"app_id":"bad","display_name":"Bad"})"); // no exec
    writeManifest(tmp.path(), QStringLiteral("good"), R"({"app_id":"good","exec":"good"})");
    const auto apps = AppDiscoverer::discoverFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_EQ(apps[0].app_id.toStdString(), "good");
}

TEST(AppDiscoverer, EmptyIconYieldsEmptyIconPath) {
    QTemporaryDir tmp;
    writeManifest(tmp.path(), QStringLiteral("calc"),
                  R"({"app_id":"calc","exec":"calc"})"); // no icon field
    const auto apps = AppDiscoverer::discoverFrom(tmp.path());
    ASSERT_EQ(apps.size(), 1);
    EXPECT_TRUE(apps[0].icon_path.isEmpty());
}
