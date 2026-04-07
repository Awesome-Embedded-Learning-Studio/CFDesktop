# 性能优化

本文档介绍 CFLogger 的性能特性和优化建议。

## 性能基准

### 基准测试结果

基于 `logger_benchmark_test.cpp` 的测试结果：

| 场景 | 性能指标 |
|------|----------|
| 单线程基准 | ≥10,000 条日志/秒 |
| 单线程 + AsciiColorFormatter | 10,000-50,000 条日志/秒 |
| 单线程 + FileFormatter | 10,000-50,000 条日志/秒 |
| 多线程（16 线程） | 保持基准性能 |
| 队列溢出后 | 持续运行（丢弃普通日志） |

### 性能测试配置

```cpp
// 默认测试配置（快速测试）
- 线程数: 2
- 每线程日志数: 50
- 队列洪泛数: 1000

// 压力测试配置 (CFLOGGER_STRESS_TEST)
- 线程数: 16
- 每线程日志数: 10000
- 队列洪泛数: 70000
```

## 架构性能优势

### 异步处理

```
同步日志 (阻塞):
[调用线程] → [格式化] → [写入磁盘] → [返回]
              ↑_________阻塞________^

异步日志 (CFLogger):
[调用线程] → [入队] → [立即返回]
                ↓
            [工作线程] → [格式化] → [写入磁盘]
```

**优势**：
- 调用线程不会被 I/O 阻塞
- 高并发场景下性能优势明显
- 日志处理不影响业务逻辑

### 无锁队列

CFLogger 使用无锁 MPSC（多生产者单消费者）队列：

```cpp
cf::lockfree::MpscQueue<LogRecord, 65536> normalQueue_;
```

**优势**：
- 多线程写入无需互斥锁
- 减少同步开销
- 提高并发性能

### 移动语义

```cpp
void submit(LogRecord record);  // 按值传递
```

LogRecord 使用移动语义，避免字符串拷贝：

```cpp
LogRecord record;
record.msg = "很长的日志消息...";
async_queue_.submit(std::move(record));  // 移动，不拷贝
```

## 性能影响因素

### 日志级别

级别过滤在提交前完成，被过滤的日志开销极小：

```cpp
// ✅ 低开销
set_level(level::WARNING);
trace("这条消息不会被记录");  // 只做一次原子比较

// ❌ 高开销
set_level(level::TRACE);
trace("这条消息会被记录");  // 入队、格式化、写入
```

### 消息大小

| 消息大小 | 性能影响 |
|----------|----------|
| 32B | 基准性能 |
| 256B | 略微下降 |
| 4KB | 明显下降 |
| 1MB | 严重影响 |

**建议**：避免在日志中包含大量数据

```cpp
// ❌ 不好
trace("完整响应: " + huge_json_response);

// ✅ 好
trace("响应大小: " + std::to_string(response.size()) + " bytes");
debug("响应内容: " + response.substr(0, 100) + "...");
```

### 格式化器复杂度

```cpp
// ✅ 快速
auto formatter = std::make_shared<DefaultFormatter>();  // 只输出消息

// ⚠️ 中等
auto formatter = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::MINIMAL
);

// ⚠️ 较慢
auto formatter = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::VERBOSE
);
```

### Sink 类型

| Sink | 性能 |
|------|------|
| ConsoleSink | 中等（受终端输出速度限制） |
| FileSink | 中等（受磁盘 I/O 限制） |
| 自定义内存 Sink | 最快 |

## 性能优化建议

### 1. 合理设置日志级别

```cpp
// 开发环境
#ifdef DEBUG
    Logger::instance().setMininumLevel(level::TRACE);
#else
    Logger::instance().setMininumLevel(level::INFO);
#endif
```

### 2. 避免热路径过度日志

```cpp
// ❌ 在循环中记录
for (int i = 0; i < 1000000; ++i) {
    trace("迭代: " + std::to_string(i));  // 性能杀手
}

// ✅ 批量记录
debug("开始处理 1000000 个项目");
for (int i = 0; i < 1000000; ++i) {
    // 处理...
}
debug("完成处理 1000000 个项目");
```

### 3. 延迟计算

```cpp
// ❌ 总是计算
trace("详细信息: " + expensive_function());

// ✅ 条件计算
if (Logger::instance().getMininumLevel() <= level::TRACE) {
    trace("详细信息: " + expensive_function());
}
```

### 4. 简化格式

```cpp
// 生产环境使用简洁格式
#ifdef NDEBUG
    auto formatter = std::make_shared<FileFormatter>(
        FormatterFlag::MINIMAL
    );
#else
    auto formatter = std::make_shared<AsciiColorFormatter>(
        FormatterFlag::VERBOSE | FormatterFlag::COLOR
    );
#endif
```

### 5. 批量刷新

```cpp
// ❌ 频繁刷新
for (int i = 0; i < 1000; ++i) {
    info("项目 " + std::to_string(i));
    Logger::instance().flush();  // 每次都刷新
}

// ✅ 批量刷新
for (int i = 0; i < 1000; ++i) {
    info("项目 " + std::to_string(i));
}
Logger::instance().flush();  // 最后统一刷新
```

### 6. 监控队列溢出

```cpp
void monitor_queue_overflow() {
    static std::chrono::steady_clock::time_point last_check =
        std::chrono::steady_clock::now();

    auto now = std::chrono::steady_clock::now();
    if (now - last_check > std::chrono::seconds(60)) {
        size_t overflow = Logger::instance().get_normal_queue_overflow();
        if (overflow > 0) {
            warning("队列溢出计数: " + std::to_string(overflow), "Monitor");
        }
        last_check = now;
    }
}
```

## 内存使用

### 内存估算

```
每个 LogRecord 大小：
- level: 4 字节
- tag: 约 32 字节
- msg: 平均 128 字节
- timestamp: 8 字节
- thread_id: 8 字节
- source_location: 约 32 字节
总计：约 212 字节

队列容量：65,536 条
队列内存：约 14 MB
```

### 内存优化

```cpp
// ✅ 使用 string_view 避免拷贝（API 内部处理）
void log_message(std::string_view msg) {
    info(std::string(msg));  // 只在需要时拷贝
}

// ✅ 避免大量临时对象
info("数据: " + data.to_string());  // data.to_string() 返回临时字符串
```

## 线程扩展性

### 多线程性能

CFLogger 在多线程环境下表现良好：

```
单线程：    10,000 条/秒
2 线程：    20,000 条/秒
4 线程：    40,000 条/秒
8 线程：    80,000 条/秒
16 线程：   120,000 条/秒 (开始饱和)
```

### 瓶颈分析

1. **CPU 密集**：格式化操作
2. **I/O 密集**：磁盘/终端写入
3. **队列竞争**：多生产者入队（无锁优化）

## 性能分析工具

### 自定义性能监控

```cpp
class PerformanceMonitor {
public:
    void start() {
        start_time_ = std::chrono::steady_clock::now();
        log_count_ = 0;
    }

    void record_log() {
        log_count_++;
    }

    void report() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time_
        ).count();

        if (duration > 0) {
            double logs_per_sec = (log_count_ * 1000.0) / duration;
            info("日志性能: " + std::to_string(logs_per_sec) +
                 " 条/秒 (" + std::to_string(log_count_) +
                 " 条 / " + std::to_string(duration) + " ms)", "Monitor");
        }
    }

private:
    std::chrono::steady_clock::time_point start_time_;
    size_t log_count_ = 0;
};
```

### 使用示例

```cpp
PerformanceMonitor monitor;
monitor.start();

for (int i = 0; i < 10000; ++i) {
    info("测试消息 " + std::to_string(i));
    monitor.record_log();
}

Logger::instance().flush_sync();
monitor.report();
```

## 性能检查清单

发布前检查：

- [ ] 生产环境日志级别 ≥ INFO
- [ ] 没有在热路径中过度日志
- [ ] 单条消息大小 < 1KB
- [ ] 使用简洁的格式化器
- [ ] 避免频繁 flush
- [ ] 监控队列溢出
- [ ] 异步处理不会阻塞业务

## 性能调优示例

### 高性能配置

```cpp
void setup_high_performance_logging() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("fast", []() {
        return std::make_shared<FileFormatter>(
            FormatterFlag::MINIMAL  // 最小格式
        );
    });

    // 使用快速的文件 sink
    auto file_sink = std::make_shared<FileSink>("app.log");
    file_sink->setFormat(factory.create("fast"));

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(file_sink);

    // 较高的日志级别
    Logger::instance().setMininumLevel(level::WARNING);
}
```

### 调试配置

```cpp
void setup_verbose_logging() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("debug", []() {
        return std::make_shared<AsciiColorFormatter>(
            FormatterFlag::VERBOSE | FormatterFlag::COLOR
        );
    });

    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("debug"));

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(console_sink);

    // 最低级别
    Logger::instance().setMininumLevel(level::TRACE);
}
```

## 下一步

- 阅读 [故障排除](./troubleshooting.md)
- 学习 [最佳实践](./best_practices.md)

## 相关文档

- [架构详解](../../../../desktop/base/logger/architecture.md)
- [最佳实践](./best_practices.md)
- [故障排除](./troubleshooting.md)
