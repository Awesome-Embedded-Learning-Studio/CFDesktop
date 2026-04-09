/**
 * @file checkbox_test.cpp
 * @brief Unit tests for Material Design 3 CheckBox widget.
 *
 * Test Coverage:
 * 1. Constructor variants and defaults
 * 2. Property setters/getters (checked, check state, error, enabled)
 * 3. Size hints (sizeHint, minimumSizeHint)
 * 4. Paint behavior across checked/unchecked, indeterminate, and error states
 */

#include "widget_test_helper.h"

#include "ui/widget/material/widget/checkbox/checkbox.h"

#include <QApplication>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Constructors
// =============================================================================

TEST(CheckBoxTest, A01_DefaultConstructor_Unchecked) {
    CheckBox cb;
    EXPECT_FALSE(cb.isChecked());
}

TEST(CheckBoxTest, A02_ConstructWithText) {
    CheckBox cb("Label");
    EXPECT_EQ(cb.text(), "Label");
}

// =============================================================================
// Test Suite B: Property Setters / Getters
// =============================================================================

TEST(CheckBoxTest, B01_SetChecked_TrueFalse) {
    CheckBox cb;
    EXPECT_FALSE(cb.isChecked());

    cb.setChecked(true);
    EXPECT_TRUE(cb.isChecked());

    cb.setChecked(false);
    EXPECT_FALSE(cb.isChecked());
}

TEST(CheckBoxTest, B02_SetCheckState_ThreeStates) {
    CheckBox cb;

    cb.setCheckState(Qt::Unchecked);
    EXPECT_EQ(cb.checkState(), Qt::Unchecked);

    cb.setCheckState(Qt::PartiallyChecked);
    EXPECT_EQ(cb.checkState(), Qt::PartiallyChecked);

    cb.setCheckState(Qt::Checked);
    EXPECT_EQ(cb.checkState(), Qt::Checked);
}

TEST(CheckBoxTest, B03_SetError) {
    CheckBox cb;
    EXPECT_FALSE(cb.hasError());

    cb.setError(true);
    EXPECT_TRUE(cb.hasError());

    cb.setError(false);
    EXPECT_FALSE(cb.hasError());
}

TEST(CheckBoxTest, B04_EnableDisable) {
    CheckBox cb;
    EXPECT_TRUE(cb.isEnabled());

    cb.setEnabled(false);
    EXPECT_FALSE(cb.isEnabled());

    cb.setEnabled(true);
    EXPECT_TRUE(cb.isEnabled());
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(CheckBoxTest, C01_SizeHint_NotZero) {
    CheckBox cb("Option");
    EXPECT_GT(cb.sizeHint().width(), 0);
    EXPECT_GT(cb.sizeHint().height(), 0);
}

TEST(CheckBoxTest, C02_MinimumSizeHint_NotZero) {
    CheckBox cb("Option");
    EXPECT_GT(cb.minimumSizeHint().width(), 0);
    EXPECT_GT(cb.minimumSizeHint().height(), 0);
}

// =============================================================================
// Test Suite D: Paint Behavior
// =============================================================================

TEST(CheckBoxTest, D01_Paint_CheckedUnchecked_DifferentPixels) {
    CheckBox uncheckedCb("Option");
    uncheckedCb.setChecked(false);

    CheckBox checkedCb("Option");
    checkedCb.setChecked(true);

    QImage uncheckedImage = widget_test::renderWidgetToImage(&uncheckedCb, QSize(200, 48));
    QImage checkedImage = widget_test::renderWidgetToImage(&checkedCb, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(uncheckedImage, checkedImage))
        << "Checked and unchecked checkboxes should produce different visual "
           "output";
}

TEST(CheckBoxTest, D02_Paint_Indeterminate_NoCrash) {
    CheckBox cb("Maybe");
    cb.setCheckState(Qt::PartiallyChecked);

    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&cb, QSize(200, 48)));
}

TEST(CheckBoxTest, D03_Paint_ErrorState_DifferentPixels) {
    CheckBox normalCb("Field");
    normalCb.setError(false);

    CheckBox errorCb("Field");
    errorCb.setError(true);

    QImage normalImage = widget_test::renderWidgetToImage(&normalCb, QSize(200, 48));
    QImage errorImage = widget_test::renderWidgetToImage(&errorCb, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(normalImage, errorImage))
        << "Error and normal state checkboxes should produce different visual "
           "output";
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
