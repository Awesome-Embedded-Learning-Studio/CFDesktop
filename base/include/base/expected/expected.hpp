/**
 * @file    base/include/base/expected/expected.hpp
 * @brief   Provides a C++17 implementation of std::expected (C++23).
 *
 * Offers a type-safe error handling mechanism that uses values instead of
 * exceptions. Compatible with compilers that do not yet support C++23.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup base_utilities
 */
#pragma once

#include <exception>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace cf {

/**
 * @brief  Exception thrown when accessing the value of an expected that
 *         contains an error.
 *
 * @ingroup base_utilities
 */
template <typename E> class bad_expected_access;

/**
 * @brief  Specialization of bad_expected_access for void error type.
 *
 * @ingroup base_utilities
 */
template <> class bad_expected_access<void> : public std::exception {
  public:
    /**
     * @brief  Returns the explanatory string.
     *
     * @return     Pointer to "bad_expected_access" string.
     *
     * @throws     None.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    const char* what() const noexcept override { return "bad_expected_access"; }
};

/**
 * @brief  Exception thrown when accessing the value of an expected in error
 *         state.
 *
 * @tparam E Error type stored in the expected.
 *
 * @ingroup base_utilities
 */
template <typename E> class bad_expected_access : public bad_expected_access<void> {
  public:
    /**
     * @brief  Constructs the exception with an error value.
     *
     * @param[in] e Error value to store.
     *
     * @ingroup base_utilities
     */
    explicit bad_expected_access(E e) : error_(std::move(e)) {}

    /**
     * @brief  Returns a const reference to the stored error.
     *
     * @return     Const reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    const E& error() const& noexcept { return error_; }

    /**
     * @brief  Returns a reference to the stored error.
     *
     * @return     Reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    E& error() & noexcept { return error_; }

    /**
     * @brief  Returns a const rvalue reference to the stored error.
     *
     * @return     Const rvalue reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    const E&& error() const&& noexcept { return std::move(error_); }

    /**
     * @brief  Returns an rvalue reference to the stored error.
     *
     * @return     Rvalue reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    E&& error() && noexcept { return std::move(error_); }

  private:
    /// Stored error value. Ownership: owner.
    E error_;
};

/**
 * @brief  Wrapper type for constructing expected objects in error state.
 *
 * @tparam E Error type to wrap.
 *
 * @ingroup base_utilities
 */
template <typename E> class unexpected {
    static_assert(!std::is_void_v<E>, "E must not be void");
    static_assert(!std::is_reference_v<E>, "E must not be a reference");

  public:
    /// Deleted default constructor.
    unexpected() = delete;

    /**
     * @brief  Constructs unexpected from an error value.
     *
     * @tparam Err Forwarded error type.
     * @param[in] e Error value to wrap.
     *
     * @ingroup base_utilities
     */
    template <typename Err = E,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<Err>, unexpected> &&
                                          std::is_constructible_v<E, Err>>>
    constexpr explicit unexpected(Err&& e) : error_(std::forward<Err>(e)) {}

    /**
     * @brief  Constructs unexpected in-place with arguments.
     *
     * @tparam Args Constructor argument types.
     * @param[in] args Arguments to forward to E constructor.
     *
     * @ingroup base_utilities
     */
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
    constexpr explicit unexpected(std::in_place_t, Args&&... args)
        : error_(std::forward<Args>(args)...) {}

    /**
     * @brief  Constructs unexpected in-place with initializer list.
     *
     * @tparam U  Element type of initializer list.
     * @tparam Args Constructor argument types.
     * @param[in] il  Initializer list.
     * @param[in] args Additional arguments.
     *
     * @ingroup base_utilities
     */
    template <
        typename U, typename... Args,
        typename = std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
    constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        : error_(il, std::forward<Args>(args)...) {}

    /**
     * @brief  Returns const reference to the error.
     *
     * @return     Const reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr const E& error() const& noexcept { return error_; }

    /**
     * @brief  Returns reference to the error.
     *
     * @return     Reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr E& error() & noexcept { return error_; }

    /**
     * @brief  Returns const rvalue reference to the error.
     *
     * @return     Const rvalue reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr const E&& error() const&& noexcept { return std::move(error_); }

    /**
     * @brief  Returns rvalue reference to the error.
     *
     * @return     Rvalue reference to the error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr E&& error() && noexcept { return std::move(error_); }

    /**
     * @brief  Swaps the error with another unexpected.
     *
     * @param[in,out] other Other unexpected to swap with.
     *
     * @throws     None if E is nothrow swappable.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>) {
        using std::swap;
        swap(error_, other.error_);
    }

    /**
     * @brief  Compares two unexpected objects for equality.
     *
     * @tparam E2 Error type of the other unexpected.
     * @param[in] a First unexpected.
     * @param[in] b Second unexpected.
     *
     * @return     true if errors are equal, false otherwise.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename E2>
    friend constexpr bool operator==(const unexpected& a, const unexpected<E2>& b) {
        return a.error_ == b.error();
    }

  private:
    /// Stored error value. Ownership: owner.
    E error_;
};

/// Deduction guide for unexpected template constructor.
/// @cond DeductionGuides
template <typename E> unexpected(E) -> unexpected<E>;
/// @endcond

/**
 * @brief  Tag type for constructing expected in error state.
 *
 * @ingroup base_utilities
 */
struct unexpect_t {
    /**
     * @brief  Default constructor.
     *
     * @ingroup base_utilities
     */
    explicit unexpect_t() = default;
};

/// Global unexpect tag instance.
inline constexpr unexpect_t unexpect{};

/**
 * @brief  A wrapper that contains either a value or an error.
 *
 * Provides type-safe error handling without exceptions. The object either
 * contains a value of type T or an error of type E.
 *
 * @tparam T Value type.
 * @tparam E Error type.
 *
 * @ingroup base_utilities
 */
template <typename T, typename E> class expected {
    static_assert(!std::is_void_v<E>, "E must not be void");
    static_assert(!std::is_reference_v<T>,
                  "T must not be a reference (use T* or std::reference_wrapper)");
    static_assert(!std::is_reference_v<E>, "E must not be a reference");

    /**
     * @brief  Internal storage union for value or error.
     *
     * Discriminated union that holds either the value or the error.
     * Only one member is active at any time.
     *
     * @note       Trivial (no explicit initialization).
     *
     * @warning    Direct access to members is unsafe; use has_val_ to
     *             determine which member is active.
     *
     * @since      0.1
     * @ingroup    base_utilities
     * @internal
     */
    union Storage {
        T val; ///< Value storage.

        E err; ///< Error storage.

        /**
         * @brief  Default constructor.
         *
         * @throws     None.
         *
         * @since      0.1
         * @ingroup    base_utilities
         */
        Storage() {}

        /**
         * @brief  Destructor.
         *
         * @throws     None.
         *
         * @since      0.1
         * @ingroup    base_utilities
         */
        ~Storage() {}
    };

    Storage storage_; ///< Internal storage.
    bool has_val_;    ///< Indicates whether value is present.

    /**
     * @brief  Destroys the currently stored value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    void destroy() noexcept {
        if (has_val_)
            storage_.val.~T();
        else
            storage_.err.~E();
    }

  public:
    using value_type = T;                  ///< Value type alias.
    using error_type = E;                  ///< Error type alias.
    using unexpected_type = unexpected<E>; ///< Unexpected type alias.

    /**
     * @brief  Default constructor. Value-initializes T.
     *
     * @throws     May throw if T default constructor throws.
     *
     * @ingroup base_utilities
     */
    constexpr expected() noexcept(std::is_nothrow_default_constructible_v<T>) : has_val_(true) {
        ::new (&storage_.val) T();
    }

    /**
     * @brief  Copy constructor.
     *
     * @param[in] o Expected to copy from.
     *
     * @throws     May throw if T or E copy constructor throws.
     *
     * @ingroup base_utilities
     */
    expected(const expected& o) : has_val_(o.has_val_) {
        if (has_val_)
            ::new (&storage_.val) T(o.storage_.val);
        else
            ::new (&storage_.err) E(o.storage_.err);
    }

    /**
     * @brief  Move constructor.
     *
     * @param[in] o Expected to move from.
     *
     * @throws     May throw if T or E move constructor throws.
     *
     * @ingroup base_utilities
     */
    expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                    std::is_nothrow_move_constructible_v<E>)
        : has_val_(o.has_val_) {
        if (has_val_)
            ::new (&storage_.val) T(std::move(o.storage_.val));
        else
            ::new (&storage_.err) E(std::move(o.storage_.err));
    }

    /**
     * @brief  Converting constructor from value.
     *
     * @tparam U Forwarded value type.
     * @param[in] v Value to construct with.
     *
     * @throws     May throw if T constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename U = T,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, expected> &&
                                          !std::is_same_v<std::decay_t<U>, std::in_place_t> &&
                                          !std::is_same_v<std::decay_t<U>, unexpect_t> &&
                                          std::is_constructible_v<T, U>>>
    constexpr expected(U&& v) : has_val_(true) {
        ::new (&storage_.val) T(std::forward<U>(v));
    }

    /**
     * @brief  Constructor from unexpected.
     *
     * @tparam G Forwarded error type.
     * @param[in] u Unexpected value containing error.
     *
     * @throws     May throw if E constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename G = E, typename = std::enable_if_t<std::is_constructible_v<E, const G&>>>
    constexpr expected(const unexpected<G>& u) : has_val_(false) {
        ::new (&storage_.err) E(u.error());
    }

    /**
     * @brief  Constructor from unexpected rvalue.
     *
     * @tparam G Forwarded error type.
     * @param[in] u Unexpected value containing error.
     *
     * @throws     May throw if E constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename G = E, typename = std::enable_if_t<std::is_constructible_v<E, G>>>
    constexpr expected(unexpected<G>&& u) : has_val_(false) {
        ::new (&storage_.err) E(std::move(u.error()));
    }

    /**
     * @brief  In-place constructor for value.
     *
     * @tparam Args Constructor argument types.
     * @param[in] args Arguments to forward to T constructor.
     *
     * @throws     May throw if T constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit expected(std::in_place_t, Args&&... args) : has_val_(true) {
        ::new (&storage_.val) T(std::forward<Args>(args)...);
    }

    /**
     * @brief  Constructor for in-place error construction.
     *
     * @tparam Args Constructor argument types.
     * @param[in] args Arguments to forward to E constructor.
     *
     * @throws     May throw if E constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
    constexpr explicit expected(unexpect_t, Args&&... args) : has_val_(false) {
        ::new (&storage_.err) E(std::forward<Args>(args)...);
    }

    /**
     * @brief  Destructor. Destroys contained value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    ~expected() { destroy(); }

    /**
     * @brief  Copy assignment operator.
     *
     * @param[in] o Expected to copy from.
     *
     * @return     Reference to this expected.
     *
     * @throws     May throw if assignment or construction throws.
     *
     * @ingroup base_utilities
     */
    expected& operator=(const expected& o) {
        if (this == &o)
            return *this;
        if (has_val_ && o.has_val_) {
            storage_.val = o.storage_.val;
        } else if (!has_val_ && !o.has_val_) {
            storage_.err = o.storage_.err;
        } else {
            destroy();
            has_val_ = o.has_val_;
            if (has_val_)
                ::new (&storage_.val) T(o.storage_.val);
            else
                ::new (&storage_.err) E(o.storage_.err);
        }
        return *this;
    }

    /**
     * @brief  Move assignment operator.
     *
     * @param[in] o Expected to move from.
     *
     * @return     Reference to this expected.
     *
     * @throws     May throw if assignment or construction throws.
     *
     * @ingroup base_utilities
     */
    expected& operator=(expected&& o) noexcept(std::is_nothrow_move_assignable_v<T> &&
                                               std::is_nothrow_move_assignable_v<E> &&
                                               std::is_nothrow_move_constructible_v<T> &&
                                               std::is_nothrow_move_constructible_v<E>) {
        if (this == &o)
            return *this;
        if (has_val_ && o.has_val_) {
            storage_.val = std::move(o.storage_.val);
        } else if (!has_val_ && !o.has_val_) {
            storage_.err = std::move(o.storage_.err);
        } else {
            destroy();
            has_val_ = o.has_val_;
            if (has_val_)
                ::new (&storage_.val) T(std::move(o.storage_.val));
            else
                ::new (&storage_.err) E(std::move(o.storage_.err));
        }
        return *this;
    }

    /**
     * @brief  Assignment from value.
     *
     * @tparam U Forwarded value type.
     * @param[in] v Value to assign.
     *
     * @return     Reference to this expected.
     *
     * @throws     May throw if assignment or construction throws.
     *
     * @ingroup base_utilities
     */
    template <typename U>
    std::enable_if_t<!std::is_same_v<std::decay_t<U>, expected> && std::is_constructible_v<T, U> &&
                         std::is_assignable_v<T&, U>,
                     expected&>
    operator=(U&& v) {
        if (has_val_) {
            storage_.val = std::forward<U>(v);
        } else {
            destroy();
            ::new (&storage_.val) T(std::forward<U>(v));
            has_val_ = true;
        }
        return *this;
    }

    /**
     * @brief  Assignment from unexpected.
     *
     * @tparam G Forwarded error type.
     * @param[in] u Unexpected value containing error.
     *
     * @return     Reference to this expected.
     *
     * @throws     May throw if assignment or construction throws.
     *
     * @ingroup base_utilities
     */
    template <typename G> expected& operator=(const unexpected<G>& u) {
        if (!has_val_) {
            storage_.err = u.error();
        } else {
            destroy();
            ::new (&storage_.err) E(u.error());
            has_val_ = false;
        }
        return *this;
    }

    /**
     * @brief  Assignment from unexpected rvalue.
     *
     * @tparam G Forwarded error type.
     * @param[in] u Unexpected value containing error.
     *
     * @return     Reference to this expected.
     *
     * @throws     May throw if assignment or construction throws.
     *
     * @ingroup base_utilities
     */
    template <typename G> expected& operator=(unexpected<G>&& u) {
        if (!has_val_) {
            storage_.err = std::move(u.error());
        } else {
            destroy();
            ::new (&storage_.err) E(std::move(u.error()));
            has_val_ = false;
        }
        return *this;
    }

    /**
     * @brief  Checks if the expected contains a value.
     *
     * @return     true if contains a value, false if contains error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr bool has_value() const noexcept { return has_val_; }

    /**
     * @brief  Boolean conversion operator.
     *
     * @return     true if contains a value, false if contains error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr explicit operator bool() const noexcept { return has_val_; }

    /**
     * @brief  Pointer operator.
     *
     * @return     Pointer to the contained value.
     *
     * @warning    Undefined behavior if expected does not contain a value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr T* operator->() noexcept { return &storage_.val; }

    /**
     * @brief  Const pointer operator.
     *
     * @return     Pointer to the contained value.
     *
     * @warning    Undefined behavior if expected does not contain a value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr const T* operator->() const noexcept { return &storage_.val; }

    /**
     * @brief  Dereference operator.
     *
     * @return     Reference to the contained value.
     *
     * @warning    Undefined behavior if expected does not contain a value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr T& operator*() & noexcept { return storage_.val; }

    /**
     * @brief  Const dereference operator.
     *
     * @return     Const reference to the contained value.
     *
     * @warning    Undefined behavior if expected does not contain a value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr const T& operator*() const& noexcept { return storage_.val; }

    /**
     * @brief  Rvalue dereference operator.
     *
     * @return     Rvalue reference to the contained value.
     *
     * @warning    Undefined behavior if expected does not contain a value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr T&& operator*() && noexcept { return std::move(storage_.val); }

    /**
     * @brief  Const rvalue dereference operator.
     *
     * @return     Const rvalue reference to the contained value.
     *
     * @warning    Undefined behavior if expected does not contain a value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr const T&& operator*() const&& noexcept { return std::move(storage_.val); }

    /**
     * @brief  Returns the contained value.
     *
     * @return     Reference to the contained value.
     *
     * @throws     bad_expected_access if expected contains an error.
     *
     * @warning    Throws bad_expected_access if this expected does not
     *             contain a value.
     *
     * @ingroup base_utilities
     */
    constexpr T& value() & {
        if (!has_val_)
            throw bad_expected_access<E>(storage_.err);
        return storage_.val;
    }

    /**
     * @brief  Returns the contained value (const lvalue).
     *
     * @return     Const reference to the contained value.
     *
     * @throws     bad_expected_access if expected contains an error.
     *
     * @warning    Throws bad_expected_access if this expected does not
     *             contain a value.
     *
     * @ingroup base_utilities
     */
    constexpr const T& value() const& {
        if (!has_val_)
            throw bad_expected_access<E>(storage_.err);
        return storage_.val;
    }

    /**
     * @brief  Returns the contained value (rvalue).
     *
     * @return     Rvalue reference to the contained value.
     *
     * @throws     bad_expected_access if expected contains an error.
     *
     * @warning    Throws bad_expected_access if this expected does not
     *             contain a value.
     *
     * @ingroup base_utilities
     */
    constexpr T&& value() && {
        if (!has_val_)
            throw bad_expected_access<E>(std::move(storage_.err));
        return std::move(storage_.val);
    }

    /**
     * @brief  Returns the contained value (const rvalue).
     *
     * @return     Const rvalue reference to the contained value.
     *
     * @throws     bad_expected_access if expected contains an error.
     *
     * @warning    Throws bad_expected_access if this expected does not
     *             contain a value.
     *
     * @ingroup base_utilities
     */
    constexpr const T&& value() const&& {
        if (!has_val_)
            throw bad_expected_access<E>(std::move(storage_.err));
        return std::move(storage_.val);
    }

    /**
     * @brief  Returns the contained error.
     *
     * @return     Reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr E& error() & noexcept { return storage_.err; }

    /**
     * @brief  Returns the contained error (const lvalue).
     *
     * @return     Const reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr const E& error() const& noexcept { return storage_.err; }

    /**
     * @brief  Returns the contained error (rvalue).
     *
     * @return     Rvalue reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr E&& error() && noexcept { return std::move(storage_.err); }

    /**
     * @brief  Returns the contained error (const rvalue).
     *
     * @return     Const rvalue reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr const E&& error() const&& noexcept { return std::move(storage_.err); }

    /**
     * @brief  Returns the contained value or a default value.
     *
     * @tparam U Default value type.
     * @param[in] default_val Default value to return if in error state.
     *
     * @return     The contained value or the default value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename U> constexpr T value_or(U&& default_val) const& {
        return has_val_ ? storage_.val : static_cast<T>(std::forward<U>(default_val));
    }

    /**
     * @brief  Returns the contained value or a default value (rvalue).
     *
     * @tparam U Default value type.
     * @param[in] default_val Default value to return if in error state.
     *
     * @return     The contained value or the default value.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename U> constexpr T value_or(U&& default_val) && {
        return has_val_ ? std::move(storage_.val) : static_cast<T>(std::forward<U>(default_val));
    }

    /**
     * @brief  Returns the contained error or a default error.
     *
     * @tparam U Default error type.
     * @param[in] default_err Default error to return if has value.
     *
     * @return     The contained error or the default error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename U> constexpr E error_or(U&& default_err) const& {
        return !has_val_ ? storage_.err : static_cast<E>(std::forward<U>(default_err));
    }

    /**
     * @brief  Returns the contained error or a default error (rvalue).
     *
     * @tparam U Default error type.
     * @param[in] default_err Default error to return if has value.
     *
     * @return     The contained error or the default error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename U> constexpr E error_or(U&& default_err) && {
        return !has_val_ ? std::move(storage_.err) : static_cast<E>(std::forward<U>(default_err));
    }

    /**
     * @brief  Monadic operation: chains if value is present.
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if value is present.
     *
     * @return     Result of f(value) or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto and_then(F&& f) & {
        using Ret = std::invoke_result_t<F, T&>;
        if (has_val_)
            return std::forward<F>(f)(storage_.val);
        else
            return Ret(unexpect, storage_.err);
    }

    /**
     * @brief  Monadic operation: chains if value is present (const).
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if value is present.
     *
     * @return     Result of f(value) or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto and_then(F&& f) const& {
        using Ret = std::invoke_result_t<F, const T&>;
        if (has_val_)
            return std::forward<F>(f)(storage_.val);
        else
            return Ret(unexpect, storage_.err);
    }

    /**
     * @brief  Monadic operation: chains if value is present (rvalue).
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if value is present.
     *
     * @return     Result of f(value) or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto and_then(F&& f) && {
        using Ret = std::invoke_result_t<F, T&&>;
        if (has_val_)
            return std::forward<F>(f)(std::move(storage_.val));
        else
            return Ret(unexpect, std::move(storage_.err));
    }

    /**
     * @brief  Monadic operation: chains if error is present.
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if error is present.
     *
     * @return     value or result of f(error).
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto or_else(F&& f) & {
        using Ret = std::invoke_result_t<F, E&>;
        if (!has_val_)
            return std::forward<F>(f)(storage_.err);
        else
            return Ret(storage_.val);
    }

    /**
     * @brief  Monadic operation: chains if error is present (const).
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if error is present.
     *
     * @return     value or result of f(error).
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto or_else(F&& f) const& {
        using Ret = std::invoke_result_t<F, const E&>;
        if (!has_val_)
            return std::forward<F>(f)(storage_.err);
        else
            return Ret(storage_.val);
    }

    /**
     * @brief  Monadic operation: chains if error is present (rvalue).
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if error is present.
     *
     * @return     value or result of f(error).
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto or_else(F&& f) && {
        using Ret = std::invoke_result_t<F, E&&>;
        if (!has_val_)
            return std::forward<F>(f)(std::move(storage_.err));
        else
            return Ret(std::move(storage_.val));
    }

    /**
     * @brief  Monadic operation: transforms the value.
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply to value.
     *
     * @return     expected with transformed value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform(F&& f) & {
        using U = std::invoke_result_t<F, T&>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)(storage_.val));
        else
            return expected<U, E>(unexpect, storage_.err);
    }

    /**
     * @brief  Monadic operation: transforms the value (const).
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply to value.
     *
     * @return     expected with transformed value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform(F&& f) const& {
        using U = std::invoke_result_t<F, const T&>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)(storage_.val));
        else
            return expected<U, E>(unexpect, storage_.err);
    }

    /**
     * @brief  Monadic operation: transforms the value (rvalue).
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply to value.
     *
     * @return     expected with transformed value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform(F&& f) && {
        using U = std::invoke_result_t<F, T&&>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)(std::move(storage_.val)));
        else
            return expected<U, E>(unexpect, std::move(storage_.err));
    }

    /**
     * @brief  Monadic operation: transforms the error.
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply to error.
     *
     * @return     expected with value or transformed error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform_error(F&& f) & {
        using G = std::invoke_result_t<F, E&>;
        if (!has_val_)
            return expected<T, G>(unexpect, std::forward<F>(f)(storage_.err));
        else
            return expected<T, G>(storage_.val);
    }

    /**
     * @brief  Monadic operation: transforms the error (const).
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply to error.
     *
     * @return     expected with value or transformed error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform_error(F&& f) const& {
        using G = std::invoke_result_t<F, const E&>;
        if (!has_val_)
            return expected<T, G>(unexpect, std::forward<F>(f)(storage_.err));
        else
            return expected<T, G>(storage_.val);
    }

    /**
     * @brief  Monadic operation: transforms the error (rvalue).
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply to error.
     *
     * @return     expected with value or transformed error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform_error(F&& f) && {
        using G = std::invoke_result_t<F, E&&>;
        if (!has_val_)
            return expected<T, G>(unexpect, std::forward<F>(f)(std::move(storage_.err)));
        else
            return expected<T, G>(std::move(storage_.val));
    }

    /**
     * @brief  Equality comparison with another expected.
     *
     * @tparam T2 Other value type.
     * @tparam E2 Other error type.
     * @param[in] a First expected.
     * @param[in] b Second expected.
     *
     * @return     true if equal, false otherwise.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename T2, typename E2>
    friend constexpr bool operator==(const expected& a, const expected<T2, E2>& b) {
        if (a.has_val_ != b.has_value())
            return false;
        if (a.has_val_)
            return *a == *b;
        return a.error() == b.error();
    }

    /**
     * @brief  Equality comparison with a value.
     *
     * @tparam T2 Value type to compare with.
     * @param[in] a Expected.
     * @param[in] v Value to compare.
     *
     * @return     true if expected contains value equal to v.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename T2> friend constexpr bool operator==(const expected& a, const T2& v) {
        return a.has_val_ && (*a == v);
    }

    /**
     * @brief  Equality comparison with unexpected.
     *
     * @tparam E2 Error type of unexpected.
     * @param[in] a Expected.
     * @param[in] u Unexpected to compare.
     *
     * @return     true if expected contains matching error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename E2>
    friend constexpr bool operator==(const expected& a, const unexpected<E2>& u) {
        return !a.has_val_ && (a.error() == u.error());
    }

    /**
     * @brief  Inequality comparison with another expected.
     *
     * @tparam T2 Other value type.
     * @tparam E2 Other error type.
     * @param[in] a First expected.
     * @param[in] b Second expected.
     *
     * @return     true if not equal, false otherwise.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename T2, typename E2>
    friend constexpr bool operator!=(const expected& a, const expected<T2, E2>& b) {
        return !(a == b);
    }

    /**
     * @brief  Inequality comparison with a value.
     *
     * @tparam T2 Value type to compare with.
     * @param[in] a Expected.
     * @param[in] v Value to compare.
     *
     * @return     true if not equal, false otherwise.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename T2> friend constexpr bool operator!=(const expected& a, const T2& v) {
        return !(a == v);
    }

    /**
     * @brief  Inequality comparison with unexpected.
     *
     * @tparam E2 Error type of unexpected.
     * @param[in] a Expected.
     * @param[in] u Unexpected to compare.
     *
     * @return     true if not equal, false otherwise.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename E2>
    friend constexpr bool operator!=(const expected& a, const unexpected<E2>& u) {
        return !(a == u);
    }

    /**
     * @brief  Swaps the contents with another expected.
     *
     * @param[in,out] o Expected to swap with.
     *
     * @throws     None if types are nothrow move constructible and swappable.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    void swap(expected& o) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                    std::is_nothrow_swappable_v<T> &&
                                    std::is_nothrow_move_constructible_v<E> &&
                                    std::is_nothrow_swappable_v<E>) {
        using std::swap;
        if (has_val_ && o.has_val_) {
            swap(storage_.val, o.storage_.val);
        } else if (!has_val_ && !o.has_val_) {
            swap(storage_.err, o.storage_.err);
        } else {
            // Swap between value-holding and error-holding expected
            if (has_val_) {
                // this has value, o has error
                T tmp_val(std::move(storage_.val));
                storage_.val.~T();

                ::new (&storage_.err) E(std::move(o.storage_.err));
                o.storage_.err.~E();

                ::new (&o.storage_.val) T(std::move(tmp_val));
                // tmp_val is destroyed here

                has_val_ = false;
                o.has_val_ = true;
            } else {
                // this has error, o has value
                E tmp_err(std::move(storage_.err));
                storage_.err.~E();

                ::new (&storage_.val) T(std::move(o.storage_.val));
                o.storage_.val.~T();

                ::new (&o.storage_.err) E(std::move(tmp_err));
                // tmp_err is destroyed here

                has_val_ = true;
                o.has_val_ = false;
            }
        }
    }

    /**
     * @brief  Non-member swap function.
     *
     * @param[in,out] a First expected.
     * @param[in,out] b Second expected.
     *
     * @throws     None if types are nothrow move constructible and swappable.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    friend void swap(expected& a, expected& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }
};

/**
 * @brief  Specialization of expected for void value type.
 *
 * This specialization represents operations that may fail but do not
 * produce a value on success.
 *
 * @tparam E Error type.
 *
 * @ingroup base_utilities
 */
template <typename E> class expected<void, E> {
    static_assert(!std::is_void_v<E>, "E must not be void");

    /// Internal storage union.
    union Storage {
        E err; ///< Error storage.
        Storage() {}
        ~Storage() {}
    };
    Storage storage_; ///< Internal storage.
    bool has_val_;    ///< Indicates whether in value (void) state.

  public:
    using value_type = void;               ///< Value type alias.
    using error_type = E;                  ///< Error type alias.
    using unexpected_type = unexpected<E>; ///< Unexpected type alias.

    /**
     * @brief  Default constructor. Creates expected in success state.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr expected() noexcept : has_val_(true) {}

    /**
     * @brief  Copy constructor.
     *
     * @param[in] o Expected to copy from.
     *
     * @throws     May throw if E copy constructor throws.
     *
     * @ingroup base_utilities
     */
    expected(const expected& o) : has_val_(o.has_val_) {
        if (!has_val_)
            ::new (&storage_.err) E(o.storage_.err);
    }

    /**
     * @brief  Move constructor.
     *
     * @param[in] o Expected to move from.
     *
     * @throws     May throw if E move constructor throws.
     *
     * @ingroup base_utilities
     */
    expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<E>)
        : has_val_(o.has_val_) {
        if (!has_val_)
            ::new (&storage_.err) E(std::move(o.storage_.err));
    }

    /**
     * @brief  Constructor from unexpected.
     *
     * @tparam G Forwarded error type.
     * @param[in] u Unexpected value containing error.
     *
     * @throws     May throw if E constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename G = E> constexpr expected(const unexpected<G>& u) : has_val_(false) {
        ::new (&storage_.err) E(u.error());
    }

    /**
     * @brief  Constructor from unexpected rvalue.
     *
     * @tparam G Forwarded error type.
     * @param[in] u Unexpected value containing error.
     *
     * @throws     May throw if E constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename G = E> constexpr expected(unexpected<G>&& u) : has_val_(false) {
        ::new (&storage_.err) E(std::move(u.error()));
    }

    /**
     * @brief  In-place constructor for success state.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr explicit expected(std::in_place_t) noexcept : has_val_(true) {}

    /**
     * @brief  Constructor for in-place error construction.
     *
     * @tparam Args Constructor argument types.
     * @param[in] args Arguments to forward to E constructor.
     *
     * @throws     May throw if E constructor throws.
     *
     * @ingroup base_utilities
     */
    template <typename... Args> constexpr explicit expected(unexpect_t, Args&&... args)
        : has_val_(false) {
        ::new (&storage_.err) E(std::forward<Args>(args)...);
    }

    /**
     * @brief  Destructor.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    ~expected() {
        if (!has_val_)
            storage_.err.~E();
    }

    /**
     * @brief  Copy assignment operator.
     *
     * @param[in] o Expected to copy from.
     *
     * @return     Reference to this expected.
     *
     * @throws     May throw if E assignment or construction throws.
     *
     * @ingroup base_utilities
     */
    expected& operator=(const expected& o) {
        if (has_val_ && o.has_val_) {
        } else if (!has_val_ && !o.has_val_) {
            storage_.err = o.storage_.err;
        } else if (has_val_) {
            ::new (&storage_.err) E(o.storage_.err);
            has_val_ = false;
        } else {
            storage_.err.~E();
            has_val_ = true;
        }
        return *this;
    }

    /**
     * @brief  Checks if the expected is in success state.
     *
     * @return     true if in success state, false if in error state.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr bool has_value() const noexcept { return has_val_; }

    /**
     * @brief  Boolean conversion operator.
     *
     * @return     true if in success state, false if in error state.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    constexpr explicit operator bool() const noexcept { return has_val_; }

    /**
     * @brief  Returns void if in success state.
     *
     * @throws     bad_expected_access if in error state.
     *
     * @warning    Throws bad_expected_access if this expected contains
     *             an error.
     *
     * @ingroup base_utilities
     */
    constexpr void value() const {
        if (!has_val_)
            throw bad_expected_access<E>(storage_.err);
    }

    /**
     * @brief  Returns the contained error.
     *
     * @return     Reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr E& error() & noexcept { return storage_.err; }

    /**
     * @brief  Returns the contained error (const lvalue).
     *
     * @return     Const reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr const E& error() const& noexcept { return storage_.err; }

    /**
     * @brief  Returns the contained error (rvalue).
     *
     * @return     Rvalue reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr E&& error() && noexcept { return std::move(storage_.err); }

    /**
     * @brief  Returns the contained error (const rvalue).
     *
     * @return     Const rvalue reference to the contained error.
     *
     * @throws     None.
     *
     * @warning    The error is only valid if has_value() returns false.
     *
     * @ingroup base_utilities
     */
    constexpr const E&& error() const&& noexcept { return std::move(storage_.err); }

    /**
     * @brief  Monadic operation: chains if in success state.
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if in success state.
     *
     * @return     Result of f() or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto and_then(F&& f) & {
        using Ret = std::invoke_result_t<F>;
        if (has_val_)
            return std::forward<F>(f)();
        else
            return Ret(unexpect, storage_.err);
    }

    /**
     * @brief  Monadic operation: chains if in success state (rvalue).
     *
     * @tparam F Callable type.
     * @param[in] f Function to call if in success state.
     *
     * @return     Result of f() or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto and_then(F&& f) && {
        using Ret = std::invoke_result_t<F>;
        if (has_val_)
            return std::forward<F>(f)();
        else
            return Ret(unexpect, std::move(storage_.err));
    }

    /**
     * @brief  Monadic operation: transforms the success state.
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply.
     *
     * @return     expected with transformed value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform(F&& f) & {
        using U = std::invoke_result_t<F>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)());
        else
            return expected<U, E>(unexpect, storage_.err);
    }

    /**
     * @brief  Monadic operation: transforms the success state (rvalue).
     *
     * @tparam F Callable type.
     * @param[in] f Function to apply.
     *
     * @return     expected with transformed value or error.
     *
     * @throws     None.
     *
     * @ingroup base_utilities
     */
    template <typename F> auto transform(F&& f) && {
        using U = std::invoke_result_t<F>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)());
        else
            return expected<U, E>(unexpect, std::move(storage_.err));
    }
};

} // namespace cf
