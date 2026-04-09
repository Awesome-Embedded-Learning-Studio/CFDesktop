/**
 * @file progressbar_test.cpp
 * @brief Unit tests for ProgressBar (Material Widget Layer)
 *
 * Test Coverage:
 * 1. Default constructor
 * 2. Set range
 * 3. Set value
 * 4. Enable/disable behavior
 * 5. SizeHint non-zero
 * 6. Paint output differs between progress values
 */

#include "ui/widget/material/widget/progressbar/progressbar.h"
#include "widget_test_helper.h"
#include <QApplication>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Construction
// =============================================================================

TEST(ProgressBarTest, A01_DefaultConstructor) {
    ProgressBar pb;
    // Verify constructed without crash
    EXPECT_TRUE(pb.isEnabled());
}

// =============================================================================
// Test Suite B: State Changes
// =============================================================================

TEST(ProgressBarTest, B01_SetRange) {
    ProgressBar pb;
    pb.setRange(0, 100);
    EXPECT_EQ(pb.minimum(), 0);
    EXPECT_EQ(pb.maximum(), 100);
}

TEST(ProgressBarTest, B02_SetValue) {
    ProgressBar pb;
    pb.setRange(0, 100);
    pb.setValue(50);
    EXPECT_EQ(pb.value(), 50);
}

TEST(ProgressBarTest, B03_EnableDisable) {
    ProgressBar pb;
    EXPECT_TRUE(pb.isEnabled());

    pb.setEnabled(false);
    EXPECT_FALSE(pb.isEnabled());

    pb.setEnabled(true);
    EXPECT_TRUE(pb.isEnabled());
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(ProgressBarTest, C01_SizeHint_NotZero) {
    ProgressBar pb;
    EXPECT_GT(pb.sizeHint().width(), 0);
    EXPECT_GT(pb.sizeHint().height(), 0);
}

TEST(ProgressBarTest, C02_MinimumSizeHint_NotZero) {
    ProgressBar pb;
    EXPECT_GT(pb.minimumSizeHint().width(), 0);
    EXPECT_GT(pb.minimumSizeHint().height(), 0);
}

// =============================================================================
// Test Suite D: Paint Verification
// =============================================================================

TEST(ProgressBarTest, D01_Paint_DifferentValues_DifferentPixels) {
    ProgressBar pbLow;
    pbLow.setRange(0, 100);
    pbLow.setValue(0);
    QImage imgLow = widget_test::renderWidgetToImage(&pbLow, QSize(200, 48));

    ProgressBar pbHigh;
    pbHigh.setRange(0, 100);
    pbHigh.setValue(100);
    QImage imgHigh = widget_test::renderWidgetToImage(&pbHigh, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(imgLow, imgHigh))
        << "ProgressBar at value=0 and value=100 should produce different visual output";
}

TEST(ProgressBarTest, D02_Paint_EnabledDisabled_DifferentPixels) {
    ProgressBar enabledPb;
    enabledPb.setRange(0, 100);
    enabledPb.setValue(50);
    enabledPb.setEnabled(true);

    ProgressBar disabledPb;
    disabledPb.setRange(0, 100);
    disabledPb.setValue(50);
    disabledPb.setEnabled(false);

    QImage enabledImage = widget_test::renderWidgetToImage(&enabledPb, QSize(200, 48));
    QImage disabledImage = widget_test::renderWidgetToImage(&disabledPb, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(enabledImage, disabledImage))
        << "Enabled and disabled ProgressBar should produce different visual output";
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
