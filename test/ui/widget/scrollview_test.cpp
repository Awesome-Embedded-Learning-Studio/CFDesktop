#include "widget_test_helper.h"

#include <QApplication>
#include <QLabel>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/scrollview/scrollview.h"

using namespace cf::ui::widget::material;

TEST(ScrollView, A01_DefaultConstructor) {
    ScrollView sv;
    EXPECT_TRUE(sv.isEnabled());
}

TEST(ScrollView, B01_SetWidget) {
    ScrollView sv;
    QLabel* label = new QLabel("This is a very long text that should require scrolling "
                               "to see all the content within the scroll view widget.");
    sv.setWidget(label);
    EXPECT_EQ(sv.widget(), label);
}

TEST(ScrollView, B02_EnableDisable) {
    ScrollView sv;
    sv.setEnabled(true);
    EXPECT_TRUE(sv.isEnabled());

    sv.setEnabled(false);
    EXPECT_FALSE(sv.isEnabled());

    sv.setEnabled(true);
    EXPECT_TRUE(sv.isEnabled());
}

TEST(ScrollView, B03_SetScrollbarFadeEnabled) {
    ScrollView sv;
    sv.setScrollbarFadeEnabled(false);
    EXPECT_FALSE(sv.scrollbarFadeEnabled());

    sv.setScrollbarFadeEnabled(true);
    EXPECT_TRUE(sv.scrollbarFadeEnabled());
}

TEST(ScrollView, B04_SetScrollbarFadeDelay) {
    ScrollView sv;
    sv.setScrollbarFadeDelay(1000);
    EXPECT_EQ(sv.scrollbarFadeDelay(), 1000);
}

TEST(ScrollView, B05_SetScrollbarHoverExpansion) {
    ScrollView sv;
    sv.setScrollbarHoverExpansion(false);
    EXPECT_FALSE(sv.scrollbarHoverExpansion());

    sv.setScrollbarHoverExpansion(true);
    EXPECT_TRUE(sv.scrollbarHoverExpansion());
}

TEST(ScrollView, C01_SizeHint_Valid) {
    ScrollView sv;
    QSize hint = sv.sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(ScrollView, D01_Paint_WithContent_NoCrash) {
    ScrollView sv;
    QLabel* label = new QLabel("This is a very long text that should require scrolling "
                               "to see all the content within the scroll view widget.");
    sv.setWidget(label);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&sv, QSize(200, 200)));
}

TEST(ScrollView, D02_Paint_WithContent_PaintsPixels) {
    ScrollView sv;
    QLabel* label = new QLabel("Content for scroll view testing");
    sv.setWidget(label);
    widget_test::verifyPaintsPixels(&sv, QSize(200, 200));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
