/**
 * @file    test/desktop/desktop_icon_layer/desktop_icon_layer_test.cpp
 * @brief   GoogleTest unit tests for computeGridDimensions.
 *
 * Exercises the pure grid-sizing math (column clamp, row capacity, cap-and-
 * truncate) without instantiating a QWidget. Layout constants mirror the
 * anonymous namespace in desktop_icon_layer.cpp: kCellSize=96, kGridSpacing=16,
 * kMargin=24 (stride = 96+16 = 112; usable = side - 2*24 + 16; no column cap).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "desktop_icon_layer/desktop_icon_layer.h"

#include <QSize>

#include <gtest/gtest.h>

using cf::desktop::desktop_component::computeGridDimensions;

TEST(DesktopIconLayerGrid, InvalidSizeReturnsZero) {
    const auto d = computeGridDimensions(QSize(0, 0), 10);
    EXPECT_EQ(d.columns, 0);
    EXPECT_EQ(d.rows, 0);
    EXPECT_EQ(d.shown, 0);
}

TEST(DesktopIconLayerGrid, InvalidCountReturnsZero) {
    const auto d = computeGridDimensions(QSize(1280, 720), 0);
    EXPECT_EQ(d.shown, 0);
}

TEST(DesktopIconLayerGrid, NegativeCountReturnsZero) {
    const auto d = computeGridDimensions(QSize(1280, 720), -3);
    EXPECT_EQ(d.shown, 0);
}

TEST(DesktopIconLayerGrid, ColumnsFillWideWidth) {
    // usable_w = 1280 - 48 + 16 = 1248; 1248 / 112 = 11 (no artificial cap).
    const auto d = computeGridDimensions(QSize(1280, 720), 5);
    EXPECT_EQ(d.columns, 11);
}

TEST(DesktopIconLayerGrid, RowsAndCapacityForTypicalDesktop) {
    // usable_h = 720 - 48 + 16 = 688; 688 / 112 = 6 rows; capacity = 8*6 = 48.
    const auto d = computeGridDimensions(QSize(1280, 720), 5);
    EXPECT_EQ(d.rows, 6);
    EXPECT_EQ(d.shown, 5); // Below capacity: all shown.
}

TEST(DesktopIconLayerGrid, TruncatesWhenOverCapacity) {
    const auto d = computeGridDimensions(QSize(1280, 720), 100);
    EXPECT_EQ(d.columns, 11);
    EXPECT_EQ(d.rows, 6);
    EXPECT_EQ(d.shown, 66); // 11*6, not 100.
}

TEST(DesktopIconLayerGrid, NarrowWidthFloorOneColumn) {
    // usable_w = 200 - 48 + 16 = 168; 168 / 112 = 1.
    const auto d = computeGridDimensions(QSize(200, 720), 3);
    EXPECT_EQ(d.columns, 1);
}

TEST(DesktopIconLayerGrid, ExtremelyNarrowStillOneColumn) {
    // usable_w = 50 - 48 + 16 = 18; 18 / 112 = 0 -> floored to 1.
    const auto d = computeGridDimensions(QSize(50, 720), 3);
    EXPECT_EQ(d.columns, 1);
}

TEST(DesktopIconLayerGrid, ZeroHeightShowsNothing) {
    // usable_h = 50 - 48 + 16 = 18; 18 / 112 = 0 rows; capacity 0.
    const auto d = computeGridDimensions(QSize(1280, 50), 10);
    EXPECT_EQ(d.rows, 0);
    EXPECT_EQ(d.shown, 0);
}

TEST(DesktopIconLayerGrid, SingleAppShown) {
    const auto d = computeGridDimensions(QSize(1280, 720), 1);
    EXPECT_EQ(d.shown, 1);
}
