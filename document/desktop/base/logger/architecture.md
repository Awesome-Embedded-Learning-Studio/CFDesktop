# CFLogger 架构详解

本文档详细阐述 CFLogger 的内部架构、各组件的交互方式以及关键设计决策。

## 整体架构

CFLogger 采用异步日志架构，将日志记录的提交与处理分离：

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              应用程序线程                                    │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐           │
│  │Thread 1 │  │Thread 2 │  │Thread 3 │  │Thread N │  │  Main   │           │
│  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘           │
│       │            │            │            │            │                   │
│       └────────────┴────────────┴────────────┴────────────┘                   │
│                            │                                                   │
│                            ▼                                                   │
│                   ┌───────────────┐                                           │
│                   │  Logger API   │                                           │
│                   │  (Singleton)  │                                           │
│                   └───────┬───────┘                                           │
└───────────────────────────┼───────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        异步队列层 (AsyncPostQueue)                            │
│  ┌──────────────────────────┐         ┌──────────────────────────┐           │
│  │   正常队列 (MPSC)         │         │    错误队列 (Mutex)       │           │
│  │   容量: 65,536 条         │         │    容量: 无限制           │           │
│  │   策略: 满时丢弃          │         │    策略: 永不丢弃         │           │
│  └──────────────────────────┘         └──────────────────────────┘           │
└────────────────────────────────────┬────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                            工作线程 (单线程)                                  │
│  ┌────────────────────────────────────────────────────────────────────┐     │
│  │  worker_loop():                                                    │     │
│  │    1. 从队列取出 LogRecord                                         │     │
│  │    2. 遍历所有 sinks                                               │     │
│  │    3. 对每个 sink: format → write                                  │     │
│  │    4. 处理 flush 请求                                              │     │
│  └────────────────────────────────────────────────────────────────────┘     │
└────────────────────────────────────┬────────────────────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              Sinks 层                                        │
│                                                                              │
│  ┌──────────────────┐         ┌──────────────────┐         ┌────────────┐  │
│  │   ConsoleSink    │         │    FileSink      │         │ CustomSink │  │
│  │   (控制台输出)    │         │    (文件输出)     │         │  (自定义)  │  │
│  └────────┬─────────┘         └────────┬─────────┘         └─────┬──────┘  │
│           │                            │                         │          │
│           └────────────┬───────────────┴─────────────────────────┘          │
│                        ▼                                                       │
│  ┌──────────────────────────────────────────────────────────────────────┐   │
│  │                         Formatter 层                                  │   │
│  │  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐   │   │
│  │  │ AsciiColorFormat │  │  FileFormatter   │  │ CustomFormatter  │   │   │
│  │  │ (带颜色的控制台)  │  │   (纯文本文件)    │  │    (自定义)      │   │   │
│  │  └──────────────────┘  └──────────────────┘  └──────────────────┘   │   │
│  └──────────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────────┘
```

## 组件详解

### 1. Logger 类（门面）

Logger 类是整个系统的入口点，继承自 `SimpleSingleton`：

```cpp
class Logger : public SimpleSingleton<Logger> {
    bool log(level log_level, std::string_view msg,
             std::string_view tag, std::source_location loc);
    void flush();
    void flush_sync();

    void setMininumLevel(const level lvl);
    void add_sink(std::shared_ptr<ISink> sink);
    void remove_sink(ISink* sink);
    void clear_sinks();

private:
    std::atomic<level> minimal_level{kDEFAULT_LEVEL};
    std::shared_ptr<LoggerImpl> logger_impl;  // PIMPL
};
```

**职责**：
- 提供简洁的公共 API
- 执行日志级别过滤（原子操作，无锁）
- 委托给 LoggerImpl 处理

### 2. LoggerImpl 类（实现）

使用 PIMPL 模式隐藏实现细节：

```cpp
class LoggerImpl {
    bool log(LogRecord record);
    void flush();
    void flush_sync();

    void add_sink(std::shared_ptr<ISink> sink);
    void remove_sink(ISink* sink);
    void clear_sinks();

private:
    AsyncPostQueue async_queue_;
};
```

**职责**：
- 拥有 AsyncPostQueue 实例
- 将日志记录提交到队列
- 管理 sink 生命周期

### 3. AsyncPostQueue（异步队列）

核心组件，管理日志的异步处理：

```cpp
class AsyncPostQueue {
    // 队列容量（必须是 2 的幂）
    static constexpr size_t kMaxNormalQueueSize = 65536;  // 2^16

    void submit(LogRecord record);
    void flush();       // 异步刷新，立即返回
    void flush_sync();  // 同步刷新，等待完成

    void add_sink(std::shared_ptr<ISink> sink);
    void remove_sink(ISink* sink);
    void clear_sinks();

    size_t get_normal_queue_overflow() const;

private:
    void worker_loop();
    void dispatch_one(const LogRecord& record);
};
```

#### 队列架构

```
                    submit(LogRecord)
                           │
                           ▼
                 ┌─────────────────┐
                 │  level == ERROR? │
                 └────────┬────────┘
                          │
           ┌──────────────┴──────────────┐
           │ YES                         │ NO
           ▼                             ▼
    ┌──────────────┐          ┌──────────────────┐
    │  errorQueue_ │          │  normalQueue_    │
    │  (无锁 deque) │          │  (MPSC Queue)    │
    │  无限制        │          │  65,536 限制     │
    │  永不丢弃      │          │  满时丢弃        │
    └──────┬───────┘          └────────┬─────────┘
           │                            │
           └────────────┬───────────────┘
                        │
                        ▼
               ┌─────────────────┐
               │   worker_loop   │
               │   (工作线程)     │
               └────────┬────────┘
                        │
                        ▼
               ┌─────────────────┐
               │  dispatch_one   │
               │  遍历所有 sinks  │
               └─────────────────┘
```

#### 队列选择策略

| 条件 | 选择的队列 | 原因 |
|------|-----------|------|
| `level == ERROR` | errorQueue_ | 错误日志必须保留 |
| `level != ERROR` | normalQueue_ | 普通日志可丢弃 |

#### 队列容量管理

**正常队列** (`normalQueue_`):
- 类型：`MpscQueue<LogRecord, 65536>`
- 容量：65,536 条消息
- 策略：满时丢弃新消息，`normalQueueOverflow_` 计数器递增

**错误队列** (`errorQueue_`):
- 类型：`std::deque<LogRecord>`
- 容量：无限制
- 策略：永不丢弃

### 4. 工作线程 (Worker Thread)

单线程工作循环，负责处理队列中的消息：

```cpp
void AsyncPostQueue::worker_loop() {
    while (running_) {
        // 1. 优先处理错误队列
        {
            std::lock_guard<std::mutex> lock(errorMu_);
            if (!errorQueue_.empty()) {
                auto record = std::move(errorQueue_.front());
                errorQueue_.pop_front();
                dispatch_one(record);
                continue;
            }
        }

        // 2. 处理正常队列
        LogRecord record;
        if (normalQueue_.try_pop(record)) {
            dispatch_one(record);
            continue;
        }

        // 3. 队列为空，等待唤醒
        if (flush_requested_.load()) {
            flush_completed_.store(true);
            flush_completed_cv_.notify_one();
            flush_requested_.store(false);
        }

        std::unique_lock<std::mutex> lock(wakeMu_);
        cv_.wait(lock);
    }
}
```

### 5. 日志记录 (LogRecord)

日志消息的数据结构：

```cpp
struct LogRecord {
    level log_level;                    // 日志级别
    std::string tag;                    // 标签（如 "CFLog"）
    std::string message;                // 日志消息
    std::chrono::system_clock::time_point timestamp;  // 时间戳
    std::thread::id thread_id;          // 线程 ID
    std::source_location source_loc;    // 源代码位置
};
```

### 6. Formatter 架构

#### Formatter 接口

```cpp
class IFormatter {
public:
    virtual ~IFormatter() = default;
    virtual std::string format(const LogRecord& record) = 0;
    virtual void set_config(std::shared_ptr<FormatterConfig> config) = 0;
};
```

#### FormatterConfig（配置系统）

```cpp
class FormatterConfig {
public:
    void enable(FormatterFlag flag);
    void disable(FormatterFlag flag);
    bool is_enabled(FormatterFlag flag) const;
    void set_timestamp_format(const std::string& fmt);
private:
    std::atomic<uint32_t> flags_;  // 原子操作，线程安全
    std::string timestamp_format_;
};
```

#### FormatterFlag（输出组件）

```cpp
enum FormatterFlag : uint32_t {
    TIMESTAMP      = 1 << 0,   // 时间戳
    LEVEL          = 1 << 1,   // 日志级别
    TAG            = 1 << 2,   // 标签
    THREAD_ID      = 1 << 3,   // 线程 ID
    SOURCE_LOCATION = 1 << 4,  // 源代码位置
    MESSAGE        = 1 << 5,   // 消息内容
    COLOR          = 1 << 6,   // ANSI 颜色

    // 预设组合
    MINIMAL = MESSAGE,
    DEFAULT = TIMESTAMP | LEVEL | MESSAGE,
    VERBOSE = TIMESTAMP | LEVEL | TAG | THREAD_ID | SOURCE_LOCATION | MESSAGE
};
```

#### 可用的 Formatter

| Formatter | 用途 | COLOR 标志处理 |
|-----------|------|---------------|
| DefaultFormatter | 最简单的输出 | 忽略 |
| AsciiColorFormatter | 控制台彩色输出 | 应用 ANSI 转义码 |
| FileFormatter | 文件纯文本输出 | 忽略 |

### 7. Sink 架构

#### Sink 接口

```cpp
class ISink {
public:
    virtual ~ISink() = default;
    virtual void write(const std::string& formatted) = 0;
    virtual void flush() = 0;
    virtual void setFormat(std::shared_ptr<IFormatter> formatter);
};
```

#### 可用的 Sink

| Sink | 输出目标 | 特点 |
|------|---------|------|
| ConsoleSink | stdout/stderr | 线程安全，适合开发 |
| FileSink | 文件 | 支持追加/截断模式 |

#### FileSink 打开模式

```cpp
enum class OpenMode {
    Truncate,  // 截断模式（覆盖现有文件）
    Append     // 追加模式（添加到文件末尾）
};
```

## 数据流

### 日志记录的完整流程

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 1. 应用调用                                                              │
│    info("Hello", "MyTag", source_location::current());                   │
└─────────────────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────────────┐
│ 2. Logger::log()                                                        │
│    • 检查 minimal_level（原子操作）                                     │
│    • 创建 LogRecord                                                     │
│    • 委托给 LoggerImpl                                                  │
└─────────────────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────────────┐
│ 3. LoggerImpl::log()                                                    │
│    • 调用 AsyncPostQueue::submit()                                      │
└─────────────────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────────────┐
│ 4. AsyncPostQueue::submit()                                             │
│    • 如果是 ERROR：推入 errorQueue_（互斥锁保护）                        │
│    • 否则：尝试推入 normalQueue_（无锁操作）                             │
│      - 成功：继续                                                        │
│      - 失败：递增 overflow 计数器，丢弃                                  │
│    • 通知工作线程                                                        │
└─────────────────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────────────┐
│ 5. Worker Thread::worker_loop()                                         │
│    • 从队列取出 LogRecord（优先错误队列）                                │
│    • 调用 dispatch_one()                                                │
└─────────────────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────────────┐
│ 6. AsyncPostQueue::dispatch_one()                                        │
│    • 遍历所有 sinks（互斥锁保护）                                        │
│    • 对每个 sink：                                                       │
│      1. 调用 formatter->format(record)                                  │
│      2. 调用 sink->write(formatted_string)                              │
└─────────────────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────────────┐
│ 7. Sink::write()                                                        │
│    • ConsoleSink: 写入 stdout/stderr                                     │
│    • FileSink: 写入文件                                                 │
│    • CustomSink: 自定义行为                                             │
└─────────────────────────────────────────────────────────────────────────┘
```

## 线程安全保证

### 原子操作

| 组件 | 操作 | 同步机制 |
|------|------|----------|
| Logger | minimal_level 读写 | `std::atomic<level>` |
| FormatterConfig | flags 读写 | `std::atomic<uint32_t>` |
| AsyncPostQueue | running_/flush_requested_ | `std::atomic<bool>` |
| AsyncPostQueue | normalQueueOverflow_ | `std::atomic<size_t>` |

### 互斥锁保护

| 组件 | 保护的数据 | 锁类型 |
|------|-----------|--------|
| AsyncPostQueue | errorQueue_ | `std::mutex errorMu_` |
| AsyncPostQueue | sinks_ | `std::mutex sinksMu_` |
| AsyncPostQueue | wakeMu_ / cv_ | `std::mutex wakeMu_` |
| FileSink | 文件写入 | 内部互斥锁 |

### 无锁数据结构

| 组件 | 类型 | 特点 |
|------|------|------|
| normalQueue_ | MpscQueue | 无锁 MPSC 队列 |

## Flush 机制

### 异步 Flush (flush())

立即返回，不等待刷新完成：

```cpp
void Logger::flush() {
    // 请求刷新
    flush_requested_.store(true);
    // 唤醒工作线程
    cv_.notify_one();
}
```

### 同步 Flush (flush_sync())

等待刷新完成后返回：

```cpp
void Logger::flush_sync() {
    // 请求刷新
    flush_requested_.store(true);
    flush_completed_.store(false);
    cv_.notify_one();

    // 等待完成
    std::unique_lock<std::mutex> lock(flush_completed_mu_);
    flush_completed_cv_.wait(lock, [] {
        return flush_completed_.load();
    });
}
```

## 队列满时的行为

### 正常日志队列满

```
应用程序提交日志
        │
        ▼
normalQueue_.try_push(record)
        │
   ┌────┴────┐
   │         │
 成功      失败
   │         │
   │         ▼
   │    normalQueueOverflow_++
   │         │
   │         ▼
   │    [丢弃日志]
   │
   ▼
继续执行
```

### 错误日志永不丢弃

```
ERROR 日志提交
        │
        ▼
推入 errorQueue_
        │
        ▼
   [等待处理]
```

## 性能优化点

1. **无锁队列**：正常日志使用无锁 MPSC 队列，减少同步开销
2. **原子操作**：级别过滤使用原子变量，无需加锁
3. **移动语义**：LogRecord 使用 std::move，避免字符串拷贝
4. **延迟格式化**：在工作线程中格式化，不阻塞调用者
5. **缓存友好**：队列大小为 2 的幂，优化取模运算

## 相关文档

- [概述](./overview.md) - 系统概述和快速开始
- [API 参考](./api_reference.md) - 完整 API 文档
- [HandBook/性能详解](../../../HandBook/desktop/base/logger/performance.md) - 性能数据和优化
