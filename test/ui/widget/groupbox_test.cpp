/**
 * @file groupbox_test.cpp
 * @brief Unit tests for Material Design 3 GroupBox widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/groupbox/groupbox.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(GroupBoxTest, A01_DefaultConstructor) {
    GroupBox gb;
    EXPECT_NO_FATAL_FAILURE(gb.title());
    EXPECT_TRUE(gb.isEnabled());
}

TEST(GroupBoxTest, A02_ConstructWithTitle) {
    GroupBox gb("Title");
    EXPECT_EQ(gb.title(), "Title");
}

// --- B: Property setters ---

TEST(GroupBoxTest, B01_SetTitle) {
    GroupBox gb;
    gb.setTitle("New Title");
    EXPECT_EQ(gb.title(), "New Title");
}

TEST(GroupBoxTest, B02_EnableDisable) {
    GroupBox gb;
    gb.setEnabled(true);
    EXPECT_TRUE(gb.isEnabled());

    gb.setEnabled(false);
    EXPECT_FALSE(gb.isEnabled());
}

TEST(GroupBoxTest, B03_SetElevation) {
    GroupBox gb;
    gb.setElevation(0);
    EXPECT_EQ(gb.elevation(), 0);

    gb.setElevation(3);
    EXPECT_EQ(gb.elevation(), 3);

    gb.setElevation(5);
    EXPECT_EQ(gb.elevation(), 5);
}

TEST(GroupBoxTest, B04_SetCornerRadius) {
    GroupBox gb;
    gb.setCornerRadius(4.0f);
    EXPECT_FLOAT_EQ(gb.cornerRadius(), 4.0f);

    gb.setCornerRadius(16.0f);
    EXPECT_FLOAT_EQ(gb.cornerRadius(), 16.0f);
}

TEST(GroupBoxTest, B05_SetHasBorder) {
    GroupBox gb;
    gb.setHasBorder(true);
    EXPECT_TRUE(gb.hasBorder());

    gb.setHasBorder(false);
    EXPECT_FALSE(gb.hasBorder());
}

// --- C: Size hints ---

TEST(GroupBoxTest, C01_SizeHint_NotZero) {
    GroupBox gb;
    gb.setTitle("Group");
    QSize hint = gb.sizeHint();
    EXPECT_FALSE(hint.isNull());
}

TEST(GroupBoxTest, C02_MinimumSizeHint_NotZero) {
    GroupBox gb;
    gb.setTitle("Group");
    EXPECT_GT(gb.minimumSizeHint().width(), 0);
    EXPECT_GT(gb.minimumSizeHint().height(), 0);
}

// --- D: Paint ---

TEST(GroupBoxTest, D01_Paint_WithTitle_NoCrash) {
    GroupBox gb;
    gb.setTitle("My Group");
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&gb, QSize(200, 100)));
}

TEST(GroupBoxTest, D02_Paint_WithWithoutTitle_DifferentPixels) {
    GroupBox untitledGb;
    QImage untitledImage = widget_test::renderWidgetToImage(&untitledGb, QSize(200, 100));

    GroupBox titledGb("My Group");
    QImage titledImage = widget_test::renderWidgetToImage(&titledGb, QSize(200, 100));

    EXPECT_TRUE(widget_test::imagesDiffer(untitledImage, titledImage))
        << "GroupBox with and without title should produce different visual output";
}

TEST(GroupBoxTest, D03_Paint_Elevation_DifferentPixels) {
    GroupBox lowGb("Shadow");
    lowGb.setElevation(0);
    QImage lowImage = widget_test::renderWidgetToImage(&lowGb, QSize(200, 100));

    GroupBox highGb("Shadow");
    highGb.setElevation(4);
    QImage highImage = widget_test::renderWidgetToImage(&highGb, QSize(200, 100));

    EXPECT_TRUE(widget_test::imagesDiffer(lowImage, highImage))
        << "GroupBox with different elevations should produce different visual output";
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
