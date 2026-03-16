# CFLogger 并发队列问题分析

## 问题概述

在高并发场景下（8+ 线程），logger 的吞吐量急剧下降，远低于性能要求。

## 测试数据

### 多线程吞吐量测试结果

| 线程数 | 吞吐量 (logs/sec) | 要求 | 状态 |
|-------|-------------------|------|------|
| 1 | 395,504 | >= 10,000 | ✅ 通过 |
| 4 | 332,664 | >= 10,000 | ✅ 通过 |
| 8 | 998 | >= 10,000 | ❌ 失败 |
| 16 | 999 | >= 10,000 | ❌ 失败 |

### 并发压力测试结果

| 测试 | 预期 | 实际 | 状态 |
|------|------|------|------|
| ConcurrentLoggingNoCrashes | >= 40,000/160,000 | ~11,000 | ❌ 失败 |

## 根因分析

### 1. 队列容量限制
- 当前队列容量：`kMaxNormalQueueSize = 4096`
- 高并发时队列迅速填满，新日志被丢弃

### 2. 锁竞争严重
位置：`src/async_queue/async_queue.cpp`

```cpp
void AsyncPostQueue::submit(LogRecord record) {
    if (record.lvl >= level::ERROR) {
        std::lock_guard<std::mutex> lock(errorMu_);  // 锁竞争
        errorQueue_.push_back(std::move(record));
    } else {
        std::lock_guard<std::mutex> lock(normalMu_);  // 锁竞争
        if (normalQueue_.size() < kMaxNormalQueueSize) {
            normalQueue_.push_back(std::move(record));
        }
    }
    cv_.notify_one();
}
```

8+ 线程同时调用 `submit()` 时：
- 多个线程争抢 `normalMu_` 互斥锁
- 大部分线程阻塞等待
- 队列满时日志直接丢弃

### 3. 单工作线程瓶颈
- 只有一个工作线程处理日志
- 消费速度跟不上生产速度

## 解决方案

### 方案 A：增加队列容量（简单但治标不治本）

```cpp
// async_queue.h
class AsyncPostQueue {
  public:
    static constexpr size_t kMaxNormalQueueSize = 65536;  // 4096 -> 65536
    // ...
};
```

**优点**：实现简单
**缺点**：内存占用增加，无法解决锁竞争根本问题

---

### 方案 B：实现无锁队列（推荐）

使用无锁队列（如 `boost::lockfree::queue` 或自定义实现）：

```cpp
// async_queue.h
#include <boost/lockfree/queue.hpp>

class AsyncPostQueue {
  public:
    static constexpr size_t kMaxNormalQueueSize = 65536;

  private:
    boost::lockfree::queue<LogRecord*> normalQueue_{kMaxNormalQueueSize};
    boost::lockfree::queue<LogRecord*> errorQueue_{kMaxNormalQueueSize * 2};
};
```

**优点**：
- 消除锁竞争
- 大幅提升并发性能
- 适合多生产者-单消费者场景

**缺点**：
- 需要额外依赖（boost）或手写无锁实现
- LogRecord 需要支持指针或移动语义

---

### 方案 C：多工作线程 + 分片队列

```cpp
// async_queue.h
class AsyncPostQueue {
  private:
    static constexpr size_t kNumWorkerThreads = 4;
    static constexpr size_t kMaxNormalQueueSize = 4096;

    std::array<std::deque<LogRecord>, kNumWorkerThreads> normalQueues_;
    std::array<std::mutex, kNumWorkerThreads> normalMus_;
    std::array<std::thread, kNumWorkerThreads> worker_threads_;

    // 按线程ID哈希选择队列
    size_t get_queue_index() {
        return std::hash<std::thread::id>{}(std::this_thread::get_id()) % kNumWorkerThreads;
    }
};
```

**优点**：
- 减少锁竞争（每个线程独立队列）
- 多消费者并行处理

**缺点**：
- 实现复杂度较高
- 需要处理 sink 的并发访问

---

### 方案 D：优化现有实现（折中方案）

```cpp
// async_queue.cpp
void AsyncPostQueue::submit(LogRecord record) {
    if (record.lvl >= level::ERROR) {
        std::unique_lock<std::mutex> lock(errorMu_, std::try_to_lock);
        if (lock.owns_lock()) {
            errorQueue_.push_back(std::move(record));
        } else {
            // 快速路径：队列已满或无法获取锁，直接丢弃
            return;  // 或统计丢弃数量
        }
    } else {
        std::unique_lock<std::mutex> lock(normalMu_, std::try_to_lock);
        if (lock.owns_lock() && normalQueue_.size() < kMaxNormalQueueSize) {
            normalQueue_.push_back(std::move(record));
        }
        // 否则静默丢弃（当前行为）
    }
    cv_.notify_one();
}
```

**优点**：
- 减少阻塞时间
- 实现改动小

**缺点**：
- 性能提升有限
- 仍存在锁竞争

## 推荐方案

**方案 B（无锁队列）+ 方案 C（多工作线程）组合**

1. 使用无锁队列消除 `submit()` 的锁竞争
2. 增加工作线程数量提高消费能力
3. 保持 sink 的线程安全（已有 `sinksMu_` 保护）

## 相关文件

- `src/async_queue/async_queue.h` - 队列接口定义
- `src/async_queue/async_queue.cpp` - 队列实现
- `test/logger/benchmark/logger_benchmark_test.cpp` - 性能测试
- `test/logger/logger_concurrency_test.cpp` - 并发测试

## 测试命令

```bash
# 运行性能测试
./out/build_develop/test/bin/logger_benchmark_test --gtest_filter="*MultiThreaded*"

# 运行并发测试
./out/build_develop/test/bin/logger_concurrency_test
```

## 性能目标

- 8 线程：>= 10,000 logs/sec
- 16 线程：>= 10,000 logs/sec
- 成功率：>= 95%
