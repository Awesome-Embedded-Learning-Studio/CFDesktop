/**
 * @file    test/base/indexed_vector/indexed_vector_test.cpp
 * @brief   Unit tests for cf::indexed_vector.
 *
 * @author  Charliechen114514
 * @date    2026-04-09
 * @version 0.1
 * @since   0.1
 */

#include <gtest/gtest.h>

#include <base/indexed_vector/indexed_vector.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// =============================================================================
// Test Suite 1: Construction
// =============================================================================

TEST(IndexedVectorConstruction, DefaultConstructor) {
    cf::indexed_vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.cursor(), 0u);
    EXPECT_EQ(v.mode(), cf::IndexingMode::Listed);
}

TEST(IndexedVectorConstruction, CountConstructor) {
    cf::indexed_vector<int> v(5);
    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v.cursor(), 0u);
}

TEST(IndexedVectorConstruction, CountValueConstructor) {
    cf::indexed_vector<int> v(3, 42);
    EXPECT_EQ(v.size(), 3u);
    for (auto& elem : v) {
        EXPECT_EQ(elem, 42);
    }
}

TEST(IndexedVectorConstruction, InitializerListConstructor) {
    cf::indexed_vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
    EXPECT_EQ(v.cursor(), 0u);
}

TEST(IndexedVectorConstruction, CopyConstructor) {
    cf::indexed_vector<int> original{10, 20, 30};
    original.set_cursor(2);
    original.set_mode(cf::IndexingMode::Recycled);

    cf::indexed_vector<int> copy(original);
    EXPECT_EQ(copy.size(), 3u);
    EXPECT_EQ(copy.cursor(), 2u);
    EXPECT_EQ(copy.mode(), cf::IndexingMode::Recycled);
    EXPECT_EQ(copy[1], 20);
}

TEST(IndexedVectorConstruction, MoveConstructor) {
    cf::indexed_vector<int> original{10, 20, 30};
    original.set_cursor(1);

    cf::indexed_vector<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_EQ(moved.cursor(), 1u);
    EXPECT_EQ(moved[0], 10);
}

// =============================================================================
// Test Suite 2: Cursor Access
// =============================================================================

TEST(IndexedVectorCursorAccess, CursorDefaultZero) {
    cf::indexed_vector<int> v{1, 2, 3};
    EXPECT_EQ(v.cursor(), 0u);
}

TEST(IndexedVectorCursorAccess, SetCursorValid) {
    cf::indexed_vector<int> v{1, 2, 3, 4, 5};
    v.set_cursor(0);
    EXPECT_EQ(v.cursor(), 0u);
    v.set_cursor(4);
    EXPECT_EQ(v.cursor(), 4u);
    v.set_cursor(2);
    EXPECT_EQ(v.cursor(), 2u);
}

TEST(IndexedVectorCursorAccess, SetCursorOutOfRange) {
    cf::indexed_vector<int> v{1, 2, 3};
    EXPECT_THROW(v.set_cursor(3), std::out_of_range);
    EXPECT_THROW(v.set_cursor(100), std::out_of_range);
}

TEST(IndexedVectorCursorAccess, AtCursorAccess) {
    cf::indexed_vector<int> v{10, 20, 30};
    v.set_cursor(1);
    EXPECT_EQ(v.at_cursor(), 20);
    v.at_cursor() = 99;
    EXPECT_EQ(v[1], 99);
}

TEST(IndexedVectorCursorAccess, AtCursorConst) {
    const cf::indexed_vector<int> v{10, 20, 30};
    // cursor is 0 by default
    EXPECT_EQ(v.at_cursor(), 10);
}

TEST(IndexedVectorCursorAccess, AtCursorEmptyThrows) {
    cf::indexed_vector<int> v;
    EXPECT_THROW(v.at_cursor(), std::out_of_range);
}

// =============================================================================
// Test Suite 3: Fixed Mode Behavior
// =============================================================================

TEST(IndexedVectorFixedMode, NextNoOp) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Fixed);
    v.set_cursor(1);
    v.next();
    EXPECT_EQ(v.cursor(), 1u);
}

TEST(IndexedVectorFixedMode, PrevNoOp) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Fixed);
    v.set_cursor(1);
    v.prev();
    EXPECT_EQ(v.cursor(), 1u);
}

TEST(IndexedVectorFixedMode, HasNextHasPrevFalse) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Fixed);
    v.set_cursor(1);
    EXPECT_FALSE(v.has_next());
    EXPECT_FALSE(v.has_prev());
}

TEST(IndexedVectorFixedMode, SetCursorStillWorks) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Fixed);
    v.set_cursor(2);
    EXPECT_EQ(v.at_cursor(), 3);
}

// =============================================================================
// Test Suite 4: Listed Mode Behavior
// =============================================================================

TEST(IndexedVectorListedMode, NextAdvances) {
    cf::indexed_vector<int> v{1, 2, 3};
    EXPECT_EQ(v.cursor(), 0u);
    v.next();
    EXPECT_EQ(v.cursor(), 1u);
    v.next();
    EXPECT_EQ(v.cursor(), 2u);
}

TEST(IndexedVectorListedMode, PrevRetreats) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(2);
    v.prev();
    EXPECT_EQ(v.cursor(), 1u);
    v.prev();
    EXPECT_EQ(v.cursor(), 0u);
}

TEST(IndexedVectorListedMode, NextAtEndThrows) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(2);
    EXPECT_THROW(v.next(), std::out_of_range);
}

TEST(IndexedVectorListedMode, PrevAtStartThrows) {
    cf::indexed_vector<int> v{1, 2, 3};
    EXPECT_THROW(v.prev(), std::out_of_range);
}

TEST(IndexedVectorListedMode, HasNextHasPrev) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(1);
    EXPECT_TRUE(v.has_next());
    EXPECT_TRUE(v.has_prev());

    v.set_cursor(0);
    EXPECT_TRUE(v.has_next());
    EXPECT_FALSE(v.has_prev());

    v.set_cursor(2);
    EXPECT_FALSE(v.has_next());
    EXPECT_TRUE(v.has_prev());
}

TEST(IndexedVectorListedMode, TraversalFullRange) {
    cf::indexed_vector<int> v{10, 20, 30};
    EXPECT_EQ(v.at_cursor(), 10);
    v.next();
    EXPECT_EQ(v.at_cursor(), 20);
    v.next();
    EXPECT_EQ(v.at_cursor(), 30);
    v.prev();
    EXPECT_EQ(v.at_cursor(), 20);
    v.prev();
    EXPECT_EQ(v.at_cursor(), 10);
}

// =============================================================================
// Test Suite 5: Recycled Mode Behavior
// =============================================================================

TEST(IndexedVectorRecycledMode, NextWraps) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Recycled);
    v.set_cursor(2);
    v.next();
    EXPECT_EQ(v.cursor(), 0u);
}

TEST(IndexedVectorRecycledMode, PrevWraps) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Recycled);
    v.prev();
    EXPECT_EQ(v.cursor(), 2u);
}

TEST(IndexedVectorRecycledMode, SingleElementRecycled) {
    cf::indexed_vector<int> v{42};
    v.set_mode(cf::IndexingMode::Recycled);
    v.next();
    EXPECT_EQ(v.cursor(), 0u);
    EXPECT_EQ(v.at_cursor(), 42);
    v.prev();
    EXPECT_EQ(v.cursor(), 0u);
    EXPECT_EQ(v.at_cursor(), 42);
}

TEST(IndexedVectorRecycledMode, RecycledEmptyThrows) {
    cf::indexed_vector<int> v;
    v.set_mode(cf::IndexingMode::Recycled);
    EXPECT_THROW(v.next(), std::out_of_range);
    EXPECT_THROW(v.prev(), std::out_of_range);
}

TEST(IndexedVectorRecycledMode, HasNextHasPrevAlwaysTrue) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_mode(cf::IndexingMode::Recycled);
    v.set_cursor(0);
    EXPECT_TRUE(v.has_next());
    EXPECT_TRUE(v.has_prev());
    v.set_cursor(2);
    EXPECT_TRUE(v.has_next());
    EXPECT_TRUE(v.has_prev());
}

// =============================================================================
// Test Suite 6: Mode Switching
// =============================================================================

TEST(IndexedVectorModeSwitching, SetModeChangesBehavior) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(2);

    // Listed: next throws
    EXPECT_THROW(v.next(), std::out_of_range);

    // Switch to Recycled: next wraps
    v.set_mode(cf::IndexingMode::Recycled);
    v.next();
    EXPECT_EQ(v.cursor(), 0u);
}

TEST(IndexedVectorModeSwitching, ModeReturnsCurrent) {
    cf::indexed_vector<int> v;
    EXPECT_EQ(v.mode(), cf::IndexingMode::Listed);
    v.set_mode(cf::IndexingMode::Recycled);
    EXPECT_EQ(v.mode(), cf::IndexingMode::Recycled);
    v.set_mode(cf::IndexingMode::Fixed);
    EXPECT_EQ(v.mode(), cf::IndexingMode::Fixed);
}

TEST(IndexedVectorModeSwitching, SetModePreservesCursor) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(2);
    v.set_mode(cf::IndexingMode::Recycled);
    EXPECT_EQ(v.cursor(), 2u);
}

TEST(IndexedVectorModeSwitching, CompileTimeDefault) {
    cf::indexed_vector<int> v_default;
    EXPECT_EQ(v_default.mode(), cf::IndexingMode::Listed);

    cf::indexed_vector<int, cf::IndexingMode::Recycled> v_recycled;
    EXPECT_EQ(v_recycled.mode(), cf::IndexingMode::Recycled);

    cf::indexed_vector<int, cf::IndexingMode::Fixed> v_fixed;
    EXPECT_EQ(v_fixed.mode(), cf::IndexingMode::Fixed);
}

// =============================================================================
// Test Suite 7: Cursor on Erase
// =============================================================================

TEST(IndexedVectorCursorOnErase, EraseBeforeCursor) {
    cf::indexed_vector<int> v{10, 20, 30, 40};
    v.set_cursor(3);
    v.erase(v.cbegin() + 1);   // erase 20
    EXPECT_EQ(v.cursor(), 2u); // shifted from 3 to 2
    EXPECT_EQ(v.at_cursor(), 40);
}

TEST(IndexedVectorCursorOnErase, EraseAfterCursor) {
    cf::indexed_vector<int> v{10, 20, 30, 40};
    v.set_cursor(1);
    v.erase(v.cbegin() + 3); // erase 40
    EXPECT_EQ(v.cursor(), 1u);
    EXPECT_EQ(v.at_cursor(), 20);
}

TEST(IndexedVectorCursorOnErase, EraseAtCursor) {
    cf::indexed_vector<int> v{10, 20, 30, 40};
    v.set_cursor(1);
    v.erase(v.cbegin() + 1);   // erase 20
    EXPECT_EQ(v.cursor(), 1u); // stays at index 1, now points to 30
    EXPECT_EQ(v.at_cursor(), 30);
}

TEST(IndexedVectorCursorOnErase, EraseLastElementAtCursor) {
    cf::indexed_vector<int> v{10, 20, 30};
    v.set_cursor(2);
    v.erase(v.cbegin() + 2);   // erase 30
    EXPECT_EQ(v.cursor(), 1u); // clamped back
    EXPECT_EQ(v.at_cursor(), 20);
}

TEST(IndexedVectorCursorOnErase, EraseRangeBeforeCursor) {
    cf::indexed_vector<int> v{10, 20, 30, 40, 50};
    v.set_cursor(4);
    v.erase(v.cbegin() + 1, v.cbegin() + 3); // erase 20, 30
    EXPECT_EQ(v.cursor(), 2u);               // 4 - 2 = 2
    EXPECT_EQ(v.at_cursor(), 50);
}

TEST(IndexedVectorCursorOnErase, EraseRangeIncludingCursor) {
    cf::indexed_vector<int> v{10, 20, 30, 40, 50};
    v.set_cursor(2);
    v.erase(v.cbegin() + 1, v.cbegin() + 4); // erase 20, 30, 40
    EXPECT_EQ(v.cursor(), 1u);               // set to erase start, clamped
    EXPECT_EQ(v.at_cursor(), 50);
}

TEST(IndexedVectorCursorOnErase, PopBackAtCursor) {
    cf::indexed_vector<int> v{10, 20, 30};
    v.set_cursor(2);
    v.pop_back();
    EXPECT_EQ(v.cursor(), 1u);
    EXPECT_EQ(v.at_cursor(), 20);
}

TEST(IndexedVectorCursorOnErase, PopBackNotAtCursor) {
    cf::indexed_vector<int> v{10, 20, 30};
    v.set_cursor(0);
    v.pop_back();
    EXPECT_EQ(v.cursor(), 0u);
    EXPECT_EQ(v.at_cursor(), 10);
}

TEST(IndexedVectorCursorOnErase, PopBackLastElement) {
    cf::indexed_vector<int> v{42};
    v.pop_back();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.cursor(), 0u);
}

// =============================================================================
// Test Suite 8: Cursor on Insert
// =============================================================================

TEST(IndexedVectorCursorOnInsert, InsertBeforeCursor) {
    cf::indexed_vector<int> v{10, 30, 40};
    v.set_cursor(2);
    v.insert(v.cbegin() + 1, 20); // insert before cursor
    EXPECT_EQ(v.cursor(), 3u);
    EXPECT_EQ(v.at_cursor(), 40);
}

TEST(IndexedVectorCursorOnInsert, InsertAfterCursor) {
    cf::indexed_vector<int> v{10, 20, 40};
    v.set_cursor(1);
    v.insert(v.cbegin() + 3, 30); // insert after cursor
    EXPECT_EQ(v.cursor(), 1u);
    EXPECT_EQ(v.at_cursor(), 20);
}

TEST(IndexedVectorCursorOnInsert, PushBackNoEffect) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(1);
    v.push_back(4);
    EXPECT_EQ(v.cursor(), 1u);
    EXPECT_EQ(v.at_cursor(), 2);
}

TEST(IndexedVectorCursorOnInsert, EmplaceBackNoEffect) {
    cf::indexed_vector<std::string> v{"a", "b", "c"};
    v.set_cursor(1);
    v.emplace_back("d");
    EXPECT_EQ(v.cursor(), 1u);
    EXPECT_EQ(v.at_cursor(), "b");
}

// =============================================================================
// Test Suite 9: Cursor on Clear and Resize
// =============================================================================

TEST(IndexedVectorCursorClearResize, ClearResetsCursor) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(2);
    v.clear();
    EXPECT_EQ(v.cursor(), 0u);
    EXPECT_TRUE(v.empty());
}

TEST(IndexedVectorCursorClearResize, ResizeSmallerAdjustsCursor) {
    cf::indexed_vector<int> v{1, 2, 3, 4, 5};
    v.set_cursor(4);
    v.resize(3);
    EXPECT_EQ(v.cursor(), 2u); // clamped to new_size - 1
}

TEST(IndexedVectorCursorClearResize, ResizeLargerNoEffect) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(1);
    v.resize(10);
    EXPECT_EQ(v.cursor(), 1u);
}

TEST(IndexedVectorCursorClearResize, ResizeToZeroResetsCursor) {
    cf::indexed_vector<int> v{1, 2, 3};
    v.set_cursor(2);
    v.resize(0);
    EXPECT_EQ(v.cursor(), 0u);
    EXPECT_TRUE(v.empty());
}

// =============================================================================
// Test Suite 10: Empty Vector Edge Cases
// =============================================================================

TEST(IndexedVectorEmptyVector, EmptyVectorNextThrows) {
    cf::indexed_vector<int> v;
    EXPECT_THROW(v.next(), std::out_of_range);
}

TEST(IndexedVectorEmptyVector, EmptyVectorPrevThrows) {
    cf::indexed_vector<int> v;
    EXPECT_THROW(v.prev(), std::out_of_range);
}

TEST(IndexedVectorEmptyVector, EmptyVectorAtCursorThrows) {
    cf::indexed_vector<int> v;
    EXPECT_THROW(v.at_cursor(), std::out_of_range);
}

TEST(IndexedVectorEmptyVector, EmptyVectorSetCursorThrows) {
    cf::indexed_vector<int> v;
    EXPECT_THROW(v.set_cursor(0), std::out_of_range);
}

TEST(IndexedVectorEmptyVector, EmptyVectorHasNextPrev) {
    cf::indexed_vector<int> v;
    EXPECT_FALSE(v.has_next());
    EXPECT_FALSE(v.has_prev());
}

// =============================================================================
// Test Suite 11: Vector API Passthrough
// =============================================================================

TEST(IndexedVectorPassthrough, OperatorBracket) {
    cf::indexed_vector<int> v{10, 20, 30};
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[2], 30);
    v[1] = 99;
    EXPECT_EQ(v[1], 99);
}

TEST(IndexedVectorPassthrough, AtMethod) {
    cf::indexed_vector<int> v{10, 20, 30};
    EXPECT_EQ(v.at(1), 20);
    EXPECT_THROW(v.at(3), std::out_of_range);
}

TEST(IndexedVectorPassthrough, FrontBack) {
    cf::indexed_vector<int> v{10, 20, 30};
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.back(), 30);
}

TEST(IndexedVectorPassthrough, IteratorRange) {
    cf::indexed_vector<int> v{1, 2, 3};
    std::vector<int> copy(v.begin(), v.end());
    EXPECT_EQ(copy.size(), 3u);
    EXPECT_EQ(copy[0], 1);
    EXPECT_EQ(copy[2], 3);
}

TEST(IndexedVectorPassthrough, CapacityMethods) {
    cf::indexed_vector<int> v;
    EXPECT_TRUE(v.empty());
    v.push_back(1);
    EXPECT_EQ(v.size(), 1u);
    v.reserve(100);
    EXPECT_GE(v.capacity(), 100u);
    EXPECT_EQ(v.max_size(), v.max_size()); // just verify it compiles
}

TEST(IndexedVectorPassthrough, Swap) {
    cf::indexed_vector<int> a{1, 2};
    a.set_cursor(1);
    cf::indexed_vector<int> b{3, 4, 5};
    b.set_cursor(2);
    b.set_mode(cf::IndexingMode::Recycled);

    a.swap(b);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a.cursor(), 2u);
    EXPECT_EQ(a.mode(), cf::IndexingMode::Recycled);
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.cursor(), 1u);
}

// =============================================================================
// Test Suite 12: Comparison
// =============================================================================

TEST(IndexedVectorComparison, EqualWhenSame) {
    cf::indexed_vector<int> a{1, 2, 3};
    cf::indexed_vector<int> b{1, 2, 3};
    EXPECT_EQ(a, b);
}

TEST(IndexedVectorComparison, NotEqualDifferentCursor) {
    cf::indexed_vector<int> a{1, 2, 3};
    cf::indexed_vector<int> b{1, 2, 3};
    b.set_cursor(2);
    EXPECT_NE(a, b);
}

TEST(IndexedVectorComparison, NotEqualDifferentContent) {
    cf::indexed_vector<int> a{1, 2, 3};
    cf::indexed_vector<int> b{1, 2, 4};
    EXPECT_NE(a, b);
}

TEST(IndexedVectorComparison, NotEqualDifferentMode) {
    cf::indexed_vector<int> a{1, 2, 3};
    cf::indexed_vector<int> b{1, 2, 3};
    b.set_mode(cf::IndexingMode::Recycled);
    EXPECT_NE(a, b);
}

// =============================================================================
// Test Suite 13: Full navigation scenario
// =============================================================================

TEST(IndexedVectorScenario, PlaylistNavigation) {
    cf::indexed_vector<std::string> playlist{"song_a", "song_b", "song_c", "song_d"};

    // Start at first song
    EXPECT_EQ(playlist.at_cursor(), "song_a");

    // Skip to next
    playlist.next();
    EXPECT_EQ(playlist.at_cursor(), "song_b");

    // Skip to last
    playlist.next();
    playlist.next();
    EXPECT_EQ(playlist.at_cursor(), "song_d");
    EXPECT_FALSE(playlist.has_next());

    // Enable loop mode
    playlist.set_mode(cf::IndexingMode::Recycled);
    playlist.next();
    EXPECT_EQ(playlist.at_cursor(), "song_a");

    // Go backwards wraps to last
    playlist.prev();
    EXPECT_EQ(playlist.at_cursor(), "song_d");
}

TEST(IndexedVectorScenario, ImageBrowserWithDeletion) {
    cf::indexed_vector<int> images{100, 200, 300, 400};
    images.set_cursor(1); // viewing image 200

    // Delete current image
    images.erase(images.cbegin() + 1);
    EXPECT_EQ(images.at_cursor(), 300); // now viewing 300

    // Delete last image while cursor is on last
    images.set_cursor(2); // viewing 400
    images.pop_back();
    EXPECT_EQ(images.at_cursor(), 300); // cursor backed up
}
