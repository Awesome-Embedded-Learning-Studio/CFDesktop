/**
 * @file device_pixel_test.cpp
 * @brief Comprehensive unit tests for cf::ui::base::device::CanvasUnitHelper
 *
 * Test Coverage:
 * 1. Construction with different device pixel ratios
 * 2. dpToPx() - Device-independent pixels to physical pixels
 * 3. spToPx() - Scalable pixels to physical pixels
 * 4. pxToDp() - Physical pixels to device-independent pixels
 * 5. dpi() - DPI calculation
 * 6. breakPoint() - Responsive breakpoint categorization
 */

#include "ui/base/device_pixel.h"
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: Construction
// =============================================================================

TEST(CanvasUnitHelperTest, Construction_StandardDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);
    EXPECT_FLOAT_EQ(helper.dpi(), 96.0);
}

TEST(CanvasUnitHelperTest, Construction_RetinaDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);
    EXPECT_FLOAT_EQ(helper.dpi(), 192.0);
}

TEST(CanvasUnitHelperTest, Construction_HighDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(3.0);
    EXPECT_FLOAT_EQ(helper.dpi(), 288.0);
}

TEST(CanvasUnitHelperTest, Construction_FractionalDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(1.5);
    EXPECT_FLOAT_EQ(helper.dpi(), 144.0);
}

// =============================================================================
// Test Suite 2: dpToPx()
// =============================================================================

TEST(CanvasUnitHelperTest, DpToPx_DPR1) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(0.0), 0.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(100.0), 100.0);
}

TEST(CanvasUnitHelperTest, DpToPx_DPR2) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.0), 32.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(0.0), 0.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(100.0), 200.0);
}

TEST(CanvasUnitHelperTest, DpToPx_DPR3) {
    cf::ui::base::device::CanvasUnitHelper helper(3.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.0), 48.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(8.0), 24.0);
}

TEST(CanvasUnitHelperTest, DpToPx_FractionalDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(1.5);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.0), 24.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(10.0), 15.0);
}

TEST(CanvasUnitHelperTest, DpToPx_FractionalDp) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.5), 33.0);
    EXPECT_DOUBLE_EQ(helper.dpToPx(0.5), 1.0);
}

TEST(CanvasUnitHelperTest, DpToPx_NegativeDp) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(-16.0), -32.0);
}

// =============================================================================
// Test Suite 3: spToPx()
// =============================================================================

TEST(CanvasUnitHelperTest, SpToPx_DPR1) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_DOUBLE_EQ(helper.spToPx(14.0), 14.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(16.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(20.0), 20.0);
}

TEST(CanvasUnitHelperTest, SpToPx_DPR2) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.spToPx(14.0), 28.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(16.0), 32.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(20.0), 40.0);
}

TEST(CanvasUnitHelperTest, SpToPx_SameAsDp) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    // sp and dp should behave the same
    double dpResult = helper.dpToPx(16.0);
    double spResult = helper.spToPx(16.0);

    EXPECT_DOUBLE_EQ(dpResult, spResult);
}

TEST(CanvasUnitHelperTest, SpToPx_FontSizes) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    // Common font sizes
    EXPECT_DOUBLE_EQ(helper.spToPx(12.0), 24.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(14.0), 28.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(16.0), 32.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(18.0), 36.0);
    EXPECT_DOUBLE_EQ(helper.spToPx(24.0), 48.0);
}

// =============================================================================
// Test Suite 4: pxToDp()
// =============================================================================

TEST(CanvasUnitHelperTest, PxToDp_DPR1) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_DOUBLE_EQ(helper.pxToDp(16.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(32.0), 32.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(100.0), 100.0);
}

TEST(CanvasUnitHelperTest, PxToDp_DPR2) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.pxToDp(32.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(48.0), 24.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(200.0), 100.0);
}

TEST(CanvasUnitHelperTest, PxToDp_DPR3) {
    cf::ui::base::device::CanvasUnitHelper helper(3.0);

    EXPECT_DOUBLE_EQ(helper.pxToDp(48.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(150.0), 50.0);
}

TEST(CanvasUnitHelperTest, PxToDp_RoundTrip) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    double originalDp = 16.0;
    double px = helper.dpToPx(originalDp);
    double convertedBack = helper.pxToDp(px);

    EXPECT_DOUBLE_EQ(convertedBack, originalDp);
}

TEST(CanvasUnitHelperTest, PxToDp_Zero) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.pxToDp(0.0), 0.0);
}

TEST(CanvasUnitHelperTest, PxToDp_Negative) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.pxToDp(-32.0), -16.0);
}

// =============================================================================
// Test Suite 5: dpi()
// =============================================================================

TEST(CanvasUnitHelperTest, Dpi_Standard96) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);
    EXPECT_DOUBLE_EQ(helper.dpi(), 96.0);
}

TEST(CanvasUnitHelperTest, Dpi_Retina192) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);
    EXPECT_DOUBLE_EQ(helper.dpi(), 192.0);
}

TEST(CanvasUnitHelperTest, Dpi_SuperRetina288) {
    cf::ui::base::device::CanvasUnitHelper helper(3.0);
    EXPECT_DOUBLE_EQ(helper.dpi(), 288.0);
}

TEST(CanvasUnitHelperTest, Dpi_Formula) {
    // DPI = 96 * devicePixelRatio
    for (qreal dpr = 0.5; dpr <= 4.0; dpr += 0.5) {
        cf::ui::base::device::CanvasUnitHelper helper(dpr);
        EXPECT_DOUBLE_EQ(helper.dpi(), 96.0 * dpr);
    }
}

// =============================================================================
// Test Suite 6: breakPoint()
// =============================================================================

TEST(CanvasUnitHelperTest, BreakPoint_Compact) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(300.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
    EXPECT_EQ(helper.breakPoint(599.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
    EXPECT_EQ(helper.breakPoint(599.99),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
}

TEST(CanvasUnitHelperTest, BreakPoint_Medium) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(600.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(700.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(839.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(839.99),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
}

TEST(CanvasUnitHelperTest, BreakPoint_Expanded) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(840.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
    EXPECT_EQ(helper.breakPoint(1000.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
    EXPECT_EQ(helper.breakPoint(1920.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
}

TEST(CanvasUnitHelperTest, BreakPoint_BoundaryValues) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    // Exactly at boundaries
    EXPECT_EQ(helper.breakPoint(600.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(840.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);

    // Just below boundaries
    EXPECT_EQ(helper.breakPoint(599.99),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
    EXPECT_EQ(helper.breakPoint(839.99),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);

    // Just above boundaries
    EXPECT_EQ(helper.breakPoint(600.01),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(840.01),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
}

TEST(CanvasUnitHelperTest, BreakPoint_NegativeWidth) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(-100.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
}

TEST(CanvasUnitHelperTest, BreakPoint_Zero) {
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(0.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
}

// =============================================================================
// Test Suite 7: Material Design Breakpoint Specifications
// =============================================================================

TEST(MaterialDesignBreakpoints, Compact_Range) {
    // Compact: < 600dp
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    // Typical phone widths
    EXPECT_EQ(helper.breakPoint(360.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
    EXPECT_EQ(helper.breakPoint(375.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
    EXPECT_EQ(helper.breakPoint(414.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);
}

TEST(MaterialDesignBreakpoints, Medium_Range) {
    // Medium: 600dp - 839dp (foldables, tablets)
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(600.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(720.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
    EXPECT_EQ(helper.breakPoint(839.0), cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);
}

TEST(MaterialDesignBreakpoints, Expanded_Range) {
    // Expanded: >= 840dp (desktops)
    cf::ui::base::device::CanvasUnitHelper helper(1.0);

    EXPECT_EQ(helper.breakPoint(840.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
    EXPECT_EQ(helper.breakPoint(1024.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
    EXPECT_EQ(helper.breakPoint(1280.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
    EXPECT_EQ(helper.breakPoint(1920.0),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
}

// =============================================================================
// Test Suite 8: Integration Tests
// =============================================================================

TEST(CanvasUnitHelperIntegration, ResponsiveLayout_Calculation) {
    // Simulate responsive layout calculation
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    // Physical width of 720px at 2x DPR = 360dp (Compact)
    double widthDp = helper.pxToDp(720.0);
    EXPECT_EQ(helper.breakPoint(widthDp),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Compact);

    // Physical width of 1400px at 2x DPR = 700dp (Medium)
    widthDp = helper.pxToDp(1400.0);
    EXPECT_EQ(helper.breakPoint(widthDp),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Medium);

    // Physical width of 2000px at 2x DPR = 1000dp (Expanded)
    widthDp = helper.pxToDp(2000.0);
    EXPECT_EQ(helper.breakPoint(widthDp),
              cf::ui::base::device::CanvasUnitHelper::BreakPoint::Expanded);
}

TEST(CanvasUnitHelperIntegration, FontScaling) {
    // Font should scale properly across different DPRs
    qreal baseSizeSp = 16.0;

    cf::ui::base::device::CanvasUnitHelper helper1x(1.0);
    cf::ui::base::device::CanvasUnitHelper helper2x(2.0);
    cf::ui::base::device::CanvasUnitHelper helper3x(3.0);

    EXPECT_DOUBLE_EQ(helper1x.spToPx(baseSizeSp), 16.0);
    EXPECT_DOUBLE_EQ(helper2x.spToPx(baseSizeSp), 32.0);
    EXPECT_DOUBLE_EQ(helper3x.spToPx(baseSizeSp), 48.0);
}

TEST(CanvasUnitHelperIntegration, LayoutMargins) {
    // Test typical layout margin (16dp) at different DPRs
    qreal marginDp = 16.0;

    cf::ui::base::device::CanvasUnitHelper helper1x(1.0);
    cf::ui::base::device::CanvasUnitHelper helper2x(2.0);
    cf::ui::base::device::CanvasUnitHelper helper3x(3.0);

    EXPECT_DOUBLE_EQ(helper1x.dpToPx(marginDp), 16.0);
    EXPECT_DOUBLE_EQ(helper2x.dpToPx(marginDp), 32.0);
    EXPECT_DOUBLE_EQ(helper3x.dpToPx(marginDp), 48.0);
}

// =============================================================================
// Test Suite 9: Edge Cases
// =============================================================================

TEST(CanvasUnitHelperEdgeCases, VerySmallDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(0.5);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.0), 8.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(8.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.dpi(), 48.0);
}

TEST(CanvasUnitHelperEdgeCases, LargeDPR) {
    cf::ui::base::device::CanvasUnitHelper helper(4.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(16.0), 64.0);
    EXPECT_DOUBLE_EQ(helper.pxToDp(64.0), 16.0);
    EXPECT_DOUBLE_EQ(helper.dpi(), 384.0);
}

TEST(CanvasUnitHelperEdgeCases, VeryLargeValue) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(10000.0), 20000.0);
}

TEST(CanvasUnitHelperEdgeCases, VerySmallValue) {
    cf::ui::base::device::CanvasUnitHelper helper(2.0);

    EXPECT_DOUBLE_EQ(helper.dpToPx(0.1), 0.2);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
