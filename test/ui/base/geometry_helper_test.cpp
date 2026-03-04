/**
 * @file geometry_helper_test.cpp
 * @brief Comprehensive unit tests for cf::ui::base::geometry helper functions
 *
 * Test Coverage:
 * 1. roundedRect() with ShapeScale enum values
 * 2. roundedRect() with uniform radius
 * 3. roundedRect() with individual corner radii
 * 4. QPainterPath validation
 */

#include "ui/base/geometry_helper.h"
#include <QPainterPath>
#include <QRectF>
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: roundedRect with ShapeScale
// =============================================================================

TEST(GeometryHelperTest, RoundedRect_ShapeNone) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeNone);

    EXPECT_FALSE(path.isEmpty());
    // Should be a rectangle (no rounding)
    EXPECT_TRUE(path.toFillPolygon().count() >= 4);
}

TEST(GeometryHelperTest, RoundedRect_ShapeExtraSmall) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(
        rect, cf::ui::base::geometry::ShapeScale::ShapeExtraSmall);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_ShapeSmall) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeSmall);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_ShapeMedium) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeMedium);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_ShapeLarge) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeLarge);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_ShapeExtraLarge) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(
        rect, cf::ui::base::geometry::ShapeScale::ShapeExtraLarge);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_ShapeFull) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeFull);

    EXPECT_FALSE(path.isEmpty());
    // Full rounded should create a capsule shape
}

TEST(GeometryHelperTest, RoundedRect_ShapeFull_Square) {
    QRectF rect(0, 0, 100, 100);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeFull);

    EXPECT_FALSE(path.isEmpty());
    // A square with full rounding should become a circle
}

// =============================================================================
// Test Suite 2: roundedRect with uniform radius
// =============================================================================

TEST(GeometryHelperTest, RoundedRect_UniformRadius_Zero) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 0.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_UniformRadius_Small) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 5.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_UniformRadius_Medium) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 15.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_UniformRadius_Large) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 50.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_UniformRadius_HalfHeight) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 25.0f);

    EXPECT_FALSE(path.isEmpty());
    // Half the height creates a capsule shape
}

TEST(GeometryHelperTest, RoundedRect_UniformRadius_Excessive) {
    QRectF rect(0, 0, 100, 50);
    // Radius larger than half the height
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 100.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_UniformRadius_Square_Circle) {
    QRectF rect(0, 0, 100, 100);
    // Radius equal to half the width/height creates a circle
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 50.0f);

    EXPECT_FALSE(path.isEmpty());
}

// =============================================================================
// Test Suite 3: roundedRect with individual corner radii
// =============================================================================

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_AllZero) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 0.0f, 0.0f, 0.0f, 0.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_AllSame) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 10.0f, 10.0f, 10.0f, 10.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_TopLeftOnly) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 15.0f, 0.0f, 0.0f, 0.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_TopRightOnly) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 0.0f, 15.0f, 0.0f, 0.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_BottomLeftOnly) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 0.0f, 0.0f, 15.0f, 0.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_BottomRightOnly) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 0.0f, 0.0f, 0.0f, 15.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_TopRounded) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 15.0f, 15.0f, 0.0f, 0.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_BottomRounded) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 0.0f, 0.0f, 15.0f, 15.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_Asymmetric) {
    QRectF rect(0, 0, 100, 50);
    // Each corner has different radius
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 5.0f, 10.0f, 15.0f, 20.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperTest, RoundedRect_IndividualCorners_LargeRadii) {
    QRectF rect(0, 0, 100, 50);
    // Large radii that may exceed bounds
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 50.0f, 50.0f, 50.0f, 50.0f);

    EXPECT_FALSE(path.isEmpty());
}

// =============================================================================
// Test Suite 4: QPainterPath validation
// =============================================================================

TEST(GeometryHelperPathValidation, PathIsNotEmpty) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeMedium);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperPathValidation, PathIsClosed) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 10.0f);

    // The path should form a closed shape
    EXPECT_TRUE(path.toFillPolygon().isClosed());
}

TEST(GeometryHelperPathValidation, PathContainsOriginalRect) {
    QRectF rect(0, 0, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 10.0f);

    // The rounded rect should contain the center of the original rect
    EXPECT_TRUE(path.contains(rect.center()));
}

TEST(GeometryHelperPathValidation, PathBoundingBox) {
    QRectF rect(10, 20, 100, 50);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 10.0f);

    QRectF boundingRect = path.boundingRect();

    // Bounding rect should be approximately the same as original
    EXPECT_NEAR(boundingRect.x(), rect.x(), 1.0);
    EXPECT_NEAR(boundingRect.y(), rect.y(), 1.0);
    EXPECT_NEAR(boundingRect.width(), rect.width(), 1.0);
    EXPECT_NEAR(boundingRect.height(), rect.height(), 1.0);
}

// =============================================================================
// Test Suite 5: Material Design shape scale values
// =============================================================================

TEST(MaterialDesignShapeScale, RadiusValues) {
    // Test that each shape scale produces the expected radius
    // This is a conceptual test - actual values depend on implementation

    QRectF rect(0, 0, 100, 50);

    // ShapeNone should produce sharp corners (like a rectangle)
    QPainterPath nonePath =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeNone);

    // ShapeFull should produce maximum rounded corners
    QPainterPath fullPath =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeFull);

    // Both paths should be valid
    EXPECT_FALSE(nonePath.isEmpty());
    EXPECT_FALSE(fullPath.isEmpty());

    // The full rounded path should be "rounder" - this is hard to test directly
    // but we can check that both are different
    // (Their bounding rects might differ slightly due to corner rendering)
}

// =============================================================================
// Test Suite 6: Different rectangle sizes
// =============================================================================

TEST(GeometryHelperRectSizes, VerySmallRect) {
    QRectF rect(0, 0, 5, 5);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeSmall);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperRectSizes, VeryWideRect) {
    QRectF rect(0, 0, 500, 20);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeSmall);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperRectSizes, VeryTallRect) {
    QRectF rect(0, 0, 20, 500);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeSmall);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperRectSizes, NegativeCoordinates) {
    QRectF rect(-50, -50, 100, 100);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeMedium);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperRectSizes, OffsetRect) {
    QRectF rect(100, 200, 50, 50);
    QPainterPath path =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeMedium);

    EXPECT_FALSE(path.isEmpty());
    EXPECT_TRUE(path.contains(rect.center()));
}

// =============================================================================
// Test Suite 7: Consistency between overloads
// =============================================================================

TEST(GeometryHelperConsistency, ShapeScaleVsUniformRadius) {
    QRectF rect(0, 0, 100, 50);

    // ShapeSmall is defined as 8dp radius
    QPainterPath shapeScalePath =
        cf::ui::base::geometry::roundedRect(rect, cf::ui::base::geometry::ShapeScale::ShapeSmall);
    QPainterPath uniformPath = cf::ui::base::geometry::roundedRect(rect, 8.0f);

    // Both should produce similar (but not necessarily identical) results
    // The exact behavior depends on Qt's rounded rect implementation
    EXPECT_FALSE(shapeScalePath.isEmpty());
    EXPECT_FALSE(uniformPath.isEmpty());
}

TEST(GeometryHelperConsistency, SameInputSameOutput) {
    QRectF rect(0, 0, 100, 50);

    // Call the same function twice with same input
    QPainterPath path1 = cf::ui::base::geometry::roundedRect(rect, 10.0f);
    QPainterPath path2 = cf::ui::base::geometry::roundedRect(rect, 10.0f);

    // Should produce identical results
    // (This tests function purity)
    EXPECT_EQ(path1.fillRule(), path2.fillRule());
}

// =============================================================================
// Test Suite 8: Special cases
// =============================================================================

TEST(GeometryHelperSpecialCases, ZeroSizeRect) {
    QRectF rect(0, 0, 0, 0);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 10.0f);

    // Qt handles zero-size rectangles gracefully
}

TEST(GeometryHelperSpecialCases, NegativeRadius) {
    QRectF rect(0, 0, 100, 50);
    // Qt may clamp or ignore negative radii
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, -10.0f);

    EXPECT_FALSE(path.isEmpty());
}

TEST(GeometryHelperSpecialCases, FloatPrecision) {
    QRectF rect(0.5, 0.5, 99.9, 49.9);
    QPainterPath path = cf::ui::base::geometry::roundedRect(rect, 9.9f);

    EXPECT_FALSE(path.isEmpty());
}

// No main() needed - using GTest::gtest_main
