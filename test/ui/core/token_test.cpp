/**
 * @file token_test.cpp
 * @brief Comprehensive unit tests for cf::ui::core Token system using GoogleTest
 *
 * Test Coverage:
 * 1. TokenRegistry singleton
 * 2. StaticToken - compile-time type-safe tokens
 * 3. DynamicToken - runtime type-erased tokens
 * 4. Thread safety - concurrent access
 * 5. Error handling - TokenError types
 * 6. Complex types
 */

#include "ui/core/token.hpp"
#include <atomic>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <vector>

using namespace cf::ui::core;
// Bring _hash literal operator into scope
using namespace cf::hash;

// =============================================================================
// Test Suite 1: TokenRegistry Singleton
// =============================================================================

TEST(TokenRegistryTest, Singleton) {
    auto& r1 = TokenRegistry::get();
    auto& r2 = TokenRegistry::get();

    EXPECT_EQ(&r1, &r2);
}

// =============================================================================
// Test Suite 2: StaticToken
// =============================================================================

TEST(StaticTokenTest, RegisterAndGet) {
    using TestToken = StaticToken<int, "testToken"_hash>;
    auto& registry = TokenRegistry::get();

    // Clean up if exists
    registry.remove("testToken"_hash);

    auto result = registry.register_token<TestToken>(42);
    EXPECT_TRUE(result) << "Registration should succeed";

    auto get_result = TestToken::get();
    EXPECT_TRUE(get_result) << "Get should succeed";
    EXPECT_EQ(**get_result, 42);

    // Cleanup
    registry.remove("testToken"_hash);
}

TEST(StaticTokenTest, MultipleTypes) {
    using IntToken = StaticToken<int, "intToken"_hash>;
    using StringToken = StaticToken<std::string, "stringToken"_hash>;
    using DoubleToken = StaticToken<double, "doubleToken"_hash>;

    auto& registry = TokenRegistry::get();

    // Clean up
    registry.remove("intToken"_hash);
    registry.remove("stringToken"_hash);
    registry.remove("doubleToken"_hash);

    EXPECT_TRUE(registry.register_token<IntToken>(123));
    EXPECT_TRUE(registry.register_token<StringToken>("hello"));
    EXPECT_TRUE(registry.register_token<DoubleToken>(3.14));

    auto int_result = IntToken::get();
    auto string_result = StringToken::get();
    auto double_result = DoubleToken::get();

    EXPECT_TRUE(int_result);
    EXPECT_TRUE(string_result);
    EXPECT_TRUE(double_result);

    EXPECT_EQ(**int_result, 123);
    EXPECT_EQ(**string_result, "hello");
    EXPECT_DOUBLE_EQ(**double_result, 3.14);

    // Cleanup
    registry.remove("intToken"_hash);
    registry.remove("stringToken"_hash);
    registry.remove("doubleToken"_hash);
}

TEST(StaticTokenTest, NotFound) {
    using NonExistentToken = StaticToken<int, "nonExistentToken"_hash>;
    auto& registry = TokenRegistry::get();

    // Make sure it doesn't exist
    registry.remove("nonExistentToken"_hash);

    auto result = NonExistentToken::get();
    EXPECT_FALSE(result);
    if (!result) {
        EXPECT_EQ(result.error().kind, TokenError::Kind::NotFound);
    }
}

TEST(StaticTokenTest, ConstAccess) {
    using ConstTestToken = StaticToken<int, "constTestToken"_hash>;
    auto& registry = TokenRegistry::get();

    // Clean up
    registry.remove("constTestToken"_hash);

    EXPECT_TRUE(registry.register_token<ConstTestToken>(999));

    auto result = ConstTestToken::get_const();
    EXPECT_TRUE(result);
    EXPECT_EQ(**result, 999);

    // Cleanup
    registry.remove("constTestToken"_hash);
}

TEST(StaticTokenTest, AlreadyRegistered) {
    using DuplicateToken = StaticToken<int, "duplicateToken"_hash>;
    auto& registry = TokenRegistry::get();

    // Clean up
    registry.remove("duplicateToken"_hash);

    EXPECT_TRUE(registry.register_token<DuplicateToken>(1));
    auto result2 = registry.register_token<DuplicateToken>(2);
    EXPECT_FALSE(result2);
    if (!result2) {
        EXPECT_EQ(result2.error().kind, TokenError::Kind::AlreadyRegistered);
    }

    // First value should still be there
    auto get_result = DuplicateToken::get();
    EXPECT_TRUE(get_result);
    EXPECT_EQ(**get_result, 1);

    // Cleanup
    registry.remove("duplicateToken"_hash);
}

// =============================================================================
// Test Suite 3: DynamicToken
// =============================================================================

TEST(DynamicTokenTest, RegisterAndGet) {
    auto& registry = TokenRegistry::get();

    std::string name = "dynamicTest";
    registry.remove(name);

    auto reg_result = registry.register_dynamic<int>(name, 42);
    EXPECT_TRUE(reg_result);

    auto get_result = registry.get_dynamic<int>(name);
    EXPECT_TRUE(get_result);
    EXPECT_EQ(**get_result, 42);

    // Cleanup
    registry.remove(name);
}

TEST(DynamicTokenTest, MultipleTypes) {
    auto& registry = TokenRegistry::get();

    std::string name1 = "dynamicInt";
    std::string name2 = "dynamicString";
    std::string name3 = "dynamicVector";

    registry.remove(name1);
    registry.remove(name2);
    registry.remove(name3);

    EXPECT_TRUE(registry.register_dynamic<int>(name1, 456));
    EXPECT_TRUE(registry.register_dynamic<std::string>(name2, "world"));
    EXPECT_TRUE(registry.register_dynamic<std::vector<int>>(name3, {1, 2, 3}));

    auto r1 = registry.get_dynamic<int>(name1);
    auto r2 = registry.get_dynamic<std::string>(name2);
    auto r3 = registry.get_dynamic<std::vector<int>>(name3);

    EXPECT_TRUE(r1);
    EXPECT_TRUE(r2);
    EXPECT_TRUE(r3);

    EXPECT_EQ(**r1, 456);
    EXPECT_EQ(**r2, "world");
    EXPECT_EQ((*r3)->size(), 3);
    EXPECT_EQ((**r3)[0], 1);
    EXPECT_EQ((**r3)[1], 2);
    EXPECT_EQ((**r3)[2], 3);

    // Cleanup
    registry.remove(name1);
    registry.remove(name2);
    registry.remove(name3);
}

TEST(DynamicTokenTest, TypeMismatch) {
    auto& registry = TokenRegistry::get();

    std::string name = "typeMismatch";
    registry.remove(name);

    EXPECT_TRUE(registry.register_dynamic<int>(name, 42));

    // Try to get as wrong type
    auto result = registry.get_dynamic<std::string>(name);
    EXPECT_FALSE(result);
    if (!result) {
        EXPECT_EQ(result.error().kind, TokenError::Kind::TypeMismatch);
    }

    // Cleanup
    registry.remove(name);
}

TEST(DynamicTokenTest, NotFoundDynamic) {
    auto& registry = TokenRegistry::get();

    std::string name = "nonExistentDynamic";
    registry.remove(name);

    auto result = registry.get_dynamic<int>(name);
    EXPECT_FALSE(result);
    if (!result) {
        EXPECT_EQ(result.error().kind, TokenError::Kind::NotFound);
    }
}

TEST(DynamicTokenTest, RegisterByCopy) {
    auto& registry = TokenRegistry::get();

    std::string name = "copyTest";
    registry.remove(name);

    std::string value = "copied value";
    EXPECT_TRUE(registry.register_dynamic<std::string>(name, value));

    auto result = registry.get_dynamic<std::string>(name);
    EXPECT_TRUE(result);
    EXPECT_EQ(**result, "copied value");
    EXPECT_EQ(value, "copied value"); // Original unchanged

    // Cleanup
    registry.remove(name);
}

TEST(DynamicTokenTest, RegisterByMove) {
    auto& registry = TokenRegistry::get();

    std::string name = "moveTest";
    registry.remove(name);

    std::string value = "moved value";
    EXPECT_TRUE(registry.register_dynamic<std::string>(name, std::move(value)));

    auto result = registry.get_dynamic<std::string>(name);
    EXPECT_TRUE(result);
    EXPECT_EQ(**result, "moved value");
    // Original may be in moved-from state

    // Cleanup
    registry.remove(name);
}

TEST(DynamicTokenTest, ConstAccess) {
    auto& registry = TokenRegistry::get();

    std::string name = "constDynamic";
    registry.remove(name);

    EXPECT_TRUE(registry.register_dynamic<int>(name, 789));

    auto result = registry.get_dynamic_const<int>(name);
    EXPECT_TRUE(result);
    EXPECT_EQ(**result, 789);

    // Cleanup
    registry.remove(name);
}

TEST(DynamicTokenTest, Contains) {
    auto& registry = TokenRegistry::get();

    std::string name = "containsTest";
    registry.remove(name);

    EXPECT_FALSE(registry.contains(name));

    EXPECT_TRUE(registry.register_dynamic<int>(name, 1));
    EXPECT_TRUE(registry.contains(name));

    // Cleanup
    registry.remove(name);
}

TEST(DynamicTokenTest, Remove) {
    auto& registry = TokenRegistry::get();

    std::string name = "removeTest";
    registry.remove(name);

    EXPECT_TRUE(registry.register_dynamic<int>(name, 1));
    EXPECT_TRUE(registry.contains(name));

    EXPECT_TRUE(registry.remove(name));
    EXPECT_FALSE(registry.contains(name));

    EXPECT_FALSE(registry.remove(name)); // Already removed
}

TEST(DynamicTokenTest, GetByHash) {
    auto& registry = TokenRegistry::get();

    std::string name = "hashLookup";
    constexpr uint64_t hash = "hashLookup"_hash;
    registry.remove(hash);

    EXPECT_TRUE(registry.register_dynamic<int>(name, 111));

    auto result = registry.get_dynamic_by_hash<int>(hash);
    EXPECT_TRUE(result);
    EXPECT_EQ(**result, 111);

    // Cleanup
    registry.remove(hash);
}

// =============================================================================
// Test Suite 4: Complex Types
// =============================================================================

// Note: std::any requires copy-constructible types, so unique_ptr and atomic
// cannot be directly stored. Use shared_ptr or regular types instead.

TEST(ComplexTypesTest, SharedPtr) {
    using PtrToken = StaticToken<std::shared_ptr<int>, "ptrToken"_hash>;
    auto& registry = TokenRegistry::get();

    registry.remove("ptrToken"_hash);

    EXPECT_TRUE(registry.register_token<PtrToken>(std::make_shared<int>(333)));

    auto result = PtrToken::get();
    EXPECT_TRUE(result);

    auto ptr = **result;
    EXPECT_EQ(*ptr.get(), 333);

    // Cleanup
    registry.remove("ptrToken"_hash);
}

TEST(ComplexTypesTest, Vector) {
    auto& registry = TokenRegistry::get();

    std::string name = "vectorToken";
    registry.remove(name);

    std::vector<int> vec = {10, 20, 30, 40, 50};
    EXPECT_TRUE(registry.register_dynamic<std::vector<int>>(name, std::move(vec)));

    auto result = registry.get_dynamic<std::vector<int>>(name);
    EXPECT_TRUE(result);
    EXPECT_EQ((*result)->size(), 5);
    EXPECT_EQ((**result)[0], 10);
    EXPECT_EQ((**result)[4], 50);

    // Cleanup
    registry.remove(name);
}

// Custom struct for testing
struct TestStruct {
    int a;
    double b;
    std::string c;

    bool operator==(const TestStruct& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
};

TEST(ComplexTypesTest, CustomStruct) {
    auto& registry = TokenRegistry::get();

    std::string name = "structToken";
    registry.remove(name);

    TestStruct s{123, 4.56, "test"};
    EXPECT_TRUE(registry.register_dynamic<TestStruct>(name, s));

    auto result = registry.get_dynamic<TestStruct>(name);
    EXPECT_TRUE(result);
    EXPECT_EQ((*result)->a, 123);
    EXPECT_DOUBLE_EQ((*result)->b, 4.56);
    EXPECT_EQ((*result)->c, "test");

    // Cleanup
    registry.remove(name);
}

// =============================================================================
// Test Suite 5: Thread Safety
// =============================================================================

// Note: std::atomic is not copy-constructible and cannot be stored in std::any.
// We use regular int and rely on TokenRegistry's internal mutex for safety.
TEST(ThreadSafetyTest, ConcurrentReads) {
    using SharedToken = StaticToken<int, "sharedToken"_hash>;
    auto& registry = TokenRegistry::get();

    registry.remove("sharedToken"_hash);

    EXPECT_TRUE(registry.register_token<SharedToken>(0));

    constexpr int num_threads = 10;
    constexpr int reads_per_thread = 100;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < reads_per_thread; ++j) {
                auto result = SharedToken::get();
                if (result) {
                    // Just verify we can read the value
                    EXPECT_GE(**result, 0);
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto final_result = SharedToken::get();
    EXPECT_TRUE(final_result);
    EXPECT_EQ(**final_result, 0);

    // Cleanup
    registry.remove("sharedToken"_hash);
}

TEST(ThreadSafetyTest, ConcurrentWrites) {
    auto& registry = TokenRegistry::get();
    std::atomic<int> success_count{0};
    std::atomic<int> failure_count{0};

    constexpr int num_threads = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            std::string name = "concurrent_" + std::to_string(i);
            auto result = registry.register_dynamic<int>(name, i);
            if (result) {
                success_count++;
            } else {
                failure_count++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // All registrations should succeed since names are different
    EXPECT_EQ(success_count, num_threads);
    EXPECT_EQ(failure_count, 0);

    // Cleanup
    for (int i = 0; i < num_threads; ++i) {
        std::string name = "concurrent_" + std::to_string(i);
        registry.remove(name);
    }
}

TEST(ThreadSafetyTest, ReadDuringWrite) {
    using ReadWriteToken = StaticToken<int, "readWriteToken"_hash>;
    auto& registry = TokenRegistry::get();

    registry.remove("readWriteToken"_hash);
    EXPECT_TRUE(registry.register_token<ReadWriteToken>(100));

    std::atomic<bool> stop{false};
    std::atomic<int> read_count{0};
    std::atomic<int> write_count{0};

    // Reader threads
    std::vector<std::thread> readers;
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back([&]() {
            while (!stop) {
                auto result = ReadWriteToken::get();
                if (result) {
                    read_count++;
                }
            }
        });
    }

    // Writer thread
    std::thread writer([&]() {
        for (int i = 0; i < 100; ++i) {
            // Re-register with new value
            registry.remove("readWriteToken"_hash);
            registry.register_token<ReadWriteToken>(100 + i);
            write_count++;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    });

    writer.join();
    stop = true;

    for (auto& t : readers) {
        t.join();
    }

    EXPECT_GT(read_count, 0);
    EXPECT_EQ(write_count, 100);

    // Cleanup
    registry.remove("readWriteToken"_hash);
}

// =============================================================================
// Test Suite 6: Size and Cleanup
// =============================================================================

TEST(TokenRegistryTest, SizeTracking) {
    auto& registry = TokenRegistry::get();

    size_t initial_size = registry.size();

    std::string name1 = "sizeTest1";
    std::string name2 = "sizeTest2";

    registry.remove(name1);
    registry.remove(name2);

    EXPECT_TRUE(registry.register_dynamic<int>(name1, 1));
    EXPECT_EQ(registry.size(), initial_size + 1);

    EXPECT_TRUE(registry.register_dynamic<int>(name2, 2));
    EXPECT_EQ(registry.size(), initial_size + 2);

    registry.remove(name1);
    EXPECT_EQ(registry.size(), initial_size + 1);

    registry.remove(name2);
    EXPECT_EQ(registry.size(), initial_size);
}

TEST(TokenRegistryTest, RemoveNonExistent) {
    auto& registry = TokenRegistry::get();

    std::string name = "doesNotExist";
    registry.remove(name);

    EXPECT_FALSE(registry.remove(name));
    EXPECT_FALSE(registry.remove("anotherNonExistent"_hash));
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
