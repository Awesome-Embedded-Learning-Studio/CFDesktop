#include "async_queue.h"
#include <algorithm>
#include <array>

namespace cf::log {

AsyncPostQueue::AsyncPostQueue() = default;

AsyncPostQueue::~AsyncPostQueue() {
    stop();
}

void AsyncPostQueue::start() {
    if (!running_.exchange(true)) {
        worker_thread_ = std::thread([this] { this->worker_loop(); });
    }
}

void AsyncPostQueue::stop() {
    if (running_.exchange(false)) {
        cv_.notify_all();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        // Clear queues
        {
            std::lock_guard<std::mutex> lock(errorMu_);
            errorQueue_.clear();
        }
        // Drain lockfree queue (already handled by worker_loop, but clear to be safe)
        LogRecord record;
        while (normalQueue_.tryPop(record)) {
            // Records are dropped during shutdown
        }
    }
}

void AsyncPostQueue::submit(LogRecord record) {
    if (record.lvl >= level::ERROR) {
        std::lock_guard<std::mutex> lock(errorMu_);
        errorQueue_.push_back(std::move(record));
    } else {
        // Check queue size before pushing to implement drop policy
        // If queue is 90%+ full, drop the record to avoid backpressure
        if (normalQueue_.size() >= kMaxNormalQueueSize * 9 / 10) {
            normalQueueOverflow_.fetch_add(1, std::memory_order_relaxed);
        } else {
            // Push to lockfree queue (will block if full, but we've already checked)
            normalQueue_.tryPush(std::move(record));
        }
    }
    cv_.notify_one();
}

void AsyncPostQueue::add_sink(std::shared_ptr<ISink> sink) {
    std::lock_guard<std::mutex> lock(sinksMu_);
    sinks_.push_back(std::move(sink));
}

void AsyncPostQueue::remove_sink(ISink* sink) {
    std::lock_guard<std::mutex> lock(sinksMu_);
    sinks_.erase(
        std::remove_if(sinks_.begin(), sinks_.end(),
                       [sink](const std::shared_ptr<ISink>& s) { return s.get() == sink; }),
        sinks_.end());
}

void AsyncPostQueue::flush() {
    flush_requested_.store(true, std::memory_order_release);
    cv_.notify_one();
}

void AsyncPostQueue::flush_sync() {
    // Get a unique token for this flush request
    uint64_t my_token = flush_token_.fetch_add(1, std::memory_order_acq_rel) + 1;

    // Set flush request flag
    flush_requested_.store(true, std::memory_order_release);
    cv_.notify_one();

    // Wait for OUR specific token to complete
    std::unique_lock<std::mutex> lock(flush_completed_mu_);
    flush_completed_cv_.wait(lock, [this, my_token] {
        return flush_completed_.load(std::memory_order_acquire) >= my_token || !running_;
    });
}

void AsyncPostQueue::clear_sinks() {
    std::lock_guard<std::mutex> lock(sinksMu_);
    sinks_.clear();
}

void AsyncPostQueue::dispatch_one(const LogRecord& record) {
    std::lock_guard<std::mutex> lock(sinksMu_);
    for (auto& sink : sinks_) {
        sink->write(record);
    }
}

// Worker thread main loop
void AsyncPostQueue::worker_loop() {
    // Batch buffer for efficient processing
    constexpr size_t kBatchSize = 64;
    std::array<LogRecord, kBatchSize> batch;

    while (running_) {
        // 优先消费 ERROR 队列
        {
            std::lock_guard<std::mutex> lock(errorMu_);
            while (!errorQueue_.empty()) {
                dispatch_one(errorQueue_.front());
                errorQueue_.pop_front();
            }
        }

        // 再消费普通队列 (使用批量操作)
        size_t count = normalQueue_.tryPopBatch(batch.data(), kBatchSize);
        if (count > 0) {
            for (size_t i = 0; i < count; ++i) {
                dispatch_one(batch[i]);
            }
            continue; // 继续处理下一批
        }

        // 队列为空，检查是否有 flush 请求
        if (flush_requested_.exchange(false, std::memory_order_acq_rel)) {
            {
                std::lock_guard<std::mutex> lock(sinksMu_);
                for (auto& sink : sinks_) {
                    sink->flush();
                }
            }
            // Update completed token to current flush_token_
            // This unblocks all flush_sync() calls with tokens <= this value
            uint64_t current_token = flush_token_.load(std::memory_order_acquire);
            flush_completed_.store(current_token, std::memory_order_release);
            flush_completed_cv_.notify_all();
        }

        std::unique_lock<std::mutex> lock(wakeMu_);
        cv_.wait_for(lock, std::chrono::milliseconds(10));
    }

    // Final flush on shutdown
    {
        std::lock_guard<std::mutex> lock(errorMu_);
        while (!errorQueue_.empty()) {
            dispatch_one(errorQueue_.front());
            errorQueue_.pop_front();
        }
    }

    // Drain remaining records from lockfree queue
    LogRecord record;
    while (normalQueue_.tryPop(record)) {
        dispatch_one(record);
    }

    // Flush all sinks on shutdown
    {
        std::lock_guard<std::mutex> lock(sinksMu_);
        for (auto& sink : sinks_) {
            sink->flush();
        }
    }
}

} // namespace cf::log
