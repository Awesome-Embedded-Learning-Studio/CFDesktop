/**
 * @file    wallpaper_animation_test.cpp
 * @brief   Unit tests for wallpaper transition compositing, selector, and engine.
 *
 * Covers the pure compositing helper (composeTransitionFrame), the rotation
 * selector (selectNextWallpaper) for both Sequential and Random, and that the
 * WallPaperEngine picks up default configuration. A custom main() spins up an
 * offscreen QGuiApplication so QPainter/QImage/QTimer work without a display.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.19
 * @ingroup wallpaper
 */

#include "wallpaper/TransitionComposer.h"
#include "wallpaper/WallPaperAccessStorage.h"
#include "wallpaper/WallPaperEngine.h"
#include "wallpaper/WallPaperLayer.h"
#include "wallpaper/WallPaperToken.h"

#include <QColor>
#include <QEasingCurve>
#include <QGuiApplication>
#include <QImage>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace cf::desktop::wallpaper;

namespace {

/// @brief Builds a solid-color test image of the given size.
QImage solidImage(const QSize& size, const QColor& color) {
    QImage img(size, QImage::Format_RGB32);
    img.fill(color);
    return img;
}

/**
 * @brief Minimal WallPaperLayer double with a controllable storage size.
 *
 * Only tokenStorage() is exercised (by the engine's size guard). The image
 * and switch methods return inert defaults.
 */
class FakeLayer : public WallPaperLayer {
  public:
    explicit FakeLayer(size_t token_count) {
        QStringList paths;
        for (size_t i = 0; i < token_count; ++i) {
            paths << QString("/fake/wallpaper_%1.png").arg(i);
        }
        storage_.addTokens(WallPaperTokenFactory::fromFiles(paths));
    }

    QImage currentImage() const override { return {}; }
    ScalingMode scalingMode() const override { return ScalingMode::Fill; }
    QColor backgroundColor() const override { return QColor(0, 0, 0); }
    void setTokenStorage(std::unique_ptr<WallPaperAccessStorage>) override {}
    WallPaperAccessStorage& tokenStorage() const override { return storage_; }
    bool showNextOne() override { return false; }
    bool showPrevOne() override { return false; }
    bool showTargetOne(const wallpaper_token_id_t&) override { return false; }

  private:
    mutable WallPaperAccessStorage storage_;
};

} // namespace

// ============================================================
// composeTransitionFrame
// ============================================================

TEST(TransitionComposer, EmptyTargetReturnsNull) {
    const QImage prev = solidImage({100, 100}, Qt::red);
    const QImage cur = solidImage({100, 100}, Qt::blue);
    const QImage out = composeTransitionFrame(prev, cur, 0.5, SwitchingMode::Gradient, QSize(0, 0));
    EXPECT_TRUE(out.isNull());
}

TEST(TransitionComposer, NullCurrentReturnsNull) {
    const QImage prev = solidImage({100, 100}, Qt::red);
    const QImage out =
        composeTransitionFrame(prev, QImage(), 0.5, SwitchingMode::Gradient, QSize(100, 100));
    EXPECT_TRUE(out.isNull());
}

TEST(TransitionComposer, OutputMatchesTargetSize) {
    const QImage prev = solidImage({100, 100}, Qt::red);
    const QImage cur = solidImage({100, 100}, Qt::blue);
    const QImage out =
        composeTransitionFrame(prev, cur, 0.5, SwitchingMode::Gradient, QSize(120, 80));
    ASSERT_FALSE(out.isNull());
    EXPECT_EQ(out.size(), QSize(120, 80));
}

TEST(TransitionComposer, GradientAtStartShowsPrevious) {
    // At t=0 the previous frame is fully opaque over the current one.
    const QImage prev = solidImage({50, 50}, Qt::red);
    const QImage cur = solidImage({50, 50}, Qt::blue);
    const QImage out =
        composeTransitionFrame(prev, cur, 0.0, SwitchingMode::Gradient, QSize(50, 50));
    ASSERT_FALSE(out.isNull());
    EXPECT_EQ(out.pixel(0, 0), prev.pixel(0, 0));
}

TEST(TransitionComposer, GradientAtEndShowsCurrent) {
    // At t=1 the previous frame is fully faded out, leaving the current one.
    const QImage prev = solidImage({50, 50}, Qt::red);
    const QImage cur = solidImage({50, 50}, Qt::blue);
    const QImage out =
        composeTransitionFrame(prev, cur, 1.0, SwitchingMode::Gradient, QSize(50, 50));
    ASSERT_FALSE(out.isNull());
    EXPECT_EQ(out.pixel(0, 0), cur.pixel(0, 0));
}

TEST(TransitionComposer, MovementSlidesOldOutNewIn) {
    // At t=0 the left edge shows the previous frame; at t=1 it shows the current.
    const QImage prev = solidImage({60, 10}, Qt::red);
    const QImage cur = solidImage({60, 10}, Qt::blue);
    const QImage out0 =
        composeTransitionFrame(prev, cur, 0.0, SwitchingMode::Movement, QSize(60, 10));
    const QImage out1 =
        composeTransitionFrame(prev, cur, 1.0, SwitchingMode::Movement, QSize(60, 10));
    ASSERT_FALSE(out0.isNull());
    ASSERT_FALSE(out1.isNull());
    EXPECT_EQ(out0.pixel(0, 0), prev.pixel(0, 0));
    EXPECT_EQ(out1.pixel(0, 0), cur.pixel(0, 0));
}

TEST(TransitionComposer, ClampsProgressBeyondRange) {
    const QImage prev = solidImage({40, 40}, Qt::red);
    const QImage cur = solidImage({40, 40}, Qt::green);
    // t > 1 clamps to 1 (current); t < 0 clamps to 0 (previous); no crash.
    const QImage over =
        composeTransitionFrame(prev, cur, 5.0, SwitchingMode::Gradient, QSize(40, 40));
    const QImage under =
        composeTransitionFrame(prev, cur, -3.0, SwitchingMode::Gradient, QSize(40, 40));
    EXPECT_EQ(over.pixel(0, 0), cur.pixel(0, 0));
    EXPECT_EQ(under.pixel(0, 0), prev.pixel(0, 0));
}

// ============================================================
// selectNextWallpaper
// ============================================================

TEST(SelectNextWallpaper, EmptyIdsReturnsEmpty) {
    std::mt19937 rng(42);
    const auto id = selectNextWallpaper({}, "x", Selector::Sequential, rng);
    EXPECT_TRUE(id.isEmpty());
}

TEST(SelectNextWallpaper, SequentialAdvancesOne) {
    std::mt19937 rng(1);
    const std::vector<QString> ids = {"a", "b", "c"};
    EXPECT_EQ(selectNextWallpaper(ids, "a", Selector::Sequential, rng).toStdString(), "b");
    EXPECT_EQ(selectNextWallpaper(ids, "b", Selector::Sequential, rng).toStdString(), "c");
}

TEST(SelectNextWallpaper, SequentialWrapsAtEnd) {
    std::mt19937 rng(1);
    const std::vector<QString> ids = {"a", "b", "c"};
    EXPECT_EQ(selectNextWallpaper(ids, "c", Selector::Sequential, rng).toStdString(), "a");
}

TEST(SelectNextWallpaper, SequentialUnknownCurrentFallsToFirst) {
    std::mt19937 rng(1);
    const std::vector<QString> ids = {"a", "b", "c"};
    EXPECT_EQ(selectNextWallpaper(ids, "zzz", Selector::Sequential, rng).toStdString(), "a");
}

TEST(SelectNextWallpaper, RandomNeverReturnsCurrent) {
    std::mt19937 rng(7);
    const std::vector<QString> ids = {"a", "b", "c", "d"};
    for (int i = 0; i < 50; ++i) {
        const auto next = selectNextWallpaper(ids, "b", Selector::Random, rng);
        EXPECT_NE(next.toStdString(), "b");
        EXPECT_FALSE(next.isEmpty());
    }
}

TEST(SelectNextWallpaper, RandomCoversAllNonCurrentCandidates) {
    // Over many draws with a fixed seed, every non-current id appears at least once.
    std::mt19937 rng(123);
    const std::vector<QString> ids = {"a", "b", "c", "d"};
    std::set<std::string> seen;
    for (int i = 0; i < 300; ++i) {
        seen.insert(selectNextWallpaper(ids, "a", Selector::Random, rng).toStdString());
    }
    EXPECT_EQ(seen.count("a"), 0u); // current never returned
    EXPECT_EQ(seen.count("b"), 1u);
    EXPECT_EQ(seen.count("c"), 1u);
    EXPECT_EQ(seen.count("d"), 1u);
}

// ============================================================
// WallPaperEngine configuration & guards
// ============================================================

TEST(WallPaperEngine, DefaultsMatchCcimxSemantics) {
    // Relies on ConfigStore returning defaults when the wallpaper domain is unset.
    WallPaperEngine engine(nullptr, WallPaperEngine::RequestTransition{}, nullptr);
    EXPECT_EQ(engine.mode(), SwitchingMode::Movement);
    EXPECT_EQ(engine.selector(), Selector::Sequential);
    EXPECT_EQ(engine.animationDurationMs(), 2000);
    EXPECT_EQ(engine.easing().type(), QEasingCurve::InOutCubic);
}

TEST(WallPaperEngine, StartStopWithMultipleWallpapersIsSafe) {
    FakeLayer layer(3);
    WallPaperEngine engine(&layer, WallPaperEngine::RequestTransition{}, nullptr);
    engine.start(); // Movement, size>1 → timer armed; no event loop means no tick.
    engine.stop();
    SUCCEED();
}

TEST(WallPaperEngine, StartWithSingleWallpaperIsHarmless) {
    FakeLayer layer(1);
    WallPaperEngine engine(&layer, WallPaperEngine::RequestTransition{}, nullptr);
    engine.start(); // size<=1 guard: timer not armed.
    engine.stop();
    SUCCEED();
}

// ============================================================
// Custom main: offscreen QGuiApplication for QPainter/QImage/QTimer.
// ============================================================

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
