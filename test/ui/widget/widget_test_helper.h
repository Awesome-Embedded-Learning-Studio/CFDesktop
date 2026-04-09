/**
 * @file widget_test_helper.h
 * @brief Common test utilities for Material Design 3 widget tests.
 *
 * Provides offscreen rendering helpers and pixel verification utilities
 * for testing widget paint behavior without a display server.
 */

#pragma once

#include <QCoreApplication>
#include <QImage>
#include <QWidget>
#include <gtest/gtest.h>

/**
 * @brief Ensure fonts are available for offscreen rendering.
 *
 * Qt6 no longer ships bundled fonts. In offscreen QPA mode on Windows,
 * set QT_QPA_FONTDIR to the system fonts directory so text renders properly.
 * Must be called before QApplication is constructed.
 */
inline void ensureFontsAvailable() {
#ifdef _WIN32
    qputenv("QT_QPA_FONTDIR", "C:/Windows/Fonts");
#endif
}

namespace widget_test {

/**
 * @brief Count non-transparent pixels in an image.
 * Used to verify that paint operations produced visible output.
 */
inline int countNonTransparentPixels(const QImage& image) {
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
 * @brief Check if the image contains any pixel with the specified color (with tolerance).
 * @param image The image to search.
 * @param color The target color to find.
 * @param tolerance Per-channel tolerance (default 20).
 */
inline bool hasPixelWithColor(const QImage& image, QRgb color, int tolerance = 20) {
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) > 0 && qAbs(qRed(pixel) - qRed(color)) <= tolerance &&
                qAbs(qGreen(pixel) - qGreen(color)) <= tolerance &&
                qAbs(qBlue(pixel) - qBlue(color)) <= tolerance) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Render a widget to an offscreen QImage.
 *
 * Resizes the widget, renders it to a transparent ARGB32 image.
 * Requires QGuiApplication to exist (offscreen platform).
 */
inline QImage renderWidgetToImage(QWidget* widget, const QSize& size) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    widget->resize(size);
    widget->show();
    QCoreApplication::processEvents();
    widget->render(&image);
    widget->hide();
    return image;
}

/**
 * @brief Verify that a widget paints non-transparent pixels.
 *
 * Renders the widget and asserts that at least one non-transparent
 * pixel was produced.
 */
inline void verifyPaintsPixels(QWidget* widget, const QSize& size) {
    QImage image = renderWidgetToImage(widget, size);
    int nonTransparent = countNonTransparentPixels(image);
    EXPECT_GT(nonTransparent, 0) << "Widget should paint non-transparent pixels";
}

/**
 * @brief Check if two images have different pixel content.
 *
 * Compares every pixel. Returns true if at least one pixel differs
 * (comparing RGBA channels). This is useful for verifying that widget
 * state changes (enabled/disabled, checked/unchecked) produce visual changes.
 */
inline bool imagesDiffer(const QImage& a, const QImage& b) {
    if (a.size() != b.size())
        return true;
    for (int y = 0; y < a.height(); ++y) {
        for (int x = 0; x < a.width(); ++x) {
            if (a.pixel(x, y) != b.pixel(x, y)) {
                return true;
            }
        }
    }
    return false;
}

} // namespace widget_test
