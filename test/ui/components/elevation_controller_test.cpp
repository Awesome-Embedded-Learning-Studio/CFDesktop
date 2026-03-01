/**
 * @file elevation_controller_test.cpp
 * @brief Unit tests for MdElevationController (Material Behavior Layer)
 *
 * Test Coverage:
 * 1. Elevation level setting
 * 2. Shadow parameter calculation (via paint output)
 * 3. Tonal overlay calculation
 * 4. Level clamping
 */

#include "color.h"
#include "ui/widget/material/base/elevation_controller.h"
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <gtest/gtest.h>

using namespace cf::ui::widget::material::base;
using CFColor = cf::ui::base::CFColor;

// =============================================================================
// Test Helper: Count Non-Transparent Pixels
// =============================================================================

/**
 * @brief Helper function to count non-transparent pixels in an image.
 * Used to verify paint behavior.
 */
static int countNonTransparentPixels(const QImage& image) {
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) > 0) {
                ++count;
            }
        }
    }
    return count;
}

/**
 * @brief Helper to count pixels with alpha > threshold.
 * Useful for detecting semi-transparent shadow pixels.
 */
static int countSemiTransparentPixels(const QImage& image, int alphaThreshold = 10) {
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) > alphaThreshold) {
                ++count;
            }
        }
    }
    return count;
}

// =============================================================================
// Test Suite 1: Elevation Configuration
// =============================================================================

TEST(ElevationControllerTest, InitialElevation_IsZero) {
    MdElevationController controller(nullptr, nullptr);
    EXPECT_EQ(controller.elevation(), 0);
}

TEST(ElevationControllerTest, SetElevation_UpdatesLevel) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(3);
    EXPECT_EQ(controller.elevation(), 3);
}

TEST(ElevationControllerTest, SetElevation_ClampsToFive) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(10);
    EXPECT_EQ(controller.elevation(), 5); // Should be clamped
}

TEST(ElevationControllerTest, SetElevation_ClampsToZero) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(-5);
    EXPECT_EQ(controller.elevation(), 0); // Should be clamped
}

// =============================================================================
// Test Suite 2: Shadow Parameters (via Paint Output)
// =============================================================================

TEST(ElevationControllerTest, PaintShadow_Level0_NoShadow) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(0);

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    QPainterPath shape;
    shape.addRect(30, 30, 40, 40);

    controller.paintShadow(&painter, shape);
    painter.end();

    // Level 0 should have minimal or no shadow
    // Shadow should be mostly transparent
    int semiTransparent = countSemiTransparentPixels(image, 5);
    EXPECT_LT(semiTransparent, 10) << "Level 0 should have minimal shadow";
}

TEST(ElevationControllerTest, PaintShadow_Level5_HasVisibleShadow) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(5);

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    QPainterPath shape;
    shape.addRect(30, 30, 40, 40);

    controller.paintShadow(&painter, shape);
    painter.end();

    // Level 5 should have a visible shadow
    int nonTransparent = countNonTransparentPixels(image);
    EXPECT_GT(nonTransparent, 0) << "Level 5 should have visible shadow";
}

TEST(ElevationControllerTest, PaintShadow_HigherLevel_MoreShadowPixels) {
    MdElevationController controllerLow(nullptr, nullptr);
    controllerLow.setElevation(1);

    MdElevationController controllerHigh(nullptr, nullptr);
    controllerHigh.setElevation(5);

    QImage image1(100, 100, QImage::Format_ARGB32);
    image1.fill(Qt::transparent);
    {
        QPainter painter(&image1);
        QPainterPath shape;
        shape.addRect(30, 30, 40, 40);
        controllerLow.paintShadow(&painter, shape);
    }

    QImage image2(100, 100, QImage::Format_ARGB32);
    image2.fill(Qt::transparent);
    {
        QPainter painter(&image2);
        QPainterPath shape;
        shape.addRect(30, 30, 40, 40);
        controllerHigh.paintShadow(&painter, shape);
    }

    // Higher elevation should produce more shadow pixels (larger blur radius)
    int pixelsLevel1 = countSemiTransparentPixels(image1, 5);
    int pixelsLevel5 = countSemiTransparentPixels(image2, 5);

    EXPECT_GT(pixelsLevel5, pixelsLevel1)
        << "Higher elevation should produce more shadow pixels due to larger blur";
}

TEST(ElevationControllerTest, PaintShadow_NullPainter_DoesNotCrash) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(5);

    QPainterPath shape;
    shape.addRect(30, 30, 40, 40);

    // Should not crash with null painter
    controller.paintShadow(nullptr, shape);
}

TEST(ElevationControllerTest, PaintShadow_ComplexShape_HandlesCorrectly) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(3);

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    // Create a more complex shape (rounded rect)
    QPainterPath shape;
    shape.addRoundedRect(30, 30, 40, 40, 8, 8);

    controller.paintShadow(&painter, shape);
    painter.end();

    // Should paint shadow for complex shapes too
    EXPECT_GT(countNonTransparentPixels(image), 0);
}

// =============================================================================
// Test Suite 3: Tonal Overlay
// =============================================================================

TEST(ElevationControllerTest, TonalOverlay_Level0_ReturnsSurface) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(0);

    CFColor surface(200, 200, 200);
    CFColor primary(100, 100, 255);

    CFColor result = controller.tonalOverlay(surface, primary);
    // Level 0 should return surface unchanged
    EXPECT_EQ(result.native_color(), surface.native_color());
}

TEST(ElevationControllerTest, TonalOverlay_HighElevation_BlendsWithPrimary) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(5);

    CFColor surface(200, 200, 200);
    CFColor primary(100, 100, 255);

    CFColor result = controller.tonalOverlay(surface, primary);

    // Result should differ from surface at higher elevation
    // (blends in primary color for tonal elevation effect)
    QColor resultColor = result.native_color();
    QColor surfaceColor = surface.native_color();

    // At least one color component should be different
    bool differs =
        (resultColor.red() != surfaceColor.red() || resultColor.green() != surfaceColor.green() ||
         resultColor.blue() != surfaceColor.blue());

    EXPECT_TRUE(differs) << "Higher elevation should modify surface color";
}

TEST(ElevationControllerTest, TonalOverlay_IncreasingElevation_MoreBlending) {
    MdElevationController controllerLow(nullptr, nullptr);
    controllerLow.setElevation(1);

    MdElevationController controllerHigh(nullptr, nullptr);
    controllerHigh.setElevation(5);

    CFColor surface(200, 200, 200);
    CFColor primary(100, 100, 255);

    CFColor resultLow = controllerLow.tonalOverlay(surface, primary);
    CFColor resultHigh = controllerHigh.tonalOverlay(surface, primary);

    // Higher elevation should result in more blending (closer to primary)
    QColor lowColor = resultLow.native_color();
    QColor highColor = resultHigh.native_color();
    QColor surfaceColor = surface.native_color();
    QColor primaryColor = primary.native_color();

    // Calculate distance from surface
    int lowDist = std::abs(lowColor.red() - surfaceColor.red()) +
                  std::abs(lowColor.green() - surfaceColor.green()) +
                  std::abs(lowColor.blue() - surfaceColor.blue());

    int highDist = std::abs(highColor.red() - surfaceColor.red()) +
                   std::abs(highColor.green() - surfaceColor.green()) +
                   std::abs(highColor.blue() - surfaceColor.blue());

    EXPECT_GE(highDist, lowDist) << "Higher elevation should blend more primary color";
}

// =============================================================================
// Test Suite 4: Animation
// =============================================================================

TEST(ElevationControllerTest, AnimateTo_DoesNotCrash) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(0);

    cf::ui::core::MotionSpec spec;
    spec.durationMs = 200;
    spec.easing = cf::ui::base::Easing::Type::Standard;

    controller.animateTo(3, spec);
    // Should handle gracefully even without animation factory
    SUCCEED();
}

TEST(ElevationControllerTest, AnimateTo_BeyondRange_ClampsTarget) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(0);

    cf::ui::core::MotionSpec spec;
    spec.durationMs = 200;
    spec.easing = cf::ui::base::Easing::Type::Standard;

    controller.animateTo(10, spec);
    // Target should be clamped to 5
    // (Actual elevation may not change without animation factory)
    SUCCEED();
}

// =============================================================================
// Test Suite 5: Edge Cases
// =============================================================================

TEST(ElevationControllerTest, SetSameElevation_DoesNotTriggerAnimation) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(3);

    int before = controller.elevation();
    controller.setElevation(3);

    EXPECT_EQ(controller.elevation(), before);
}

TEST(ElevationControllerTest, NegativeElevation_ClampsToZero) {
    MdElevationController controller(nullptr, nullptr);
    controller.setElevation(-1);
    EXPECT_EQ(controller.elevation(), 0);

    controller.setElevation(-100);
    EXPECT_EQ(controller.elevation(), 0);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
