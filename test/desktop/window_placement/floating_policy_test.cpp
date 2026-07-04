/**
 * @file    floating_policy_test.cpp
 * @brief   GoogleTest unit tests for FloatingPolicy.
 *
 * Covers the pure geometry seam (cascadeOffset + clampToMinimum +
 * initialGeometry) with no IWindow, no backend, and no display — the full
 * initial-placement decision is tested as a pure function of QRect / QSize
 * inputs.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-05
 * @version 0.1
 * @since   0.21
 * @ingroup components
 */

#include "window_placement/floating_policy.h"

#include <QPoint>
#include <QRect>
#include <QSize>
#include <gtest/gtest.h>

namespace {

using cf::desktop::placement::FloatingPolicy;

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

// ── cascadeOffset (pure linear, no wrapping on its own) ──────────────────────

TEST(FloatingPolicy, CascadeOffset_Zero_IsOrigin) {
    EXPECT_EQ(FloatingPolicy::cascadeOffset(0), QPoint(0, 0));
}

TEST(FloatingPolicy, CascadeOffset_One_IsOneStep) {
    EXPECT_EQ(FloatingPolicy::cascadeOffset(1),
              QPoint(FloatingPolicy::kCascadeStep, FloatingPolicy::kCascadeStep));
}

TEST(FloatingPolicy, CascadeOffset_Three_IsThreeSteps) {
    EXPECT_EQ(FloatingPolicy::cascadeOffset(3), QPoint(72, 72));
}

TEST(FloatingPolicy, CascadeOffset_Negative_IsOrigin) {
    EXPECT_EQ(FloatingPolicy::cascadeOffset(-5), QPoint(0, 0));
}

// ── clampToMinimum (component-wise floor) ───────────────────────────────────

TEST(FloatingPolicy, Clamp_BelowMin_RaisedToMin) {
    EXPECT_EQ(FloatingPolicy::clampToMinimum(QSize(100, 100)), QSize(320, 240));
}

TEST(FloatingPolicy, Clamp_AboveMin_Unchanged) {
    EXPECT_EQ(FloatingPolicy::clampToMinimum(QSize(400, 500)), QSize(400, 500));
}

TEST(FloatingPolicy, Clamp_OneBelow_RaisedPartially) {
    EXPECT_EQ(FloatingPolicy::clampToMinimum(QSize(500, 100)), QSize(500, 240));
}

// ── initialGeometry (centered, shrunk-to-fit, cascaded, clamped to work) ────

TEST(FloatingPolicy, Initial_Default_CenteredInWorkArea) {
    // 800x600 in 1000x700 work -> centered at (100, 98).
    expectRect(FloatingPolicy::initialGeometry(kWork), 100, 98, 800, 600);
}

TEST(FloatingPolicy, Initial_CustomDesired_Centered) {
    // 400x300 in kWork -> centered at (300, 248).
    expectRect(FloatingPolicy::initialGeometry(kWork, 0, QSize(400, 300)), 300, 248, 400, 300);
}

TEST(FloatingPolicy, Initial_DesiredBelowMin_RaisedToMinThenCentered) {
    // 100x100 -> min 320x240 -> centered at (340, 278).
    expectRect(FloatingPolicy::initialGeometry(kWork, 0, QSize(100, 100)), 340, 278, 320, 240);
}

TEST(FloatingPolicy, Initial_WorkSmallerThanDefault_ShrinksToWork) {
    // 800x600 default in a 200x200 work -> shrink-to-fit wins over the minimum.
    expectRect(FloatingPolicy::initialGeometry(QRect(0, 0, 200, 200)), 0, 25, 200, 150);
}

TEST(FloatingPolicy, Initial_CascadeOne_OffsetAddedClampedToWork) {
    // Centered (100,98) + (24,24) = (124,122), still inside the work area.
    expectRect(FloatingPolicy::initialGeometry(kWork, 1), 124, 122, 800, 600);
}

TEST(FloatingPolicy, Initial_HighCascade_ClampedToWorkCorner) {
    // Offset (1200,1200) clamped against the work area's right/bottom edge:
    // max_x = 999-800+1 = 200, max_y = 747-600+1 = 148.
    expectRect(FloatingPolicy::initialGeometry(kWork, 50), 200, 148, 800, 600);
}

TEST(FloatingPolicy, Initial_EmptyWorkArea_ReturnsClampedSizeAtOrigin) {
    // No work area to place within -> clamped-to-min size at the origin.
    expectRect(FloatingPolicy::initialGeometry(QRect{}, 0, QSize(100, 100)), 0, 0, 320, 240);
}
