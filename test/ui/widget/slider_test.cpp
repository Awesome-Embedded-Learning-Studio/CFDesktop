/**
 * @file slider_test.cpp
 * @brief Unit tests for Slider (Material Widget Layer)
 *
 * Test Coverage:
 * 1. Default constructor
 * 2. Set range
 * 3. Set value
 * 4. Enable/disable behavior
 * 5. SizeHint non-zero
 * 6. Paint output differs between slider values
 */

#include "ui/widget/material/widget/slider/slider.h"
#include "widget_test_helper.h"
#include <QApplication>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Construction
// =============================================================================

TEST(SliderTest, A01_DefaultConstructor) {
    Slider slider;
    // Verify constructed without crash
    EXPECT_TRUE(slider.isEnabled());
}

TEST(SliderTest, A02_ConstructWithOrientation) {
    Slider hSlider(Qt::Horizontal);
    EXPECT_EQ(hSlider.orientation(), Qt::Horizontal);

    Slider vSlider(Qt::Vertical);
    EXPECT_EQ(vSlider.orientation(), Qt::Vertical);
}

// =============================================================================
// Test Suite B: State Changes
// =============================================================================

TEST(SliderTest, B01_SetRange) {
    Slider slider;
    slider.setRange(0, 100);
    EXPECT_EQ(slider.minimum(), 0);
    EXPECT_EQ(slider.maximum(), 100);
}

TEST(SliderTest, B02_SetValue) {
    Slider slider;
    slider.setRange(0, 100);
    slider.setValue(50);
    EXPECT_EQ(slider.value(), 50);
}

TEST(SliderTest, B03_EnableDisable) {
    Slider slider;
    EXPECT_TRUE(slider.isEnabled());

    slider.setEnabled(false);
    EXPECT_FALSE(slider.isEnabled());

    slider.setEnabled(true);
    EXPECT_TRUE(slider.isEnabled());
}

TEST(SliderTest, B04_SetValue_BoundaryClamped) {
    Slider slider;
    slider.setRange(0, 100);

    slider.setValue(-10);
    EXPECT_GE(slider.value(), 0) << "Value below minimum should be clamped";

    slider.setValue(200);
    EXPECT_LE(slider.value(), 100) << "Value above maximum should be clamped";
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(SliderTest, C01_SizeHint_NotZero) {
    Slider slider;
    EXPECT_GT(slider.sizeHint().width(), 0);
    EXPECT_GT(slider.sizeHint().height(), 0);
}

TEST(SliderTest, C02_MinimumSizeHint_NotZero) {
    Slider slider;
    EXPECT_GT(slider.minimumSizeHint().width(), 0);
    EXPECT_GT(slider.minimumSizeHint().height(), 0);
}

// =============================================================================
// Test Suite D: Paint Verification
// =============================================================================

TEST(SliderTest, D01_Paint_DifferentValues_DifferentPixels) {
    Slider sliderLow;
    sliderLow.setRange(0, 100);
    sliderLow.setValue(0);
    QImage imgLow = widget_test::renderWidgetToImage(&sliderLow, QSize(200, 48));

    Slider sliderHigh;
    sliderHigh.setRange(0, 100);
    sliderHigh.setValue(100);
    QImage imgHigh = widget_test::renderWidgetToImage(&sliderHigh, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(imgLow, imgHigh))
        << "Slider at value=0 and value=100 should produce different visual output";
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
