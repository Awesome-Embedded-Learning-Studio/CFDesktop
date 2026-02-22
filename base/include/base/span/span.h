/**
 * @file span.h
 * @brief A simple span implementation for C++17 (std::span is C++20)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace cf {

template <typename T> class span {
    T* data_;
    size_t size_;

  public:
    // Default constructor
    constexpr span() noexcept : data_(nullptr), size_(0) {}

    // Constructor from pointer and size
    constexpr span(T* data, size_t size) noexcept : data_(data), size_(size) {}

    // Constructor from C-array
    template <size_t N> constexpr span(T (&arr)[N]) noexcept : data_(arr), size_(N) {}

    // Constructor from std::vector (non-const)
    constexpr span(std::vector<T>& vec) noexcept : data_(vec.data()), size_(vec.size()) {}

    // Constructor from const std::vector (const T only)
    template <typename U = T, std::enable_if_t<std::is_const<U>::value, int> = 0>
    constexpr span(const std::vector<std::remove_const_t<T>>& vec) noexcept
        : data_(vec.data()), size_(vec.size()) {}

    // Constructor from std::array
    template <size_t N> constexpr span(std::array<T, N>& arr) noexcept
        : data_(arr.data()), size_(N) {}

    template <size_t N, typename U = T, std::enable_if_t<std::is_const<U>::value, int> = 0>
    constexpr span(const std::array<std::remove_const_t<T>, N>& arr) noexcept
        : data_(arr.data()), size_(N) {}

    // Copy constructor
    constexpr span(const span& other) noexcept = default;

    // Copy assignment
    constexpr span& operator=(const span& other) noexcept = default;

    // Element access
    constexpr T* data() const noexcept { return data_; }
    constexpr size_t size() const noexcept { return size_; }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr T& operator[](size_t index) const noexcept { return data_[index]; }

    constexpr T& front() const noexcept { return data_[0]; }
    constexpr T& back() const noexcept { return data_[size_ - 1]; }

    // Iterators
    constexpr T* begin() const noexcept { return data_; }
    constexpr T* end() const noexcept { return data_ + size_; }

    // Subspan
    constexpr span<T> first(size_t count) const noexcept { return span<T>(data_, count); }

    constexpr span<T> last(size_t count) const noexcept {
        return span<T>(data_ + size_ - count, count);
    }

    constexpr span<T> subspan(size_t offset,
                              size_t count = static_cast<size_t>(-1)) const noexcept {
        if (count == static_cast<size_t>(-1)) {
            count = size_ - offset;
        }
        return span<T>(data_ + offset, count);
    }
};

// Type deduction guides
template <typename T> span(T*, size_t) -> span<T>;

template <typename T, size_t N> span(T (&)[N]) -> span<T>;

template <typename T> span(std::vector<T>&) -> span<T>;

template <typename T> span(const std::vector<T>&) -> span<const T>;

template <typename T, size_t N> span(std::array<T, N>&) -> span<T>;

template <typename T, size_t N> span(const std::array<T, N>&) -> span<const T>;

} // namespace cf
