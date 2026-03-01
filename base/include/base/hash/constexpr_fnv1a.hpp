/**
 * @file    constexpr_fnv1a.hpp
 * @brief   Compile-time FNV-1a hash for string interning.
 *
 * Provides constexpr FNV-1a hashing algorithm for compile-time
 * string hash computation. Supports both 32-bit and 64-bit variants.
 *
 * @author  Charliechen114514
 * @date    2026-02-25
 * @version 0.1
 * @since   0.1
 * @ingroup base_hash
 */
#pragma once

#include <cstdint>
#include <string_view>

namespace cf::hash {

/**
 * @brief  FNV-1a 64-bit offset basis.
 *
 * @since  0.1
 * @ingroup base_hash
 * @internal
 */
inline constexpr uint64_t fnv1a64_offset_basis = 14695981039346656037ULL;

/**
 * @brief  FNV-1a 64-bit prime.
 *
 * @since  0.1
 * @ingroup base_hash
 * @internal
 */
inline constexpr uint64_t fnv1a64_prime = 1099511628211ULL;

/**
 * @brief  FNV-1a 32-bit offset basis.
 *
 * @since  0.1
 * @ingroup base_hash
 * @internal
 */
inline constexpr uint32_t fnv1a32_offset_basis = 2166136261U;

/**
 * @brief  FNV-1a 32-bit prime.
 *
 * @since  0.1
 * @ingroup base_hash
 * @internal
 */
inline constexpr uint32_t fnv1a32_prime = 16777619U;

/**
 * @brief  FNV-1a 64-bit hash constexpr implementation.
 *
 * Computes FNV-1a hash at compile time for string literals.
 * The FNV-1a algorithm provides excellent distribution and
 * minimal collisions for string identifiers.
 *
 * @param[in]  str Null-terminated string to hash.
 * @param[in]  seed Starting hash value (default: FNV offset basis).
 *
 * @return     64-bit hash value.
 *
 * @throws     None.
 *
 * @note       The hash is computed at compile time for string literals.
 *
 * @warning    Different inputs may produce the same hash (collision).
 *             Use string comparison as fallback when hashes match.
 *
 * @since      0.1
 * @ingroup    base_hash
 *
 * @code
 * constexpr uint64_t h1 = fnv1a64("TokenA");  // Compile-time
 * constexpr uint64_t h2 = fnv1a64("TokenB");  // Different value
 * static_assert(h1 != h2, "Hash collision!");
 * @endcode
 */
constexpr uint64_t fnv1a64(const char* str, uint64_t seed = fnv1a64_offset_basis) {
    return (*str == 0) ? seed
                       : fnv1a64(str + 1, (seed ^ static_cast<uint64_t>(*str)) * fnv1a64_prime);
}

/**
 * @brief  FNV-1a 64-bit hash for std::string_view (runtime/constexpr).
 *
 * @param[in]  sv String view to hash.
 * @param[in]  seed Starting hash value (default: FNV offset basis).
 *
 * @return    64-bit hash value.
 *
 * @throws    None.
 *
 * @note      This function can be evaluated at compile time if the
 *            input is a constant expression.
 *
 * @warning   Different inputs may produce the same hash (collision).
 *            Use string comparison as fallback when hashes match.
 *
 * @since     0.1
 * @ingroup   base_hash
 */
constexpr uint64_t fnv1a64(std::string_view sv, uint64_t seed = fnv1a64_offset_basis) {
    uint64_t result = seed;
    for (char c : sv) {
        result = (result ^ static_cast<uint64_t>(c)) * fnv1a64_prime;
    }
    return result;
}

/**
 * @brief  FNV-1a 32-bit hash constexpr implementation.
 *
 * @param[in]  str Null-terminated string to hash.
 * @param[in]  seed Starting hash value (default: FNV offset basis).
 *
 * @return     32-bit hash value.
 *
 * @throws     None.
 *
 * @note       The hash is computed at compile time for string literals.
 *
 * @warning    Different inputs may produce the same hash (collision).
 *            Use string comparison as fallback when hashes match.
 *
 * @since      0.1
 * @ingroup    base_hash
 */
constexpr uint32_t fnv1a32(const char* str, uint32_t seed = fnv1a32_offset_basis) {
    return (*str == 0) ? seed
                       : fnv1a32(str + 1, (seed ^ static_cast<uint32_t>(*str)) * fnv1a32_prime);
}

/**
 * @brief  FNV-1a 32-bit hash for std::string_view.
 *
 * @param[in]  sv String view to hash.
 * @param[in]  seed Starting hash value (default: FNV offset basis).
 *
 * @return    32-bit hash value.
 *
 * @throws    None.
 *
 * @note      This function can be evaluated at compile time if the
 *            input is a constant expression.
 *
 * @warning   Different inputs may produce the same hash (collision).
 *            Use string comparison as fallback when hashes match.
 *
 * @since     0.1
 * @ingroup   base_hash
 */
constexpr uint32_t fnv1a32(std::string_view sv, uint32_t seed = fnv1a32_offset_basis) {
    uint32_t result = seed;
    for (char c : sv) {
        result = (result ^ static_cast<uint32_t>(c)) * fnv1a32_prime;
    }
    return result;
}

/**
 * @brief  User-defined literal for compile-time FNV-1a 64-bit hashing.
 *
 * Enables syntax: `"TokenName"_hash` for compile-time hash computation.
 *
 * @param[in]  str Null-terminated string to hash.
 * @param[in]  len Length of the string (unused, required by UDL syntax).
 *
 * @return     64-bit hash value.
 *
 * @throws     None.
 *
 * @note       This function is constexpr and evaluated at compile time.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    base_hash
 *
 * @code
 * constexpr uint64_t h = "MyToken"_hash;  // Compile-time hash
 * static_assert(h == fnv1a64("MyToken"), "Hash must match");
 * @endcode
 */
constexpr uint64_t operator""_hash(const char* str, size_t) {
    return fnv1a64(str);
}

} // namespace cf::hash
