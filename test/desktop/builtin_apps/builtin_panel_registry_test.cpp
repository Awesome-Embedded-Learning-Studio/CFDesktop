/**
 * @file    builtin_panel_registry_test.cpp
 * @brief   Unit tests for BuiltinPanelRegistry.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.2
 * @since   0.20
 * @ingroup components
 */

#include "builtin_apps/builtin_panel_registry.h"

#include <QRect>
#include <QString>
#include <gtest/gtest.h>

namespace {

/// Minimal IBuiltinPanel stub for registry tests (no real rendering).
class FakePanel : public cf::desktop::desktop_component::IBuiltinPanel {
  public:
    FakePanel(QString id, QString name) : id_(std::move(id)), name_(std::move(name)) {}

    QString appId() const override { return id_; }
    QString displayName() const override { return name_; }
    void popup(const QRect&) override {}
    void hidePanel() override {}

  private:
    QString id_;
    QString name_;
};

} // namespace

// BuiltinPanelRegistry is a process-wide singleton holding non-owning
// pointers. Each test registers stack-local FakePanels; clearing in SetUp
// avoids dangling pointers accumulating across tests (which crashes Clang/MSVC
// when all() iterates them after the FakePanels went out of scope).
class BuiltinPanelRegistryTest : public ::testing::Test {
  protected:
    void SetUp() override {
        cf::desktop::desktop_component::BuiltinPanelRegistry::instance().clear();
    }
};

TEST_F(BuiltinPanelRegistryTest, FindUnknownReturnsNull) {
    auto& reg = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    EXPECT_EQ(reg.find(QStringLiteral("definitely_unknown_id")), nullptr);
}

TEST_F(BuiltinPanelRegistryTest, RegisterAndFind) {
    auto& reg = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    FakePanel panel(QStringLiteral("register_and_find"), QStringLiteral("RegisterAndFind"));
    reg.registerPanel(&panel);

    auto* found = reg.find(QStringLiteral("register_and_find"));
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->appId().toStdString(), "register_and_find");
    EXPECT_EQ(found->displayName().toStdString(), "RegisterAndFind");
}

TEST_F(BuiltinPanelRegistryTest, ContainsMatchesRegistered) {
    auto& reg = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    EXPECT_FALSE(reg.contains(QStringLiteral("contains_match")));
    FakePanel panel(QStringLiteral("contains_match"), QStringLiteral("Contains"));
    reg.registerPanel(&panel);
    EXPECT_TRUE(reg.contains(QStringLiteral("contains_match")));
}

TEST_F(BuiltinPanelRegistryTest, DuplicateIdReplaces) {
    auto& reg = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    FakePanel first(QStringLiteral("dup_id"), QStringLiteral("First"));
    FakePanel second(QStringLiteral("dup_id"), QStringLiteral("Second"));
    reg.registerPanel(&first);
    reg.registerPanel(&second);

    auto* found = reg.find(QStringLiteral("dup_id"));
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->displayName().toStdString(), "Second");
}

TEST_F(BuiltinPanelRegistryTest, AllIncludesRegistered) {
    auto& reg = cf::desktop::desktop_component::BuiltinPanelRegistry::instance();
    FakePanel panel(QStringLiteral("in_all"), QStringLiteral("InAll"));
    reg.registerPanel(&panel);

    const auto all = reg.all();
    EXPECT_EQ(all.size(), 1u);
    ASSERT_NE(all[0], nullptr);
    EXPECT_EQ(all[0]->appId().toStdString(), "in_all");
}
