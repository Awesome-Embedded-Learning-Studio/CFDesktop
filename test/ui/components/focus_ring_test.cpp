/**
 * @file focus_ring_test.cpp
 * @brief Unit tests for MdFocusIndicator (Material Behavior Layer)
 *
 * Test Coverage:
 * 1. Focus state changes (via paint output)
 * 2. Progress animation behavior
 * 3. Paint method behavior with QImage verification
 */

#include "color.h"
#include "ui/widget/material/base/focus_ring.h"
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

// =============================================================================
// Test Suite 1: Focus Events (via Paint Output)
// =============================================================================

TEST(FocusRingTest, InitialState_PaintDoesNothing) {
    MdFocusIndicator indicator(nullptr, nullptr);

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    QPainterPath shape;
    shape.addRect(20, 20, 60, 60);

    indicator.paint(&painter, shape, CFColor(100, 150, 255));
    painter.end();

    // Without focus, progress is 0, so paint should do nothing
    EXPECT_EQ(countNonTransparentPixels(image), 0);
}

TEST(FocusRingTest, OnFocusIn_PaintsFocusRing) {
    MdFocusIndicator indicator(nullptr, nullptr);
    indicator.onFocusIn();

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    QPainterPath shape;
    shape.addRect(20, 20, 60, 60);

    indicator.paint(&painter, shape, CFColor(100, 150, 255));
    painter.end();

    // With focus, should paint the focus ring
    int nonTransparent = countNonTransparentPixels(image);
    EXPECT_GT(nonTransparent, 0) << "Focus ring should be painted after onFocusIn";
}

TEST(FocusRingTest, OnFocusOut_RemovesFocusRing) {
    MdFocusIndicator indicator(nullptr, nullptr);
    indicator.onFocusIn();
    indicator.onFocusOut();

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    QPainterPath shape;
    shape.addRect(20, 20, 60, 60);

    indicator.paint(&painter, shape, CFColor(100, 150, 255));
    painter.end();

    // After focus out, progress should be 0 again
    EXPECT_EQ(countNonTransparentPixels(image), 0);
}

// =============================================================================
// Test Suite 2: Multiple Focus Cycles
// =============================================================================

TEST(FocusRingTest, MultipleFocusCycles_PaintsCorrectly) {
    MdFocusIndicator indicator(nullptr, nullptr);

    // First focus cycle
    indicator.onFocusIn();

    QImage image1(100, 100, QImage::Format_ARGB32);
    image1.fill(Qt::transparent);
    {
        QPainter painter(&image1);
        QPainterPath shape;
        shape.addRect(20, 20, 60, 60);
        indicator.paint(&painter, shape, CFColor(100, 150, 255));
    }
    int pixelsAfterFirstFocus = countNonTransparentPixels(image1);

    indicator.onFocusOut();

    QImage image2(100, 100, QImage::Format_ARGB32);
    image2.fill(Qt::transparent);
    {
        QPainter painter(&image2);
        QPainterPath shape;
        shape.addRect(20, 20, 60, 60);
        indicator.paint(&painter, shape, CFColor(100, 150, 255));
    }
    EXPECT_EQ(countNonTransparentPixels(image2), 0) << "Should be clear after first focus out";

    // Second focus cycle
    indicator.onFocusIn();

    QImage image3(100, 100, QImage::Format_ARGB32);
    image3.fill(Qt::transparent);
    {
        QPainter painter(&image3);
        QPainterPath shape;
        shape.addRect(20, 20, 60, 60);
        indicator.paint(&painter, shape, CFColor(100, 150, 255));
    }
    int pixelsAfterSecondFocus = countNonTransparentPixels(image3);

    // Both focus states should paint the same amount
    EXPECT_EQ(pixelsAfterFirstFocus, pixelsAfterSecondFocus)
        << "Multiple focus cycles should produce consistent paint output";

    indicator.onFocusOut();

    QImage image4(100, 100, QImage::Format_ARGB32);
    image4.fill(Qt::transparent);
    {
        QPainter painter(&image4);
        QPainterPath shape;
        shape.addRect(20, 20, 60, 60);
        indicator.paint(&painter, shape, CFColor(100, 150, 255));
    }
    EXPECT_EQ(countNonTransparentPixels(image4), 0) << "Should be clear after second focus out";
}

// =============================================================================
// Test Suite 3: Edge Cases
// =============================================================================

TEST(FocusRingTest, Paint_NullPainter_DoesNotCrash) {
    MdFocusIndicator indicator(nullptr, nullptr);
    indicator.onFocusIn();

    QPainterPath shape;
    shape.addRect(20, 20, 60, 60);

    // Should not crash with null painter
    indicator.paint(nullptr, shape, CFColor(100, 150, 255));
}

TEST(FocusRingTest, Paint_WithDifferentColors_AppliesColor) {
    MdFocusIndicator indicator(nullptr, nullptr);
    indicator.onFocusIn();

    // Test with red color
    QImage image1(100, 100, QImage::Format_ARGB32);
    image1.fill(Qt::transparent);
    {
        QPainter painter(&image1);
        QPainterPath shape;
        shape.addRect(20, 20, 60, 60);
        indicator.paint(&painter, shape, CFColor(255, 0, 0));
    }

    // Find a red pixel (with some tolerance for alpha)
    bool foundRed = false;
    for (int y = 0; y < image1.height() && !foundRed; ++y) {
        for (int x = 0; x < image1.width() && !foundRed; ++x) {
            QRgb pixel = image1.pixel(x, y);
            if (qAlpha(pixel) > 0) {
                // Check if red channel is dominant
                if (qRed(pixel) > qGreen(pixel) && qRed(pixel) > qBlue(pixel)) {
                    foundRed = true;
                }
            }
        }
    }
    EXPECT_TRUE(foundRed) << "Painted ring should have red color component";
}

TEST(FocusRingTest, Paint_ComplexShape_HandlesCorrectly) {
    MdFocusIndicator indicator(nullptr, nullptr);
    indicator.onFocusIn();

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    // Create a more complex shape (rounded rect)
    QPainterPath shape;
    shape.addRoundedRect(20, 20, 60, 60, 10, 10);

    indicator.paint(&painter, shape, CFColor(100, 150, 255));
    painter.end();

    // Should paint something for complex shapes too
    EXPECT_GT(countNonTransparentPixels(image), 0);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
