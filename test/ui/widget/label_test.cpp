/**
 * @file label_test.cpp
 * @brief Unit tests for Label (Material Widget Layer)
 *
 * Test Coverage:
 * 1. Default and text constructors
 * 2. Set text
 * 3. Enable/disable behavior
 * 4. SizeHint non-zero (with text set)
 * 5. Paint with text produces visible pixels
 * 6. Different text produces different sizeHint values
 */

#include "ui/widget/material/widget/label/label.h"
#include "widget_test_helper.h"
#include <QApplication>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material;

// =============================================================================
// Test Suite A: Construction
// =============================================================================

TEST(LabelTest, A01_DefaultConstructor) {
    Label label;
    EXPECT_TRUE(label.text().isEmpty());
}

TEST(LabelTest, A02_ConstructWithText) {
    Label label("Hello");
    EXPECT_EQ(label.text(), "Hello");
}

// =============================================================================
// Test Suite B: State Changes
// =============================================================================

TEST(LabelTest, B01_SetText) {
    Label label;
    label.setText("World");
    EXPECT_EQ(label.text(), "World");
}

TEST(LabelTest, B02_EnableDisable) {
    Label label("Test");
    EXPECT_TRUE(label.isEnabled());

    label.setEnabled(false);
    EXPECT_FALSE(label.isEnabled());

    label.setEnabled(true);
    EXPECT_TRUE(label.isEnabled());
}

TEST(LabelTest, B03_SetTypographyStyle) {
    Label label;
    label.setTypographyStyle(TypographyStyle::DisplayLarge);
    EXPECT_EQ(label.typographyStyle(), TypographyStyle::DisplayLarge);

    label.setTypographyStyle(TypographyStyle::HeadlineMedium);
    EXPECT_EQ(label.typographyStyle(), TypographyStyle::HeadlineMedium);

    label.setTypographyStyle(TypographyStyle::TitleSmall);
    EXPECT_EQ(label.typographyStyle(), TypographyStyle::TitleSmall);

    label.setTypographyStyle(TypographyStyle::BodyLarge);
    EXPECT_EQ(label.typographyStyle(), TypographyStyle::BodyLarge);

    label.setTypographyStyle(TypographyStyle::LabelSmall);
    EXPECT_EQ(label.typographyStyle(), TypographyStyle::LabelSmall);
}

TEST(LabelTest, B04_SetColorVariant) {
    Label label("Text");
    label.setColorVariant(LabelColorVariant::Primary);
    EXPECT_EQ(label.colorVariant(), LabelColorVariant::Primary);

    label.setColorVariant(LabelColorVariant::OnSurface);
    EXPECT_EQ(label.colorVariant(), LabelColorVariant::OnSurface);

    label.setColorVariant(LabelColorVariant::Error);
    EXPECT_EQ(label.colorVariant(), LabelColorVariant::Error);

    label.setColorVariant(LabelColorVariant::InverseSurface);
    EXPECT_EQ(label.colorVariant(), LabelColorVariant::InverseSurface);
}

TEST(LabelTest, B05_SetAutoHiding) {
    Label label("Text");
    EXPECT_FALSE(label.autoHiding());

    label.setAutoHiding(true);
    EXPECT_TRUE(label.autoHiding());

    label.setAutoHiding(false);
    EXPECT_FALSE(label.autoHiding());
}

// =============================================================================
// Test Suite C: Size Hints
// =============================================================================

TEST(LabelTest, C01_SizeHint_NotZero) {
    Label label("Sample Text");
    EXPECT_GT(label.sizeHint().width(), 0);
    EXPECT_GT(label.sizeHint().height(), 0);
}

TEST(LabelTest, C02_MinimumSizeHint_NotZero) {
    Label label("Sample");
    EXPECT_GT(label.minimumSizeHint().width(), 0);
    EXPECT_GT(label.minimumSizeHint().height(), 0);
}

// =============================================================================
// Test Suite D: Paint Verification
// =============================================================================

TEST(LabelTest, D01_Paint_WithText_NoCrash) {
    Label label("Rendered Text");
    widget_test::verifyPaintsPixels(&label, QSize(200, 48));
}

TEST(LabelTest, D02_Paint_DifferentText_DifferentSize) {
    Label shortLabel("Hi");
    QSize shortSize = shortLabel.sizeHint();

    Label longLabel("This is a much longer text string for size comparison");
    QSize longSize = longLabel.sizeHint();

    EXPECT_NE(shortSize.width(), longSize.width())
        << "Short and long text labels should have different sizeHint widths";
}

TEST(LabelTest, D03_Paint_TypographyStyles_NoCrash) {
    const TypographyStyle styles[] = {TypographyStyle::DisplayLarge,
                                      TypographyStyle::HeadlineMedium, TypographyStyle::TitleSmall,
                                      TypographyStyle::BodyMedium, TypographyStyle::LabelLarge};
    for (auto style : styles) {
        Label label("Test Text");
        label.setTypographyStyle(style);
        EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&label, QSize(200, 48)));
    }
}

TEST(LabelTest, D04_Paint_DifferentTypography_DifferentSize) {
    Label displayLabel("Text");
    displayLabel.setTypographyStyle(TypographyStyle::DisplayLarge);
    QSize displaySize = displayLabel.sizeHint();

    Label bodyLabel("Text");
    bodyLabel.setTypographyStyle(TypographyStyle::BodySmall);
    QSize bodySize = bodyLabel.sizeHint();

    EXPECT_NE(displaySize.height(), bodySize.height())
        << "Display and Body typography should have different heights";
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
