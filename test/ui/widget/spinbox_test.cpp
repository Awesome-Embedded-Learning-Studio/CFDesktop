/**
 * @file spinbox_test.cpp
 * @brief Unit tests for Material Design 3 SpinBox widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/spinbox/spinbox.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(SpinBoxTest, A01_DefaultConstructor) {
    SpinBox sb;
    EXPECT_NO_FATAL_FAILURE(sb.value());
    EXPECT_TRUE(sb.isEnabled());
}

// --- B: Property setters ---

TEST(SpinBoxTest, B01_SetRange) {
    SpinBox sb;
    sb.setRange(0, 100);
    EXPECT_EQ(sb.minimum(), 0);
    EXPECT_EQ(sb.maximum(), 100);
}

TEST(SpinBoxTest, B02_SetValue) {
    SpinBox sb;
    sb.setValue(42);
    EXPECT_EQ(sb.value(), 42);
}

TEST(SpinBoxTest, B03_SetPrefixSuffix) {
    SpinBox sb;
    sb.setPrefix("$");
    sb.setSuffix("px");
    EXPECT_EQ(sb.prefix(), "$");
    EXPECT_EQ(sb.suffix(), "px");
}

TEST(SpinBoxTest, B04_EnableDisable) {
    SpinBox sb;
    sb.setEnabled(true);
    EXPECT_TRUE(sb.isEnabled());

    sb.setEnabled(false);
    EXPECT_FALSE(sb.isEnabled());
}

TEST(SpinBoxTest, B05_SetValue_BoundaryClamped) {
    SpinBox sb;
    sb.setRange(0, 100);

    sb.setValue(-10);
    EXPECT_GE(sb.value(), 0) << "Value below minimum should be clamped";

    sb.setValue(200);
    EXPECT_LE(sb.value(), 100) << "Value above maximum should be clamped";
}

// --- C: Size hints ---

TEST(SpinBoxTest, C01_SizeHint_NotZero) {
    SpinBox sb;
    QSize hint = sb.sizeHint();
    EXPECT_FALSE(hint.isNull());
}

TEST(SpinBoxTest, C02_MinimumSizeHint_NotZero) {
    SpinBox sb;
    sb.setValue(42);
    EXPECT_GT(sb.minimumSizeHint().width(), 0);
    EXPECT_GT(sb.minimumSizeHint().height(), 0);
}

// --- D: Paint ---

TEST(SpinBoxTest, D01_Paint_WithValue_NoCrash) {
    SpinBox sb;
    sb.setRange(0, 100);
    sb.setValue(42);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&sb, QSize(120, 48)));
}

TEST(SpinBoxTest, D02_Paint_DifferentValues_DifferentPixels) {
    SpinBox sbLow;
    sbLow.setRange(0, 100);
    sbLow.setValue(0);
    QImage imgLow = widget_test::renderWidgetToImage(&sbLow, QSize(120, 48));

    SpinBox sbHigh;
    sbHigh.setRange(0, 100);
    sbHigh.setValue(99);
    QImage imgHigh = widget_test::renderWidgetToImage(&sbHigh, QSize(120, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(imgLow, imgHigh))
        << "SpinBox at value=0 and value=99 should produce different visual output";
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
