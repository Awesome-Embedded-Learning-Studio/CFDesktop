/**
 * @file    desktop/base/logger/src/async_queue/async_queue.h
 * @brief   Asynchronous logging queue with worker thread.
 *
 * Provides an asynchronous queue for log records with a dedicated worker thread
 * that processes and dispatches records to registered sinks.
 *
 * @author  Charliechen114514
 * @date    2026-03-16
 * @version 0.1
 * @since   0.1
 * @ingroup cflog_internal
 */
#pragma once

#include "base/lockfree/mpsc_queue.hpp"
#include "cflog/cflog_level.hpp"
#include "cflog/cflog_record.h"
#include "cflog/cflog_sink.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace cf::log {

/**
 * @brief  Asynchronous queue for processing log records.
 *
 * Manages a worker thread that processes log records from multiple
 * producer threads and dispatches them to registered sinks.
 *
 * @ingroup cflog_internal
 *
 * @note   Thread-safe for producers. Not thread-safe for sink management
 *         operations from multiple threads.
 *
 * @code
 * AsyncPostQueue queue;
 * queue.start();
 * queue.submit(LogRecord{...});
 * queue.stop();
 * @endcode
 */
class AsyncPostQueue {
  public:
    /**
     * @brief  Maximum capacity of the normal priority queue.
     *
     * Must be a power of 2 for the lockfree queue implementation.
     */
    static constexpr size_t kMaxNormalQueueSize = 65536; // 2^16

    /**
     * @brief  Constructs a new AsyncPostQueue.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    AsyncPostQueue();

    /**
     * @brief  Destroys the AsyncPostQueue.
     *
     * Stops the worker thread and processes remaining records.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    ~AsyncPostQueue();

    // ========================================================================
    // Lifecycle control
    // ========================================================================

    /**
     * @brief  Starts the worker thread.
     *
     * Begins processing log records from the queue.
     *
     * @throws None
     * @note   Must be called before submitting records.
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void start();

    /**
     * @brief  Stops the worker thread.
     *
     * Processes remaining records before terminating.
     *
     * @throws None
     * @note   Blocks until the worker thread finishes.
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void stop();

    // ========================================================================
    // Queue operations
    // ========================================================================

    /**
     * @brief  Submits a log record for processing.
     *
     * @param[in] record The log record to process.
     * @throws          None
     * @note            Records are queued and processed asynchronously.
     * @warning         None
     * @since           0.1
     * @ingroup         cflog_internal
     */
    void submit(LogRecord record);

    /**
     * @brief  Requests a flush of pending records.
     *
     * Returns immediately without waiting for completion.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void flush();

    /**
     * @brief  Requests a flush and waits for completion.
     *
     * Blocks until all pending records are processed.
     *
     * @throws None
     * @note   This operation blocks until the queue is empty.
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void flush_sync();

    // ========================================================================
    // Statistics
    // ========================================================================

    /**
     * @brief  Gets the count of dropped normal queue records.
     *
     * @return Number of records dropped due to queue overflow.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    size_t get_normal_queue_overflow() const {
        return normalQueueOverflow_.load(std::memory_order_relaxed);
    }

    // ========================================================================
    // Sink management
    // ========================================================================

    /**
     * @brief  Adds a sink to receive log records.
     *
     * @param[in] sink Shared pointer to the sink to add.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog_internal
     */
    void add_sink(std::shared_ptr<ISink> sink);

    /**
     * @brief  Removes a sink from the logger.
     *
     * @param[in] sink Pointer to the sink to remove.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    cflog_internal
     */
    void remove_sink(ISink* sink);

    /**
     * @brief  Removes all sinks from the logger.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void clear_sinks();

  private:
    /**
     * @brief  Worker thread main loop.
     *
     * Processes log records from the queue until stopped.
     *
     * @throws None
     * @note   Runs in a dedicated thread.
     * @warning None
     * @since  0.1
     * @ingroup cflog_internal
     */
    void worker_loop();

    /**
     * @brief  Dispatches a single record to all sinks.
     *
     * @param[in] record The log record to dispatch.
     * @throws          None
     * @note            None
     * @warning         None
     * @since           0.1
     * @ingroup         cflog_internal
     */
    void dispatch_one(const LogRecord& record);

  private:
    // Worker thread
    std::thread worker_thread_;               ///< Worker thread for processing records.
    std::atomic_bool running_{false};         ///< Indicates if the worker is running.
    std::atomic_bool flush_requested_{false}; ///< Indicates if a flush was requested.

    // Error queue (guaranteed channel, unlimited)
    std::deque<LogRecord> errorQueue_; ///< Fallback queue for error messages.
    std::mutex errorMu_;               ///< Mutex for error queue access.

    // Normal queue (lockfree MPSC, bounded, drop when full)
    cf::lockfree::MpscQueue<LogRecord, kMaxNormalQueueSize> normalQueue_; ///< Normal queue.
    std::atomic<size_t> normalQueueOverflow_{0}; ///< Count of dropped normal messages.

    // Notification
    std::condition_variable cv_; ///< Condition variable for waking the worker.
    std::mutex wakeMu_;          ///< Mutex for the wake condition variable.

    // Flush completion notification (token-based for concurrent flush_sync calls)
    std::condition_variable flush_completed_cv_; ///< CV for flush completion.
    std::mutex flush_completed_mu_;              ///< Mutex for flush completion CV.
    std::atomic<uint64_t> flush_token_{0};       ///< Incrementing token for each flush request.
    std::atomic<uint64_t> flush_completed_{0};   ///< Highest completed flush token.

    // Sink management
    std::mutex sinksMu_;                        ///< Mutex for sink list access.
    std::vector<std::shared_ptr<ISink>> sinks_; ///< Registered sinks.
};

} // namespace cf::log
