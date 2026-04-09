#include "widget_test_helper.h"

#include <QApplication>
#include <QStandardItemModel>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/tableview/tableview.h"

using namespace cf::ui::widget::material;

TEST(TableView, A01_DefaultConstructor) {
    TableView tv;
    EXPECT_TRUE(tv.isEnabled());
}

TEST(TableView, B01_SetModel) {
    TableView tv;
    QStandardItemModel model(3, 2);
    tv.setModel(&model);
    EXPECT_EQ(tv.model(), &model);
}

TEST(TableView, B02_EnableDisable) {
    TableView tv;
    tv.setEnabled(true);
    EXPECT_TRUE(tv.isEnabled());

    tv.setEnabled(false);
    EXPECT_FALSE(tv.isEnabled());

    tv.setEnabled(true);
    EXPECT_TRUE(tv.isEnabled());
}

TEST(TableView, C01_SizeHint_Valid) {
    TableView tv;
    QSize hint = tv.sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(TableView, D01_Paint_WithModel_NoCrash) {
    TableView tv;
    QStandardItemModel model(3, 2);
    tv.setModel(&model);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&tv, QSize(300, 200)));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
