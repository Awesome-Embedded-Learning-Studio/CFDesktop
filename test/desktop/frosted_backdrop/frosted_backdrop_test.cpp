/**
 * @file    frosted_backdrop_test.cpp
 * @brief   GoogleTest unit tests for the FrostedBackdrop renderer.
 *
 * Covers: null-source handling, output sizing, determinism across instances,
 * cache validity/hit, and that changing the tint, strip rect, or source image
 * forces a rebuild. A custom main() spins up an offscreen QGuiApplication so
 * QPixmap/QPainter work without a display (CI / headless friendly).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "frosted_backdrop/frosted_backdrop.h"

#include <QColor>
#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QtGlobal>

#include <gtest/gtest.h>

namespace {

/// @brief Deterministic params (grain disabled so renders are pixel-stable).
cf::desktop::FrostedParams defaultParams() {
    cf::desktop::FrostedParams p;
    p.tint = QColor(255, 255, 255);
    p.tint_alpha = 0.60;
    p.blur_radius_px = 12;
    p.downsample = 4;
    p.box_passes = 2;
    p.enable_grain = false;
    p.top_highlight = false;
    return p;
}

/// @brief A 400x80 RGB32 image: red left half, blue right half.
QImage twoColorSource() {
    QImage src(400, 80, QImage::Format_RGB32);
    src.fill(Qt::red);
    for (int y = 0; y < src.height(); ++y) {
        for (int x = 200; x < src.width(); ++x) {
            src.setPixel(x, y, qRgb(0, 0, 255));
        }
    }
    return src;
}

} // namespace

TEST(FrostedBackdrop, NullSourceReturnsNullPixmap) {
    cf::desktop::FrostedBackdrop fb;
    const QPixmap pm = fb.render(QImage{}, QRect(0, 0, 100, 40), 1.0, defaultParams());
    EXPECT_TRUE(pm.isNull());
}

TEST(FrostedBackdrop, EmptyStripReturnsNullPixmap) {
    cf::desktop::FrostedBackdrop fb;
    const QImage src = twoColorSource();
    const QPixmap pm = fb.render(src, QRect(0, 0, 0, 0), 1.0, defaultParams());
    EXPECT_TRUE(pm.isNull());
}

TEST(FrostedBackdrop, OutputSizeMatchesStrip) {
    cf::desktop::FrostedBackdrop fb;
    const QImage src = twoColorSource();
    const QRect strip(0, 0, 200, 40);
    const QPixmap pm = fb.render(src, strip, 1.0, defaultParams());
    ASSERT_FALSE(pm.isNull());
    EXPECT_EQ(pm.size(), strip.size());
}

TEST(FrostedBackdrop, DeterministicAcrossInstances) {
    cf::desktop::FrostedBackdrop fb1;
    cf::desktop::FrostedBackdrop fb2;
    const QImage src = twoColorSource();
    const QRect strip(0, 0, 200, 40);
    const auto p = defaultParams();
    // Two independent renderers with identical inputs produce identical pixels
    // (grain disabled); this is the determinism contract callers rely on.
    EXPECT_EQ(fb1.render(src, strip, 1.0, p).toImage(), fb2.render(src, strip, 1.0, p).toImage());
}

TEST(FrostedBackdrop, CacheReportsValidAfterRender) {
    cf::desktop::FrostedBackdrop fb;
    const QImage src = twoColorSource();
    const QRect strip(0, 0, 200, 40);
    const auto p = defaultParams();
    EXPECT_FALSE(fb.isCacheValid(src, strip, 1.0, p));
    (void)fb.render(src, strip, 1.0, p);
    EXPECT_TRUE(fb.isCacheValid(src, strip, 1.0, p));
}

TEST(FrostedBackdrop, RepeatedRenderIsCacheHit) {
    cf::desktop::FrostedBackdrop fb;
    const QImage src = twoColorSource();
    const QRect strip(0, 0, 200, 40);
    const auto p = defaultParams();
    const QPixmap first = fb.render(src, strip, 1.0, p);
    const QPixmap second = fb.render(src, strip, 1.0, p);
    ASSERT_FALSE(first.isNull());
    // A cache hit returns the cached data (shared) -> identical pixmap key.
    EXPECT_EQ(first.cacheKey(), second.cacheKey());
}

TEST(FrostedBackdrop, ChangingTintForcesRebuild) {
    cf::desktop::FrostedBackdrop fb;
    const QImage src = twoColorSource();
    const QRect strip(0, 0, 200, 40);
    auto p = defaultParams();
    const QPixmap a = fb.render(src, strip, 1.0, p);
    p.tint = QColor(0, 0, 0);
    EXPECT_FALSE(fb.isCacheValid(src, strip, 1.0, p));
    const QPixmap b = fb.render(src, strip, 1.0, p);
    EXPECT_NE(a.cacheKey(), b.cacheKey());
}

TEST(FrostedBackdrop, ChangingStripForcesRebuild) {
    cf::desktop::FrostedBackdrop fb;
    const QImage src = twoColorSource();
    const QRect strip_a(0, 0, 200, 40);
    const QRect strip_b(10, 10, 200, 40);
    const auto p = defaultParams();
    const QPixmap a = fb.render(src, strip_a, 1.0, p);
    const QPixmap b = fb.render(src, strip_b, 1.0, p);
    EXPECT_NE(a.cacheKey(), b.cacheKey());
}

TEST(FrostedBackdrop, ChangingSourceForcesRebuild) {
    cf::desktop::FrostedBackdrop fb;
    QImage src = twoColorSource();
    const QRect strip(0, 0, 200, 40);
    const auto p = defaultParams();
    const QPixmap a = fb.render(src, strip, 1.0, p);
    src.fill(Qt::green); // New content -> new QImage::cacheKey().
    EXPECT_FALSE(fb.isCacheValid(src, strip, 1.0, p));
    const QPixmap b = fb.render(src, strip, 1.0, p);
    EXPECT_NE(a.cacheKey(), b.cacheKey());
}

int main(int argc, char** argv) {
    // QPixmap/QPainter need a QGuiApplication; offscreen avoids a display.
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QGuiApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
