# Sink 详解

Sink 决定日志的输出目标。CFLogger 提供了 ConsoleSink 和 FileSink，并支持自定义 Sink。

## Sink 概览

### 什么是 Sink？

```
LogRecord (日志记录)
         ↓
    Formatter (格式化)
         ↓
   std::string (格式化文本)
         ↓
      Sink (输出目标)
         ↓
    实际存储位置
```

### Sink 职责

1. **接收格式化后的文本**：从 Formatter 获取格式化的字符串
2. **写入目标**：将文本写入到目标位置
3. **缓冲刷新**：确保数据及时写入

## 内置 Sink

### ConsoleSink

输出到标准输出（stdout）。

```cpp
#include "cflog/sinks/console_sink.h"

using namespace cf::log;

auto console_sink = std::make_shared<ConsoleSink>();
console_sink->setFormat(std::make_shared<AsciiColorFormatter>());

Logger::instance().add_sink(console_sink);
```

**特点**：
- 线程安全
- 自动刷新
- 适合开发调试

### FileSink

输出到文件。

```cpp
#include "cflog/sinks/file_sink.h"

using namespace cf::log;

// 追加模式（默认）
auto file_sink = std::make_shared<FileSink>("app.log");

// 覆盖模式
auto file_sink = std::make_shared<FileSink>(
    "app.log",
    OpenMode::Truncate
);

file_sink->setFormat(std::make_shared<FileFormatter>());

Logger::instance().add_sink(file_sink);
```

**特点**：
- 线程安全
- 自动刷新
- 支持追加和覆盖模式
- 不可拷贝（删除了拷贝构造和赋值）

### OpenMode 选项

| 模式 | 说明 | 适用场景 |
|------|------|----------|
| `OpenMode::Append` | 追加到文件末尾 | 生产环境、长期日志 |
| `OpenMode::Truncate` | 覆盖现有文件 | 测试、每次运行新日志 |

## 多 Sink 配置

### 同时输出到控制台和文件

```cpp
void setup_logging() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("console", []() {
        return std::make_shared<AsciiColorFormatter>();
    });
    factory.register_formatter("file", []() {
        return std::make_shared<FileFormatter>();
    });

    // 控制台：带颜色
    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink);

    // 文件：纯文本
    auto file_sink = std::make_shared<FileSink>("app.log");
    file_sink->setFormat(factory.create("file"));
    Logger::instance().add_sink(file_sink);
}
```

### 分级输出（普通日志 vs 错误日志）

```cpp
void setup_split_logging() {
    using namespace cf::log;

    FormatterFactory factory;

    // 普通日志文件
    auto normal_sink = std::make_shared<FileSink>("app.log");
    auto normal_formatter = std::make_shared<FileFormatter>(
        FormatterFlag::DEFAULT
    );
    normal_sink->setFormat(normal_formatter);

    // 错误日志文件（需要自定义 Sink 过滤）
    // 这里简化为使用不同格式
    auto error_formatter = std::make_shared<FileFormatter>(
        FormatterFlag::DEFAULT
    );
    // 实际使用需要自定义 Sink 来过滤 ERROR 级别
}
```

## 自定义 Sink

### 基本结构

```cpp
#include "cflog/cflog_sink.h"

class MyCustomSink : public ISink {
public:
    MyCustomSink() = default;

    // 必须实现
    bool write(const LogRecord& record) override {
        // 写入逻辑
        return true;  // 成功返回 true
    }

    bool flush() override {
        // 刷新逻辑
        return true;
    }

    // 可选：设置格式化器
    bool setFormat(std::shared_ptr<IFormatter> formatter) override {
        formatter_ = formatter;
        return true;
    }

protected:
    // 检查是否有可用格式化器
    bool formatable() const override {
        return formatter_ != nullptr;
    }

    std::shared_ptr<IFormatter> formatter_;
};
```

### 旋转文件 Sink

```cpp
#include "cflog/cflog_sink.h"
#include <fstream>
#include <sstream>

class RotatingFileSink : public ISink {
public:
    RotatingFileSink(const std::string& base_path,
                     size_t max_size = 1024 * 1024)  // 1MB
        : base_path_(base_path),
          max_size_(max_size),
          current_size_(0),
          file_index_(0) {
        open_new_file();
    }

    ~RotatingFileSink() override {
        if (file_.is_open()) {
            file_.close();
        }
    }

    bool write(const LogRecord& record) override {
        std::string content;

        if (formatable() && formatter_) {
            content = formatter_->format_me(record);
        } else {
            content = record.msg;
        }

        content += "\n";

        // 检查是否需要旋转
        if (current_size_ + content.size() > max_size_) {
            rotate();
        }

        file_ << content;
        current_size_ += content.size();
        return file_.good();
    }

    bool flush() override {
        if (file_.is_open()) {
            file_ << std::flush;
            return true;
        }
        return false;
    }

private:
    void open_new_file() {
        if (file_.is_open()) {
            file_.close();
        }

        std::string path = get_current_path();
        file_.open(path, std::ios::out | std::ios::app);
        current_size_ = 0;
    }

    void rotate() {
        file_index_++;
        open_new_file();
    }

    std::string get_current_path() const {
        std::ostringstream oss;
        oss << base_path_;
        if (file_index_ > 0) {
            oss << "." << file_index_;
        }
        return oss.str();
    }

    std::string base_path_;
    size_t max_size_;
    size_t current_size_;
    size_t file_index_;
    std::ofstream file_;
};
```

### 过滤 Sink

只记录特定级别的日志：

```cpp
class LevelFilterSink : public ISink {
public:
    LevelFilterSink(std::shared_ptr<ISink> wrapped_sink,
                    level min_level = level::INFO)
        : wrapped_sink_(wrapped_sink),
          min_level_(min_level) {}

    bool write(const LogRecord& record) override {
        if (as<int>(record.lvl) >= as<int>(min_level_)) {
            return wrapped_sink_->write(record);
        }
        return true;  // 跳过，视为成功
    }

    bool flush() override {
        return wrapped_sink_->flush();
    }

    bool setFormat(std::shared_ptr<IFormatter> formatter) override {
        return wrapped_sink_->setFormat(formatter);
    }

private:
    std::shared_ptr<ISink> wrapped_sink_;
    level min_level_;
};
```

### 统计 Sink

记录日志级别的统计信息：

```cpp
#include <map>
#include <mutex>

class StatisticsSink : public ISink {
public:
    bool write(const LogRecord& record) override {
        std::lock_guard<std::mutex> lock(mutex_);
        counts_[record.lvl]++;
        return true;
    }

    bool flush() override {
        return true;
    }

    size_t get_count(level lvl) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = counts_.find(lvl);
        return it != counts_.end() ? it->second : 0;
    }

    void print_summary() const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [lvl, count] : counts_) {
            std::cout << to_string(lvl) << ": " << count << std::endl;
        }
    }

private:
    mutable std::mutex mutex_;
    std::map<level, size_t> counts_;
};
```

### 网络 Sink

将日志发送到远程服务器：

```cpp
#include <asio.hpp>
#include <queue>
#include <mutex>

class NetworkSink : public ISink {
public:
    NetworkSink(const std::string& host, int port)
        : host_(host),
          port_(port),
          work_(io_context_),
          socket_(io_context_) {

        // 启动网络线程
        network_thread_ = std::thread([this]() {
            io_context_.run();
        });

        // 连接服务器
        connect();
    }

    ~NetworkSink() override {
        io_context_.stop();
        if (network_thread_.joinable()) {
            network_thread_.join();
        }
    }

    bool write(const LogRecord& record) override {
        std::string content;

        if (formatable() && formatter_) {
            content = formatter_->format_me(record);
        } else {
            content = record.msg;
        }

        // 将消息加入队列
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            message_queue_.push(content);
        }

        // 异步发送
        send_next();

        return true;
    }

    bool flush() override {
        // 等待队列清空
        while (!message_queue_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return true;
    }

private:
    void connect() {
        try {
            asio::ip::tcp::endpoint endpoint(
                asio::ip::make_address(host_),
                port_
            );
            asio::connect(socket_, &endpoint, &endpoint + 1);
        } catch (...) {
            // 连接失败，稍后重试
        }
    }

    void send_next() {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (!message_queue_.empty() && sending_.empty()) {
            sending_ = message_queue_.front();
            message_queue_.pop();

            async_send();
        }
    }

    void async_send() {
        asio::async_write(
            socket_,
            asio::buffer(sending_ + "\n"),
            [this](std::error_code ec, size_t /*length*/) {
                if (!ec) {
                    sending_.clear();
                    send_next();
                } else {
                    // 发送失败，重连
                    socket_.close();
                    connect();
                }
            }
        );
    }

    std::string host_;
    int port_;

    asio::io_context io_context_;
    asio::io_context::work work_;
    asio::ip::tcp::socket socket_;
    std::thread network_thread_;

    std::queue<std::string> message_queue_;
    std::string sending_;
    std::mutex queue_mutex_;
};
```

## Sink 最佳实践

### 1. 线程安全

所有 Sink 必须是线程安全的。使用互斥锁保护共享数据：

```cpp
class ThreadSafeSink : public ISink {
public:
    bool write(const LogRecord& record) override {
        std::lock_guard<std::mutex> lock(mutex_);
        // 写入操作...
        return true;
    }

private:
    std::mutex mutex_;
};
```

### 2. 错误处理

Sink 不应抛出异常，应该返回错误状态：

```cpp
bool write(const LogRecord& record) override {
    try {
        // 写入操作...
        return true;
    } catch (...) {
        // 记录错误但不抛出异常
        return false;
    }
}
```

### 3. 资源清理

在析构函数中确保资源正确释放：

```cpp
~MyCustomSink() override {
    if (file_.is_open()) {
        file_.close();
    }
    if (socket_.is_open()) {
        socket_.close();
    }
}
```

## 使用示例

### 组合多个 Sink

```cpp
void setup_comprehensive_logging() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("console", []() {
        return std::make_shared<AsciiColorFormatter>(
            FormatterFlag::DEFAULT | FormatterFlag::COLOR
        );
    });
    factory.register_formatter("file", []() {
        return std::make_shared<FileFormatter>(
            FormatterFlag::DEFAULT
        );
    });

    auto& logger = Logger::instance();
    logger.clear_sinks();

    // 控制台
    auto console = std::make_shared<ConsoleSink>();
    console->setFormat(factory.create("console"));
    logger.add_sink(console);

    // 主日志文件
    auto main_file = std::make_shared<FileSink>("app.log");
    main_file->setFormat(factory.create("file"));
    logger.add_sink(main_file);

    // 旋转日志文件（每日）
    auto daily_file = std::make_shared<RotatingFileSink>(
        "daily.log",
        24 * 60 * 60 * 1000  // 24小时
    );
    daily_file->setFormat(factory.create("file"));
    logger.add_sink(daily_file);

    // 统计
    auto stats = std::make_shared<StatisticsSink>();
    logger.add_sink(stats);

    logger.setMininumLevel(level::INFO);
}
```

## 下一步

- 阅读 [配置选项](./configuration.md)
- 学习 [最佳实践](./best_practices.md)

## 相关文档

- [格式化器详解](./formatters.md)
- [高级用法](./advanced_usage.md)
- [API 参考](../../../../desktop/base/logger/api_reference.md)
