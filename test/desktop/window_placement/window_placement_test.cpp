/**
 * @file    window_placement_test.cpp
 * @brief   GoogleTest unit tests for WindowPlacementPolicy.
 *
 * Covers the pure geometry seam (centerInWorkArea + computeConstrain) with no
 * IWindow, no backend, and no display — the full placement decision is tested
 * as a pure function of QRect inputs.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.2
 * @since   0.20
 * @ingroup components
 */

#include "window_placement/window_placement_policy.h"

#include <QRect>
#include <gtest/gtest.h>

namespace {

using cf::desktop::placement::WindowPlacementPolicy;

/// Work area used throughout: x in [0,999], y in [48,747], center (500,398).
const QRect kWork{0, 48, 1000, 700};

/// @brief Asserts a QRect matches the given components with readable failures.
void expectRect(const QRect& actual, int x, int y, int w, int h) {
    EXPECT_EQ(actual.x(), x);
    EXPECT_EQ(actual.y(), y);
    EXPECT_EQ(actual.width(), w);
    EXPECT_EQ(actual.height(), h);
}

} // namespace

// ── centerInWorkArea (size-driven, aspect-preserving, ignores input position) ─

TEST(WindowPlacement, Center_FitsSmaller_CenteredIgnoringInputPos) {
    // Input position (200,200) is ignored; result is centered by size.
    expectRect(WindowPlacementPolicy::centerInWorkArea({200, 200, 200, 200}, kWork), 400, 298, 200,
               200);
}

TEST(WindowPlacement, Center_LargerBothAxes_ProportionalShrinkCentered) {
    expectRect(WindowPlacementPolicy::centerInWorkArea({0, 0, 2000, 2000}, kWork), 150, 48, 700,
               700);
}

TEST(WindowPlacement, Center_TallerThanWork_HeightClampedAspectKept) {
    // 500x1400 -> scale 0.5 -> 250x700, centered.
    expectRect(WindowPlacementPolicy::centerInWorkArea({0, 0, 500, 1400}, kWork), 375, 48, 250,
               700);
}

TEST(WindowPlacement, Center_WiderThanWork_WidthClampedAspectKept) {
    // 2500x350 -> scale 0.4 -> 1000x140, centered.
    expectRect(WindowPlacementPolicy::centerInWorkArea({0, 0, 2500, 350}, kWork), 0, 328, 1000,
               140);
}

TEST(WindowPlacement, Center_ExactWorkSize_FillsWork) {
    expectRect(WindowPlacementPolicy::centerInWorkArea({0, 0, 1000, 700}, kWork), 0, 48, 1000, 700);
}

TEST(WindowPlacement, Center_EmptyWorkArea_Unchanged) {
    expectRect(WindowPlacementPolicy::centerInWorkArea({500, 500, 200, 200}, QRect{}), 500, 500,
               200, 200);
}

// ── computeConstrain (the decision seam: only outside windows get centered) ──

TEST(WindowPlacement, Constrain_Disabled_ReturnsNullopt) {
    EXPECT_FALSE(
        WindowPlacementPolicy::computeConstrain({-50, -50, 200, 200}, kWork, false).has_value());
}

TEST(WindowPlacement, Constrain_EmptyWorkArea_ReturnsNullopt) {
    EXPECT_FALSE(
        WindowPlacementPolicy::computeConstrain({100, 100, 200, 200}, QRect{}, true).has_value());
}

TEST(WindowPlacement, Constrain_EmptyCurrent_ReturnsNullopt) {
    EXPECT_FALSE(WindowPlacementPolicy::computeConstrain(QRect{}, kWork, true).has_value());
}

TEST(WindowPlacement, Constrain_FullyInside_ReturnsNullopt_NoYank) {
    // Already inside the work area -> leave where it is.
    EXPECT_FALSE(
        WindowPlacementPolicy::computeConstrain({100, 100, 200, 200}, kWork, true).has_value());
}

TEST(WindowPlacement, Constrain_ExactWorkFill_ReturnsNullopt) {
    EXPECT_FALSE(
        WindowPlacementPolicy::computeConstrain({0, 48, 1000, 700}, kWork, true).has_value());
}

TEST(WindowPlacement, Constrain_OutsideRight_ReturnsCenteredTarget) {
    // {900,600,300,300}: right=1199 > 999 -> outside -> centered 300x300.
    auto target = WindowPlacementPolicy::computeConstrain({900, 600, 300, 300}, kWork, true);
    ASSERT_TRUE(target.has_value());
    expectRect(*target, 350, 248, 300, 300);
}

TEST(WindowPlacement, Constrain_LargerThanWork_ReturnsShrunkCenteredTarget) {
    auto target = WindowPlacementPolicy::computeConstrain({0, 0, 2000, 2000}, kWork, true);
    ASSERT_TRUE(target.has_value());
    expectRect(*target, 150, 48, 700, 700);
}
