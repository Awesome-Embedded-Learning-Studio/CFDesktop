/**
 * @file scope_guard_test.cpp
 * @brief Comprehensive unit tests for cf::ScopeGuard using GoogleTest
 *
 * Test Coverage:
 * 1. Basic Functionality - cleanup on scope exit
 * 2. dismiss() - canceling cleanup
 * 3. Non-copyable and Non-movable Guarantees
 * 4. Exception Safety
 * 5. Multiple Guards in Same Scope
 * 6. Move-only Capture in Lambda
 * 7. Order of Destruction (LIFO)
 * 8. Interaction with Control Flow (return, break, continue, throw)
 * 9. Empty/No-op Functions
 * 10. Complex Capture Scenarios
 * 11. Resource Management Patterns
 * 12. Edge Cases
 * 13. Real-world Usage Patterns
 */

#include "base/scope_guard/scope_guard.hpp"
#include <functional>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

// =============================================================================
// Test Suite 1: Basic Functionality
// =============================================================================

TEST(ScopeGuardTest, BasicCleanup) {
    int counter = 0;

    {
        cf::ScopeGuard guard([&counter]() { counter = 42; });
    }

    EXPECT_EQ(counter, 42);
}

TEST(ScopeGuardTest, CleanupExecutesOnce) {
    int counter = 0;

    {
        cf::ScopeGuard guard([&counter]() { counter++; });
    }

    EXPECT_EQ(counter, 1);
}

TEST(ScopeGuardTest, ModifiesCapturedVariable) {
    std::string result = "initial";

    {
        cf::ScopeGuard guard([&result]() { result = "cleaned"; });
    }

    EXPECT_EQ(result, "cleaned");
}

TEST(ScopeGuardTest, MultipleActionsInLambda) {
    int a = 0, b = 0, c = 0;

    {
        cf::ScopeGuard guard([&a, &b, &c]() {
            a = 1;
            b = 2;
            c = 3;
        });
    }

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    EXPECT_EQ(c, 3);
}

// =============================================================================
// Test Suite 2: dismiss() Functionality
// =============================================================================

TEST(ScopeGuardTest, DismissPreventsCleanup) {
    int counter = 0;

    {
        cf::ScopeGuard guard([&counter]() { counter = 42; });
        guard.dismiss();
    }

    EXPECT_EQ(counter, 0);
}

TEST(ScopeGuardTest, DismissMultipleTimes) {
    int counter = 0;

    {
        cf::ScopeGuard guard([&counter]() { counter = 42; });
        guard.dismiss();
        guard.dismiss(); // Safe to call multiple times
    }

    EXPECT_EQ(counter, 0);
}

TEST(ScopeGuardTest, DismissConditional) {
    int counter = 0;
    bool condition = true;

    {
        cf::ScopeGuard guard([&counter]() { counter = 42; });
        if (condition) {
            guard.dismiss();
        }
    }

    EXPECT_EQ(counter, 0);
}

// =============================================================================
// Test Suite 3: Non-copyable and Non-movable Guarantees
// =============================================================================

TEST(ScopeGuardTest, IsNotCopyConstructible) {
    // Compile-time test: ScopeGuard should not be copy constructible
    EXPECT_FALSE((std::is_copy_constructible_v<cf::ScopeGuard>));
    EXPECT_FALSE((std::is_copy_assignable_v<cf::ScopeGuard>));
}

// =============================================================================
// Test Suite 4: Exception Safety
// =============================================================================

TEST(ScopeGuardTest, CleanupWhenExceptionThrown) {
    int counter = 0;

    try {
        cf::ScopeGuard guard([&counter]() { counter = 42; });
        throw std::runtime_error("test exception");
    } catch (...) {
        // Exception caught
    }

    EXPECT_EQ(counter, 42);
}

TEST(ScopeGuardTest, CleanupFunctionThrows) {
    // When cleanup throws, it should propagate
    EXPECT_THROW({ cf::ScopeGuard guard([]() { throw std::runtime_error("cleanup error"); }); },
                 std::runtime_error);
}

TEST(ScopeGuardTest, ExceptionInGuardedCode) {
    int cleanup_called = 0;

    try {
        cf::ScopeGuard guard([&cleanup_called]() { cleanup_called++; });
        throw std::runtime_error("error");
    } catch (...) {
        // Exception from the guarded code
    }

    EXPECT_EQ(cleanup_called, 1);
}

// =============================================================================
// Test Suite 5: Multiple Guards in Same Scope
// =============================================================================

TEST(ScopeGuardTest, MultipleGuardsAllExecute) {
    int counter1 = 0, counter2 = 0, counter3 = 0;

    {
        cf::ScopeGuard guard1([&counter1]() { counter1 = 1; });
        cf::ScopeGuard guard2([&counter2]() { counter2 = 2; });
        cf::ScopeGuard guard3([&counter3]() { counter3 = 3; });
    }

    EXPECT_EQ(counter1, 1);
    EXPECT_EQ(counter2, 2);
    EXPECT_EQ(counter3, 3);
}

TEST(ScopeGuardTest, MultipleGuardsWithSomeDismissed) {
    int counter1 = 0, counter2 = 0, counter3 = 0;

    {
        cf::ScopeGuard guard1([&counter1]() { counter1 = 1; });
        cf::ScopeGuard guard2([&counter2]() { counter2 = 2; });
        cf::ScopeGuard guard3([&counter3]() { counter3 = 3; });

        guard2.dismiss();
    }

    EXPECT_EQ(counter1, 1);
    EXPECT_EQ(counter2, 0);
    EXPECT_EQ(counter3, 3);
}

// =============================================================================
// Test Suite 6: Move-only Capture in Lambda
// =============================================================================
// NOTE: Current ScopeGuard implementation uses std::function which requires
// copyable callables. Move-only captures like unique_ptr by value are not
// supported. Capturing by reference works fine.

TEST(ScopeGuardTest, CaptureUniquePtrByReference) {
    auto ptr = std::make_unique<int>(42);
    int result = 0;

    {
        cf::ScopeGuard guard([&ptr, &result]() {
            if (ptr)
                result = *ptr;
        });
    }

    EXPECT_EQ(result, 42);
}

// =============================================================================
// Test Suite 7: Order of Destruction (LIFO)
// =============================================================================

TEST(ScopeGuardTest, LIFODestructionOrder) {
    std::vector<int> order;

    {
        cf::ScopeGuard guard1([&order]() { order.push_back(1); });
        cf::ScopeGuard guard2([&order]() { order.push_back(2); });
        cf::ScopeGuard guard3([&order]() { order.push_back(3); });
    }

    ASSERT_EQ(order.size(), 3);
    EXPECT_EQ(order[0], 3); // Last created, first destroyed
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 1); // First created, last destroyed
}

TEST(ScopeGuardTest, NestedScopesOrder) {
    std::vector<int> order;

    {
        cf::ScopeGuard outer1([&order]() { order.push_back(1); });

        {
            cf::ScopeGuard inner([&order]() { order.push_back(2); });
        } // inner executes here

        cf::ScopeGuard outer2([&order]() { order.push_back(3); });
    } // outer2 executes, then outer1

    ASSERT_EQ(order.size(), 3);
    EXPECT_EQ(order[0], 2); // Inner scope guard executes first
    EXPECT_EQ(order[1], 3);
    EXPECT_EQ(order[2], 1);
}

// =============================================================================
// Test Suite 8: Interaction with Control Flow
// =============================================================================

TEST(ScopeGuardTest, WithEarlyReturn) {
    int counter = 0;

    auto func = [&counter]() -> void {
        cf::ScopeGuard guard([&counter]() { counter++; });
        return; // Early return
    };

    func();

    EXPECT_EQ(counter, 1);
}

TEST(ScopeGuardTest, WithMultipleReturns) {
    int counter = 0;

    auto func = [&counter](bool condition) -> void {
        cf::ScopeGuard guard([&counter]() { counter++; });
        if (condition) {
            return;
        }
        counter = 100; // Won't execute if condition is true
    };

    func(true);
    EXPECT_EQ(counter, 1);

    counter = 0;
    func(false);
    EXPECT_EQ(counter, 101); // 100 from code + 1 from cleanup
}

TEST(ScopeGuardTest, WithBreakStatement) {
    int counter = 0;

    for (int i = 0; i < 10; ++i) {
        cf::ScopeGuard guard([&counter]() { counter++; });
        if (i == 2)
            break;
    }

    EXPECT_EQ(counter, 3); // Iterations 0, 1, 2
}

TEST(ScopeGuardTest, WithContinueStatement) {
    int counter = 0;

    for (int i = 0; i < 3; ++i) {
        cf::ScopeGuard guard([&counter]() { counter++; });
        if (i < 2)
            continue;
    }

    EXPECT_EQ(counter, 3); // All 3 iterations
}

TEST(ScopeGuardTest, WithGotoStatement) {
    int counter = 0;

    auto func = [&counter]() -> void {
        cf::ScopeGuard guard([&counter]() { counter++; });
        goto end;
        counter = 100; // Skipped
    end:;
    };

    func();

    EXPECT_EQ(counter, 1);
}

// =============================================================================
// Test Suite 9: Empty/No-op Functions
// =============================================================================

TEST(ScopeGuardTest, EmptyLambda) {
    // Empty lambda should be safe
    {
        cf::ScopeGuard guard([]() {});
    }
    SUCCEED();
}

TEST(ScopeGuardTest, NoopGuard) {
    int counter = 0;

    {
        cf::ScopeGuard guard([&counter]() {
            // Do nothing
        });
    }

    EXPECT_EQ(counter, 0);
}

// =============================================================================
// Test Suite 10: Complex Capture Scenarios
// =============================================================================

TEST(ScopeGuardTest, CaptureThisPointer) {
    struct MyClass {
        int value = 0;

        void method() {
            cf::ScopeGuard guard([this]() { this->value = 42; });
        }
    };

    MyClass obj;
    obj.method();

    EXPECT_EQ(obj.value, 42);
}

TEST(ScopeGuardTest, CaptureByValueAndReference) {
    int a = 1, b = 2;
    int result_a = 0, result_b = 0;

    {
        cf::ScopeGuard guard([a, &b, &result_a, &result_b]() {
            result_a = a;
            result_b = b;
            b = 999; // Modify reference
        });
    }

    EXPECT_EQ(result_a, 1);
    EXPECT_EQ(result_b, 2);
    EXPECT_EQ(b, 999);
}

TEST(ScopeGuardTest, MixedCaptureWithInitializer) {
    int x = 10;
    int result = 0;

    {
        cf::ScopeGuard guard([y = x + 5, &result]() { result = y; });
    }

    EXPECT_EQ(result, 15);
}

TEST(ScopeGuardTest, CaptureConstReference) {
    const int value = 42;
    int result = 0;

    {
        cf::ScopeGuard guard([&value, &result]() { result = value; });
    }

    EXPECT_EQ(result, 42);
}

// =============================================================================
// Test Suite 11: Resource Management Patterns
// =============================================================================

TEST(ScopeGuardTest, FileHandlePattern) {
    bool closed = false;

    {
        cf::ScopeGuard close_guard([&closed]() { closed = true; });
        EXPECT_FALSE(closed);
    }

    EXPECT_TRUE(closed);
}

TEST(ScopeGuardTest, RollbackPattern) {
    int state = 0;

    {
        cf::ScopeGuard rollback([&state]() { state = 0; });

        state = 100;

        // Commit successful - dismiss rollback
        rollback.dismiss();
    }

    EXPECT_EQ(state, 100);
}

TEST(ScopeGuardTest, RollbackOnException) {
    int state = 0;

    try {
        cf::ScopeGuard rollback([&state]() { state = 0; });

        state = 100;

        throw std::runtime_error("operation failed");

        // rollback.dismiss();  // Never reached
    } catch (...) {
        // Exception handled
    }

    EXPECT_EQ(state, 0);
}

// =============================================================================
// Test Suite 12: Edge Cases
// =============================================================================

TEST(ScopeGuardTest, GuardInLoop) {
    int counter = 0;

    for (int i = 0; i < 5; ++i) {
        cf::ScopeGuard guard([&counter]() { counter++; });
    }

    EXPECT_EQ(counter, 5);
}

TEST(ScopeGuardTest, GuardWithStaticVariable) {
    static int counter = 0;

    {
        cf::ScopeGuard guard([]() { counter++; });
    }

    EXPECT_EQ(counter, 1);
    counter = 0; // Reset
}

TEST(ScopeGuardTest, LargeLambdaCapture) {
    std::vector<int> large_vec(1000);
    for (size_t i = 0; i < large_vec.size(); ++i) {
        large_vec[i] = static_cast<int>(i);
    }

    int sum = 0;

    {
        cf::ScopeGuard guard([large_vec, &sum]() {
            for (int v : large_vec) {
                sum += v;
            }
        });
    }

    int expected = 999 * 1000 / 2; // Sum of 0 to 999
    EXPECT_EQ(sum, expected);
}

TEST(ScopeGuardTest, NestedScopeGuards) {
    int outer = 0, inner = 0;

    {
        cf::ScopeGuard outer_guard([&outer]() { outer = 1; });

        {
            cf::ScopeGuard inner_guard([&inner]() { inner = 2; });
        }

        EXPECT_EQ(inner, 2);
        EXPECT_EQ(outer, 0);
    }

    EXPECT_EQ(outer, 1);
}

TEST(ScopeGuardTest, StdFunctionCompatibility) {
    int counter = 0;

    std::function<void()> func = [&counter]() { counter = 42; };

    { cf::ScopeGuard guard(func); }

    EXPECT_EQ(counter, 42);
}

// =============================================================================
// Test Suite 13: Real-world Usage Patterns
// =============================================================================

TEST(ScopeGuardTest, LockGuardPattern) {
    bool locked = true;

    {
        cf::ScopeGuard unlock_guard([&locked]() { locked = false; });
        EXPECT_TRUE(locked);
    }

    EXPECT_FALSE(locked);
}

TEST(ScopeGuardTest, PointerResetPattern) {
    int* ptr = new int(42);
    bool deleted = false;

    {
        cf::ScopeGuard delete_guard([&ptr, &deleted]() {
            delete ptr;
            ptr = nullptr;
            deleted = true;
        });

        EXPECT_FALSE(deleted);
        EXPECT_NE(ptr, nullptr);
    }

    EXPECT_TRUE(deleted);
    EXPECT_EQ(ptr, nullptr);
}

TEST(ScopeGuardTest, TransactionPattern) {
    struct Transaction {
        bool committed = false;
        bool rolled_back = false;

        void commit() { committed = true; }
        void rollback() { rolled_back = true; }
    };

    Transaction txn;

    {
        cf::ScopeGuard guard([&txn]() {
            if (!txn.committed) {
                txn.rollback();
            }
        });

        txn.commit();
        guard.dismiss();
    }

    EXPECT_TRUE(txn.committed);
    EXPECT_FALSE(txn.rolled_back);
}

TEST(ScopeGuardTest, TransactionFailureRollback) {
    struct Transaction {
        bool committed = false;
        bool rolled_back = false;

        void commit() { committed = true; }
        void rollback() { rolled_back = true; }
    };

    Transaction txn;

    {
        cf::ScopeGuard guard([&txn]() {
            if (!txn.committed) {
                txn.rollback();
            }
        });

        // Simulate failure - don't commit
        // guard.dismiss();  // Not called
    }

    EXPECT_FALSE(txn.committed);
    EXPECT_TRUE(txn.rolled_back);
}

TEST(ScopeGuardTest, BufferRestorePattern) {
    int original_value = 10;
    int current_value = original_value;

    {
        cf::ScopeGuard restore(
            [&current_value, original_value]() { current_value = original_value; });

        current_value = 999;
        EXPECT_EQ(current_value, 999);
    }

    EXPECT_EQ(current_value, original_value);
}

// =============================================================================
// Test: ScopeGuard with std::function
// =============================================================================

TEST(ScopeGuardTest, ConstructFromStdFunction) {
    int counter = 0;
    std::function<void()> func = [&counter]() { counter++; };

    { cf::ScopeGuard guard(func); }

    EXPECT_EQ(counter, 1);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
