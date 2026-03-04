/**
 * @file color_test.cpp
 * @brief Comprehensive unit tests for cf::ui::base::CFColor class
 *
 * Test Coverage:
 * 1. Construction (default, from QColor, from hex string, from HCT)
 * 2. HCT component accessors (hue, chroma, tone)
 * 3. Native color access
 * 4. Relative luminance calculation (WCAG 2.1)
 * 5. HCT-RGB conversion consistency
 * 6. Invalid input handling
 * 7. Boundary value clamping
 */

#include "ui/base/color.h"
#include <QColor>
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: Default Construction
// =============================================================================

TEST(CFColorTest, DefaultConstructor) {
    cf::ui::base::CFColor color;

    // Default color should be black (invalid QColor defaults to black)
    EXPECT_EQ(color.native_color(), Qt::black);

    // HCT values for black
    EXPECT_FLOAT_EQ(color.hue(), 0.0f);
    EXPECT_FLOAT_EQ(color.chroma(), 0.0f);
    EXPECT_FLOAT_EQ(color.tone(), 0.0f);
}

// =============================================================================
// Test Suite 2: Construction from QColor
// =============================================================================

TEST(CFColorTest, ConstructFromQColor_Black) {
    QColor qColor(Qt::black);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), Qt::black);
    EXPECT_FLOAT_EQ(color.tone(), 0.0f);
}

TEST(CFColorTest, ConstructFromQColor_White) {
    QColor qColor(Qt::white);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), Qt::white);
    EXPECT_FLOAT_EQ(color.tone(), 100.0f);
}

TEST(CFColorTest, ConstructFromQColor_Red) {
    QColor qColor(Qt::red);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), Qt::red);
    EXPECT_GT(color.chroma(), 0.0f); // Red has high chroma
}

TEST(CFColorTest, ConstructFromQColor_Green) {
    QColor qColor(Qt::green);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), Qt::green);
    EXPECT_GT(color.chroma(), 0.0f);
}

TEST(CFColorTest, ConstructFromQColor_Blue) {
    QColor qColor(Qt::blue);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), Qt::blue);
    EXPECT_GT(color.chroma(), 0.0f);
}

TEST(CFColorTest, ConstructFromQColor_Gray) {
    QColor qColor(128, 128, 128);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), QColor(128, 128, 128));
    EXPECT_NEAR(color.chroma(), 0.0f, 1.0f); // Gray has low chroma
    EXPECT_GT(color.tone(), 40.0f);
    EXPECT_LT(color.tone(), 60.0f);
}

TEST(CFColorTest, ConstructFromQColor_WithAlpha) {
    QColor qColor(255, 0, 0, 128); // Semi-transparent red
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), QColor(255, 0, 0, 128));
}

// =============================================================================
// Test Suite 3: Construction from Hex String
// =============================================================================

TEST(CFColorTest, ConstructFromHex_SixDigit) {
    cf::ui::base::CFColor color("#FF0000");
    EXPECT_EQ(color.native_color(), QColor(255, 0, 0));
}

TEST(CFColorTest, ConstructFromHex_EightDigit) {
    cf::ui::base::CFColor color("#80FF0000");                // #AARRGGBB
    EXPECT_EQ(color.native_color(), QColor(255, 0, 0, 128)); // Note: order is #AARRGGBB
}

TEST(CFColorTest, ConstructFromHex_White) {
    cf::ui::base::CFColor color("#FFFFFF");
    EXPECT_EQ(color.native_color(), Qt::white);
    EXPECT_FLOAT_EQ(color.tone(), 100.0f);
}

TEST(CFColorTest, ConstructFromHex_Black) {
    cf::ui::base::CFColor color("#000000");
    EXPECT_EQ(color.native_color(), Qt::black);
    EXPECT_FLOAT_EQ(color.tone(), 0.0f);
}

TEST(CFColorTest, ConstructFromHex_Gray) {
    cf::ui::base::CFColor color("#808080");
    EXPECT_EQ(color.native_color(), QColor(128, 128, 128));
}

TEST(CFColorTest, ConstructFromHex_WithWhitespace) {
    cf::ui::base::CFColor color("  #FF0000  ");
    EXPECT_EQ(color.native_color(), QColor(255, 0, 0));
}

TEST(CFColorTest, ConstructFromHex_NoHashSign) {
    // Invalid format - should return black
    cf::ui::base::CFColor color("FF0000");
    EXPECT_EQ(color.native_color(), Qt::black);
}

TEST(CFColorTest, ConstructFromHex_InvalidLength) {
    // Invalid format - should return black
    cf::ui::base::CFColor color("#FFF");
    EXPECT_EQ(color.native_color(), Qt::black);
}

TEST(CFColorTest, ConstructFromHex_EmptyString) {
    cf::ui::base::CFColor color("");
    EXPECT_EQ(color.native_color(), Qt::black);
}

// =============================================================================
// Test Suite 4: Construction from HCT Values
// =============================================================================

TEST(CFColorTest, ConstructFromHCT_Basic) {
    cf::ui::base::CFColor color(180.0f, 50.0f, 50.0f); // Cyan-ish

    EXPECT_FLOAT_EQ(color.hue(), 180.0f);
    EXPECT_FLOAT_EQ(color.chroma(), 50.0f);
    EXPECT_FLOAT_EQ(color.tone(), 50.0f);
}

TEST(CFColorTest, ConstructFromHCT_Clamping) {
    // Values should be clamped to valid ranges
    cf::ui::base::CFColor color(500.0f, 200.0f, 150.0f);

    EXPECT_FLOAT_EQ(color.hue(), 360.0f);    // Clamped to 360
    EXPECT_FLOAT_EQ(color.chroma(), 150.0f); // Clamped to 150
    EXPECT_FLOAT_EQ(color.tone(), 100.0f);   // Clamped to 100
}

TEST(CFColorTest, ConstructFromHCT_NegativeValues) {
    cf::ui::base::CFColor color(-10.0f, -5.0f, -10.0f);

    EXPECT_FLOAT_EQ(color.hue(), 0.0f);    // Clamped to 0
    EXPECT_FLOAT_EQ(color.chroma(), 0.0f); // Clamped to 0
    EXPECT_FLOAT_EQ(color.tone(), 0.0f);   // Clamped to 0
}

TEST(CFColorTest, ConstructFromHCT_ZeroChroma_Grayscale) {
    cf::ui::base::CFColor color(0.0f, 0.0f, 50.0f); // Gray

    EXPECT_NEAR(color.chroma(), 0.0f, 0.1f);
    EXPECT_NEAR(color.tone(), 50.0f, 1.0f);
}

TEST(CFColorTest, ConstructFromHCT_Red) {
    cf::ui::base::CFColor color(0.0f, 100.0f, 50.0f); // Red

    EXPECT_NEAR(color.hue(), 0.0f, 5.0f);
    EXPECT_GT(color.native_color().red(), 200); // High red component
}

TEST(CFColorTest, ConstructFromHCT_Green) {
    cf::ui::base::CFColor color(120.0f, 80.0f, 50.0f); // Green

    EXPECT_NEAR(color.hue(), 120.0f, 10.0f);
    EXPECT_GT(color.native_color().green(), 150);
}

TEST(CFColorTest, ConstructFromHCT_Blue) {
    cf::ui::base::CFColor color(240.0f, 80.0f, 50.0f); // Blue

    EXPECT_NEAR(color.hue(), 240.0f, 10.0f);
    EXPECT_GT(color.native_color().blue(), 150);
}

TEST(CFColorTest, ConstructFromHCT_LightTone) {
    cf::ui::base::CFColor color(0.0f, 50.0f, 90.0f); // Light color

    EXPECT_GT(color.tone(), 85.0f);
    // Light colors have higher RGB values
    int total =
        color.native_color().red() + color.native_color().green() + color.native_color().blue();
    EXPECT_GT(total, 500);
}

TEST(CFColorTest, ConstructFromHCT_DarkTone) {
    cf::ui::base::CFColor color(0.0f, 50.0f, 10.0f); // Dark color

    EXPECT_LT(color.tone(), 15.0f);
    // Dark colors have lower RGB values
    int total =
        color.native_color().red() + color.native_color().green() + color.native_color().blue();
    EXPECT_LT(total, 150);
}

// =============================================================================
// Test Suite 5: Relative Luminance
// =============================================================================

TEST(CFColorTest, RelativeLuminance_Black) {
    cf::ui::base::CFColor color(Qt::black);
    EXPECT_FLOAT_EQ(color.relativeLuminance(), 0.0f);
}

TEST(CFColorTest, RelativeLuminance_White) {
    cf::ui::base::CFColor color(Qt::white);
    EXPECT_FLOAT_EQ(color.relativeLuminance(), 1.0f);
}

TEST(CFColorTest, RelativeLuminance_Gray) {
    cf::ui::base::CFColor color(128, 128, 128);
    float lum = color.relativeLuminance();
    EXPECT_GT(lum, 0.1f);
    EXPECT_LT(lum, 0.4f);
}

TEST(CFColorTest, RelativeLuminance_Red) {
    cf::ui::base::CFColor color(Qt::red);
    float lum = color.relativeLuminance();
    // Red has lower perceived brightness
    EXPECT_GT(lum, 0.1f);
    EXPECT_LT(lum, 0.4f);
}

TEST(CFColorTest, RelativeLuminance_Green) {
    cf::ui::base::CFColor color(Qt::green);
    float lum = color.relativeLuminance();
    // Green has high perceived brightness
    EXPECT_GT(lum, 0.5f);
    EXPECT_LT(lum, 0.9f);
}

TEST(CFColorTest, RelativeLuminance_Blue) {
    cf::ui::base::CFColor color(Qt::blue);
    float lum = color.relativeLuminance();
    // Blue has low perceived brightness
    EXPECT_GT(lum, 0.05f);
    EXPECT_LT(lum, 0.2f);
}

TEST(CFColorTest, RelativeLuminance_WCAGFormula) {
    // Verify WCAG 2.1 formula: 0.2126*R + 0.7152*G + 0.0722*B
    // For pure white (1,1,1): 0.2126 + 0.7152 + 0.0722 = 1.0
    cf::ui::base::CFColor color(Qt::white);
    EXPECT_NEAR(color.relativeLuminance(), 1.0f, 0.001f);
}

// =============================================================================
// Test Suite 6: HCT-RGB Round-trip Conversion
// =============================================================================

TEST(CFColorTest, RoundTrip_HCTtoRGBtoHCT) {
    // Create color from HCT
    cf::ui::base::CFColor original(180.0f, 60.0f, 50.0f);
    float origHue = original.hue();
    float origChroma = original.chroma();
    float origTone = original.tone();

    // Create new color from RGB
    cf::ui::base::CFColor fromRgb(original.native_color());

    // HCT values should be approximately preserved
    EXPECT_NEAR(fromRgb.hue(), origHue, 15.0f);
    EXPECT_NEAR(fromRgb.chroma(), origChroma, 20.0f);
    EXPECT_NEAR(fromRgb.tone(), origTone, 15.0f);
}

TEST(CFColorTest, RoundTrip_RGBtoHCTtoRGB) {
    // Create color from RGB
    QColor originalRgb(100, 150, 200);
    cf::ui::base::CFColor original(originalRgb);

    // Create new color from HCT
    cf::ui::base::CFColor fromHct(original.hue(), original.chroma(), original.tone());

    // RGB values should be approximately preserved
    EXPECT_NEAR(fromHct.native_color().red(), originalRgb.red(), 30);
    EXPECT_NEAR(fromHct.native_color().green(), originalRgb.green(), 30);
    EXPECT_NEAR(fromHct.native_color().blue(), originalRgb.blue(), 30);
}

// =============================================================================
// Test Suite 7: Edge Cases and Special Colors
// =============================================================================

TEST(CFColorTest, Hue_BoundaryValues) {
    cf::ui::base::CFColor color1(0.0f, 50.0f, 50.0f);
    EXPECT_FLOAT_EQ(color1.hue(), 0.0f);

    cf::ui::base::CFColor color2(360.0f, 50.0f, 50.0f);
    EXPECT_FLOAT_EQ(color2.hue(), 360.0f);
}

TEST(CFColorTest, Chroma_BoundaryValues) {
    cf::ui::base::CFColor color1(180.0f, 0.0f, 50.0f);
    EXPECT_FLOAT_EQ(color1.chroma(), 0.0f);

    cf::ui::base::CFColor color2(180.0f, 150.0f, 50.0f);
    EXPECT_FLOAT_EQ(color2.chroma(), 150.0f);
}

TEST(CFColorTest, Tone_BoundaryValues) {
    cf::ui::base::CFColor color1(180.0f, 50.0f, 0.0f);
    EXPECT_FLOAT_EQ(color1.tone(), 0.0f);

    cf::ui::base::CFColor color2(180.0f, 50.0f, 100.0f);
    EXPECT_FLOAT_EQ(color2.tone(), 100.0f);
}

TEST(CFColorTest, NativeColor_ReturnsQColor) {
    QColor qColor(123, 45, 67);
    cf::ui::base::CFColor color(qColor);

    EXPECT_EQ(color.native_color(), qColor);
}

TEST(CFColorTest, CopyConstructor) {
    cf::ui::base::CFColor original("#FF00FF");
    cf::ui::base::CFColor copy(original);

    EXPECT_EQ(copy.native_color(), original.native_color());
    EXPECT_FLOAT_EQ(copy.hue(), original.hue());
    EXPECT_FLOAT_EQ(copy.chroma(), original.chroma());
    EXPECT_FLOAT_EQ(copy.tone(), original.tone());
}

// =============================================================================
// Test Suite 8: Color Consistency
// =============================================================================

TEST(CFColorTest, MultipleConstructorsSameColor) {
    // Create same color using different methods
    cf::ui::base::CFColor fromHex("#FF0000");
    cf::ui::base::CFColor fromQColor(QColor(255, 0, 0));

    // RGB values should match
    EXPECT_EQ(fromHex.native_color().red(), fromQColor.native_color().red());
    EXPECT_EQ(fromHex.native_color().green(), fromQColor.native_color().green());
    EXPECT_EQ(fromHex.native_color().blue(), fromQColor.native_color().blue());
}

TEST(CFColorTest, HCTRangeValidation) {
    // Test that HCT values are always in valid range
    cf::ui::base::CFColor color(500.0f, -50.0f, 150.0f);

    EXPECT_GE(color.hue(), 0.0f);
    EXPECT_LE(color.hue(), 360.0f);
    EXPECT_GE(color.chroma(), 0.0f);
    EXPECT_LE(color.chroma(), 150.0f);
    EXPECT_GE(color.tone(), 0.0f);
    EXPECT_LE(color.tone(), 100.0f);
}

// No main() needed - using GTest::gtest_main
