#pragma once

/**
 * @file expected.hpp
 * @brief A simple implementation of std::expected (C++23) for environments
 *        where __cpp_concepts < 202002L (e.g. GCC 13 MinGW).
 *
 * Usage:
 *   charlie::expected<int, std::string> ok  = 42;
 *   charlie::expected<int, std::string> err = charlie::unexpected("oops");
 *
 *   if (ok)          { use(*ok); }
 *   if (!err)        { handle(err.error()); }
 *   int v = ok.value_or(-1);
 *   int v = ok.value();          // throws bad_expected_access if error
 */

#include <exception>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace cf {

// ──────────────────────────────────────────────
//  bad_expected_access
// ──────────────────────────────────────────────

template <typename E> class bad_expected_access;

template <> class bad_expected_access<void> : public std::exception {
  public:
    const char* what() const noexcept override { return "bad_expected_access"; }
};

template <typename E> class bad_expected_access : public bad_expected_access<void> {
  public:
    explicit bad_expected_access(E e) : error_(std::move(e)) {}
    const E& error() const& noexcept { return error_; }
    E& error() & noexcept { return error_; }
    const E&& error() const&& noexcept { return std::move(error_); }
    E&& error() && noexcept { return std::move(error_); }

  private:
    E error_;
};

// ──────────────────────────────────────────────
//  unexpected  —  wraps an error value
// ──────────────────────────────────────────────

template <typename E> class unexpected {
    static_assert(!std::is_void_v<E>, "E must not be void");
    static_assert(!std::is_reference_v<E>, "E must not be a reference");

  public:
    unexpected() = delete;

    template <typename Err = E,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<Err>, unexpected> &&
                                          std::is_constructible_v<E, Err>>>
    constexpr explicit unexpected(Err&& e) : error_(std::forward<Err>(e)) {}

    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
    constexpr explicit unexpected(std::in_place_t, Args&&... args)
        : error_(std::forward<Args>(args)...) {}

    template <
        typename U, typename... Args,
        typename = std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
    constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        : error_(il, std::forward<Args>(args)...) {}

    constexpr const E& error() const& noexcept { return error_; }
    constexpr E& error() & noexcept { return error_; }
    constexpr const E&& error() const&& noexcept { return std::move(error_); }
    constexpr E&& error() && noexcept { return std::move(error_); }

    void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>) {
        using std::swap;
        swap(error_, other.error_);
    }

    template <typename E2>
    friend constexpr bool operator==(const unexpected& a, const unexpected<E2>& b) {
        return a.error_ == b.error();
    }

  private:
    E error_;
};

// Deduction guide
template <typename E> unexpected(E) -> unexpected<E>;

// ──────────────────────────────────────────────
//  in_place / unexpect tags
// ──────────────────────────────────────────────

struct unexpect_t {
    explicit unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

// ──────────────────────────────────────────────
//  expected<T, E>
// ──────────────────────────────────────────────

template <typename T, typename E> class expected {
    static_assert(!std::is_void_v<E>, "E must not be void");
    static_assert(!std::is_reference_v<T>,
                  "T must not be a reference (use T* or std::reference_wrapper)");
    static_assert(!std::is_reference_v<E>, "E must not be a reference");

    // Internal storage — we avoid std::variant for wider compatibility
    union Storage {
        T val;
        E err;
        Storage() {}
        ~Storage() {}
    };

    Storage storage_;
    bool has_val_;

    void destroy() noexcept {
        if (has_val_)
            storage_.val.~T();
        else
            storage_.err.~E();
    }

  public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    // ── Constructors ──────────────────────────

    // Default: value-initialise T
    constexpr expected() noexcept(std::is_nothrow_default_constructible_v<T>) : has_val_(true) {
        ::new (&storage_.val) T();
    }

    // Copy
    expected(const expected& o) : has_val_(o.has_val_) {
        if (has_val_)
            ::new (&storage_.val) T(o.storage_.val);
        else
            ::new (&storage_.err) E(o.storage_.err);
    }

    // Move
    expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                    std::is_nothrow_move_constructible_v<E>)
        : has_val_(o.has_val_) {
        if (has_val_)
            ::new (&storage_.val) T(std::move(o.storage_.val));
        else
            ::new (&storage_.err) E(std::move(o.storage_.err));
    }

    // From value (implicit)
    template <typename U = T,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, expected> &&
                                          !std::is_same_v<std::decay_t<U>, std::in_place_t> &&
                                          !std::is_same_v<std::decay_t<U>, unexpect_t> &&
                                          std::is_constructible_v<T, U>>>
    constexpr expected(U&& v) : has_val_(true) {
        ::new (&storage_.val) T(std::forward<U>(v));
    }

    // From unexpected (error)
    template <typename G = E, typename = std::enable_if_t<std::is_constructible_v<E, const G&>>>
    constexpr expected(const unexpected<G>& u) : has_val_(false) {
        ::new (&storage_.err) E(u.error());
    }

    template <typename G = E, typename = std::enable_if_t<std::is_constructible_v<E, G>>>
    constexpr expected(unexpected<G>&& u) : has_val_(false) {
        ::new (&storage_.err) E(std::move(u.error()));
    }

    // in_place value
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit expected(std::in_place_t, Args&&... args) : has_val_(true) {
        ::new (&storage_.val) T(std::forward<Args>(args)...);
    }

    // unexpect (in-place error)
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
    constexpr explicit expected(unexpect_t, Args&&... args) : has_val_(false) {
        ::new (&storage_.err) E(std::forward<Args>(args)...);
    }

    ~expected() { destroy(); }

    // ── Assignment ────────────────────────────

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

    template <typename U = T>
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

    // ── Observers ─────────────────────────────

    constexpr bool has_value() const noexcept { return has_val_; }
    constexpr explicit operator bool() const noexcept { return has_val_; }

    constexpr T* operator->() noexcept { return &storage_.val; }
    constexpr const T* operator->() const noexcept { return &storage_.val; }

    constexpr T& operator*() & noexcept { return storage_.val; }
    constexpr const T& operator*() const& noexcept { return storage_.val; }
    constexpr T&& operator*() && noexcept { return std::move(storage_.val); }
    constexpr const T&& operator*() const&& noexcept { return std::move(storage_.val); }

    constexpr T& value() & {
        if (!has_val_)
            throw bad_expected_access<E>(storage_.err);
        return storage_.val;
    }
    constexpr const T& value() const& {
        if (!has_val_)
            throw bad_expected_access<E>(storage_.err);
        return storage_.val;
    }
    constexpr T&& value() && {
        if (!has_val_)
            throw bad_expected_access<E>(std::move(storage_.err));
        return std::move(storage_.val);
    }
    constexpr const T&& value() const&& {
        if (!has_val_)
            throw bad_expected_access<E>(std::move(storage_.err));
        return std::move(storage_.val);
    }

    constexpr E& error() & noexcept { return storage_.err; }
    constexpr const E& error() const& noexcept { return storage_.err; }
    constexpr E&& error() && noexcept { return std::move(storage_.err); }
    constexpr const E&& error() const&& noexcept { return std::move(storage_.err); }

    template <typename U> constexpr T value_or(U&& default_val) const& {
        return has_val_ ? storage_.val : static_cast<T>(std::forward<U>(default_val));
    }
    template <typename U> constexpr T value_or(U&& default_val) && {
        return has_val_ ? std::move(storage_.val) : static_cast<T>(std::forward<U>(default_val));
    }

    template <typename U> constexpr E error_or(U&& default_err) const& {
        return !has_val_ ? storage_.err : static_cast<E>(std::forward<U>(default_err));
    }
    template <typename U> constexpr E error_or(U&& default_err) && {
        return !has_val_ ? std::move(storage_.err) : static_cast<E>(std::forward<U>(default_err));
    }

    // ── Monadic operations ────────────────────

    // and_then: f(T) -> expected<U,E>
    template <typename F> auto and_then(F&& f) & {
        using Ret = std::invoke_result_t<F, T&>;
        if (has_val_)
            return std::forward<F>(f)(storage_.val);
        else
            return Ret(unexpect, storage_.err);
    }
    template <typename F> auto and_then(F&& f) const& {
        using Ret = std::invoke_result_t<F, const T&>;
        if (has_val_)
            return std::forward<F>(f)(storage_.val);
        else
            return Ret(unexpect, storage_.err);
    }
    template <typename F> auto and_then(F&& f) && {
        using Ret = std::invoke_result_t<F, T&&>;
        if (has_val_)
            return std::forward<F>(f)(std::move(storage_.val));
        else
            return Ret(unexpect, std::move(storage_.err));
    }

    // or_else: f(E) -> expected<T,G>
    template <typename F> auto or_else(F&& f) & {
        using Ret = std::invoke_result_t<F, E&>;
        if (!has_val_)
            return std::forward<F>(f)(storage_.err);
        else
            return Ret(storage_.val);
    }
    template <typename F> auto or_else(F&& f) const& {
        using Ret = std::invoke_result_t<F, const E&>;
        if (!has_val_)
            return std::forward<F>(f)(storage_.err);
        else
            return Ret(storage_.val);
    }
    template <typename F> auto or_else(F&& f) && {
        using Ret = std::invoke_result_t<F, E&&>;
        if (!has_val_)
            return std::forward<F>(f)(std::move(storage_.err));
        else
            return Ret(std::move(storage_.val));
    }

    // transform: f(T) -> U  =>  expected<U,E>
    template <typename F> auto transform(F&& f) & {
        using U = std::invoke_result_t<F, T&>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)(storage_.val));
        else
            return expected<U, E>(unexpect, storage_.err);
    }
    template <typename F> auto transform(F&& f) const& {
        using U = std::invoke_result_t<F, const T&>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)(storage_.val));
        else
            return expected<U, E>(unexpect, storage_.err);
    }
    template <typename F> auto transform(F&& f) && {
        using U = std::invoke_result_t<F, T&&>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)(std::move(storage_.val)));
        else
            return expected<U, E>(unexpect, std::move(storage_.err));
    }

    // transform_error: f(E) -> G  =>  expected<T,G>
    template <typename F> auto transform_error(F&& f) & {
        using G = std::invoke_result_t<F, E&>;
        if (!has_val_)
            return expected<T, G>(unexpect, std::forward<F>(f)(storage_.err));
        else
            return expected<T, G>(storage_.val);
    }
    template <typename F> auto transform_error(F&& f) const& {
        using G = std::invoke_result_t<F, const E&>;
        if (!has_val_)
            return expected<T, G>(unexpect, std::forward<F>(f)(storage_.err));
        else
            return expected<T, G>(storage_.val);
    }
    template <typename F> auto transform_error(F&& f) && {
        using G = std::invoke_result_t<F, E&&>;
        if (!has_val_)
            return expected<T, G>(unexpect, std::forward<F>(f)(std::move(storage_.err)));
        else
            return expected<T, G>(std::move(storage_.val));
    }

    // ── Equality ──────────────────────────────

    template <typename T2, typename E2>
    friend constexpr bool operator==(const expected& a, const expected<T2, E2>& b) {
        if (a.has_val_ != b.has_value())
            return false;
        if (a.has_val_)
            return *a == *b;
        return a.error() == b.error();
    }

    template <typename T2> friend constexpr bool operator==(const expected& a, const T2& v) {
        return a.has_val_ && (*a == v);
    }

    template <typename E2>
    friend constexpr bool operator==(const expected& a, const unexpected<E2>& u) {
        return !a.has_val_ && (a.error() == u.error());
    }

    template <typename T2, typename E2>
    friend constexpr bool operator!=(const expected& a, const expected<T2, E2>& b) {
        return !(a == b);
    }

    template <typename T2> friend constexpr bool operator!=(const expected& a, const T2& v) {
        return !(a == v);
    }

    template <typename E2>
    friend constexpr bool operator!=(const expected& a, const unexpected<E2>& u) {
        return !(a == u);
    }

    // ── Swap ──────────────────────────────────

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
            expected* src = has_val_ ? this : &o;
            expected* dst = has_val_ ? &o : this;
            ::new (&dst->storage_.val) T(std::move(src->storage_.val));
            src->storage_.val.~T();
            ::new (&src->storage_.err) E(std::move(dst->storage_.err));
            dst->storage_.err.~E();
            swap(src->has_val_, dst->has_val_);
        }
    }

    friend void swap(expected& a, expected& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }
};

// ──────────────────────────────────────────────
//  expected<void, E>  specialisation
// ──────────────────────────────────────────────

template <typename E> class expected<void, E> {
    static_assert(!std::is_void_v<E>, "E must not be void");

    union Storage {
        E err;
        Storage() {}
        ~Storage() {}
    };
    Storage storage_;
    bool has_val_;

  public:
    using value_type = void;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    constexpr expected() noexcept : has_val_(true) {}

    expected(const expected& o) : has_val_(o.has_val_) {
        if (!has_val_)
            ::new (&storage_.err) E(o.storage_.err);
    }
    expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<E>)
        : has_val_(o.has_val_) {
        if (!has_val_)
            ::new (&storage_.err) E(std::move(o.storage_.err));
    }

    template <typename G = E> constexpr expected(const unexpected<G>& u) : has_val_(false) {
        ::new (&storage_.err) E(u.error());
    }
    template <typename G = E> constexpr expected(unexpected<G>&& u) : has_val_(false) {
        ::new (&storage_.err) E(std::move(u.error()));
    }

    constexpr explicit expected(std::in_place_t) noexcept : has_val_(true) {}

    template <typename... Args> constexpr explicit expected(unexpect_t, Args&&... args)
        : has_val_(false) {
        ::new (&storage_.err) E(std::forward<Args>(args)...);
    }

    ~expected() {
        if (!has_val_)
            storage_.err.~E();
    }

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

    constexpr bool has_value() const noexcept { return has_val_; }
    constexpr explicit operator bool() const noexcept { return has_val_; }

    constexpr void value() const {
        if (!has_val_)
            throw bad_expected_access<E>(storage_.err);
    }
    constexpr E& error() & noexcept { return storage_.err; }
    constexpr const E& error() const& noexcept { return storage_.err; }
    constexpr E&& error() && noexcept { return std::move(storage_.err); }
    constexpr const E&& error() const&& noexcept { return std::move(storage_.err); }

    template <typename F> auto and_then(F&& f) & {
        using Ret = std::invoke_result_t<F>;
        if (has_val_)
            return std::forward<F>(f)();
        else
            return Ret(unexpect, storage_.err);
    }
    template <typename F> auto and_then(F&& f) && {
        using Ret = std::invoke_result_t<F>;
        if (has_val_)
            return std::forward<F>(f)();
        else
            return Ret(unexpect, std::move(storage_.err));
    }

    template <typename F> auto transform(F&& f) & {
        using U = std::invoke_result_t<F>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)());
        else
            return expected<U, E>(unexpect, storage_.err);
    }
    template <typename F> auto transform(F&& f) && {
        using U = std::invoke_result_t<F>;
        if (has_val_)
            return expected<U, E>(std::forward<F>(f)());
        else
            return expected<U, E>(unexpect, std::move(storage_.err));
    }
};

} // namespace cf