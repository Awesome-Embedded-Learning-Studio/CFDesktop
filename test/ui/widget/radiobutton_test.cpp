/**
 * @file radiobutton_test.cpp
 * @brief Unit tests for RadioButton (Material Widget Layer)
 *
 * Test Coverage:
 * 1. Default and text constructors
 * 2. Checked state toggling
 * 3. Enable/disable behavior
 * 4. SizeHint non-zero
 * 5. Paint output differs between checked and unchecked states
 */

#include "ui/widget/material/widget/radiobutton/radiobutton.h"
#include "widget_test_helper.h"
#include <QApplication>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Construction
// =============================================================================

TEST(RadioButtonTest, A01_DefaultConstructor) {
    RadioButton rb;
    EXPECT_FALSE(rb.isChecked());
}

TEST(RadioButtonTest, A02_ConstructWithText) {
    RadioButton rb("Option");
    EXPECT_EQ(rb.text(), "Option");
}

// =============================================================================
// Test Suite B: State Changes
// =============================================================================

TEST(RadioButtonTest, B01_SetChecked) {
    RadioButton rb;
    EXPECT_FALSE(rb.isChecked());

    rb.setChecked(true);
    EXPECT_TRUE(rb.isChecked());
}

TEST(RadioButtonTest, B02_EnableDisable) {
    RadioButton rb;
    EXPECT_TRUE(rb.isEnabled());

    rb.setEnabled(false);
    EXPECT_FALSE(rb.isEnabled());

    rb.setEnabled(true);
    EXPECT_TRUE(rb.isEnabled());
}

TEST(RadioButtonTest, B03_SetError) {
    RadioButton rb;
    EXPECT_FALSE(rb.hasError());

    rb.setError(true);
    EXPECT_TRUE(rb.hasError());

    rb.setError(false);
    EXPECT_FALSE(rb.hasError());
}

TEST(RadioButtonTest, B04_PressEffectToggle) {
    RadioButton rb;
    EXPECT_TRUE(rb.pressEffectEnabled());

    rb.setPressEffectEnabled(false);
    EXPECT_FALSE(rb.pressEffectEnabled());

    rb.setPressEffectEnabled(true);
    EXPECT_TRUE(rb.pressEffectEnabled());
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(RadioButtonTest, C01_SizeHint_NotZero) {
    RadioButton rb;
    EXPECT_GT(rb.sizeHint().width(), 0);
    EXPECT_GT(rb.sizeHint().height(), 0);
}

TEST(RadioButtonTest, C02_MinimumSizeHint_NotZero) {
    RadioButton rb("Option");
    EXPECT_GT(rb.minimumSizeHint().width(), 0);
    EXPECT_GT(rb.minimumSizeHint().height(), 0);
}

// =============================================================================
// Test Suite D: Paint Verification
// =============================================================================

TEST(RadioButtonTest, D01_Paint_CheckedUnchecked_DifferentPixels) {
    RadioButton rbUnchecked;
    QImage imgUnchecked = widget_test::renderWidgetToImage(&rbUnchecked, QSize(200, 48));

    RadioButton rbChecked;
    rbChecked.setChecked(true);
    QImage imgChecked = widget_test::renderWidgetToImage(&rbChecked, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(imgUnchecked, imgChecked))
        << "Checked and unchecked RadioButton should produce different visual output";
}

TEST(RadioButtonTest, D02_Paint_ErrorState_DifferentPixels) {
    RadioButton normalRb("Option");
    normalRb.setError(false);

    RadioButton errorRb("Option");
    errorRb.setError(true);

    QImage normalImage = widget_test::renderWidgetToImage(&normalRb, QSize(200, 48));
    QImage errorImage = widget_test::renderWidgetToImage(&errorRb, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(normalImage, errorImage))
        << "Error and normal RadioButton should produce different visual output";
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
