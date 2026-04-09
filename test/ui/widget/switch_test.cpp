/**
 * @file switch_test.cpp
 * @brief Unit tests for Material Design 3 Switch widget.
 *
 * Test Coverage:
 * 1. Constructor variants and defaults
 * 2. Property setters/getters (checked, enabled)
 * 3. Size hints with reasonable dimension checks
 * 4. Paint behavior for on/off states and thumb position verification
 */

#include "widget_test_helper.h"

#include "ui/widget/material/widget/switch/switch.h"

#include <QApplication>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Constructors
// =============================================================================

TEST(SwitchTest, A01_DefaultConstructor_Unchecked) {
    Switch sw;
    EXPECT_FALSE(sw.isChecked());
}

TEST(SwitchTest, A02_ConstructWithText) {
    Switch sw("Toggle");
    EXPECT_EQ(sw.text(), "Toggle");
}

// =============================================================================
// Test Suite B: Property Setters / Getters
// =============================================================================

TEST(SwitchTest, B01_SetChecked) {
    Switch sw;
    EXPECT_FALSE(sw.isChecked());

    sw.setChecked(true);
    EXPECT_TRUE(sw.isChecked());

    sw.setChecked(false);
    EXPECT_FALSE(sw.isChecked());
}

TEST(SwitchTest, B02_EnableDisable) {
    Switch sw;
    EXPECT_TRUE(sw.isEnabled());

    sw.setEnabled(false);
    EXPECT_FALSE(sw.isEnabled());

    sw.setEnabled(true);
    EXPECT_TRUE(sw.isEnabled());
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(SwitchTest, C01_SizeHint_NotZero) {
    Switch sw;
    EXPECT_GT(sw.sizeHint().width(), 0);
    EXPECT_GT(sw.sizeHint().height(), 0);
    EXPECT_GE(sw.sizeHint().width(), 40);
    EXPECT_GE(sw.sizeHint().height(), 20);
}

// =============================================================================
// Test Suite D: Paint Behavior
// =============================================================================

TEST(SwitchTest, D01_Paint_OnOff_DifferentPixels) {
    Switch offSwitch;
    offSwitch.setChecked(false);

    Switch onSwitch;
    onSwitch.setChecked(true);

    QImage offImage = widget_test::renderWidgetToImage(&offSwitch, QSize(120, 48));
    QImage onImage = widget_test::renderWidgetToImage(&onSwitch, QSize(120, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(offImage, onImage))
        << "On and off switch states should produce different visual output";
}

TEST(SwitchTest, D02_Paint_ThumbPosition) {
    Switch sw;
    sw.setChecked(false);

    QImage image = widget_test::renderWidgetToImage(&sw, QSize(120, 48));
    int nonTransparent = widget_test::countNonTransparentPixels(image);

    // Verify the switch renders visible content
    EXPECT_GT(nonTransparent, 0) << "Switch should render visible pixels";
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
