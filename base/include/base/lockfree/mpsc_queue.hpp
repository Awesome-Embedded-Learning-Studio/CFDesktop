/**
 * @file    base/include/base/lockfree/mpsc_queue.hpp
 * @brief   Multi-Producer Single-Consumer (MPSC) lock-free queue implementation.
 *
 * Provides a fixed-capacity ring buffer optimized for MPSC scenarios.
 * Suitable for high-throughput logging, event processing, and task queues.
 * Uses index-based operations to avoid ABA problems and maintain cache-friendly behavior.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup base_lockfree
 */

#pragma once

#include <array>
#include <atomic>
#include <cstddef>

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#    include <intrin.h>
#endif

#include "base/span/span.h"

namespace cf {
namespace lockfree {

namespace detail {

/**
 * @brief  Issues a short processor-friendly spin-wait hint.
 *
 * Uses the platform pause intrinsic on x86 targets and falls back to a compiler
 * signal fence on other architectures.
 *
 * @throws None.
 * @note   Intended for tight lock-free retry loops.
 * @warning Does not yield the current thread or provide scheduling fairness.
 * @since  0.1
 * @ingroup base_lockfree
 */
inline void cpu_relax() noexcept {
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
    _mm_pause();
#elif (defined(__x86_64__) || defined(__i386__)) && (defined(__GNUC__) || defined(__clang__))
    __builtin_ia32_pause();
#else
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

} // namespace detail

/**
 * @brief Multi-Producer Single-Consumer lock-free queue.
 *
 * Implements a fixed-capacity ring buffer optimized for MPSC scenarios.
 * Multiple producer threads can safely call push operations concurrently,
 * while only one consumer thread should call pop operations.
 *
 * @tparam T        Element type. Must be move-constructible and move-assignable.
 * @tparam Capacity Queue capacity. Must be a power of 2 for efficient indexing.
 *
 * @ingroup base_lockfree
 *
 * @note Thread-safe for producers. NOT thread-safe for multiple consumers.
 *
 * @code
 * cf::lockfree::MpscQueue<int, 1024> queue;
 *
 * // Producer thread(s):
 * queue.tryPush(42);
 *
 * // Consumer thread:
 * int value;
 * if (queue.tryPop(value)) {
 *     // Process value
 * }
 * @endcode
 */
template <typename T, size_t Capacity> class MpscQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

  public:
    using value_type = T;
    using size_type = size_t;

    /**
     * @brief Default constructor.
     *
     * Initializes an empty queue. All slots are pre-allocated and ready for use.
     *
     * @ingroup base_lockfree
     */
    MpscQueue() noexcept {
        // Initialize sequence for each cell
        // Each cell starts with sequence == its index
        for (size_type i = 0; i < Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    /**
     * @brief Destructor.
     *
     * Destroys any remaining elements in the queue.
     *
     * @warning Only the consumer thread should destroy the queue.
     * @ingroup base_lockfree
     */
    ~MpscQueue() noexcept {
        // Destroy any remaining elements
        while (!empty()) {
            T dummy;
            tryPop(dummy);
        }
    }

    // Non-copyable, non-movable
    MpscQueue(const MpscQueue&) = delete;
    MpscQueue& operator=(const MpscQueue&) = delete;
    MpscQueue(MpscQueue&&) = delete;
    MpscQueue& operator=(MpscQueue&&) = delete;

    // ========================================================================
    // Core Operations
    // ========================================================================

    /**
     * @brief Attempts to push a single value into the queue.
     *
     * Thread-safe: Multiple producers can call this method concurrently.
     * Spin-waits if the queue is full.
     *
     * @param[in] value Value to push (moved into the queue).
     * @return          true if successful (always returns true).
     *
     * @note Uses release semantics on success to ensure proper publication.
     * @ingroup base_lockfree
     */
    bool tryPush(T&& value) noexcept {
        Cell* cell;
        size_type pos = writePos_.fetch_add(1, std::memory_order_relaxed);
        cell = &buffer_[pos & (Capacity - 1)];

        size_type seq = cell->sequence.load(std::memory_order_acquire);

        // For MPSC: wait for slot to be available
        // The slot is available when seq == pos
        // If seq < pos, it means another producer is using this slot (wait)
        // If seq > pos, it means consumer hasn't caught up (queue full)
        // Uses a simple approach: just wait without timeout for MPSC
        while (seq != pos) {
            detail::cpu_relax();
            seq = cell->sequence.load(std::memory_order_acquire);
        }

        // Construct in-place
        new (cell->ptr()) T(std::move(value));

        // Publish with release semantics
        cell->sequence.store(pos + 1, std::memory_order_release);

        return true;
    }

    /**
     * @brief Attempts to pop a single value from the queue.
     *
     * NOT thread-safe for consumers: Only one consumer thread should call this.
     *
     * @param[out] out Reference to store the popped value.
     * @return          true if successful, false if queue is empty.
     *
     * @note Uses acquire semantics to synchronize with producers.
     * @ingroup base_lockfree
     */
    bool tryPop(T& out) noexcept {
        const size_type rp = readPos_.load(std::memory_order_relaxed);
        Cell* cell = &buffer_[rp & (Capacity - 1)];

        size_type seq = cell->sequence.load(std::memory_order_acquire);
        if (seq != rp + 1) {
            return false; // Empty
        }

        out = std::move(*cell->ptr());
        cell->ptr()->~T(); // Destroy the object in the cell

        // Publish slot availability for next round
        // Set to rp + Capacity so producer at pos = rp + Capacity can use it
        cell->sequence.store(rp + Capacity, std::memory_order_release);

        // Advance the consumer cursor with release ordering so that producer threads
        // reading the approximate size()/empty() observe a consistent value instead of
        // racing with this write.
        readPos_.store(rp + 1, std::memory_order_release);
        return true;
    }

    // ========================================================================
    // Batch Operations
    // ========================================================================

    /**
     * @brief Attempts to push multiple values in one operation.
     *
     * More efficient than individual pushes due to:
     * - Single sequence number fetch
     * - Better cache locality
     *
     * Thread-safe: Multiple producers can call this concurrently.
     *
     * @param[in] values Span of values to push.
     * @return          Number of values actually pushed (0 to values.size()).
     *
     * @ingroup base_lockfree
     */
    size_type tryPushBatch(cf::span<T> values) noexcept {
        if (values.empty()) {
            return 0;
        }

        const size_type count = values.size();
        const size_type start = writePos_.fetch_add(count, std::memory_order_relaxed);

        // For each slot, wait for it to become available, then write
        for (size_type i = 0; i < count; ++i) {
            size_type pos = start + i;
            Cell* cell = &buffer_[pos & (Capacity - 1)];

            size_type seq = cell->sequence.load(std::memory_order_acquire);
            // Wait for this slot to become available (like tryPush)
            while (seq != pos) {
                detail::cpu_relax();
                seq = cell->sequence.load(std::memory_order_acquire);
            }

            // Slot is available, write the value
            new (cell->ptr()) T(std::move(values[i]));
            cell->sequence.store(pos + 1, std::memory_order_release);
        }

        return count;
    }

    /**
     * @brief Attempts to pop multiple values in one operation.
     *
     * Critical for worker thread efficiency:
     * - Reduces per-item overhead
     * - Amortizes synchronization cost
     * - Better cache utilization
     *
     * NOT thread-safe for consumers: Only one consumer thread should call this.
     *
     * @param[out] out      Output buffer to store popped values.
     * @param[in]  max_count Maximum number of values to pop.
     * @return              Number of values actually popped.
     *
     * @ingroup base_lockfree
     */
    size_type tryPopBatch(T* out, size_type max_count) noexcept {
        size_type popped = 0;

        for (; popped < max_count; ++popped) {
            const size_type rp = readPos_.load(std::memory_order_relaxed);
            Cell* cell = &buffer_[rp & (Capacity - 1)];

            size_type seq = cell->sequence.load(std::memory_order_acquire);
            if (seq != rp + 1) {
                break; // No more available
            }

            out[popped] = std::move(*cell->ptr());
            cell->ptr()->~T();

            cell->sequence.store(rp + Capacity, std::memory_order_release);

            readPos_.store(rp + 1, std::memory_order_release);
        }

        return popped;
    }

    // ========================================================================
    // Capacity and State Queries
    // ========================================================================

    /**
     * @brief Checks if the queue is approximately empty.
     *
     * @return true if the queue appears empty, false otherwise.
     *
     * @warning May return stale data in concurrent scenarios.
     *          This is an approximate check, not a thread-safe guarantee.
     * @ingroup base_lockfree
     */
    bool empty() const noexcept {
        return readPos_.load(std::memory_order_acquire) >=
               writePos_.load(std::memory_order_acquire);
    }

    /**
     * @brief Gets the approximate current size.
     *
     * @return Approximate number of elements in the queue.
     *
     * @warning Expensive operation. May return stale data in concurrent scenarios.
     *          Use sparingly, primarily for monitoring/debugging.
     * @ingroup base_lockfree
     */
    size_type size() const noexcept {
        size_type writePos = writePos_.load(std::memory_order_acquire);
        size_type rp = readPos_.load(std::memory_order_acquire);
        if (writePos < rp) {
            return 0;
        }
        size_type size = writePos - rp;
        return size > Capacity ? Capacity : size;
    }

    /**
     * @brief Gets the maximum capacity of the queue.
     *
     * @return Maximum number of elements the queue can hold.
     *
     * @ingroup base_lockfree
     */
    static constexpr size_type capacity() noexcept { return Capacity; }

  private:
    /**
     * @brief Internal cell structure for ring buffer storage.
     *
     * Each cell contains a sequence number for synchronization
     * and raw storage for the element.
     */
    struct Cell {
        std::atomic<size_type> sequence; ///< Sequence number for synchronization
        alignas(alignof(T)) unsigned char storage[sizeof(T)]; ///< Raw storage for T

        /**
         * @brief Gets a typed pointer to the storage.
         * @return Pointer to the stored element.
         */
        T* ptr() noexcept { return reinterpret_cast<T*>(storage); }

        /**
         * @brief Gets a const typed pointer to the storage.
         * @return Const pointer to the stored element.
         */
        const T* ptr() const noexcept { return reinterpret_cast<const T*>(storage); }
    };

    std::array<Cell, Capacity> buffer_;  ///< Ring buffer storage
    std::atomic<size_type> writePos_{0}; ///< Current write position (multi-producer)
    std::atomic<size_type> readPos_{0};  ///< Current read position (single-consumer)

    // Padding to prevent false sharing between producer and consumer
    char padding_[64 - sizeof(std::atomic<size_type>) - sizeof(std::atomic<size_type>) -
                  sizeof(buffer_) % 64];
};

} // namespace lockfree
} // namespace cf
