/**
 * @file color_helper_test.cpp
 * @brief Comprehensive unit tests for cf::ui::base color helper functions
 *
 * Test Coverage:
 * 1. blend() - Color blending with ratio
 * 2. elevationOverlay() - Material Design elevation overlay
 * 3. contrastRatio() - WCAG contrast ratio calculation
 * 4. tonalPalette() - Tonal palette generation from key color
 */

#include "ui/base/color_helper.h"
#include <QGuiApplication>
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: blend()
// =============================================================================

TEST(ColorHelperTest, Blend_Ratio0_ReturnsBase) {
    cf::ui::base::CFColor base(QColor(255, 0, 0));    // Red
    cf::ui::base::CFColor overlay(QColor(0, 0, 255)); // Blue

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 0.0f);

    EXPECT_EQ(result.native_color(), base.native_color());
}

TEST(ColorHelperTest, Blend_Ratio1_ReturnsOverlay) {
    cf::ui::base::CFColor base(QColor(255, 0, 0));    // Red
    cf::ui::base::CFColor overlay(QColor(0, 0, 255)); // Blue

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 1.0f);

    EXPECT_EQ(result.native_color(), overlay.native_color());
}

TEST(ColorHelperTest, Blend_Ratio05_ReturnsMidpoint) {
    cf::ui::base::CFColor base(QColor(0, 0, 0));          // Black
    cf::ui::base::CFColor overlay(QColor(255, 255, 255)); // White

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 0.5f);

    // Should be gray
    EXPECT_EQ(result.native_color(), QColor(128, 128, 128));
}

TEST(ColorHelperTest, Blend_RedAndWhite) {
    cf::ui::base::CFColor base(QColor(255, 0, 0));        // Red
    cf::ui::base::CFColor overlay(QColor(255, 255, 255)); // White

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 0.5f);

    // Pink-ish
    EXPECT_GT(result.native_color().red(), 200);
    EXPECT_GT(result.native_color().green(), 100);
    EXPECT_GT(result.native_color().blue(), 100);
}

TEST(ColorHelperTest, Blend_ClampsRatioAbove1) {
    cf::ui::base::CFColor base(QColor(0, 0, 0));          // Black
    cf::ui::base::CFColor overlay(QColor(255, 255, 255)); // White

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 1.5f);

    EXPECT_EQ(result.native_color(), overlay.native_color());
}

TEST(ColorHelperTest, Blend_ClampsRatioBelow0) {
    cf::ui::base::CFColor base(QColor(0, 0, 0));          // Black
    cf::ui::base::CFColor overlay(QColor(255, 255, 255)); // White

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, -0.5f);

    EXPECT_EQ(result.native_color(), base.native_color());
}

TEST(ColorHelperTest, Blend_WithAlpha) {
    cf::ui::base::CFColor base(QColor(255, 0, 0, 255));    // Opaque red
    cf::ui::base::CFColor overlay(QColor(0, 0, 255, 128)); // Semi-transparent blue

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 0.5f);

    // Alpha should also be blended
    EXPECT_GT(result.native_color().alpha(), 128);
    EXPECT_LT(result.native_color().alpha(), 255);
}

TEST(ColorHelperTest, Blend_SameColor) {
    cf::ui::base::CFColor base(QColor(100, 100, 100));
    cf::ui::base::CFColor overlay(QColor(100, 100, 100));

    cf::ui::base::CFColor result = cf::ui::base::blend(base, overlay, 0.5f);

    EXPECT_EQ(result.native_color(), base.native_color());
}

TEST(ColorHelperTest, Blend_MultipleRatios) {
    cf::ui::base::CFColor base(QColor(0, 0, 0));
    cf::ui::base::CFColor overlay(QColor(100, 0, 0));

    auto r1 = cf::ui::base::blend(base, overlay, 0.25f);
    auto r2 = cf::ui::base::blend(base, overlay, 0.5f);
    auto r3 = cf::ui::base::blend(base, overlay, 0.75f);

    EXPECT_LT(r1.native_color().red(), r2.native_color().red());
    EXPECT_LT(r2.native_color().red(), r3.native_color().red());
}

// =============================================================================
// Test Suite 2: elevationOverlay()
// =============================================================================

TEST(ColorHelperTest, ElevationOverlay_Level0_NoOverlay) {
    cf::ui::base::CFColor surface(QColor(200, 200, 200));
    cf::ui::base::CFColor primary(QColor(100, 100, 255));

    cf::ui::base::CFColor result = cf::ui::base::elevationOverlay(surface, primary, 0);

    // Level 0 should have no overlay (alpha = 0.00)
    EXPECT_EQ(result.native_color(), surface.native_color());
}

TEST(ColorHelperTest, ElevationOverlay_PositiveLevel_Darkens) {
    cf::ui::base::CFColor surface(QColor(255, 255, 255)); // White
    cf::ui::base::CFColor primary(QColor(0, 0, 0));       // Black

    cf::ui::base::CFColor result = cf::ui::base::elevationOverlay(surface, primary, 5);

    // Should be darker than white
    int total =
        result.native_color().red() + result.native_color().green() + result.native_color().blue();
    EXPECT_LT(total, 765); // 765 = 255+255+255
}

TEST(ColorHelperTest, ElevationOverlay_ClampsNegativeElevation) {
    cf::ui::base::CFColor surface(QColor(200, 200, 200));
    cf::ui::base::CFColor primary(QColor(100, 100, 100));

    cf::ui::base::CFColor result = cf::ui::base::elevationOverlay(surface, primary, -10);

    // Should be clamped to level 0
    EXPECT_EQ(result.native_color(), surface.native_color());
}

TEST(ColorHelperTest, ElevationOverlay_ClampsElevationAbove5) {
    cf::ui::base::CFColor surface(QColor(200, 200, 200));
    cf::ui::base::CFColor primary(QColor(0, 0, 0));

    cf::ui::base::CFColor result1 = cf::ui::base::elevationOverlay(surface, primary, 5);
    cf::ui::base::CFColor result2 = cf::ui::base::elevationOverlay(surface, primary, 10);

    // Both should be clamped to level 5
    EXPECT_EQ(result1.native_color(), result2.native_color());
}

TEST(ColorHelperTest, ElevationOverlay_IncreasingLevels) {
    cf::ui::base::CFColor surface(QColor(255, 255, 255));
    cf::ui::base::CFColor primary(QColor(0, 0, 0));

    auto level1 = cf::ui::base::elevationOverlay(surface, primary, 1);
    auto level3 = cf::ui::base::elevationOverlay(surface, primary, 3);
    auto level5 = cf::ui::base::elevationOverlay(surface, primary, 5);

    // Higher elevation = more overlay effect (darker)
    int l1_total =
        level1.native_color().red() + level1.native_color().green() + level1.native_color().blue();
    int l3_total =
        level3.native_color().red() + level3.native_color().green() + level3.native_color().blue();
    int l5_total =
        level5.native_color().red() + level5.native_color().green() + level5.native_color().blue();

    EXPECT_GT(l1_total, l3_total);
    EXPECT_GT(l3_total, l5_total);
}

TEST(ColorHelperTest, ElevationOverlay_AlphaValues) {
    // Verify alpha values match Material Design spec
    cf::ui::base::CFColor surface(QColor(255, 255, 255));
    cf::ui::base::CFColor primary(QColor(0, 0, 0));

    // Level 5 should have 0.17 alpha
    cf::ui::base::CFColor result = cf::ui::base::elevationOverlay(surface, primary, 5);

    // Result should be 17% black blended with white
    // Expected value: 255 * (1 - 0.17) = 211.65 ≈ 212
    EXPECT_NEAR(result.native_color().red(), 212, 2);
}

// =============================================================================
// Test Suite 3: contrastRatio()
// =============================================================================

TEST(ColorHelperTest, ContrastRatio_BlackWhite) {
    cf::ui::base::CFColor black(Qt::black);
    cf::ui::base::CFColor white(Qt::white);

    float ratio = cf::ui::base::contrastRatio(black, white);

    // Maximum contrast ratio is approximately 21:1
    EXPECT_NEAR(ratio, 21.0f, 1.0f);
}

TEST(ColorHelperTest, ContrastRatio_SameColor) {
    cf::ui::base::CFColor color(QColor(128, 128, 128));

    float ratio = cf::ui::base::contrastRatio(color, color);

    EXPECT_FLOAT_EQ(ratio, 1.0f);
}

TEST(ColorHelperTest, ContrastRatio_DarkGrayWhite) {
    cf::ui::base::CFColor dark(QColor(50, 50, 50));
    cf::ui::base::CFColor white(Qt::white);

    float ratio = cf::ui::base::contrastRatio(dark, white);

    // Should be high contrast
    EXPECT_GT(ratio, 10.0f);
}

TEST(ColorHelperTest, ContrastRatio_RedGreen) {
    cf::ui::base::CFColor red(Qt::red);
    cf::ui::base::CFColor green(Qt::green);

    float ratio = cf::ui::base::contrastRatio(red, green);

    // Red and green have similar luminance, so contrast is low
    EXPECT_LT(ratio, 3.0f);
}

TEST(ColorHelperTest, ContrastRatio_WCAG_AA) {
    // WCAG AA requires 4.5:1 for normal text
    cf::ui::base::CFColor dark(QColor(30, 30, 30));
    cf::ui::base::CFColor light(QColor(240, 240, 240));

    float ratio = cf::ui::base::contrastRatio(dark, light);

    EXPECT_GT(ratio, 4.5f);
}

TEST(ColorHelperTest, ContrastRatio_WCAG_AAA) {
    // WCAG AAA requires 7:1 for normal text
    cf::ui::base::CFColor black(Qt::black);
    cf::ui::base::CFColor white(Qt::white);

    float ratio = cf::ui::base::contrastRatio(black, white);

    EXPECT_GT(ratio, 7.0f);
}

TEST(ColorHelperTest, ContrastRatio_Symmetric) {
    cf::ui::base::CFColor color1(QColor(100, 50, 50));
    cf::ui::base::CFColor color2(QColor(200, 200, 200));

    float ratio1 = cf::ui::base::contrastRatio(color1, color2);
    float ratio2 = cf::ui::base::contrastRatio(color2, color1);

    EXPECT_FLOAT_EQ(ratio1, ratio2);
}

TEST(ColorHelperTest, ContrastRatio_LightOnDark) {
    cf::ui::base::CFColor dark(QColor(20, 20, 20));
    cf::ui::base::CFColor light(QColor(230, 230, 230));

    float ratio = cf::ui::base::contrastRatio(dark, light);

    // Should exceed WCAG AA
    EXPECT_GT(ratio, 4.5f);
}

TEST(ColorHelperTest, ContrastRatio_GrayOnWhite) {
    cf::ui::base::CFColor gray(QColor(120, 120, 120));
    cf::ui::base::CFColor white(Qt::white);

    float ratio = cf::ui::base::contrastRatio(gray, white);

    // Gray on white typically fails WCAG AA
    EXPECT_LT(ratio, 4.5f);
}

// =============================================================================
// Test Suite 4: tonalPalette()
// =============================================================================

TEST(ColorHelperTest, TonalPalette_Size) {
    cf::ui::base::CFColor keyColor(QColor(100, 150, 200));

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    EXPECT_EQ(palette.size(), 13);
}

TEST(ColorHelperTest, TonalPalette_ToneValues) {
    cf::ui::base::CFColor keyColor(QColor(100, 150, 200));

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    // Check standard tonal values: 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 95, 99, 100
    EXPECT_NEAR(palette[0].tone(), 0.0f, 1.0f);
    EXPECT_NEAR(palette[1].tone(), 10.0f, 1.0f);
    EXPECT_NEAR(palette[5].tone(), 50.0f, 1.0f);
    EXPECT_NEAR(palette[12].tone(), 100.0f, 1.0f);
}

TEST(ColorHelperTest, TonalPalette_PreservesHue) {
    cf::ui::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    // All colors should have the same hue
    for (const auto& color : palette) {
        EXPECT_NEAR(color.hue(), 180.0f, 5.0f);
    }
}

TEST(ColorHelperTest, TonalPalette_PreservesChroma) {
    cf::ui::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    // All colors should have approximately the same chroma
    // (may vary slightly due to tone constraints)
    for (const auto& color : palette) {
        EXPECT_GT(color.chroma(), 30.0f);
        EXPECT_LT(color.chroma(), 70.0f);
    }
}

TEST(ColorHelperTest, TonalPalette_LightToDark) {
    cf::ui::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    // Tone should increase from index 0 to 12
    for (int i = 1; i < palette.size(); ++i) {
        EXPECT_GT(palette[i].tone(), palette[i - 1].tone());
    }
}

TEST(ColorHelperTest, TonalPalette_FirstColorIsNearBlack) {
    cf::ui::base::CFColor keyColor(QColor(100, 150, 200));

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    EXPECT_NEAR(palette[0].tone(), 0.0f, 1.0f);
    int total = palette[0].native_color().red() + palette[0].native_color().green() +
                palette[0].native_color().blue();
    EXPECT_LT(total, 50);
}

TEST(ColorHelperTest, TonalPalette_LastColorIsNearWhite) {
    cf::ui::base::CFColor keyColor(QColor(100, 150, 200));

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    EXPECT_NEAR(palette[12].tone(), 100.0f, 1.0f);
    int total = palette[12].native_color().red() + palette[12].native_color().green() +
                palette[12].native_color().blue();
    EXPECT_GT(total, 700);
}

TEST(ColorHelperTest, TonalPalette_MidTone) {
    cf::ui::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<cf::ui::base::CFColor> palette = cf::ui::base::tonalPalette(keyColor);

    // Index 5 should be tone 50
    EXPECT_NEAR(palette[5].tone(), 50.0f, 2.0f);
}

TEST(ColorHelperTest, TonalPalette_DifferentKeyColors) {
    cf::ui::base::CFColor red(0.0f, 80.0f, 50.0f);
    cf::ui::base::CFColor blue(240.0f, 80.0f, 50.0f);

    QList<cf::ui::base::CFColor> redPalette = cf::ui::base::tonalPalette(red);
    QList<cf::ui::base::CFColor> bluePalette = cf::ui::base::tonalPalette(blue);

    // Palettes should be different
    EXPECT_NE(redPalette[6].native_color(), bluePalette[6].native_color());
}

// =============================================================================
// Test Suite 5: Integration Tests
// =============================================================================

TEST(ColorHelperIntegration, BlendThenContrast) {
    cf::ui::base::CFColor black(Qt::black);
    cf::ui::base::CFColor white(Qt::white);

    auto blended = cf::ui::base::blend(black, white, 0.5f);

    // Contrast with white should be lower than black-white
    float originalContrast = cf::ui::base::contrastRatio(black, white);
    float blendedContrast = cf::ui::base::contrastRatio(blended, white);

    EXPECT_LT(blendedContrast, originalContrast);
}

TEST(ColorHelperIntegration, ElevationThenContrast) {
    cf::ui::base::CFColor surface(QColor(240, 240, 240));
    cf::ui::base::CFColor primary(QColor(30, 30, 30));

    auto elevated = cf::ui::base::elevationOverlay(surface, primary, 3);

    // Elevated surface should have less contrast with primary
    float originalContrast = cf::ui::base::contrastRatio(surface, primary);
    float elevatedContrast = cf::ui::base::contrastRatio(elevated, primary);

    EXPECT_LT(elevatedContrast, originalContrast);
}

TEST(ColorHelperIntegration, TonalPaletteThenContrast) {
    cf::ui::base::CFColor keyColor(180.0f, 50.0f, 50.0f);
    auto palette = cf::ui::base::tonalPalette(keyColor);

    // First and last colors should have high contrast
    float contrast = cf::ui::base::contrastRatio(palette[0], palette[12]);

    EXPECT_GT(contrast, 10.0f);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    // QCoreApplication must be created before InitGoogleTest and kept alive
    // The instance must persist for the entire test duration
    QGuiApplication* app = new QGuiApplication(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    delete app;
    return result;
}
