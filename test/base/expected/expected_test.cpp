/**
 * @file expected_test.cpp
 * @brief Comprehensive unit tests for cf::expected<T,E> using GoogleTest
 *
 * Test Coverage:
 * 1. Construction and Destruction
 * 2. Value and Error Access
 * 3. Assignment Operators
 * 4. Monadic Operations (and_then, or_else, transform, transform_error)
 * 5. Comparison Operations
 * 6. Swap Operations
 * 7. Exception Handling (bad_expected_access)
 * 8. Type Traits and Compile-time Properties
 * 9. Move Semantics
 * 10. Const Correctness
 * 11. expected<void, E> Specialization
 */

#include "base/expected/expected.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// =============================================================================
// Test Suite 1: Construction and Destruction
// =============================================================================

TEST(ExpectedTest, DefaultConstructor) {
    cf::expected<int, std::string> e1;
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(*e1, 0);

    cf::expected<std::string, int> e2;
    EXPECT_TRUE(e2.has_value());
    EXPECT_TRUE(e2->empty());
}

TEST(ExpectedTest, ValueConstructor) {
    cf::expected<int, std::string> e1(42);
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(*e1, 42);

    // Implicit conversion
    cf::expected<int, std::string> e2 = 100;
    EXPECT_EQ(*e2, 100);

    // Move-only type
    cf::expected<std::unique_ptr<int>, std::string> e3(std::make_unique<int>(999));
    EXPECT_TRUE(e3.has_value());
    EXPECT_EQ(**e3, 999);
}

TEST(ExpectedTest, ErrorConstructor) {
    cf::expected<int, std::string> e1(cf::unexpected<std::string>("error"));
    EXPECT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error(), "error");

    // Implicit conversion
    cf::expected<int, std::string> e2 = cf::unexpected("failure");
    EXPECT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error(), "failure");
}

TEST(ExpectedTest, CopyConstructor) {
    // Copy with value
    cf::expected<int, std::string> e1(42);
    cf::expected<int, std::string> e2(e1);
    EXPECT_TRUE(e2.has_value());
    EXPECT_EQ(*e2, 42);
    EXPECT_EQ(*e1, 42);  // Original unchanged

    // Copy with error
    cf::expected<int, std::string> e3(cf::unexpected("error"));
    cf::expected<int, std::string> e4(e3);
    EXPECT_FALSE(e4.has_value());
    EXPECT_EQ(e4.error(), "error");
}

TEST(ExpectedTest, MoveConstructor) {
    cf::expected<std::string, int> e1(std::string("hello"));
    cf::expected<std::string, int> e2(std::move(e1));
    EXPECT_TRUE(e2.has_value());
    EXPECT_EQ(*e2, "hello");

    cf::expected<int, std::string> e3(cf::unexpected("error"));
    cf::expected<int, std::string> e4(std::move(e3));
    EXPECT_FALSE(e4.has_value());
    EXPECT_EQ(e4.error(), "error");
}

TEST(ExpectedTest, InPlaceConstructors) {
    cf::expected<std::string, int> e1(std::in_place, 5, 'a');
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(*e1, "aaaaa");

    cf::expected<int, std::string> e2(cf::unexpect, 3, 'b');
    EXPECT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error(), "bbb");
}

// =============================================================================
// Test Suite 2: Value and Error Access
// =============================================================================

TEST(ExpectedTest, OperatorArrow) {
    struct S {
        int value = 42;
        int get() const { return value; }
    };

    cf::expected<S, int> e(std::in_place);
    EXPECT_EQ(e->value, 42);
    EXPECT_EQ(e->get(), 42);

    const cf::expected<S, int> ce(std::in_place);
    EXPECT_EQ(ce->value, 42);
}

TEST(ExpectedTest, OperatorDereference) {
    cf::expected<int, std::string> e(42);
    EXPECT_EQ(*e, 42);

    cf::expected<std::string, int> e2(std::string("hello"));
    std::string moved = *std::move(e2);
    EXPECT_EQ(moved, "hello");
}

TEST(ExpectedTest, ValueMethod) {
    cf::expected<int, std::string> e(42);
    EXPECT_EQ(e.value(), 42);

    cf::expected<int, std::string> e_err(cf::unexpected("error"));
    EXPECT_THROW(e_err.value(), cf::bad_expected_access<std::string>);

    // Check exception content
    try {
        e_err.value();
    } catch (const cf::bad_expected_access<std::string>& ex) {
        EXPECT_EQ(ex.error(), "error");
        EXPECT_STREQ(ex.what(), "bad_expected_access");
    }
}

TEST(ExpectedTest, ErrorMethod) {
    cf::expected<int, std::string> e_err(cf::unexpected("error"));
    EXPECT_EQ(e_err.error(), "error");

    cf::expected<void, int> e_void(cf::unexpected(42));
    EXPECT_EQ(e_void.error(), 42);
}

TEST(ExpectedTest, ValueOr) {
    cf::expected<int, std::string> e(42);
    EXPECT_EQ(e.value_or(-1), 42);

    cf::expected<int, std::string> e_err(cf::unexpected("error"));
    EXPECT_EQ(e_err.value_or(-1), -1);
}

TEST(ExpectedTest, ErrorOr) {
    cf::expected<int, std::string> e(42);
    EXPECT_EQ(e.error_or("default"), "default");

    cf::expected<int, std::string> e_err(cf::unexpected("error"));
    EXPECT_EQ(e_err.error_or("default"), "error");
}

TEST(ExpectedTest, HasValueAndBoolConversion) {
    cf::expected<int, std::string> e_val(42);
    EXPECT_TRUE(e_val.has_value());
    EXPECT_TRUE(e_val);
    EXPECT_TRUE(static_cast<bool>(e_val));

    cf::expected<int, std::string> e_err(cf::unexpected("error"));
    EXPECT_FALSE(e_err.has_value());
    EXPECT_FALSE(e_err);
}

// =============================================================================
// Test Suite 3: Assignment Operators
// =============================================================================

TEST(ExpectedTest, CopyAssignment) {
    // Value to value
    cf::expected<int, std::string> e1(10);
    cf::expected<int, std::string> e2(20);
    e1 = e2;
    EXPECT_EQ(*e1, 20);
    EXPECT_EQ(*e2, 20);

    // Error to error
    cf::expected<int, std::string> e3(cf::unexpected("err1"));
    cf::expected<int, std::string> e4(cf::unexpected("err2"));
    e3 = e4;
    EXPECT_EQ(e3.error(), "err2");

    // Value to error (state change)
    cf::expected<int, std::string> e5(cf::unexpected("error"));
    cf::expected<int, std::string> e6(42);
    e5 = e6;
    EXPECT_TRUE(e5.has_value());
    EXPECT_EQ(*e5, 42);

    // Error to value (state change)
    cf::expected<int, std::string> e7(42);
    cf::expected<int, std::string> e8(cf::unexpected("error"));
    e7 = e8;
    EXPECT_FALSE(e7.has_value());
    EXPECT_EQ(e7.error(), "error");

    // Self assignment
    cf::expected<int, std::string> e9(99);
    e9 = e9;
    EXPECT_EQ(*e9, 99);
}

TEST(ExpectedTest, MoveAssignment) {
    cf::expected<std::string, int> e1(std::string("hello"));
    cf::expected<std::string, int> e2(std::string("world"));
    e1 = std::move(e2);
    EXPECT_EQ(*e1, "world");

    cf::expected<int, std::string> e3(cf::unexpected("err1"));
    cf::expected<int, std::string> e4(cf::unexpected("err2"));
    e3 = std::move(e4);
    EXPECT_EQ(e3.error(), "err2");
}

TEST(ExpectedTest, ValueAssignment) {
    cf::expected<int, std::string> e(cf::unexpected("error"));
    e = 42;
    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 42);

    e = 100;
    EXPECT_EQ(*e, 100);
}

TEST(ExpectedTest, UnexpectedAssignment) {
    cf::expected<int, std::string> e(42);
    e = cf::unexpected("error");
    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error(), "error");
}

// =============================================================================
// Test Suite 4: Monadic Operations
// =============================================================================

TEST(ExpectedTest, AndThen) {
    auto add_one = [](int x) -> cf::expected<int, std::string> {
        return x + 1;
    };

    cf::expected<int, std::string> e1(5);
    auto result1 = e1.and_then(add_one);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(*result1, 6);

    cf::expected<int, std::string> e2(cf::unexpected("error"));
    auto result2 = e2.and_then(add_one);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), "error");

    // Chaining
    auto multiply_by_two = [](int x) -> cf::expected<int, std::string> {
        return x * 2;
    };

    cf::expected<int, std::string> e3(3);
    auto result3 = e3.and_then(add_one).and_then(multiply_by_two);
    EXPECT_EQ(*result3, 8);  // (3+1)*2

    auto always_error = [](int) -> cf::expected<int, std::string> {
        return cf::unexpected("chain error");
    };

    auto result4 = e3.and_then(add_one).and_then(always_error);
    EXPECT_FALSE(result4.has_value());
    EXPECT_EQ(result4.error(), "chain error");
}

TEST(ExpectedTest, OrElse) {
    auto recover = [](const std::string& err) -> cf::expected<int, int> {
        return -1;
    };

    cf::expected<int, std::string> e1(cf::unexpected("error"));
    auto result1 = e1.or_else(recover);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(*result1, -1);

    cf::expected<int, std::string> e2(42);
    auto result2 = e2.or_else(recover);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(*result2, 42);

    auto convert_error = [](const std::string& err) -> cf::expected<int, int> {
        return cf::unexpected(static_cast<int>(err.size()));
    };

    cf::expected<int, std::string> e3(cf::unexpected("test"));
    auto result3 = e3.or_else(convert_error);
    EXPECT_FALSE(result3.has_value());
    EXPECT_EQ(result3.error(), 4);
}

TEST(ExpectedTest, Transform) {
    auto square = [](int x) { return x * x; };

    cf::expected<int, std::string> e1(5);
    auto result1 = e1.transform(square);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(*result1, 25);

    cf::expected<int, std::string> e2(cf::unexpected("error"));
    auto result2 = e2.transform(square);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), "error");

    // Type change
    auto to_string = [](int x) { return "value: " + std::to_string(x); };

    cf::expected<int, std::string> e3(42);
    auto result3 = e3.transform(to_string);
    EXPECT_TRUE(result3.has_value());
    EXPECT_EQ(*result3, "value: 42");
}

TEST(ExpectedTest, TransformError) {
    auto prepend_prefix = [](const std::string& err) { return "Error: " + err; };

    cf::expected<int, std::string> e1(cf::unexpected("test"));
    auto result1 = e1.transform_error(prepend_prefix);
    EXPECT_FALSE(result1.has_value());
    EXPECT_EQ(result1.error(), "Error: test");

    cf::expected<int, std::string> e2(42);
    auto result2 = e2.transform_error(prepend_prefix);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(*result2, 42);

    auto to_error_code = [](const std::string& err) { return static_cast<int>(err.size()); };

    cf::expected<int, std::string> e3(cf::unexpected("error"));
    auto result3 = e3.transform_error(to_error_code);
    EXPECT_FALSE(result3.has_value());
    EXPECT_EQ(result3.error(), 5);
}

// =============================================================================
// Test Suite 5: Comparison Operations
// =============================================================================

TEST(ExpectedTest, EqualityComparison) {
    cf::expected<int, std::string> e1(42);
    cf::expected<int, std::string> e2(42);
    cf::expected<int, std::string> e3(43);
    cf::expected<int, std::string> e4(cf::unexpected("error"));
    cf::expected<int, std::string> e5(cf::unexpected("error"));
    cf::expected<int, std::string> e6(cf::unexpected("other"));

    EXPECT_EQ(e1, e2);
    EXPECT_NE(e1, e3);
    EXPECT_NE(e1, e4);
    EXPECT_EQ(e4, e5);
    EXPECT_NE(e4, e6);

    // Comparison with value
    EXPECT_EQ(e1, 42);
    EXPECT_NE(e1, 43);

    // Comparison with unexpected
    EXPECT_EQ(e4, cf::unexpected("error"));
    EXPECT_NE(e1, cf::unexpected("error"));
}

// =============================================================================
// Test Suite 6: Swap Operations
// =============================================================================

TEST(ExpectedTest, SwapValueValue) {
    cf::expected<int, std::string> e1(10);
    cf::expected<int, std::string> e2(20);

    e1.swap(e2);

    EXPECT_EQ(*e1, 20);
    EXPECT_EQ(*e2, 10);
    EXPECT_TRUE(e1.has_value() && e2.has_value());
}

TEST(ExpectedTest, SwapErrorError) {
    cf::expected<int, std::string> e1(cf::unexpected("error1"));
    cf::expected<int, std::string> e2(cf::unexpected("error2"));

    e1.swap(e2);

    EXPECT_EQ(e1.error(), "error2");
    EXPECT_EQ(e2.error(), "error1");
}

TEST(ExpectedTest, SwapValueError) {
    cf::expected<std::string, int> e1(std::string("value"));
    cf::expected<std::string, int> e2(cf::unexpected(42));

    e1.swap(e2);

    EXPECT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error(), 42);
    EXPECT_TRUE(e2.has_value());
    EXPECT_EQ(*e2, "value");
}

TEST(ExpectedTest, StdSwap) {
    cf::expected<int, std::string> e1(10);
    cf::expected<int, std::string> e2(20);

    std::swap(e1, e2);

    EXPECT_EQ(*e1, 20);
    EXPECT_EQ(*e2, 10);
}

TEST(ExpectedTest, SelfSwap) {
    cf::expected<int, std::string> e(42);
    e.swap(e);
    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 42);
}

// =============================================================================
// Test Suite 7: Exception Handling
// =============================================================================

TEST(ExpectedTest, BadExpectedAccess) {
    cf::expected<int, std::string> e(cf::unexpected("error message"));

    EXPECT_THROW(e.value(), cf::bad_expected_access<std::string>);

    try {
        e.value();
    } catch (const cf::bad_expected_access<std::string>& ex) {
        EXPECT_STREQ(ex.what(), "bad_expected_access");
        EXPECT_EQ(ex.error(), "error message");
    }
}

TEST(ExpectedTest, ExceptionInMonadicOps) {
    auto throwing_func = [](int) -> cf::expected<int, std::string> {
        throw std::runtime_error("user exception");
    };

    cf::expected<int, std::string> e(42);
    EXPECT_THROW(e.and_then(throwing_func), std::runtime_error);
}

// =============================================================================
// Test Suite 8: Type Traits and Compile-time Properties
// =============================================================================

TEST(ExpectedTest, TypeTraits) {
    using E = cf::expected<int, std::string>;

    EXPECT_TRUE((std::is_copy_constructible_v<E>));
    EXPECT_TRUE((std::is_move_constructible_v<E>));
    EXPECT_TRUE((std::is_copy_assignable_v<E>));
    EXPECT_TRUE((std::is_move_assignable_v<E>));

    EXPECT_TRUE((std::is_same_v<E::value_type, int>));
    EXPECT_TRUE((std::is_same_v<E::error_type, std::string>));
    EXPECT_TRUE((std::is_same_v<E::unexpected_type, cf::unexpected<std::string>>));
}

TEST(ExpectedTest, NoexceptSpecifications) {
    cf::expected<int, int> e(42);

    EXPECT_TRUE(noexcept(e.has_value()));
    EXPECT_TRUE(noexcept(static_cast<bool>(e)));
    EXPECT_TRUE(noexcept(e.error()));
}

// =============================================================================
// Test Suite 9: Move Semantics and Resource Management
// =============================================================================

TEST(ExpectedTest, MoveOnlyTypes) {
    // unique_ptr as value
    cf::expected<std::unique_ptr<int>, std::string> e1(std::make_unique<int>(42));
    EXPECT_TRUE(e1.has_value());
    EXPECT_EQ(**e1, 42);

    cf::expected<std::unique_ptr<int>, std::string> e2(std::move(e1));
    EXPECT_TRUE(e2.has_value());
    EXPECT_EQ(**e2, 42);

    // unique_ptr as error
    cf::expected<int, std::unique_ptr<std::string>> e3(
        cf::unexpected(std::make_unique<std::string>("error")));
    EXPECT_FALSE(e3.has_value());
    EXPECT_EQ(*e3.error(), "error");
}

TEST(ExpectedTest, ResourceManagement) {
    // Use namespace-level counters instead of static members in local struct
    static int constructions = 0;
    static int destructions = 0;

    struct Counter {
        int value;

        Counter(int v) : value(v) { constructions++; }
        ~Counter() { destructions++; }
        Counter(const Counter& other) : value(other.value) { constructions++; }
        Counter(Counter&& other) noexcept : value(other.value) { constructions++; }
        Counter& operator=(const Counter&) = default;
        Counter& operator=(Counter&&) noexcept = default;
    };

    constructions = 0;
    destructions = 0;

    {
        cf::expected<Counter, int> e(42);
        EXPECT_GT(constructions, 0);
    }

    EXPECT_GT(destructions, 0);
}

// =============================================================================
// Test Suite 10: Const Correctness
// =============================================================================

TEST(ExpectedTest, ConstExpectedAccess) {
    const cf::expected<int, std::string> e(42);

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 42);
    EXPECT_EQ(e.value(), 42);
    EXPECT_EQ(*e.operator->(), 42);
}

TEST(ExpectedTest, ConstErrorAccess) {
    const cf::expected<int, std::string> e(cf::unexpected("error"));

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error(), "error");
}

TEST(ExpectedTest, ConstMonadicOperations) {
    const cf::expected<int, std::string> e(42);

    auto square = [](int x) { return x * x; };
    auto result = e.transform(square);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1764);
}

// =============================================================================
// Test Suite 11: expected<void, E> Specialization
// =============================================================================

TEST(ExpectedVoidTest, DefaultConstructor) {
    cf::expected<void, std::string> e;
    EXPECT_TRUE(e.has_value());
    EXPECT_NO_THROW(e.value());
}

TEST(ExpectedVoidTest, FromUnexpected) {
    cf::expected<void, std::string> e(cf::unexpected("error"));
    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error(), "error");
    EXPECT_THROW(e.value(), cf::bad_expected_access<std::string>);
}

TEST(ExpectedVoidTest, Copy) {
    cf::expected<void, std::string> e1;
    cf::expected<void, std::string> e2 = e1;
    EXPECT_TRUE(e2.has_value());

    cf::expected<void, std::string> e3(cf::unexpected("err"));
    cf::expected<void, std::string> e4 = e3;
    EXPECT_FALSE(e4.has_value());
    EXPECT_EQ(e4.error(), "err");
}

TEST(ExpectedVoidTest, Assignment) {
    cf::expected<void, std::string> e1;
    cf::expected<void, std::string> e2(cf::unexpected("err"));

    e1 = e2;
    EXPECT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error(), "err");
}

TEST(ExpectedVoidTest, AndThen) {
    cf::expected<void, std::string> e;

    auto return_int = []() -> cf::expected<int, std::string> {
        return 42;
    };

    auto result = e.and_then(return_int);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);

    cf::expected<void, std::string> e_err(cf::unexpected("error"));
    auto result_err = e_err.and_then(return_int);
    EXPECT_FALSE(result_err.has_value());
    EXPECT_EQ(result_err.error(), "error");
}

TEST(ExpectedVoidTest, Transform) {
    cf::expected<void, std::string> e;

    auto return_int = []() { return 42; };
    auto result = e.transform(return_int);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);

    cf::expected<void, std::string> e_err(cf::unexpected("error"));
    auto result_err = e_err.transform(return_int);

    EXPECT_FALSE(result_err.has_value());
    EXPECT_EQ(result_err.error(), "error");
}

// =============================================================================
// Test Suite 12: Edge Cases and Corner Cases
// =============================================================================

TEST(ExpectedEdgeCases, EmptyStringValue) {
    cf::expected<std::string, int> e("");
    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->size(), 0);
}

TEST(ExpectedEdgeCases, EmptyStringError) {
    cf::expected<int, std::string> e(cf::unexpected(""));
    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().size(), 0);
}

TEST(ExpectedEdgeCases, ZeroValues) {
    cf::expected<int, std::string> e(0);
    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 0);

    cf::expected<std::string, int> e2(cf::unexpected(0));
    EXPECT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error(), 0);
}

TEST(ExpectedEdgeCases, BooleanValues) {
    cf::expected<bool, std::string> e_true(true);
    cf::expected<bool, std::string> e_false(false);

    EXPECT_TRUE(e_true.has_value());
    EXPECT_TRUE(*e_true);

    EXPECT_TRUE(e_false.has_value());
    EXPECT_FALSE(*e_false);

    // bool() checks has_value(), not the contained bool
    EXPECT_TRUE(static_cast<bool>(e_true));
    EXPECT_TRUE(static_cast<bool>(e_false));
}

TEST(ExpectedEdgeCases, LargeObjects) {
    std::vector<int> large_vec(1000);
    for (size_t i = 0; i < large_vec.size(); ++i) {
        large_vec[i] = static_cast<int>(i);
    }

    cf::expected<std::vector<int>, std::string> e(std::move(large_vec));

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(e->size(), 1000);
    EXPECT_EQ((*e)[500], 500);
}

TEST(ExpectedEdgeCases, MultipleStateChanges) {
    cf::expected<int, std::string> e(1);

    e = cf::unexpected("error1");
    EXPECT_FALSE(e.has_value());

    e = 2;
    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 2);

    e = cf::unexpected("error2");
    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error(), "error2");

    e = 3;
    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(*e, 3);
}

TEST(ExpectedEdgeCases, StructErrorType) {
    struct ErrorInfo {
        int code;
        std::string message;
        bool operator==(const ErrorInfo& other) const {
            return code == other.code && message == other.message;
        }
    };

    cf::expected<int, ErrorInfo> e(cf::unexpected(ErrorInfo{404, "Not Found"}));

    EXPECT_FALSE(e.has_value());
    EXPECT_EQ(e.error().code, 404);
    EXPECT_EQ(e.error().message, "Not Found");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
