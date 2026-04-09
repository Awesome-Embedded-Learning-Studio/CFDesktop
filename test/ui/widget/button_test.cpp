/**
 * @file button_test.cpp
 * @brief Unit tests for Material Design 3 Button widget.
 *
 * Test Coverage:
 * 1. Constructor variants and defaults
 * 2. Property setters/getters (variant, elevation, icon, enabled, press effect, light source)
 * 3. Size hints (sizeHint, minimumSizeHint)
 * 4. Paint behavior across variants, enabled/disabled states, and with icons
 */

#include "widget_test_helper.h"

#include "ui/widget/material/widget/button/button.h"

#include <QApplication>
#include <QIcon>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Constructors
// =============================================================================

TEST(ButtonTest, A01_DefaultConstructor_IsFilled) {
    Button button;
    EXPECT_EQ(button.variant(), Button::ButtonVariant::Filled);
}

TEST(ButtonTest, A02_ConstructWithText) {
    Button button("Hello");
    EXPECT_EQ(button.text(), "Hello");
}

TEST(ButtonTest, A03_ConstructWithVariant) {
    Button tonalBtn("Tonal", Button::ButtonVariant::Tonal);
    EXPECT_EQ(tonalBtn.variant(), Button::ButtonVariant::Tonal);

    Button outlinedBtn("Outlined", Button::ButtonVariant::Outlined);
    EXPECT_EQ(outlinedBtn.variant(), Button::ButtonVariant::Outlined);

    Button textBtn("Text", Button::ButtonVariant::Text);
    EXPECT_EQ(textBtn.variant(), Button::ButtonVariant::Text);

    Button elevatedBtn("Elevated", Button::ButtonVariant::Elevated);
    EXPECT_EQ(elevatedBtn.variant(), Button::ButtonVariant::Elevated);
}

// =============================================================================
// Test Suite B: Property Setters / Getters
// =============================================================================

TEST(ButtonTest, B01_SetVariant) {
    Button button;

    button.setVariant(Button::ButtonVariant::Filled);
    EXPECT_EQ(button.variant(), Button::ButtonVariant::Filled);

    button.setVariant(Button::ButtonVariant::Tonal);
    EXPECT_EQ(button.variant(), Button::ButtonVariant::Tonal);

    button.setVariant(Button::ButtonVariant::Outlined);
    EXPECT_EQ(button.variant(), Button::ButtonVariant::Outlined);

    button.setVariant(Button::ButtonVariant::Text);
    EXPECT_EQ(button.variant(), Button::ButtonVariant::Text);

    button.setVariant(Button::ButtonVariant::Elevated);
    EXPECT_EQ(button.variant(), Button::ButtonVariant::Elevated);
}

TEST(ButtonTest, B02_SetElevation) {
    Button button;
    button.setElevation(3);
    EXPECT_EQ(button.elevation(), 3);

    button.setElevation(0);
    EXPECT_EQ(button.elevation(), 0);

    button.setElevation(5);
    EXPECT_EQ(button.elevation(), 5);
}

TEST(ButtonTest, B03_SetLeadingIcon) {
    Button button;
    EXPECT_NO_FATAL_FAILURE(button.setLeadingIcon(QIcon()));
}

TEST(ButtonTest, B04_EnableDisable) {
    Button button;
    EXPECT_TRUE(button.isEnabled());

    button.setEnabled(false);
    EXPECT_FALSE(button.isEnabled());

    button.setEnabled(true);
    EXPECT_TRUE(button.isEnabled());
}

TEST(ButtonTest, B05_PressEffectToggle) {
    Button button;
    EXPECT_TRUE(button.pressEffectEnabled());

    button.setPressEffectEnabled(false);
    EXPECT_FALSE(button.pressEffectEnabled());

    button.setPressEffectEnabled(true);
    EXPECT_TRUE(button.pressEffectEnabled());
}

TEST(ButtonTest, B06_SetLightSourceAngle) {
    Button button;
    button.setLightSourceAngle(45.0f);
    EXPECT_FLOAT_EQ(button.lightSourceAngle(), 45.0f);

    button.setLightSourceAngle(-30.0f);
    EXPECT_FLOAT_EQ(button.lightSourceAngle(), -30.0f);

    button.setLightSourceAngle(0.0f);
    EXPECT_FLOAT_EQ(button.lightSourceAngle(), 0.0f);
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(ButtonTest, C01_SizeHint_NotZero) {
    Button button("Click Me");
    EXPECT_GT(button.sizeHint().width(), 0);
    EXPECT_GT(button.sizeHint().height(), 0);
}

TEST(ButtonTest, C02_MinimumSizeHint_NotZero) {
    Button button("Click Me");
    EXPECT_GT(button.minimumSizeHint().width(), 0);
    EXPECT_GT(button.minimumSizeHint().height(), 0);
}

// =============================================================================
// Test Suite D: Paint Behavior
// =============================================================================

TEST(ButtonTest, D01_Paint_AllVariants_NoCrash) {
    const Button::ButtonVariant variants[] = {
        Button::ButtonVariant::Filled, Button::ButtonVariant::Tonal,
        Button::ButtonVariant::Outlined, Button::ButtonVariant::Text,
        Button::ButtonVariant::Elevated};

    for (auto v : variants) {
        Button btn("Test", v);
        EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&btn, QSize(200, 48)));
    }
}

TEST(ButtonTest, D02_Paint_EnabledDisabled_DifferentPixels) {
    Button enabledBtn("Test");
    enabledBtn.setEnabled(true);

    Button disabledBtn("Test");
    disabledBtn.setEnabled(false);

    QImage enabledImage = widget_test::renderWidgetToImage(&enabledBtn, QSize(200, 48));
    QImage disabledImage = widget_test::renderWidgetToImage(&disabledBtn, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(enabledImage, disabledImage))
        << "Enabled and disabled buttons should produce different visual output";
}

TEST(ButtonTest, D03_Paint_WithIcon_NoCrash) {
    Button button("Icon Button");
    button.setIcon(QIcon());

    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&button, QSize(200, 48)));
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
