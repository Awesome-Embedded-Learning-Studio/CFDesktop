/**
 * @file    base/include/base/span/span.h
 * @brief   Provides a C++17 implementation of std::span (C++20).
 *
 * Offers a zero-overhead view into contiguous sequences of elements.
 * Supports C arrays, std::vector, and std::array as underlying storage.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup base_containers
 */
#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace cf {

/**
 * @brief  A non-owning view into a contiguous sequence of elements.
 *
 * Provides a lightweight, bounds-checked (when using methods) interface
 * to access elements without owning the underlying data. Similar to
 * std::span from C++20 but implemented for C++17 compatibility.
 *
 * @tparam T Element type. Must be trivially copyable.
 *
 * @ingroup base_containers
 *
 * @code
 * std::vector<int> vec = {1, 2, 3, 4, 5};
 * cf::span<int> s = vec;
 * int first = s[0];  // Access first element
 * @endcode
 */
template <typename T> class span {
    T* data_;
    size_t size_;

  public:
    /**
     * @brief  Default constructor. Creates an empty span.
     *
     * @ingroup base_containers
     */
    constexpr span() noexcept : data_(nullptr), size_(0) {}

    /**
     * @brief  Constructs a span from a pointer and size.
     *
     * @param[in] data Pointer to the first element. Must be non-null if
     *                 size > 0.
     * @param[in] size Number of elements in the sequence.
     *
     * @ingroup base_containers
     */
    constexpr span(T* data, size_t size) noexcept : data_(data), size_(size) {}

    /**
     * @brief  Constructs a span from a C array.
     *
     * @tparam N Array size deduced from the input.
     * @param[in] arr C array to view.
     *
     * @ingroup base_containers
     */
    template <size_t N> constexpr span(T (&arr)[N]) noexcept : data_(arr), size_(N) {}

    /**
     * @brief  Constructs a span from a std::vector.
     *
     * @param[in] vec Vector to view. Does not take ownership.
     *
     * @ingroup base_containers
     */
    constexpr span(std::vector<T>& vec) noexcept : data_(vec.data()), size_(vec.size()) {}

    /**
     * @brief  Constructs a const span from a const std::vector.
     *
     * @tparam U Template parameter to enable only for const T.
     * @param[in] vec Const vector to view. Does not take ownership.
     *
     * @ingroup base_containers
     */
    template <typename U = T, std::enable_if_t<std::is_const<U>::value, int> = 0>
    constexpr span(const std::vector<std::remove_const_t<T>>& vec) noexcept
        : data_(vec.data()), size_(vec.size()) {}

    /**
     * @brief  Constructs a span from a std::array.
     *
     * @tparam N Array size.
     * @param[in] arr Array to view.
     *
     * @ingroup base_containers
     */
    template <size_t N> constexpr span(std::array<T, N>& arr) noexcept
        : data_(arr.data()), size_(N) {}

    /**
     * @brief  Constructs a const span from a const std::array.
     *
     * @tparam N  Array size.
     * @tparam U   Template parameter to enable only for const T.
     * @param[in] arr Const array to view.
     *
     * @ingroup base_containers
     */
    template <size_t N, typename U = T, std::enable_if_t<std::is_const<U>::value, int> = 0>
    constexpr span(const std::array<std::remove_const_t<T>, N>& arr) noexcept
        : data_(arr.data()), size_(N) {}

    /**
     * @brief  Default copy constructor.
     *
     * @ingroup base_containers
     */
    constexpr span(const span& other) noexcept = default;

    /**
     * @brief  Default copy assignment operator.
     *
     * @ingroup base_containers
     */
    constexpr span& operator=(const span& other) noexcept = default;

    /**
     * @brief  Returns pointer to the first element.
     *
     * @return     Pointer to the underlying data, or nullptr if empty.
     *
     * @ingroup base_containers
     */
    constexpr T* data() const noexcept { return data_; }

    /**
     * @brief  Returns the number of elements in the span.
     *
     * @return     Number of elements in the sequence.
     *
     * @ingroup base_containers
     */
    constexpr size_t size() const noexcept { return size_; }

    /**
     * @brief  Checks if the span is empty.
     *
     * @return     true if size() == 0, false otherwise.
     *
     * @ingroup base_containers
     */
    constexpr bool empty() const noexcept { return size_ == 0; }

    /**
     * @brief  Accesses element at specified index.
     *
     * @param[in] index Zero-based element index.
     *
     * @return     Reference to the element at the specified index.
     *
     * @warning    No bounds checking is performed. Caller must ensure
     *             index < size().
     *
     * @ingroup base_containers
     */
    constexpr T& operator[](size_t index) const noexcept { return data_[index]; }

    /**
     * @brief  Returns the first element.
     *
     * @return     Reference to the first element.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    Undefined behavior if the span is empty.
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr T& front() const noexcept { return data_[0]; }

    /**
     * @brief  Returns the last element.
     *
     * @return     Reference to the last element.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    Undefined behavior if the span is empty.
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr T& back() const noexcept { return data_[size_ - 1]; }

    /**
     * @brief  Returns iterator to the first element.
     *
     * @return     Pointer to the first element (begin iterator).
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr T* begin() const noexcept { return data_; }

    /**
     * @brief  Returns iterator to one past the last element.
     *
     * @return     Pointer to one past the last element (end iterator).
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr T* end() const noexcept { return data_ + size_; }

    /**
     * @brief  Creates a span of the first `count` elements.
     *
     * @param[in] count Number of elements to include.
     *
     * @return     New span containing the first `count` elements.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    Undefined behavior if count > size().
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr span<T> first(size_t count) const noexcept { return span<T>(data_, count); }

    /**
     * @brief  Creates a span of the last `count` elements.
     *
     * @param[in] count Number of elements to include.
     *
     * @return     New span containing the last `count` elements.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    Undefined behavior if count > size().
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr span<T> last(size_t count) const noexcept {
        return span<T>(data_ + size_ - count, count);
    }

    /**
     * @brief  Creates a subspan starting at `offset`.
     *
     * @param[in] offset Starting element index.
     * @param[in] count  Number of elements to include. If -1 (default),
     *                   includes all elements from offset to end.
     *
     * @return     New span containing the specified elements.
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    Undefined behavior if offset + count > size().
     *
     * @since      0.1
     * @ingroup    base_containers
     */
    constexpr span<T> subspan(size_t offset,
                              size_t count = static_cast<size_t>(-1)) const noexcept {
        if (count == static_cast<size_t>(-1)) {
            count = size_ - offset;
        }
        return span<T>(data_ + offset, count);
    }
};

// Type deduction guides
/// @cond DeductionGuides
template <typename T> span(T*, size_t) -> span<T>;

template <typename T, size_t N> span(T (&)[N]) -> span<T>;

template <typename T> span(std::vector<T>&) -> span<T>;

template <typename T> span(const std::vector<T>&) -> span<const T>;

template <typename T, size_t N> span(std::array<T, N>&) -> span<T>;

template <typename T, size_t N> span(const std::array<T, N>&) -> span<const T>;
/// @endcond

} // namespace cf
