/**
 * @file constexpr_fnv1a_test.cpp
 * @brief Unit tests for compile-time FNV-1a hash
 *
 * Test Coverage:
 * 1. Compile-time hash computation
 * 2. User-defined literal "_hash"
 * 3. String_view hash
 * 4. Hash distribution (different strings, different hashes)
 * 5. 32-bit variant
 */

#include "base/hash/constexpr_fnv1a.hpp"
#include <gtest/gtest.h>
#include <string>
#include <string_view>

using namespace cf::hash;

// =============================================================================
// Test Suite 1: Compile-time Hash (64-bit)
// =============================================================================

TEST(Fnv1a64Test, CompileTimeHash) {
    constexpr uint64_t h1 = fnv1a64("TokenA");
    constexpr uint64_t h2 = fnv1a64("TokenB");
    constexpr uint64_t h3 = fnv1a64("TokenA"); // Same as h1

    EXPECT_NE(h1, h2);
    EXPECT_EQ(h1, h3);
}

TEST(Fnv1a64Test, UserDefinedLiteral) {
    constexpr uint64_t h1 = "testToken"_hash;
    constexpr uint64_t h2 = fnv1a64("testToken");

    EXPECT_EQ(h1, h2);
}

TEST(Fnv1a64Test, LiteralIsConstexpr) {
    // This should compile if the literal is truly constexpr
    constexpr uint64_t h = "constexprTest"_hash;
    static_assert(h > 0, "Hash should be non-zero");
    EXPECT_GT(h, 0);
}

TEST(Fnv1a64Test, StringViewHash) {
    std::string_view sv = "myToken";
    constexpr uint64_t h1 = "myToken"_hash;
    uint64_t h2 = fnv1a64(sv);

    EXPECT_EQ(h1, h2);
}

TEST(Fnv1a64Test, StringHash) {
    std::string s = "myString";
    uint64_t h1 = fnv1a64(s);
    uint64_t h2 = fnv1a64(std::string_view(s));

    EXPECT_EQ(h1, h2);
}

TEST(Fnv1a64Test, DifferentStringsHaveDifferentHashes) {
    constexpr uint64_t h1 = "userId"_hash;
    constexpr uint64_t h2 = "userName"_hash;
    constexpr uint64_t h3 = "settings"_hash;
    constexpr uint64_t h4 = "counter"_hash;
    constexpr uint64_t h5 = "config"_hash;

    // All hashes should be different
    EXPECT_NE(h1, h2);
    EXPECT_NE(h1, h3);
    EXPECT_NE(h1, h4);
    EXPECT_NE(h1, h5);
    EXPECT_NE(h2, h3);
    EXPECT_NE(h2, h4);
    EXPECT_NE(h2, h5);
    EXPECT_NE(h3, h4);
    EXPECT_NE(h3, h5);
    EXPECT_NE(h4, h5);
}

TEST(Fnv1a64Test, EmptyString) {
    constexpr uint64_t h_empty = ""_hash;
    EXPECT_GT(h_empty, 0);

    uint64_t h_empty_sv = fnv1a64(std::string_view(""));
    EXPECT_EQ(h_empty, h_empty_sv);
}

TEST(Fnv1a64Test, SingleCharacter) {
    constexpr uint64_t h_a = "a"_hash;
    constexpr uint64_t h_b = "b"_hash;
    constexpr uint64_t h_A = "A"_hash;

    EXPECT_NE(h_a, h_b);
    EXPECT_NE(h_a, h_A);
}

TEST(Fnv1a64Test, SimilarStringsDifferentHashes) {
    constexpr uint64_t h1 = "test"_hash;
    constexpr uint64_t h2 = "Test"_hash;
    constexpr uint64_t h3 = "TEST"_hash;
    constexpr uint64_t h4 = "test "_hash; // trailing space
    constexpr uint64_t h5 = " test"_hash; // leading space

    EXPECT_NE(h1, h2);
    EXPECT_NE(h1, h3);
    EXPECT_NE(h2, h3);
    EXPECT_NE(h1, h4);
    EXPECT_NE(h1, h5);
}

TEST(Fnv1a64Test, LongString) {
    // Test with a longer string
    std::string long_str(1000, 'a'); // 1000 'a' characters
    uint64_t h = fnv1a64(long_str);

    EXPECT_GT(h, 0);

    // Different length should give different hash
    std::string longer_str(1001, 'a');
    uint64_t h2 = fnv1a64(longer_str);
    EXPECT_NE(h, h2);
}

TEST(Fnv1a64Test, CustomSeed) {
    constexpr uint64_t seed = 12345;
    constexpr uint64_t h1 = fnv1a64("test", seed);
    constexpr uint64_t h2 = fnv1a64("test", seed);
    constexpr uint64_t h3 = fnv1a64("test"); // default seed

    EXPECT_EQ(h1, h2);
    EXPECT_NE(h1, h3);
}

// =============================================================================
// Test Suite 2: 32-bit Variant
// =============================================================================

TEST(Fnv1a32Test, CompileTimeHash32) {
    constexpr uint32_t h1 = fnv1a32("TokenA");
    constexpr uint32_t h2 = fnv1a32("TokenB");
    constexpr uint32_t h3 = fnv1a32("TokenA");

    EXPECT_NE(h1, h2);
    EXPECT_EQ(h1, h3);
}

TEST(Fnv1a32Test, StringViewHash32) {
    std::string_view sv = "myToken32";
    uint32_t h1 = fnv1a32(sv);
    uint32_t h2 = fnv1a32("myToken32");

    EXPECT_EQ(h1, h2);
}

TEST(Fnv1a32Test, DifferentFrom64Bit) {
    constexpr uint64_t h64 = "test"_hash;
    constexpr uint32_t h32 = fnv1a32("test");

    // 32-bit hash should be different from (at least part of) 64-bit
    // (they use different constants so results will differ)
    EXPECT_NE(static_cast<uint32_t>(h64), h32);
}

// =============================================================================
// Test Suite 3: Hash Distribution
// =============================================================================

TEST(HashDistribution, NoCollisionsForCommonTokens) {
    // Test a set of common token-like names
    const char* token_names[] = {
        "userId",  "userName", "userEmail", "settings", "config", "preferences", "token",
        "session", "cache",    "request",   "response", "error",  "id",          "name",
        "value",   "type",     "true",      "false",    "null",   "width",       "height",
        "x",       "y",        "red",       "green",    "blue",   "alpha"};

    constexpr size_t num_names = sizeof(token_names) / sizeof(token_names[0]);
    uint64_t hashes[num_names];

    for (size_t i = 0; i < num_names; ++i) {
        hashes[i] = fnv1a64(token_names[i]);
    }

    // Check no duplicates
    for (size_t i = 0; i < num_names; ++i) {
        for (size_t j = i + 1; j < num_names; ++j) {
            EXPECT_NE(hashes[i], hashes[j])
                << "Collision between " << token_names[i] << " and " << token_names[j];
        }
    }
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
