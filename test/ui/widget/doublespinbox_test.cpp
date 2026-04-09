/**
 * @file doublespinbox_test.cpp
 * @brief Unit tests for Material Design 3 DoubleSpinBox widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/doublespinbox/doublespinbox.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(DoubleSpinBoxTest, A01_DefaultConstructor) {
    DoubleSpinBox dsb;
    EXPECT_NO_FATAL_FAILURE(dsb.value());
    EXPECT_TRUE(dsb.isEnabled());
}

// --- B: Property setters ---

TEST(DoubleSpinBoxTest, B01_SetRange) {
    DoubleSpinBox dsb;
    dsb.setRange(0.0, 10.0);
    EXPECT_DOUBLE_EQ(dsb.minimum(), 0.0);
    EXPECT_DOUBLE_EQ(dsb.maximum(), 10.0);
}

TEST(DoubleSpinBoxTest, B02_SetValue) {
    DoubleSpinBox dsb;
    dsb.setValue(3.14);
    EXPECT_DOUBLE_EQ(dsb.value(), 3.14);
}

TEST(DoubleSpinBoxTest, B03_SetDecimals) {
    DoubleSpinBox dsb;
    dsb.setDecimals(2);
    EXPECT_EQ(dsb.decimals(), 2);
}

TEST(DoubleSpinBoxTest, B04_EnableDisable) {
    DoubleSpinBox dsb;
    dsb.setEnabled(true);
    EXPECT_TRUE(dsb.isEnabled());

    dsb.setEnabled(false);
    EXPECT_FALSE(dsb.isEnabled());
}

TEST(DoubleSpinBoxTest, B05_SetPrefixSuffix) {
    DoubleSpinBox dsb;
    dsb.setPrefix("$");
    dsb.setSuffix(" kg");
    EXPECT_EQ(dsb.prefix(), "$");
    EXPECT_EQ(dsb.suffix(), " kg");
}

// --- C: Size hints ---

TEST(DoubleSpinBoxTest, C01_SizeHint_NotZero) {
    DoubleSpinBox dsb;
    QSize hint = dsb.sizeHint();
    EXPECT_FALSE(hint.isNull());
}

TEST(DoubleSpinBoxTest, C02_MinimumSizeHint_NotZero) {
    DoubleSpinBox dsb;
    dsb.setValue(3.14);
    EXPECT_GT(dsb.minimumSizeHint().width(), 0);
    EXPECT_GT(dsb.minimumSizeHint().height(), 0);
}

// --- D: Paint ---

TEST(DoubleSpinBoxTest, D01_Paint_WithValue_NoCrash) {
    DoubleSpinBox dsb;
    dsb.setRange(0.0, 10.0);
    dsb.setValue(3.14);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&dsb, QSize(120, 48)));
}

TEST(DoubleSpinBoxTest, D02_Paint_DifferentValues_DifferentPixels) {
    DoubleSpinBox dsbLow;
    dsbLow.setRange(0.0, 10.0);
    dsbLow.setValue(0.0);
    QImage imgLow = widget_test::renderWidgetToImage(&dsbLow, QSize(120, 48));

    DoubleSpinBox dsbHigh;
    dsbHigh.setRange(0.0, 10.0);
    dsbHigh.setValue(9.99);
    QImage imgHigh = widget_test::renderWidgetToImage(&dsbHigh, QSize(120, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(imgLow, imgHigh))
        << "DoubleSpinBox at value=0 and value=9.99 should produce different visual output";
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
