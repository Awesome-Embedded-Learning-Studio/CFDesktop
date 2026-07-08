/**
 * @file    test/desktop/desktop_icon_layer/desktop_icon_layer_test.cpp
 * @brief   GoogleTest unit tests for computeGridDimensions.
 *
 * Exercises the pure grid-sizing math (column floor, row capacity, adaptive
 * tile scaling, floor-and-truncate) without instantiating a QWidget. Layout
 * constants mirror the anonymous namespace in desktop_icon_layer.cpp:
 * kCellMax=96, kCellMin=48, kGridSpacing=16, kMargin=24 (at cell=96 the stride
 * is 96+16=112; usable = side - 2*24 + 16 = side - 32). When app_count exceeds
 * the cell=96 capacity, computeGridDimensions scans cell from 96 down to 48 and
 * picks the largest edge whose columns*rows still fits, truncating only when
 * even cell=48 cannot fit everything.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-07
 * @version 0.2
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
    EXPECT_EQ(d.cell, 0);
}

TEST(DesktopIconLayerGrid, InvalidCountReturnsZero) {
    const auto d = computeGridDimensions(QSize(1280, 720), 0);
    EXPECT_EQ(d.shown, 0);
    EXPECT_EQ(d.cell, 0);
}

TEST(DesktopIconLayerGrid, NegativeCountReturnsZero) {
    const auto d = computeGridDimensions(QSize(1280, 720), -3);
    EXPECT_EQ(d.shown, 0);
    EXPECT_EQ(d.cell, 0);
}

TEST(DesktopIconLayerGrid, ColumnsFillWideWidth) {
    // usable_w = 1280 - 32 = 1248; at cell=96 stride=112 -> 1248/112 = 11 cols.
    // count=5 fits at cell=96, so no shrink.
    const auto d = computeGridDimensions(QSize(1280, 720), 5);
    EXPECT_EQ(d.columns, 11);
    EXPECT_EQ(d.cell, 96);
}

TEST(DesktopIconLayerGrid, RowsAndCapacityForTypicalDesktop) {
    // usable_h = 720 - 32 = 688; at cell=96 -> 688/112 = 6 rows.
    const auto d = computeGridDimensions(QSize(1280, 720), 5);
    EXPECT_EQ(d.rows, 6);
    EXPECT_EQ(d.shown, 5); // Below capacity: all shown.
    EXPECT_EQ(d.cell, 96);
}

TEST(DesktopIconLayerGrid, ScalesToFitWhenShrunk) {
    // 1280x720, count=100. At cell=96 capacity is 11*6=66 < 100, so the scan
    // shrinks the tile. cell=70 (stride 86): cols=floor(1248/86)=14,
    // rows=floor(688/86)=8, capacity 112 >= 100. cell=71 (stride 87) drops rows
    // to 7 -> 14*7=98 < 100, so 70 is the largest fitting edge. All 100 shown.
    const auto d = computeGridDimensions(QSize(1280, 720), 100);
    EXPECT_EQ(d.cell, 70);
    EXPECT_EQ(d.columns, 14);
    EXPECT_EQ(d.rows, 8);
    EXPECT_EQ(d.shown, 100);
}

TEST(DesktopIconLayerGrid, TruncatesOnlyAtFloor) {
    // 300x300, count=100. usable 268x268. Even cell=48 (stride 64) fits only
    // floor(268/64)=4 cols * 4 rows = 16 < 100, so the scan exhausts and cell
    // stays at the 48 floor; shown is honestly truncated to capacity.
    const auto d = computeGridDimensions(QSize(300, 300), 100);
    EXPECT_EQ(d.cell, 48);
    EXPECT_EQ(d.columns, 4);
    EXPECT_EQ(d.rows, 4);
    EXPECT_EQ(d.shown, 16);
}

TEST(DesktopIconLayerGrid, NarrowWidthFloorOneColumn) {
    // usable_w = 200 - 32 = 168; at cell=96 -> 168/112 = 1.
    const auto d = computeGridDimensions(QSize(200, 720), 3);
    EXPECT_EQ(d.columns, 1);
    EXPECT_EQ(d.cell, 96);
}

TEST(DesktopIconLayerGrid, ExtremelyNarrowStillOneColumn) {
    // usable_w = 50 - 32 = 18; at cell=96 -> 18/112 = 0 -> floored to 1.
    const auto d = computeGridDimensions(QSize(50, 720), 3);
    EXPECT_EQ(d.columns, 1);
    EXPECT_EQ(d.cell, 96);
}

TEST(DesktopIconLayerGrid, ZeroHeightReportsNoLayout) {
    // usable_h = 50 - 32 = 18; rows=0 at every cell -> capacity 0. cell reports
    // 0 (no valid layout), not the floor, since no tile can be placed.
    const auto d = computeGridDimensions(QSize(1280, 50), 10);
    EXPECT_EQ(d.rows, 0);
    EXPECT_EQ(d.shown, 0);
    EXPECT_EQ(d.cell, 0);
}

TEST(DesktopIconLayerGrid, SingleAppShown) {
    const auto d = computeGridDimensions(QSize(1280, 720), 1);
    EXPECT_EQ(d.shown, 1);
    EXPECT_EQ(d.cell, 96);
}
