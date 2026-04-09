#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/tabview/tabview.h"

using namespace cf::ui::widget::material;

TEST(TabView, A01_DefaultConstructor) {
    TabView tv;
    EXPECT_EQ(tv.count(), 0);
}

TEST(TabView, B01_AddTab) {
    TabView tv;
    tv.addTab(new QWidget(), "Tab1");
    tv.addTab(new QWidget(), "Tab2");
    tv.addTab(new QWidget(), "Tab3");
    EXPECT_EQ(tv.count(), 3);
}

TEST(TabView, B02_SetCurrentIndex) {
    TabView tv;
    tv.addTab(new QWidget(), "Tab1");
    tv.addTab(new QWidget(), "Tab2");
    tv.addTab(new QWidget(), "Tab3");
    tv.setCurrentIndex(1);
    EXPECT_EQ(tv.currentIndex(), 1);
}

TEST(TabView, B03_EnableDisable) {
    TabView tv;
    tv.setEnabled(true);
    EXPECT_TRUE(tv.isEnabled());

    tv.setEnabled(false);
    EXPECT_FALSE(tv.isEnabled());

    tv.setEnabled(true);
    EXPECT_TRUE(tv.isEnabled());
}

TEST(TabView, B04_SetTabHeight) {
    TabView tv;
    tv.setTabHeight(48);
    EXPECT_EQ(tv.tabHeight(), 48);
}

TEST(TabView, B05_SetTabMinWidth) {
    TabView tv;
    tv.setTabMinWidth(80);
    EXPECT_EQ(tv.tabMinWidth(), 80);
}

TEST(TabView, B06_SetShowIndicator) {
    TabView tv;
    tv.setShowIndicator(true);
    EXPECT_TRUE(tv.showIndicator());

    tv.setShowIndicator(false);
    EXPECT_FALSE(tv.showIndicator());
}

TEST(TabView, B07_SetTabCloseable) {
    TabView tv;
    tv.addTab(new QWidget(), "Tab1");
    tv.addTab(new QWidget(), "Tab2");
    EXPECT_NO_FATAL_FAILURE(tv.setTabCloseable(0, true));
    EXPECT_NO_FATAL_FAILURE(tv.setTabCloseable(1, false));
}

TEST(TabView, C01_SizeHint_Valid) {
    TabView tv;
    QSize hint = tv.sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(TabView, C02_MinimumSizeHint_NotZero) {
    TabView tv;
    tv.addTab(new QWidget(), "Tab1");
    EXPECT_GT(tv.minimumSizeHint().width(), 0);
    EXPECT_GT(tv.minimumSizeHint().height(), 0);
}

TEST(TabView, D01_Paint_WithTabs_NoCrash) {
    TabView tv;
    tv.addTab(new QWidget(), "Tab1");
    tv.addTab(new QWidget(), "Tab2");
    tv.addTab(new QWidget(), "Tab3");
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&tv, QSize(300, 200)));
}

TEST(TabView, D02_Paint_DifferentIndex_DifferentPixels) {
    TabView tv;
    tv.addTab(new QWidget(), "First");
    tv.addTab(new QWidget(), "Second");
    tv.resize(300, 200);

    tv.setCurrentIndex(0);
    QImage img0 = widget_test::renderWidgetToImage(&tv, QSize(300, 200));

    tv.setCurrentIndex(1);
    QImage img1 = widget_test::renderWidgetToImage(&tv, QSize(300, 200));

    EXPECT_TRUE(widget_test::imagesDiffer(img0, img1))
        << "TabView with different active tabs should produce different visual output";
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
