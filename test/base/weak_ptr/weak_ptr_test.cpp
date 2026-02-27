/**
 * @file weak_ptr_test.cpp
 * @brief Comprehensive unit tests for cf::WeakPtr using GoogleTest
 *
 * Test Coverage:
 * 1. Default Construction
 * 2. Validity While Owner Alive
 * 3. Invalidation After Owner Destroyed
 * 4. Multiple WeakPtrs
 * 5. Manual Invalidation
 * 6. HasWeakPtrs Tracking
 * 7. Mutation Through WeakPtr
 * 8. Copy and Move Semantics
 * 9. Reset Operation
 * 10. Covariant Conversion (Derived to Base)
 * 11. Integration Patterns
 */

#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include <gtest/gtest.h>
#include <string>

using namespace cf;

// =============================================================================
// Test Helper Classes
// =============================================================================

class Resource {
  public:
    explicit Resource(std::string name) : name_(std::move(name)) {}

    const std::string& name() const { return name_; }
    void Rename(std::string s) { name_ = std::move(s); }

    WeakPtr<Resource> GetWeakPtr() { return weak_factory_.GetWeakPtr(); }
    bool HasWeakPtrs() const { return weak_factory_.HasWeakPtrs(); }
    void InvalidateWeakPtrs() { weak_factory_.InvalidateWeakPtrs(); }

  private:
    std::string name_;
    WeakPtrFactory<Resource> weak_factory_{this}; // Must be last member
};

// Base/Derived classes for covariant tests
struct Base {
    virtual ~Base() = default;
    int x = 1;
};

struct Derived : Base {
    int y = 2;
};

// =============================================================================
// Test Suite 1: Default Construction
// =============================================================================

TEST(WeakPtrTest, DefaultConstructedWeakPtrIsInvalid) {
    WeakPtr<Resource> wp;
    EXPECT_FALSE(wp.IsValid());
    EXPECT_FALSE(static_cast<bool>(wp));
    EXPECT_EQ(wp.Get(), nullptr);
    EXPECT_EQ(wp, nullptr);
}

// =============================================================================
// Test Suite 2: Validity While Owner Alive
// =============================================================================

TEST(WeakPtrTest, WeakPtrIsValidWhileOwnerAlive) {
    Resource res("theme");
    WeakPtr<Resource> wp = res.GetWeakPtr();

    EXPECT_TRUE(wp.IsValid());
    EXPECT_TRUE(static_cast<bool>(wp));
    EXPECT_EQ(wp.Get(), &res);
    EXPECT_EQ(wp->name(), "theme");
    EXPECT_EQ((*wp).name(), "theme");
}

// =============================================================================
// Test Suite 3: Invalidation After Owner Destroyed
// =============================================================================

TEST(WeakPtrTest, WeakPtrInvalidatedAfterOwnerDestroyed) {
    WeakPtr<Resource> wp;
    {
        Resource res("temp");
        wp = res.GetWeakPtr();
        EXPECT_TRUE(wp.IsValid());
    } // res destroyed
    EXPECT_FALSE(wp.IsValid());
    EXPECT_EQ(wp.Get(), nullptr);
}

// =============================================================================
// Test Suite 4: Multiple WeakPtrs
// =============================================================================

TEST(WeakPtrTest, MultipleWeakPtrsAllInvalidatedTogether) {
    WeakPtr<Resource> wp1, wp2, wp3;
    {
        Resource res("multi");
        wp1 = res.GetWeakPtr();
        wp2 = res.GetWeakPtr();
        wp3 = wp1; // Copy
        EXPECT_TRUE(wp1 && wp2 && wp3);
    }
    EXPECT_FALSE(wp1.IsValid());
    EXPECT_FALSE(wp2.IsValid());
    EXPECT_FALSE(wp3.IsValid());
}

// =============================================================================
// Test Suite 5: Manual Invalidation
// =============================================================================

TEST(WeakPtrTest, ManualInvalidateWeakPtrs) {
    Resource res("manual");
    WeakPtr<Resource> wp = res.GetWeakPtr();
    EXPECT_TRUE(wp.IsValid());

    res.InvalidateWeakPtrs();
    EXPECT_FALSE(wp.IsValid()); // Old references invalidated

    // Owner still alive, can get new valid WeakPtr
    WeakPtr<Resource> wp2 = res.GetWeakPtr();
    EXPECT_TRUE(wp2.IsValid());
}

// =============================================================================
// Test Suite 6: HasWeakPtrs Tracking
// =============================================================================

TEST(WeakPtrTest, HasWeakPtrsTracksOutstandingRefs) {
    Resource res("tracker");
    EXPECT_FALSE(res.HasWeakPtrs());

    {
        WeakPtr<Resource> wp = res.GetWeakPtr();
        EXPECT_TRUE(res.HasWeakPtrs());
    } // wp destroyed

    EXPECT_FALSE(res.HasWeakPtrs());
}

// =============================================================================
// Test Suite 7: Mutation Through WeakPtr
// =============================================================================

TEST(WeakPtrTest, WeakPtrCanMutateThroughOwner) {
    Resource res("old");
    WeakPtr<Resource> wp = res.GetWeakPtr();

    wp->Rename("new");
    EXPECT_EQ(res.name(), "new");
}

// =============================================================================
// Test Suite 8: Copy and Move Semantics
// =============================================================================

TEST(WeakPtrTest, WeakPtrCopyAndMove) {
    Resource res("copy_move");
    WeakPtr<Resource> wp1 = res.GetWeakPtr();

    // Copy
    WeakPtr<Resource> wp2 = wp1;
    EXPECT_TRUE(wp2.IsValid());
    EXPECT_EQ(wp2.Get(), &res);

    // Move
    WeakPtr<Resource> wp3 = std::move(wp2);
    EXPECT_TRUE(wp3.IsValid());
    // wp2 after move is null (implementation detail)
}

// =============================================================================
// Test Suite 9: Reset Operation
// =============================================================================

TEST(WeakPtrTest, WeakPtrResetMakesItNull) {
    Resource res("reset_test");
    WeakPtr<Resource> wp = res.GetWeakPtr();
    EXPECT_TRUE(wp.IsValid());

    wp.Reset();
    EXPECT_FALSE(wp.IsValid());
    EXPECT_EQ(wp.Get(), nullptr);
}

// =============================================================================
// Test Suite 10: Covariant Conversion
// =============================================================================

TEST(WeakPtrTest, CovariantConversionDerivedToBase) {
    Derived d;
    WeakPtrFactory<Derived> factory(&d);

    WeakPtr<Derived> derived_wp = factory.GetWeakPtr();
    WeakPtr<Base> base_wp = derived_wp; // Implicit upcast

    EXPECT_TRUE(base_wp.IsValid());
    EXPECT_EQ(base_wp->x, 1);
}

TEST(WeakPtrTest, CovariantInvalidationFollowsOwner) {
    WeakPtr<Base> base_wp;
    {
        Derived d;
        WeakPtrFactory<Derived> factory(&d);
        WeakPtr<Derived> derived_wp = factory.GetWeakPtr();
        base_wp = derived_wp;
        EXPECT_TRUE(base_wp.IsValid());
    }
    EXPECT_FALSE(base_wp.IsValid());
}

// =============================================================================
// Test Suite 11: Integration Patterns
// =============================================================================

class ThemeManager {
  public:
    static ThemeManager& Instance() {
        static ThemeManager inst;
        return inst;
    }

    WeakPtr<ThemeManager> GetWeakPtr() { return weak_factory_.GetWeakPtr(); }

    std::string current_theme = "light";

    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

  private:
    ThemeManager() = default;
    WeakPtrFactory<ThemeManager> weak_factory_{this};
};

class ToolBar {
  public:
    explicit ToolBar(WeakPtr<ThemeManager> tm) : theme_(std::move(tm)) {}

    std::string Render() {
        if (!theme_) {
            return "[ToolBar] ThemeManager gone, skip render.";
        }
        return "[ToolBar] Rendering with theme: " + theme_->current_theme;
    }

  private:
    WeakPtr<ThemeManager> theme_;
};

TEST(WeakPtrTest, ThemeManagerIntegration) {
    auto& tm = ThemeManager::Instance();
    ToolBar tb(tm.GetWeakPtr());

    EXPECT_EQ(tb.Render(), "[ToolBar] Rendering with theme: light");
    tm.current_theme = "dark";
    EXPECT_EQ(tb.Render(), "[ToolBar] Rendering with theme: dark");

    // Reset local copy only
    tm.GetWeakPtr().Reset();
    EXPECT_EQ(tb.Render(), "[ToolBar] Rendering with theme: dark");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
