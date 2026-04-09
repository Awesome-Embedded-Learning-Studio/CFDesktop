#include "widget_test_helper.h"

#include <QApplication>
#include <QStringListModel>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/listview/listview.h"

using namespace cf::ui::widget::material;

TEST(ListView, A01_DefaultConstructor) {
    ListView lv;
    EXPECT_TRUE(lv.isEnabled());
}

TEST(ListView, B01_SetModel) {
    ListView lv;
    QStringListModel model(QStringList{"A", "B", "C"});
    lv.setModel(&model);
    EXPECT_EQ(lv.model(), &model);
}

TEST(ListView, B02_EnableDisable) {
    ListView lv;
    lv.setEnabled(true);
    EXPECT_TRUE(lv.isEnabled());

    lv.setEnabled(false);
    EXPECT_FALSE(lv.isEnabled());

    lv.setEnabled(true);
    EXPECT_TRUE(lv.isEnabled());
}

TEST(ListView, B03_SetCurrentIndex) {
    ListView lv;
    QStringListModel model(QStringList{"A", "B", "C"});
    lv.setModel(&model);
    lv.setCurrentIndex(model.index(1, 0));
    EXPECT_EQ(lv.currentIndex().row(), 1);
}

TEST(ListView, C01_SizeHint_Valid) {
    ListView lv;
    QSize hint = lv.sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(ListView, D01_Paint_WithModel_NoCrash) {
    ListView lv;
    QStringListModel model(QStringList{"A", "B", "C"});
    lv.setModel(&model);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&lv, QSize(200, 200)));
}

TEST(ListView, D02_Paint_WithItems_PaintsPixels) {
    ListView lv;
    QStringListModel model(QStringList{"Item1", "Item2", "Item3"});
    lv.setModel(&model);
    widget_test::verifyPaintsPixels(&lv, QSize(200, 200));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
