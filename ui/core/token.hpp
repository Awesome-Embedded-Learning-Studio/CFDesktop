/**
 * @file    token.hpp
 * @brief   High-performance type-safe token system with runtime registration.
 *
 * Provides a dual-mode token system:
 * 1. StaticToken: Compile-time type-safe tokens with zero overhead
 * 2. DynamicToken: Runtime type-erased tokens using std::any
 * 3. TokenRegistry: Read-safe registry using shared_mutex for read-heavy workloads
 *
 * @author  Charliechen114514
 * @date    2026-02-25
 * @version 0.1
 * @since   0.1
 * @ingroup ui_core
 */
#pragma once

#include <any>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "base/expected/expected.hpp"
#include "base/hash/constexpr_fnv1a.hpp"

namespace cf::ui::core {

// =============================================================================
// Forward Declarations
// =============================================================================
class TokenRegistry;
/// @brief Compile-time type-safe token with zero runtime overhead.
template <typename T, uint64_t Hash> class StaticToken;

// =============================================================================
// Error Types
// =============================================================================
/**
 * @brief  Error type for Token operations.
 *
 * @since  0.1
 * @ingroup ui_core
 */
struct TokenError {
    /**
     * @brief  Error kind enumeration.
     *
     * @since  0.1
     */
    enum class Kind {
        NotFound,          ///< Token not found in registry
        TypeMismatch,      ///< Type mismatch during get<T>()
        AlreadyRegistered, ///< Token already registered
        Empty              ///< Token has no value
    } kind = Kind::NotFound;

    /**
     * @brief  Human-readable error message.
     *
     * @since  0.1
     */
    std::string message;

    /**
     * @brief  Default constructor.
     *
     * @since  0.1
     */
    TokenError() = default;

    /**
     * @brief  Construct from kind and message.
     *
     * @param  k Error kind.
     * @param  msg Error message.
     *
     * @since  0.1
     */
    TokenError(Kind k, std::string msg) : kind(k), message(std::move(msg)) {}

    /**
     * @brief  Implicit bool conversion for error checking.
     *
     * @return true if this represents an error.
     *
     * @since  0.1
     */
    explicit operator bool() const noexcept { return kind != Kind::NotFound; }
};

// =============================================================================
// StaticToken - Compile-Time Type-Safe Token
// =============================================================================

/**
 * @brief  Compile-time type-safe token with zero runtime overhead.
 *
 * StaticToken provides compile-time type safety and hash-based lookup.
 * The token type and hash are template parameters, enabling complete
 * compiler optimization.
 *
 * @tparam T Value type stored in this token.
 * @tparam Hash Compile-time hash of the token name.
 *
 * @ingroup ui_core
 *
 * @note    Thread-safe for all operations via TokenRegistry's shared_mutex.
 *
 * @warning Do not use the same Hash value with different T types.
 *
 * @code
 * // Define a token at compile time
 * using CounterToken = StaticToken<int, "counter"_hash>;
 *
 * // Register and use
 * TokenRegistry::get().register_token<CounterToken>(42);
 * auto result = CounterToken::get();
 * if (result) { int value = *result; }
 * @endcode
 */
template <typename T, uint64_t Hash> class StaticToken {
  public:
    /// @brief Value type stored in this token.
    using value_type = T;

    /// @brief Compile-time hash of the token name.
    static constexpr uint64_t hash_value = Hash;

    /// @brief Default constructor.
    StaticToken() = default;

    /// @brief Copy constructor is deleted.
    StaticToken(const StaticToken&) = delete;

    /// @brief Copy assignment operator is deleted.
    StaticToken& operator=(const StaticToken&) = delete;

    /**
     * @brief  Type-safe value accessor for registry.
     *
     * @tparam  T Value type stored in this token.
     * @return     cf::expected containing pointer to the token's value or TokenError.
     * @throws     None
     * @note       Thread-safe for concurrent reads.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_core
     */
    static cf::expected<T*, TokenError> get();

    /**
     * @brief  Const version of value accessor.
     *
     * @tparam     T Value type stored in this token.
     * @return     cf::expected containing const pointer to the token's value or TokenError.
     * @throws     None
     * @note       Thread-safe for concurrent reads.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_core
     */
    static cf::expected<const T*, TokenError> get_const();
};

// =============================================================================
// TokenRegistry - Shared-Mutex Protected Storage
// =============================================================================

namespace detail {

/**
 * @brief  Storage slot for a single token entry.
 *
 * @ingroup ui_core
 * @internal
 */
struct TokenSlot {
    std::unique_ptr<std::any> data;            ///< Type-erased token data.
    const std::type_info* type_info = nullptr; ///< Type identifier for validation.
    std::string name;                          ///< Human-readable name (debug).

    TokenSlot() = default;
    TokenSlot(const TokenSlot&) = delete;
    TokenSlot& operator=(const TokenSlot&) = delete;

    TokenSlot(TokenSlot&& other) noexcept
        : data(std::move(other.data)), type_info(other.type_info), name(std::move(other.name)) {}

    TokenSlot& operator=(TokenSlot&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            type_info = other.type_info;
            name = std::move(other.name);
        }
        return *this;
    }
};

} // namespace detail

/**
 * @brief  Thread-safe token registry.
 *
 * Uses std::shared_mutex to allow concurrent reads while serialising writes.
 * All get() calls hold the shared lock for the duration of the access,
 * which prevents use-after-free when a concurrent writer calls remove().
 *
 * @ingroup ui_core
 *
 * @note    Thread-safe for all operations.
 *
 * @warning Token registration is relatively expensive; design for
 *          infrequent writes.
 *
 * @code
 * auto& registry = TokenRegistry::get();
 * registry.register_token<StaticToken<int, "settings"_hash>>(42);
 * registry.register_dynamic("userId", 12345);
 * auto result = registry.get_dynamic<int>("userId");
 * if (result) { int id = *result; }
 * @endcode
 */
class TokenRegistry {
  public:
    /**
     * @brief  Result type for token operations.
     */
    template <typename T> using Result = cf::expected<T, TokenError>;

    /**
     * @brief  Gets the singleton registry instance.
     *
     * @return Reference to the singleton registry instance.
     * @throws None
     * @note   Thread-safe singleton initialization.
     * @warning None
     * @since  0.1
     * @ingroup ui_core
     */
    static TokenRegistry& get();

    // Non-copyable, non-movable
    TokenRegistry(const TokenRegistry&) = delete;
    TokenRegistry& operator=(const TokenRegistry&) = delete;
    TokenRegistry(TokenRegistry&&) = delete;
    TokenRegistry& operator=(TokenRegistry&&) = delete;

    /**
     * @brief  Registers a static token with a value.
     *
     * @tparam TokenToken StaticToken type (contains T and Hash).
     * @tparam Args Constructor argument types.
     * @param  args Arguments to construct the value.
     * @return     Result containing void or TokenError::AlreadyRegistered.
     */
    template <typename TokenToken, typename... Args> Result<void> register_token(Args&&... args);

    /**
     * @brief  Gets a static token's value.
     *
     * @tparam TokenToken StaticToken type.
     * @return     Result containing pointer to the token's value or TokenError.
     */
    template <typename TokenToken> Result<typename TokenToken::value_type*> get();

    /**
     * @brief  Gets a static token's value (const).
     *
     * @tparam TokenToken StaticToken type.
     * @return     Result containing const pointer to the token's value or TokenError.
     */
    template <typename TokenToken> Result<const typename TokenToken::value_type*> get_const() const;

    /**
     * @brief  Registers a dynamic token (forwarding constructor).
     * @return Result containing void or TokenError::AlreadyRegistered.
     */
    template <typename T, typename... Args>
    Result<void> register_dynamic(std::string_view name, Args&&... args);

    /**
     * @brief  Registers a dynamic token (copy).
     * @return Result containing void or TokenError::AlreadyRegistered.
     */
    template <typename T> Result<void> register_dynamic(std::string_view name, const T& value);

    /**
     * @brief  Registers a dynamic token (move).
     * @return Result containing void or TokenError::AlreadyRegistered.
     */
    template <typename T> Result<void> register_dynamic(std::string_view name, T&& value);

    /**
     * @brief  Gets a dynamic token's value.
     * @return Result containing pointer to the token's value or TokenError.
     */
    template <typename T> Result<T*> get_dynamic(std::string_view name);

    /**
     * @brief  Gets a dynamic token's value by hash.
     * @return Result containing pointer to the token's value or TokenError.
     */
    template <typename T> Result<T*> get_dynamic_by_hash(uint64_t hash);

    /**
     * @brief  Gets a dynamic token's value (const).
     * @return Result containing const pointer to the token's value or TokenError.
     */
    template <typename T> Result<const T*> get_dynamic_const(std::string_view name) const;

    /**
     * @brief  Checks if a token exists by hash.
     * @return true if the token exists, false otherwise.
     */
    bool contains(uint64_t hash) const noexcept;

    /**
     * @brief  Checks if a dynamic token exists by name.
     * @return true if the token exists, false otherwise.
     */
    bool contains(std::string_view name) const noexcept;

    /**
     * @brief  Removes a token from the registry by hash.
     * @return true if removed, false if not found.
     */
    bool remove(uint64_t hash);

    /**
     * @brief  Removes a dynamic token from the registry by name.
     * @return true if removed, false if not found.
     */
    bool remove(std::string_view name);

    /**
     * @brief  Gets the number of registered tokens.
     * @return Number of tokens in the registry.
     */
    size_t size() const noexcept;

  private:
    TokenRegistry() = default;
    ~TokenRegistry() = default;

    // Internal helpers — caller must already hold the appropriate lock.
    // Returns nullptr if not found. Lock must be held by caller.
    detail::TokenSlot* find_slot_locked(uint64_t hash);
    const detail::TokenSlot* find_slot_locked(uint64_t hash) const;

    // Shared implementation for get_dynamic / get_dynamic_by_hash (non-const).
    template <typename T> Result<T*> get_by_hash_impl(uint64_t hash, const std::string& name_hint);

    // Shared implementation for get_dynamic_const.
    template <typename T>
    Result<const T*> get_by_hash_impl_const(uint64_t hash, const std::string& name_hint) const;

    mutable std::shared_mutex registry_mutex_;
    std::unordered_map<uint64_t, detail::TokenSlot> slot_map_;
};

// =============================================================================
// Inline Implementations - StaticToken
// =============================================================================

/**
 * @brief  Type-safe value accessor for registry (implementation).
 *
 * @tparam  T Value type stored in this token.
 * @tparam  Hash Compile-time hash of the token name.
 * @return     cf::expected containing pointer to the token's value or TokenError.
 * @throws     None
 * @note       Delegates to TokenRegistry::get().
 * @warning    None
 * @since      0.1
 * @ingroup    ui_core
 */
template <typename T, uint64_t Hash>
auto StaticToken<T, Hash>::get() -> cf::expected<T*, TokenError> {
    return TokenRegistry::get().get<StaticToken<T, Hash>>();
}

/**
 * @brief  Const version of value accessor (implementation).
 *
 * @tparam     T Value type stored in this token.
 * @tparam     Hash Compile-time hash of the token name.
 * @return     cf::expected containing const pointer to the token's value or TokenError.
 * @throws     None
 * @note       Delegates to TokenRegistry::get_const().
 * @warning    None
 * @since      0.1
 * @ingroup    ui_core
 */
template <typename T, uint64_t Hash>
auto StaticToken<T, Hash>::get_const() -> cf::expected<const T*, TokenError> {
    return TokenRegistry::get().get_const<StaticToken<T, Hash>>();
}

// =============================================================================
// Inline Implementations - TokenRegistry
// =============================================================================

/**
 * @brief  Gets the singleton TokenRegistry instance.
 *
 * @return Reference to the singleton registry instance.
 * @throws None
 * @note   Thread-safe singleton initialization.
 * @warning None
 * @since  0.1
 * @ingroup ui_core
 */
inline TokenRegistry& TokenRegistry::get() {
    static TokenRegistry instance;
    return instance;
}

/**
 * @brief  Gets the number of registered tokens.
 *
 * @return Number of tokens in the registry.
 * @throws None
 * @note   Thread-safe for concurrent reads.
 * @warning None
 * @since  0.1
 * @ingroup ui_core
 */
inline size_t TokenRegistry::size() const noexcept {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return slot_map_.size();
}

/**
 * @brief  Checks if a token exists by hash.
 *
 * @param[in] hash Hash value of the token to check.
 * @return        true if the token exists, false otherwise.
 * @throws        None
 * @note          Thread-safe for concurrent reads.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool TokenRegistry::contains(uint64_t hash) const noexcept {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return slot_map_.find(hash) != slot_map_.end();
}

/**
 * @brief  Checks if a token exists by name.
 *
 * @param[in] name Name of the token to check.
 * @return        true if the token exists, false otherwise.
 * @throws        None
 * @note          Thread-safe for concurrent reads.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool TokenRegistry::contains(std::string_view name) const noexcept {
    return contains(cf::hash::fnv1a64(name));
}

/**
 * @brief  Removes a token from the registry by hash.
 *
 * @param[in] hash Hash value of the token to remove.
 * @return        true if the token was removed, false if not found.
 * @throws        None
 * @note          Thread-safe for exclusive access.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool TokenRegistry::remove(uint64_t hash) {
    std::unique_lock<std::shared_mutex> lock(registry_mutex_);
    return slot_map_.erase(hash) > 0;
}

/**
 * @brief  Removes a token from the registry by name.
 *
 * @param[in] name Name of the token to remove.
 * @return        true if the token was removed, false if not found.
 * @throws        None
 * @note          Thread-safe for exclusive access.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool TokenRegistry::remove(std::string_view name) {
    return remove(cf::hash::fnv1a64(name));
}

// These helpers are only called while the caller already holds the mutex.
inline detail::TokenSlot* TokenRegistry::find_slot_locked(uint64_t hash) {
    auto it = slot_map_.find(hash);
    return (it != slot_map_.end()) ? &it->second : nullptr;
}

inline const detail::TokenSlot* TokenRegistry::find_slot_locked(uint64_t hash) const {
    auto it = slot_map_.find(hash);
    return (it != slot_map_.end()) ? &it->second : nullptr;
}

// -----------------------------------------------------------------------------
// Static Token Registration
// -----------------------------------------------------------------------------

template <typename TokenToken, typename... Args>
auto TokenRegistry::register_token(Args&&... args) -> Result<void> {
    using T = typename TokenToken::value_type;
    constexpr uint64_t hash = TokenToken::hash_value;

    std::unique_lock<std::shared_mutex> lock(registry_mutex_);

    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(
            TokenError{TokenError::Kind::AlreadyRegistered,
                       "Token already registered, hash: " + std::to_string(hash)});
    }

    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(T(std::forward<Args>(args)...));
    slot.type_info = &typeid(T);
    slot.name = "static_token_" + std::to_string(hash);

    slot_map_.emplace(hash, std::move(slot));
    return {};
}

// -----------------------------------------------------------------------------
// Static Token Get
// -----------------------------------------------------------------------------

template <typename TokenToken>
auto TokenRegistry::get() -> Result<typename TokenToken::value_type*> {
    using T = typename TokenToken::value_type;
    constexpr uint64_t hash = TokenToken::hash_value;

    // Hold shared lock for the entire operation to prevent remove() from
    // destroying the slot while reading from it.
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);

    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(TokenError{TokenError::Kind::NotFound,
                                         "Token not found, hash: " + std::to_string(hash)});
    }

    std::any* a = slot->data.get();
    if (!a || !a->has_value()) {
        return cf::unexpected(TokenError{TokenError::Kind::Empty, "Token has no value"});
    }

    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(
            TokenError{TokenError::Kind::TypeMismatch, "Type mismatch for token"});
    }

    return std::any_cast<T>(a);
}

template <typename TokenToken>
auto TokenRegistry::get_const() const -> Result<const typename TokenToken::value_type*> {
    using T = typename TokenToken::value_type;
    constexpr uint64_t hash = TokenToken::hash_value;

    std::shared_lock<std::shared_mutex> lock(registry_mutex_);

    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(TokenError{TokenError::Kind::NotFound,
                                         "Token not found, hash: " + std::to_string(hash)});
    }

    const std::any* a = slot->data.get();
    if (!a || !a->has_value()) {
        return cf::unexpected(TokenError{TokenError::Kind::Empty, "Token has no value"});
    }

    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(
            TokenError{TokenError::Kind::TypeMismatch, "Type mismatch for token"});
    }

    return std::any_cast<T>(a);
}

// -----------------------------------------------------------------------------
// Dynamic Token Registration
// -----------------------------------------------------------------------------

template <typename T, typename... Args>
auto TokenRegistry::register_dynamic(std::string_view name, Args&&... args) -> Result<void> {
    uint64_t hash = cf::hash::fnv1a64(name);

    std::unique_lock<std::shared_mutex> lock(registry_mutex_);

    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(TokenError{TokenError::Kind::AlreadyRegistered,
                                         "Already registered: " + std::string(name)});
    }

    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(T(std::forward<Args>(args)...));
    slot.type_info = &typeid(T);
    slot.name = name;

    slot_map_.emplace(hash, std::move(slot));
    return {};
}

template <typename T>
auto TokenRegistry::register_dynamic(std::string_view name, const T& value) -> Result<void> {
    uint64_t hash = cf::hash::fnv1a64(name);

    std::unique_lock<std::shared_mutex> lock(registry_mutex_);

    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(TokenError{TokenError::Kind::AlreadyRegistered,
                                         "Already registered: " + std::string(name)});
    }

    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(value);
    slot.type_info = &typeid(T);
    slot.name = name;

    slot_map_.emplace(hash, std::move(slot));
    return {};
}

template <typename T>
auto TokenRegistry::register_dynamic(std::string_view name, T&& value) -> Result<void> {
    uint64_t hash = cf::hash::fnv1a64(name);

    std::unique_lock<std::shared_mutex> lock(registry_mutex_);

    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(TokenError{TokenError::Kind::AlreadyRegistered,
                                         "Already registered: " + std::string(name)});
    }

    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(std::forward<T>(value));
    slot.type_info = &typeid(T);
    slot.name = name;

    slot_map_.emplace(hash, std::move(slot));
    return {};
}

// -----------------------------------------------------------------------------
// Dynamic Token Get — shared impl
// -----------------------------------------------------------------------------

template <typename T>
auto TokenRegistry::get_by_hash_impl(uint64_t hash, const std::string& name_hint) -> Result<T*> {
    // Lock held by callers (get_dynamic / get_dynamic_by_hash) for entire scope.
    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(
            TokenError{TokenError::Kind::NotFound, "Token not found: " + name_hint});
    }

    std::any* a = slot->data.get();
    if (!a || !a->has_value()) {
        return cf::unexpected(
            TokenError{TokenError::Kind::Empty, "Token has no value: " + name_hint});
    }

    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(
            TokenError{TokenError::Kind::TypeMismatch, "Type mismatch for token: " + name_hint});
    }

    return std::any_cast<T>(a);
}

template <typename T>
auto TokenRegistry::get_by_hash_impl_const(uint64_t hash,
                                           const std::string& name_hint) const -> Result<const T*> {
    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(
            TokenError{TokenError::Kind::NotFound, "Token not found: " + name_hint});
    }

    const std::any* a = slot->data.get();
    if (!a || !a->has_value()) {
        return cf::unexpected(
            TokenError{TokenError::Kind::Empty, "Token has no value: " + name_hint});
    }

    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(
            TokenError{TokenError::Kind::TypeMismatch, "Type mismatch for token: " + name_hint});
    }

    return std::any_cast<T>(a);
}

template <typename T> auto TokenRegistry::get_dynamic(std::string_view name) -> Result<T*> {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return get_by_hash_impl<T>(cf::hash::fnv1a64(name), std::string(name));
}

template <typename T> auto TokenRegistry::get_dynamic_by_hash(uint64_t hash) -> Result<T*> {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return get_by_hash_impl<T>(hash, std::to_string(hash));
}

template <typename T>
auto TokenRegistry::get_dynamic_const(std::string_view name) const -> Result<const T*> {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return get_by_hash_impl_const<T>(cf::hash::fnv1a64(name), std::string(name));
}

// =============================================================================
// EmbeddedTokenRegistry - Non-Singleton Embeddable Registry
// =============================================================================

/**
 * @brief  Embeddable token registry for component-scoped token storage.
 *
 * Provides the same API as TokenRegistry but without singleton semantics.
 * Can be embedded in other classes for independent token management.
 *
 * @ingroup ui_core
 *
 * @note    Thread-safe for all operations.
 *
 * @code
 * class MyComponent {
 * public:
 *     EmbeddedTokenRegistry registry;
 *     void init() { registry.register_dynamic<int>("counter", 0); }
 * };
 * @endcode
 */
class EmbeddedTokenRegistry {
  public:
    template <typename T> using Result = cf::expected<T, TokenError>;

    EmbeddedTokenRegistry() = default;
    ~EmbeddedTokenRegistry() = default;

    EmbeddedTokenRegistry(const EmbeddedTokenRegistry&) = delete;
    EmbeddedTokenRegistry& operator=(const EmbeddedTokenRegistry&) = delete;

    EmbeddedTokenRegistry(EmbeddedTokenRegistry&& other) noexcept
        : slot_map_(std::move(other.slot_map_)) {}

    EmbeddedTokenRegistry& operator=(EmbeddedTokenRegistry&& other) noexcept {
        if (this != &other)
            slot_map_ = std::move(other.slot_map_);
        return *this;
    }

    template <typename T, typename... Args>
    Result<void> register_dynamic(std::string_view name, Args&&... args);

    template <typename T> Result<void> register_dynamic(std::string_view name, const T& value);
    template <typename T> Result<void> register_dynamic(std::string_view name, T&& value);

    /**
     * @brief  Gets a dynamic token's value by name.
     * @return Result containing pointer to the token's value or TokenError.
     */
    template <typename T> Result<T*> get_dynamic(std::string_view name);
    /**
     * @brief  Gets a dynamic token's value by hash.
     * @return Result containing pointer to the token's value or TokenError.
     */
    template <typename T> Result<T*> get_dynamic_by_hash(uint64_t hash);
    /**
     * @brief  Gets a dynamic token's value by name (const).
     * @return Result containing const pointer to the token's value or TokenError.
     */
    template <typename T> Result<const T*> get_dynamic_const(std::string_view name) const;

    /**
     * @brief  Checks if a token exists by hash.
     * @return true if the token exists, false otherwise.
     */
    bool contains(uint64_t hash) const noexcept;
    /**
     * @brief  Checks if a token exists by name.
     * @return true if the token exists, false otherwise.
     */
    bool contains(std::string_view name) const noexcept;
    /**
     * @brief  Removes a token from the registry by hash.
     * @return true if the token was removed, false if not found.
     */
    bool remove(uint64_t hash);
    /**
     * @brief  Removes a token from the registry by name.
     * @return true if the token was removed, false if not found.
     */
    bool remove(std::string_view name);
    /**
     * @brief  Gets the number of registered tokens.
     * @return Number of tokens in the registry.
     */
    size_t size() const noexcept;

  private:
    detail::TokenSlot* find_slot_locked(uint64_t hash);
    const detail::TokenSlot* find_slot_locked(uint64_t hash) const;

    template <typename T> Result<T*> get_by_hash_impl(uint64_t hash, const std::string& name_hint);

    template <typename T>
    Result<const T*> get_by_hash_impl_const(uint64_t hash, const std::string& name_hint) const;

    mutable std::shared_mutex registry_mutex_;
    std::unordered_map<uint64_t, detail::TokenSlot> slot_map_;
};

// =============================================================================
// Inline Implementations - EmbeddedTokenRegistry
// =============================================================================

/**
 * @brief  Gets the number of registered tokens.
 *
 * @return Number of tokens in the registry.
 * @throws None
 * @note   Thread-safe for concurrent reads.
 * @warning None
 * @since  0.1
 * @ingroup ui_core
 */
inline size_t EmbeddedTokenRegistry::size() const noexcept {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return slot_map_.size();
}

/**
 * @brief  Checks if a token exists by hash.
 *
 * @param[in] hash Hash value of the token to check.
 * @return        true if the token exists, false otherwise.
 * @throws        None
 * @note          Thread-safe for concurrent reads.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool EmbeddedTokenRegistry::contains(uint64_t hash) const noexcept {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return slot_map_.find(hash) != slot_map_.end();
}

/**
 * @brief  Checks if a token exists by name.
 *
 * @param[in] name Name of the token to check.
 * @return        true if the token exists, false otherwise.
 * @throws        None
 * @note          Thread-safe for concurrent reads.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool EmbeddedTokenRegistry::contains(std::string_view name) const noexcept {
    return contains(cf::hash::fnv1a64(name));
}

/**
 * @brief  Removes a token from the registry by hash.
 *
 * @param[in] hash Hash value of the token to remove.
 * @return        true if the token was removed, false if not found.
 * @throws        None
 * @note          Thread-safe for exclusive access.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool EmbeddedTokenRegistry::remove(uint64_t hash) {
    std::unique_lock<std::shared_mutex> lock(registry_mutex_);
    return slot_map_.erase(hash) > 0;
}

/**
 * @brief  Removes a token from the registry by name.
 *
 * @param[in] name Name of the token to remove.
 * @return        true if the token was removed, false if not found.
 * @throws        None
 * @note          Thread-safe for exclusive access.
 * @warning       None
 * @since         0.1
 * @ingroup       ui_core
 */
inline bool EmbeddedTokenRegistry::remove(std::string_view name) {
    return remove(cf::hash::fnv1a64(name));
}

inline detail::TokenSlot* EmbeddedTokenRegistry::find_slot_locked(uint64_t hash) {
    auto it = slot_map_.find(hash);
    return (it != slot_map_.end()) ? &it->second : nullptr;
}

inline const detail::TokenSlot* EmbeddedTokenRegistry::find_slot_locked(uint64_t hash) const {
    auto it = slot_map_.find(hash);
    return (it != slot_map_.end()) ? &it->second : nullptr;
}

template <typename T, typename... Args> auto
EmbeddedTokenRegistry::register_dynamic(std::string_view name, Args&&... args) -> Result<void> {
    uint64_t hash = cf::hash::fnv1a64(name);
    std::unique_lock<std::shared_mutex> lock(registry_mutex_);
    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(TokenError{TokenError::Kind::AlreadyRegistered,
                                         "Already registered: " + std::string(name)});
    }
    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(T(std::forward<Args>(args)...));
    slot.type_info = &typeid(T);
    slot.name = name;
    slot_map_.emplace(hash, std::move(slot));
    return {};
}

template <typename T> auto EmbeddedTokenRegistry::register_dynamic(std::string_view name,
                                                                   const T& value) -> Result<void> {
    uint64_t hash = cf::hash::fnv1a64(name);
    std::unique_lock<std::shared_mutex> lock(registry_mutex_);
    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(TokenError{TokenError::Kind::AlreadyRegistered,
                                         "Already registered: " + std::string(name)});
    }
    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(value);
    slot.type_info = &typeid(T);
    slot.name = name;
    slot_map_.emplace(hash, std::move(slot));
    return {};
}

template <typename T>
auto EmbeddedTokenRegistry::register_dynamic(std::string_view name, T&& value) -> Result<void> {
    uint64_t hash = cf::hash::fnv1a64(name);
    std::unique_lock<std::shared_mutex> lock(registry_mutex_);
    if (slot_map_.find(hash) != slot_map_.end()) {
        return cf::unexpected(TokenError{TokenError::Kind::AlreadyRegistered,
                                         "Already registered: " + std::string(name)});
    }
    detail::TokenSlot slot;
    slot.data = std::make_unique<std::any>(std::forward<T>(value));
    slot.type_info = &typeid(T);
    slot.name = name;
    slot_map_.emplace(hash, std::move(slot));
    return {};
}

template <typename T> auto
EmbeddedTokenRegistry::get_by_hash_impl(uint64_t hash, const std::string& name_hint) -> Result<T*> {
    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(
            TokenError{TokenError::Kind::NotFound, "Token not found: " + name_hint});
    }
    std::any* a = slot->data.get();
    if (!a || !a->has_value()) {
        return cf::unexpected(
            TokenError{TokenError::Kind::Empty, "Token has no value: " + name_hint});
    }
    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(
            TokenError{TokenError::Kind::TypeMismatch, "Type mismatch for token: " + name_hint});
    }
    return std::any_cast<T>(a);
}

template <typename T> auto EmbeddedTokenRegistry::get_by_hash_impl_const(
    uint64_t hash, const std::string& name_hint) const -> Result<const T*> {
    const detail::TokenSlot* slot = find_slot_locked(hash);
    if (!slot) {
        return cf::unexpected(
            TokenError{TokenError::Kind::NotFound, "Token not found: " + name_hint});
    }
    const std::any* a = slot->data.get();
    if (!a || !a->has_value()) {
        return cf::unexpected(
            TokenError{TokenError::Kind::Empty, "Token has no value: " + name_hint});
    }
    if (slot->type_info != &typeid(T)) {
        return cf::unexpected(
            TokenError{TokenError::Kind::TypeMismatch, "Type mismatch for token: " + name_hint});
    }
    return std::any_cast<T>(a);
}

template <typename T> auto EmbeddedTokenRegistry::get_dynamic(std::string_view name) -> Result<T*> {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return get_by_hash_impl<T>(cf::hash::fnv1a64(name), std::string(name));
}

template <typename T> auto EmbeddedTokenRegistry::get_dynamic_by_hash(uint64_t hash) -> Result<T*> {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return get_by_hash_impl<T>(hash, std::to_string(hash));
}

template <typename T>
auto EmbeddedTokenRegistry::get_dynamic_const(std::string_view name) const -> Result<const T*> {
    std::shared_lock<std::shared_mutex> lock(registry_mutex_);
    return get_by_hash_impl_const<T>(cf::hash::fnv1a64(name), std::string(name));
}

} // namespace cf::ui::core