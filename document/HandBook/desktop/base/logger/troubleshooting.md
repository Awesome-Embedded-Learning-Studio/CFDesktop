# 故障排除

本文档列出 CFLogger 使用中的常见问题和解决方案。

## 常见问题

### Q1: 日志没有输出

#### 可能原因和解决方法

**原因 1：没有调用 flush()**

```cpp
// ❌ 程序结束太快，工作线程还没处理
int main() {
    info("Hello");
    return 0;  // 日志可能未写入
}

// ✅ 确保刷新
int main() {
    info("Hello");
    flush();  // 或 Logger::instance().flush_sync()
    return 0;
}
```

**原因 2：日志级别设置过高**

```cpp
// ❌ 日志被过滤
set_level(level::WARNING);
info("这条不会显示");  // INFO < WARNING

// ✅ 调整级别
set_level(level::INFO);
info("这条会显示");
```

**原因 3：没有添加 Sink**

```cpp
// ❌ 没有输出目标
Logger::instance().log(level::INFO, "Hello", "Tag", {});

// ✅ 添加 Sink
auto sink = std::make_shared<ConsoleSink>();
Logger::instance().add_sink(sink);
Logger::instance().log(level::INFO, "Hello", "Tag", {});
```

### Q2: 程序崩溃后日志丢失

#### 原因

CFLogger 使用异步队列，崩溃可能导致未写入的日志丢失。

#### 解决方法

```cpp
// 在关键操作前同步刷新
void critical_operation() {
    Logger::instance().flush_sync();  // 确保之前的日志已写入
    perform_critical_task();
    Logger::instance().flush_sync();  // 确保操作日志已写入
}

// 程序退出前
int main() {
    // ... 应用逻辑 ...

    Logger::instance().flush_sync();  // 确保所有日志写入
    return 0;
}
```

### Q3: 队列溢出导致日志丢失

#### 检查方法

```cpp
size_t overflow = Logger::instance().get_normal_queue_overflow();
if (overflow > 0) {
    warning("队列溢出，丢失 " + std::to_string(overflow) + " 条日志");
}
```

#### 解决方法

**方法 1：降低日志量**

```cpp
// 提高日志级别
Logger::instance().setMininumLevel(level::WARNING);

// 减少详细日志
// trace() → debug() → info()
```

**方法 2：批量日志**

```cpp
// ❌ 逐条记录
for (int i = 0; i < 10000; ++i) {
    trace("项目: " + std::to_string(i));
}

// ✅ 批量记录
trace("开始处理 10000 个项目");
for (int i = 0; i < 10000; ++i) {
    // 处理...
}
trace("完成处理 10000 个项目");
```

**方法 3：异步处理加速**

确保 Sink 写入高效：

```cpp
// 使用更快的 Sink
// 考虑使用内存缓冲 + 批量写入
```

### Q4: 颜色显示异常

#### 问题

在文件中看到 ANSI 转义码：

```
[14:23:45] [INFO] [CFLog] ^[[92m消息^[[0m
```

#### 原因

在文件 Sink 中使用了带 COLOR 的 Formatter。

#### 解决方法

```cpp
// ❌ 文件中使用颜色
auto file_formatter = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::DEFAULT | FormatterFlag::COLOR
);

// ✅ 方法 1：使用 FileFormatter
auto file_formatter = std::make_shared<FileFormatter>(
    FormatterFlag::DEFAULT
);

// ✅ 方法 2：禁用颜色
auto config = std::make_shared<FormatterConfig>(FormatterFlag::DEFAULT);
config->disable(FormatterFlag::COLOR);
formatter->set_config(config);
```

### Q5: 编译错误

#### 问题：找不到头文件

```
fatal error: cflog/cflog.h: No such file or directory
```

#### 解决方法

```cmake
# 确保 CMakeLists.txt 正确配置
find_package(CFDesktop REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE CFDesktop::logger)

# 或者添加 include 路径
target_include_directories(my_app PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/base/logger/include
)
```

#### 问题：链接错误

```
undefined reference to `cf::log::info(std::string_view, ...)`
```

#### 解决方法

```cmake
# 确保链接了 logger 库
target_link_libraries(my_app PRIVATE CFDesktop::logger)

# 检查 logger 库是否被构建
# 确保 CMake 选项 CFDESKTOP_BUILD_LOGGER 为 ON
```

### Q6: 多线程日志混乱

#### 问题

日志输出交错或格式异常。

#### 原因

CFLogger 是线程安全的，但如果使用非线程安全的自定义 Sink 可能出现问题。

#### 解决方法

```cpp
// ✅ 确保自定义 Sink 线程安全
class ThreadSafeSink : public ISink {
public:
    bool write(const LogRecord& record) override {
        std::lock_guard<std::mutex> lock(mutex_);  // 加锁
        // 写入操作...
        return true;
    }

private:
    std::mutex mutex_;
};
```

### Q7: 性能问题

#### 问题

日志导致应用变慢。

#### 诊断方法

```cpp
// 记录性能指标
class PerformanceTracker {
public:
    void log_start() {
        start_ = std::chrono::steady_clock::now();
    }

    void log_end() {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end - start_
        ).count();
        if (duration > 1000) {  // 超过 1ms
            warning("日志操作耗时: " + std::to_string(duration) + " μs");
        }
    }

private:
    std::chrono::steady_clock::time_point start_;
};
```

#### 解决方法

1. 提高日志级别
2. 减少日志量
3. 简化格式
4. 使用更快的 Sink

参考 [性能优化](./performance.md) 获取更多建议。

### Q8: 内存泄漏

#### 问题

长时间运行后内存持续增长。

#### 可能原因

**原因 1：队列堆积**

```cpp
// 检查队列状态
size_t overflow = Logger::instance().get_normal_queue_overflow();
if (overflow > 0) {
    // 日志产生速度 > 消费速度
}
```

**原因 2：自定义 Sink 泄漏**

```cpp
// ❌ 错误的自定义 Sink
class LeakySink : public ISink {
public:
    bool write(const LogRecord& record) override {
        messages_.push_back(record.msg);  // 无限增长
        return true;
    }

private:
    std::vector<std::string> messages_;  // 从不清理
};
```

**解决方法**

```cpp
// ✅ 正确的做法
class BoundedSink : public ISink {
public:
    bool write(const LogRecord& record) override {
        std::lock_guard<std::mutex> lock(mutex_);
        messages_.push_back(record.msg);
        if (messages_.size() > max_size_) {
            messages_.erase(messages_.begin());
        }
        return true;
    }

private:
    std::vector<std::string> messages_;
    size_t max_size_ = 1000;
    std::mutex mutex_;
};
```

### Q9: 文件打开失败

#### 问题

FileSink 无法创建或写入文件。

#### 原因

- 目录不存在
- 权限不足
- 磁盘空间不足

#### 解决方法

```cpp
// ✅ 检查并创建目录
#include <filesystem>

void ensure_log_directory(const std::string& log_path) {
    std::filesystem::path path(log_path);
    std::filesystem::path dir = path.parent_path();

    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

// 使用
ensure_log_directory("/var/log/myapp/app.log");
auto sink = std::make_shared<FileSink>("/var/log/myapp/app.log");
```

### Q10: 时间戳不正确

#### 问题

日志时间与实际时间不符。

#### 原因

系统时区或时间设置问题。

#### 解决方法

```cpp
// 自定义时间格式包含时区
auto config = std::make_shared<FormatterConfig>();
config->set_timestamp_format("%Y-%m-%d %H:%M:%S %z");  // 添加时区
formatter->set_config(config);
```

## 调试技巧

### 启用详细日志

```cpp
// 临时启用所有日志
Logger::instance().setMininumLevel(level::TRACE);

// 添加详细格式
auto formatter = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::VERBOSE | FormatterFlag::COLOR
);
```

### 监控队列状态

```cpp
class QueueMonitor {
public:
    void start() {
        running_ = true;
        thread_ = std::thread([this]() {
            while (running_) {
                size_t overflow = Logger::instance().get_normal_queue_overflow();
                if (overflow > last_overflow_) {
                    std::cout << "[监控] 队列溢出: "
                              << (overflow - last_overflow_) << " 条" << std::endl;
                    last_overflow_ = overflow;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }

    void stop() {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    bool running_ = false;
    std::thread thread_;
    size_t last_overflow_ = 0;
};
```

### 捕获异常

```cpp
// 在自定义 Sink 中捕获异常
class SafeSink : public ISink {
public:
    bool write(const LogRecord& record) override {
        try {
            // 写入逻辑
            return true;
        } catch (const std::exception& e) {
            // 输出到 stderr 避免递归
            std::cerr << "Sink 错误: " << e.what() << std::endl;
            return false;
        }
    }
};
```

## 获取帮助

如果以上方法无法解决问题：

1. 检查 [测试代码](../../../../test/logger/) 了解正确用法
2. 查看 [示例代码](../../../../example/desktop/base/logger/)
3. 参考 [API 文档](../../../../desktop/base/logger/api_reference.md)
4. 搜索或提交 Issue 到项目仓库

## 常用调试命令

```bash
# 检查日志文件
tail -f app.log

# 查看错误日志
grep ERROR app.log

# 统计各级别日志数量
grep TRACE app.log | wc -l
grep DEBUG app.log | wc -l
grep INFO app.log | wc -l
grep WARNING app.log | wc -l
grep ERROR app.log | wc -l

# 查看特定标签
grep "\[Network\]" app.log

# 实时过滤
tail -f app.log | grep ERROR
```

## 下一步

- 阅读 [最佳实践](./best_practices.md) 避免常见问题
- 学习 [性能优化](./performance.md)

## 相关文档

- [快速入门](./quick_start.md)
- [最佳实践](./best_practices.md)
- [性能优化](./performance.md)
- [API 参考](../../../../desktop/base/logger/api_reference.md)
