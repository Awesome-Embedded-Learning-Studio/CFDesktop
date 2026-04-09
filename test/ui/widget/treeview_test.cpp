#include "widget_test_helper.h"

#include <QApplication>
#include <QStandardItemModel>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/treeview/treeview.h"

using namespace cf::ui::widget::material;

TEST(TreeView, A01_DefaultConstructor) {
    TreeView tv;
    EXPECT_TRUE(tv.isEnabled());
}

TEST(TreeView, B01_SetModel) {
    TreeView tv;
    QStandardItemModel model;
    QStandardItem* root = model.invisibleRootItem();
    QStandardItem* parentItem = new QStandardItem("Parent");
    root->appendRow(parentItem);
    parentItem->appendRow(new QStandardItem("Child1"));
    parentItem->appendRow(new QStandardItem("Child2"));
    tv.setModel(&model);
    EXPECT_EQ(tv.model(), &model);
}

TEST(TreeView, B02_EnableDisable) {
    TreeView tv;
    tv.setEnabled(true);
    EXPECT_TRUE(tv.isEnabled());

    tv.setEnabled(false);
    EXPECT_FALSE(tv.isEnabled());

    tv.setEnabled(true);
    EXPECT_TRUE(tv.isEnabled());
}

TEST(TreeView, C01_SizeHint_Valid) {
    TreeView tv;
    QSize hint = tv.sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(TreeView, D01_Paint_WithModel_NoCrash) {
    TreeView tv;
    QStandardItemModel model;
    QStandardItem* root = model.invisibleRootItem();
    QStandardItem* parentItem = new QStandardItem("Parent");
    root->appendRow(parentItem);
    parentItem->appendRow(new QStandardItem("Child1"));
    parentItem->appendRow(new QStandardItem("Child2"));
    tv.setModel(&model);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&tv, QSize(200, 200)));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
