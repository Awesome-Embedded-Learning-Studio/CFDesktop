/**
 * @file    base/include/base/indexed_vector/indexed_vector.hpp
 * @brief   Provides a cursor-enhanced vector container.
 *
 * Implements a vector wrapper with an embedded cursor (position indicator)
 * and configurable indexing modes: Fixed (cursor locked), Listed (linear,
 * throws on bounds), and Recycled (circular wrap-around). Suitable for
 * playlist, image browser, and similar navigation scenarios.
 *
 * @author  Charliechen114514
 * @date    2026-04-09
 * @version 0.1
 * @since   0.1
 * @ingroup base_containers
 */

#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cf {

/**
 * @brief  Controls how the cursor navigates an indexed_vector.
 *
 * @ingroup base_containers
 */
enum class IndexingMode {
    Fixed,   ///< Cursor is locked; next/prev are no-ops.
    Listed,  ///< Linear traversal; throws on out-of-bounds.
    Recycled ///< Circular traversal; wraps at both ends.
};

/**
 * @brief  A cursor-enhanced vector with configurable navigation modes.
 *
 * Wraps a @c std::vector<T> and adds an embedded cursor that tracks the
 * "current" element. Three indexing modes control how @c next() and
 * @c prev() behave:
 *
 * - @c IndexingMode::Fixed:    cursor is locked; @c next()/@c prev() are no-ops.
 * - @c IndexingMode::Listed:   linear traversal; throws @c std::out_of_range on bounds.
 * - @c IndexingMode::Recycled: circular traversal; wraps around at both ends.
 *
 * The default mode can be set at compile time via the @p DefaultMode
 * template parameter, and changed at runtime with @c set_mode().
 *
 * @tparam T           Element type.
 * @tparam DefaultMode Compile-time default indexing mode (Listed if omitted).
 *
 * @ingroup base_containers
 *
 * @code
 * cf::indexed_vector<int> playlist{1, 2, 3, 4, 5};
 * playlist.set_cursor(2);   // cursor -> 3
 * playlist.next();           // cursor -> 4
 *
 * // Listed mode: next() at end throws
 * playlist.set_mode(cf::IndexingMode::Recycled);
 * playlist.set_cursor(4);
 * playlist.next();           // wraps to cursor 0
 * @endcode
 */
template <typename T, IndexingMode DefaultMode = IndexingMode::Listed> class indexed_vector {
  public:
    // =========================================================================
    // Type aliases (delegate to std::vector<T>)
    // =========================================================================

    using value_type = T;
    using allocator_type = typename std::vector<T>::allocator_type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    // =========================================================================
    // Constructors / Destructor / Assignment
    // =========================================================================

    /**
     * @brief  Default-constructs an empty indexed_vector.
     *
     * Cursor is 0, mode is @p DefaultMode.
     *
     * @since  0.1
     */
    indexed_vector() : mode_(DefaultMode) {}

    /**
     * @brief  Constructs with @p count default-inserted elements.
     * @param  count  Number of elements.
     *
     * @since  0.1
     */
    explicit indexed_vector(size_type count) : data_(count), mode_(DefaultMode) {}

    /**
     * @brief  Constructs with @p count copies of @p value.
     * @param  count  Number of elements.
     * @param  value  Value to copy.
     *
     * @since  0.1
     */
    indexed_vector(size_type count, const T& value) : data_(count, value), mode_(DefaultMode) {}

    /**
     * @brief  Constructs from iterator range [first, last).
     * @tparam InputIt  Input iterator type.
     * @param  first    Start of range.
     * @param  last     End of range.
     *
     * @since  0.1
     */
    template <typename InputIt> indexed_vector(InputIt first, InputIt last)
        : data_(first, last), mode_(DefaultMode) {}

    /**
     * @brief  Constructs from initializer list.
     * @param  init  Initializer list.
     *
     * @since  0.1
     */
    indexed_vector(std::initializer_list<T> init) : data_(init), mode_(DefaultMode) {}

    /** @brief Copy constructor. @since 0.1 */
    indexed_vector(const indexed_vector&) = default;
    /** @brief Move constructor. @since 0.1 */
    indexed_vector(indexed_vector&&) noexcept = default;
    /** @brief Copy assignment. @since 0.1 */
    indexed_vector& operator=(const indexed_vector&) = default;
    /** @brief Move assignment. @since 0.1 */
    indexed_vector& operator=(indexed_vector&&) noexcept = default;
    /** @brief Destructor. @since 0.1 */
    ~indexed_vector() = default;

    // =========================================================================
    // Cursor API
    // =========================================================================

    /**
     * @brief  Returns the current cursor position.
     * @return Cursor index (0 on empty vector).
     *
     * @throws None.
     *
     * @since  0.1
     */
    size_type cursor() const noexcept { return cursor_; }

    /**
     * @brief  Sets the cursor to @p pos.
     * @param[in] pos  New cursor position.
     *
     * @throws std::out_of_range  If @p pos >= size() (including empty vector).
     *
     * @since  0.1
     */
    void set_cursor(size_type pos) {
        if (pos >= data_.size()) {
            throw std::out_of_range("indexed_vector::set_cursor: pos out of range");
        }
        cursor_ = pos;
    }

    /**
     * @brief  Returns a reference to the element at the cursor.
     * @return Reference to current element.
     *
     * @throws std::out_of_range  If the vector is empty.
     *
     * @since  0.1
     */
    reference at_cursor() {
        if (data_.empty()) {
            throw std::out_of_range("indexed_vector::at_cursor: vector is empty");
        }
        return data_[cursor_];
    }

    /**
     * @brief  Returns a const reference to the element at the cursor.
     * @return Const reference to current element.
     *
     * @throws std::out_of_range  If the vector is empty.
     *
     * @since  0.1
     */
    const_reference at_cursor() const {
        if (data_.empty()) {
            throw std::out_of_range("indexed_vector::at_cursor: vector is empty");
        }
        return data_[cursor_];
    }

    /**
     * @brief  Advances the cursor forward.
     *
     * Behavior depends on the current mode:
     * - @c Fixed:    No-op.
     * - @c Listed:   Increments cursor; throws if already at end.
     * - @c Recycled: Increments cursor; wraps to 0 at end.
     *
     * @throws std::out_of_range  (Listed) cursor at end or vector empty;
     *                            (Recycled) vector empty.
     *
     * @since  0.1
     */
    void next() {
        switch (mode_) {
            case IndexingMode::Fixed:
                return;
            case IndexingMode::Listed:
                if (data_.empty() || cursor_ + 1 >= data_.size()) {
                    throw std::out_of_range("indexed_vector::next: cursor at end");
                }
                ++cursor_;
                return;
            case IndexingMode::Recycled:
                if (data_.empty()) {
                    throw std::out_of_range("indexed_vector::next: vector is empty");
                }
                cursor_ = (cursor_ + 1) % data_.size();
                return;
        }
    }

    /**
     * @brief  Moves the cursor backward.
     *
     * Behavior depends on the current mode:
     * - @c Fixed:    No-op.
     * - @c Listed:   Decrements cursor; throws if already at start.
     * - @c Recycled: Decrements cursor; wraps to end at start.
     *
     * @throws std::out_of_range  (Listed) cursor at start or vector empty;
     *                            (Recycled) vector empty.
     *
     * @since  0.1
     */
    void prev() {
        switch (mode_) {
            case IndexingMode::Fixed:
                return;
            case IndexingMode::Listed:
                if (data_.empty() || cursor_ == 0) {
                    throw std::out_of_range("indexed_vector::prev: cursor at beginning");
                }
                --cursor_;
                return;
            case IndexingMode::Recycled:
                if (data_.empty()) {
                    throw std::out_of_range("indexed_vector::prev: vector is empty");
                }
                cursor_ = (cursor_ == 0) ? data_.size() - 1 : cursor_ - 1;
                return;
        }
    }

    /**
     * @brief  Checks whether a next element is reachable.
     * @return @c true if next() would succeed.
     *
     * @throws None.
     *
     * @since  0.1
     */
    bool has_next() const noexcept {
        switch (mode_) {
            case IndexingMode::Fixed:
                return false;
            case IndexingMode::Listed:
                return cursor_ + 1 < data_.size();
            case IndexingMode::Recycled:
                return !data_.empty();
        }
        return false;
    }

    /**
     * @brief  Checks whether a previous element is reachable.
     * @return @c true if prev() would succeed.
     *
     * @throws None.
     *
     * @since  0.1
     */
    bool has_prev() const noexcept {
        switch (mode_) {
            case IndexingMode::Fixed:
                return false;
            case IndexingMode::Listed:
                return cursor_ > 0;
            case IndexingMode::Recycled:
                return !data_.empty();
        }
        return false;
    }

    // =========================================================================
    // Mode API
    // =========================================================================

    /**
     * @brief  Returns the current indexing mode.
     * @return Current mode.
     *
     * @throws None.
     *
     * @since  0.1
     */
    IndexingMode mode() const noexcept { return mode_; }

    /**
     * @brief  Changes the indexing mode at runtime.
     * @param[in] mode  New mode.
     *
     * @note   Does not affect cursor position or vector contents.
     *
     * @throws None.
     *
     * @since  0.1
     */
    void set_mode(IndexingMode mode) noexcept { mode_ = mode; }

    // =========================================================================
    // Element access (passthrough)
    // =========================================================================

    reference operator[](size_type pos) { return data_[pos]; }
    const_reference operator[](size_type pos) const { return data_[pos]; }

    /**
     * @brief  Accesses element at @p pos with bounds checking.
     *
     * @param[in] pos  Position of the element.
     *
     * @return         Reference to the element.
     *
     * @throws         std::out_of_range If @p pos >= size().
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    reference at(size_type pos) { return data_.at(pos); }

    /**
     * @brief  Accesses element at @p pos with bounds checking (const).
     *
     * @param[in] pos  Position of the element.
     *
     * @return         Const reference to the element.
     *
     * @throws         std::out_of_range If @p pos >= size().
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reference at(size_type pos) const { return data_.at(pos); }

    /**
     * @brief  Returns a reference to the first element.
     *
     * @return         Reference to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    reference front() { return data_.front(); }

    /**
     * @brief  Returns a const reference to the first element.
     *
     * @return         Const reference to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reference front() const { return data_.front(); }

    /**
     * @brief  Returns a reference to the last element.
     *
     * @return         Reference to the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    reference back() { return data_.back(); }

    /**
     * @brief  Returns a const reference to the last element.
     *
     * @return         Const reference to the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reference back() const { return data_.back(); }

    /**
     * @brief  Returns a pointer to the underlying element storage.
     *
     * @return         Pointer to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    T* data() noexcept { return data_.data(); }

    /**
     * @brief  Returns a const pointer to the underlying element storage.
     *
     * @return         Const pointer to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const T* data() const noexcept { return data_.data(); }

    // =========================================================================
    // Iterators (passthrough)
    // =========================================================================

    /**
     * @brief  Returns an iterator to the beginning.
     *
     * @return         Mutable iterator to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    iterator begin() noexcept { return data_.begin(); }

    /**
     * @brief  Returns a const iterator to the beginning.
     *
     * @return         Const iterator to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_iterator begin() const noexcept { return data_.begin(); }

    /**
     * @brief  Returns a const iterator to the beginning.
     *
     * @return         Const iterator to the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_iterator cbegin() const noexcept { return data_.cbegin(); }

    /**
     * @brief  Returns an iterator to the end.
     *
     * @return         Mutable iterator past the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    iterator end() noexcept { return data_.end(); }

    /**
     * @brief  Returns a const iterator to the end.
     *
     * @return         Const iterator past the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_iterator end() const noexcept { return data_.end(); }

    /**
     * @brief  Returns a const iterator to the end.
     *
     * @return         Const iterator past the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_iterator cend() const noexcept { return data_.cend(); }

    /**
     * @brief  Returns a reverse iterator to the beginning.
     *
     * @return         Mutable reverse iterator to the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    reverse_iterator rbegin() noexcept { return data_.rbegin(); }

    /**
     * @brief  Returns a const reverse iterator to the beginning.
     *
     * @return         Const reverse iterator to the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }

    /**
     * @brief  Returns a const reverse iterator to the beginning.
     *
     * @return         Const reverse iterator to the last element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reverse_iterator crbegin() const noexcept { return data_.crbegin(); }

    /**
     * @brief  Returns a reverse iterator to the end.
     *
     * @return         Mutable reverse iterator past the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    reverse_iterator rend() noexcept { return data_.rend(); }

    /**
     * @brief  Returns a const reverse iterator to the end.
     *
     * @return         Const reverse iterator past the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reverse_iterator rend() const noexcept { return data_.rend(); }

    /**
     * @brief  Returns a const reverse iterator to the end.
     *
     * @return         Const reverse iterator past the first element.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    const_reverse_iterator crend() const noexcept { return data_.crend(); }

    // =========================================================================
    // Capacity (passthrough)
    // =========================================================================

    /**
     * @brief  Checks whether the container is empty.
     *
     * @return         @c true if the container is empty.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    bool empty() const noexcept { return data_.empty(); }

    /**
     * @brief  Returns the number of elements.
     *
     * @return         Current number of elements.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    size_type size() const noexcept { return data_.size(); }

    /**
     * @brief  Returns the maximum number of elements possible.
     *
     * @return         Maximum possible number of elements.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    size_type max_size() const noexcept { return data_.max_size(); }

    /**
     * @brief  Returns the current allocated capacity.
     *
     * @return         Number of elements that can be held.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    size_type capacity() const noexcept { return data_.capacity(); }

    /**
     * @brief  Reserves storage for at least @p new_cap elements.
     *
     * @param[in] new_cap  New capacity to reserve.
     *
     * @throws         std::length_error If @p new_cap > max_size().
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    void reserve(size_type new_cap) { data_.reserve(new_cap); }

    /**
     * @brief  Reduces capacity to fit the current size.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.1
     * @ingroup        base_containers
     */
    void shrink_to_fit() { data_.shrink_to_fit(); }

    // =========================================================================
    // Modifiers (with cursor adjustment)
    // =========================================================================

    /**
     * @brief  Clears all elements and resets the cursor to 0.
     *
     * @since  0.1
     */
    void clear() {
        data_.clear();
        cursor_ = 0;
    }

    /**
     * @brief  Appends an element to the end. Cursor is not affected.
     * @param[in] value  Value to copy.
     *
     * @since  0.1
     */
    void push_back(const T& value) { data_.push_back(value); }

    /**
     * @brief  Appends an element to the end. Cursor is not affected.
     * @param[in] value  Value to move.
     *
     * @since  0.1
     */
    void push_back(T&& value) { data_.push_back(std::move(value)); }

    /**
     * @brief  Constructs an element in-place at the end. Cursor is not affected.
     * @tparam Args  Constructor argument types.
     * @param[in] args  Arguments to forward.
     * @return Reference to the inserted element.
     *
     * @since  0.1
     */
    template <typename... Args> reference emplace_back(Args&&... args) {
        return data_.emplace_back(std::forward<Args>(args)...);
    }

    /**
     * @brief  Removes the last element. Adjusts cursor if necessary.
     *
     * If the cursor was on the last element, it moves back one position.
     * If the vector becomes empty, cursor resets to 0.
     *
     * @since  0.1
     */
    void pop_back() {
        if (data_.size() <= 1) {
            data_.pop_back();
            cursor_ = 0;
            return;
        }
        if (cursor_ == data_.size() - 1) {
            cursor_ = data_.size() - 2;
        }
        data_.pop_back();
    }

    /**
     * @brief  Inserts an element before @p pos. Adjusts cursor if needed.
     * @param[in] pos   Iterator before which to insert.
     * @param[in] value Value to copy.
     * @return          Iterator pointing to the inserted element.
     *
     * @since  0.1
     */
    iterator insert(const_iterator pos, const T& value) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto it = data_.insert(pos, value);
        if (idx <= cursor_) {
            ++cursor_;
        }
        return it;
    }

    /**
     * @brief  Inserts an element before @p pos. Adjusts cursor if needed.
     * @param[in] pos   Iterator before which to insert.
     * @param[in] value Value to move.
     * @return          Iterator pointing to the inserted element.
     *
     * @since  0.1
     */
    iterator insert(const_iterator pos, T&& value) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto it = data_.insert(pos, std::move(value));
        if (idx <= cursor_) {
            ++cursor_;
        }
        return it;
    }

    /**
     * @brief  Inserts @p count copies of @p value before @p pos.
     * @param[in] pos   Iterator before which to insert.
     * @param[in] count Number of copies.
     * @param[in] value Value to copy.
     * @return          Iterator pointing to the first inserted element.
     *
     * @since  0.1
     */
    iterator insert(const_iterator pos, size_type count, const T& value) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto it = data_.insert(pos, count, value);
        if (idx <= cursor_) {
            cursor_ += count;
        }
        return it;
    }

    /**
     * @brief  Inserts elements from range [first, last) before @p pos.
     * @tparam InputIt  Input iterator type.
     * @param[in] pos    Iterator before which to insert.
     * @param[in] first  Start of range.
     * @param[in] last   End of range.
     * @return           Iterator pointing to the first inserted element.
     *
     * @since  0.1
     */
    template <typename InputIt> iterator insert(const_iterator pos, InputIt first, InputIt last) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto size_before = data_.size();
        auto it = data_.insert(pos, first, last);
        auto inserted = data_.size() - size_before;
        if (idx <= cursor_) {
            cursor_ += inserted;
        }
        return it;
    }

    /**
     * @brief  Inserts elements from initializer list before @p pos.
     * @param[in] pos   Iterator before which to insert.
     * @param[in] ilist Initializer list.
     * @return          Iterator pointing to the first inserted element.
     *
     * @since  0.1
     */
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto size_before = data_.size();
        auto it = data_.insert(pos, ilist);
        auto inserted = data_.size() - size_before;
        if (idx <= cursor_) {
            cursor_ += inserted;
        }
        return it;
    }

    /**
     * @brief  Constructs an element in-place before @p pos.
     * @tparam Args  Constructor argument types.
     * @param[in] pos   Iterator before which to insert.
     * @param[in] args  Arguments to forward.
     * @return          Iterator pointing to the inserted element.
     *
     * @since  0.1
     */
    template <typename... Args> iterator emplace(const_iterator pos, Args&&... args) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto it = data_.emplace(pos, std::forward<Args>(args)...);
        if (idx <= cursor_) {
            ++cursor_;
        }
        return it;
    }

    /**
     * @brief  Erases the element at @p pos. Adjusts cursor if needed.
     * @param[in] pos  Iterator to the element to erase.
     * @return         Iterator following the erased element.
     *
     * @since  0.1
     */
    iterator erase(const_iterator pos) {
        auto idx = static_cast<size_type>(pos - data_.cbegin());
        auto it = data_.erase(pos);
        adjust_cursor_on_erase(idx, 1);
        return it;
    }

    /**
     * @brief  Erases elements in range [first, last). Adjusts cursor if needed.
     * @param[in] first  Start of range.
     * @param[in] last   End of range.
     * @return           Iterator following the last erased element.
     *
     * @since  0.1
     */
    iterator erase(const_iterator first, const_iterator last) {
        auto erased_start = static_cast<size_type>(first - data_.cbegin());
        auto erased_count = static_cast<size_type>(last - first);
        auto it = data_.erase(first, last);
        adjust_cursor_on_erase(erased_start, erased_count);
        return it;
    }

    /**
     * @brief  Resizes the container. Adjusts cursor if shrunk.
     * @param[in] count  New size.
     *
     * @since  0.1
     */
    void resize(size_type count) {
        data_.resize(count);
        clamp_cursor();
    }

    /**
     * @brief  Resizes the container. Adjusts cursor if shrunk.
     * @param[in] count  New size.
     * @param[in] value  Value to fill new elements with.
     *
     * @since  0.1
     */
    void resize(size_type count, const T& value) {
        data_.resize(count, value);
        clamp_cursor();
    }

    /**
     * @brief  Swaps contents with another indexed_vector.
     * @param[in,out] other  The other indexed_vector.
     *
     * @since  0.1
     */
    void swap(indexed_vector& other) noexcept {
        using std::swap;
        swap(data_, other.data_);
        swap(cursor_, other.cursor_);
        swap(mode_, other.mode_);
    }

    // =========================================================================
    // Non-member functions (friend)
    // =========================================================================

    /**
     * @brief  Equality comparison.
     * @return @c true if contents, cursor, and mode are equal.
     *
     * @since  0.1
     * @ingroup base_containers
     */
    friend bool operator==(const indexed_vector& a, const indexed_vector& b) {
        return a.data_ == b.data_ && a.cursor_ == b.cursor_ && a.mode_ == b.mode_;
    }

    /**
     * @brief  Inequality comparison.
     * @return @c true if contents, cursor, or mode differ.
     *
     * @since  0.1
     * @ingroup base_containers
     */
    friend bool operator!=(const indexed_vector& a, const indexed_vector& b) { return !(a == b); }

    /**
     * @brief  Swaps two indexed_vector instances.
     *
     * @since  0.1
     * @ingroup base_containers
     */
    friend void swap(indexed_vector& a, indexed_vector& b) noexcept { a.swap(b); }

  private:
    std::vector<T> data_;
    size_type cursor_ = 0;
    IndexingMode mode_ = DefaultMode;

    /**
     * @brief  Adjusts cursor after an erase operation.
     * @param  erased_start  Index where erasure began.
     * @param  erased_count  Number of elements erased.
     *
     * @since  0.1
     */
    void adjust_cursor_on_erase(size_type erased_start, size_type erased_count) {
        size_type erased_end = erased_start + erased_count;
        if (cursor_ >= erased_start && cursor_ < erased_end) {
            // Cursor was in the erased range — point to erase start.
            cursor_ = erased_start;
        } else if (cursor_ >= erased_end) {
            // Cursor was after the erased range — shift back.
            cursor_ -= erased_count;
        }
        // else: cursor was before erased range — no adjustment.

        clamp_cursor();
    }

    /**
     * @brief  Clamps cursor to valid range [0, size). Resets to 0 if empty.
     *
     * @since  0.1
     */
    void clamp_cursor() {
        if (data_.empty()) {
            cursor_ = 0;
        } else if (cursor_ >= data_.size()) {
            cursor_ = data_.size() - 1;
        }
    }
};

} // namespace cf
