# 高级用法

本文档介绍 CFLogger 的高级功能，包括自定义 Sink、Formatter 以及运行时配置。

## 为什么使用高级 API？

简单 API (`cflog.h`) 适合快速上手，但高级 API (`cflog.hpp`) 提供更多控制：

| 功能 | 简单 API | 高级 API |
|------|----------|----------|
| 基本日志记录 | ✅ | ✅ |
| 自定义 Sink | ❌ | ✅ |
| 自定义 Formatter | ❌ | ✅ |
| 同步刷新 | ❌ | ✅ |
| 运行时配置 | ⚠️ 有限 | ✅ 完全控制 |

## 高级 API 基础

### 引入头文件

```cpp
#include "cflog/cflog.hpp"
#include "cflog/cflog_format_factory.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/sinks/console_sink.h"
```

### 获取 Logger 实例

```cpp
using namespace cf::log;

auto& logger = Logger::instance();
```

### 基本用法

```cpp
// 记录日志
logger.log(level::INFO, "消息内容", "标签", std::source_location::current());

// 设置最低级别
logger.setMininumLevel(level::INFO);

// 刷新
logger.flush();       // 异步
logger.flush_sync();  // 同步
```

## 自定义 Sink

Sink 决定日志的输出目标。CFLogger 提供 ConsoleSink 和 FileSink，你也可以创建自定义 Sink。

### 创建自定义 Sink

```cpp
#include "cflog/cflog_sink.h"
#include "cflog/cflog_record.h"
#include <iostream>

class MyCustomSink : public ISink {
public:
    MyCustomSink(const std::string& prefix = "[MY LOG] ")
        : prefix_(prefix) {}

    bool write(const LogRecord& record) override {
        // 如果有格式化器，使用格式化器
        if (formatable() && formatter_) {
            auto formatted = formatter_->format_me(record);
            output_stream_ << prefix_ << formatted << std::endl;
        } else {
            // 没有格式化器，简单输出
            output_stream_ << prefix_ << record.msg << std::endl;
        }
        return true;
    }

    bool flush() override {
        output_stream_ << std::flush;
        return true;
    }

private:
    std::string prefix_;
    std::ostream& output_stream_ = std::cout;
};
```

### 使用自定义 Sink

```cpp
// 创建自定义 Sink
auto my_sink = std::make_shared<MyCustomSink>("[CUSTOM] ");

// 设置格式化器
my_sink->setFormat(std::make_shared<AsciiColorFormatter>());

// 添加到 Logger
Logger::instance().add_sink(my_sink);
```

### 网络 Sink 示例

```cpp
#include <asio.hpp>

class NetworkSink : public ISink {
public:
    NetworkSink(const std::string& host, int port)
        : endpoint_(asio::ip::make_address(host), port),
          socket_(io_context_) {}

    bool write(const LogRecord& record) override {
        if (formatable() && formatter_) {
            messages_.push_back(formatter_->format_me(record));
        }
        return true;
    }

    bool flush() override {
        try {
            asio::ip::tcp::socket socket(io_context_);
            socket.connect(endpoint_);
            for (const auto& msg : messages_) {
                asio::write(socket, asio::buffer(msg + "\n"));
            }
            messages_.clear();
            return true;
        } catch (...) {
            return false;
        }
    }

private:
    asio::io_context io_context_;
    asio::ip::tcp::endpoint endpoint_;
    asio::ip::tcp::socket socket_;
    std::vector<std::string> messages_;
};
```

## 自定义 Formatter

Formatter 决定日志的输出格式。

### 创建自定义 Formatter

```cpp
#include "cflog/cflog_format.h"
#include "cflog/cflog_record.h"
#include <sstream>
#include <iomanip>

class JsonFormatter : public IFormatter {
public:
    std::string format_me(const LogRecord& r) override {
        std::ostringstream oss;
        oss << "{";
        oss << "\"level\":\"" << to_string(r.lvl) << "\",";
        oss << "\"tag\":\"" << r.tag << "\",";
        oss << "\"message\":\"" << r.msg << "\",";
        oss << "\"thread\":\"" << r.tid << "\"";
        oss << "}";
        return oss.str();
    }

    bool configurable() const override { return false; }
};
```

### 可配置的 Formatter

```cpp
class ConfigurableFormatter : public IFormatter {
public:
    ConfigurableFormatter() {
        config_ = std::make_shared<FormatterConfig>(
            FormatterFlag::DEFAULT
        );
    }

    std::string format_me(const LogRecord& r) override {
        std::string result;

        auto flags = config_->get_flags();

        if ((flags & FormatterFlag::TIMESTAMP)) {
            result += format_timestamp(r);
        }
        if ((flags & FormatterFlag::LEVEL)) {
            result += "[" + std::string(to_string(r.lvl)) + "] ";
        }
        if ((flags & FormatterFlag::MESSAGE)) {
            result += r.msg;
        }

        return result;
    }

    bool configurable() const override { return true; }

    bool set_config(std::shared_ptr<FormatterConfig> config) override {
        if (config) {
            config_ = config;
            return true;
        }
        return false;
    }

    std::shared_ptr<FormatterConfig> get_config() const override {
        return config_;
    }

private:
    std::shared_ptr<FormatterConfig> config_;
};
```

## FormatterFactory 使用

FormatterFactory 管理和创建 Formatter 实例。

### 基本用法

```cpp
FormatterFactory factory;

// 注册格式化器（函数形式）
factory.register_formatter("console", []() {
    return std::make_shared<AsciiColorFormatter>();
});

// 注册格式化器（单例形式）
auto json_formatter = std::make_shared<JsonFormatter>();
factory.register_formatter("json", json_formatter);

// 创建新实例
auto formatter = factory.create("console");

// 获取或创建（带缓存）
auto cached = factory.get_or_create("console");

// 清除缓存
factory.clear_cache();
```

### 运行时切换格式

```cpp
FormatterFactory factory;
factory.register_formatter("simple", []() {
    return std::make_shared<AsciiColorFormatter>(FormatterFlag::MINIMAL);
});
factory.register_formatter("verbose", []() {
    return std::make_shared<AsciiColorFormatter>(FormatterFlag::VERBOSE);
});

auto sink = std::make_shared<ConsoleSink>();

// 使用简单格式
sink->setFormat(factory.create("simple"));

// 运行时切换到详细格式
sink->setFormat(factory.create("verbose"));
```

## 动态 Sink 管理

### 运行时添加 Sink

```cpp
void enable_file_logging(const std::string& path) {
    auto file_sink = std::make_shared<FileSink>(path);
    file_sink->setFormat(std::make_shared<FileFormatter>());
    Logger::instance().add_sink(file_sink);
}
```

### 运行时移除 Sink

```cpp
void disable_file_logging(FileSink* sink) {
    Logger::instance().remove_sink(sink);
}
```

### 清除所有 Sink

```cpp
Logger::instance().clear_sinks();
```

## 运行时配置修改

### 动态修改日志级别

```cpp
// 白天使用 INFO 级别
Logger::instance().setMininumLevel(level::INFO);

// 晚上调试时使用 DEBUG 级别
Logger::instance().setMininumLevel(level::DEBUG);
```

### 动态修改 Formatter 配置

```cpp
auto formatter = std::make_shared<AsciiColorFormatter>();
auto config = std::make_shared<FormatterConfig>();

// 禁用颜色输出
config->disable(FormatterFlag::COLOR);

// 设置时间格式
config->set_timestamp_format("%Y-%m-%d %H:%M:%S");

formatter->set_config(config);
```

## 多环境配置

### 开发环境配置

```cpp
void setup_dev_logging() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("dev", []() {
        // 开发环境：详细输出 + 颜色
        return std::make_shared<AsciiColorFormatter>(
            FormatterFlag::VERBOSE | FormatterFlag::COLOR
        );
    });

    auto console = std::make_shared<ConsoleSink>();
    console->setFormat(factory.create("dev"));

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(console);
    Logger::instance().setMininumLevel(level::TRACE);
}
```

### 生产环境配置

```cpp
void setup_prod_logging() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("prod", []() {
        // 生产环境：简洁输出，无颜色
        return std::make_shared<FileFormatter>(
            FormatterFlag::DEFAULT
        );
    });

    // 文件输出
    auto file = std::make_shared<FileSink>(
        "/var/log/myapp/app.log",
        OpenMode::Append
    );
    file->setFormat(factory.create("prod"));

    // 错误单独输出
    auto error_file = std::make_shared<FileSink>(
        "/var/log/myapp/error.log",
        OpenMode::Append
    );
    error_file->setFormat(factory.create("prod"));

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(file);
    Logger::instance().add_sink(error_file);
    Logger::instance().setMininumLevel(level::INFO);
}
```

## 完整示例

```cpp
#include "cflog/cflog.hpp"
#include "cflog/cflog_format_factory.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/sinks/console_sink.h"
#include "cflog/sinks/file_sink.h"
#include <memory>

class AdvancedLoggerConfig {
public:
    static void initialize(bool is_production = false) {
        using namespace cf::log;

        FormatterFactory factory;

        // 注册格式化器
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

        // 清除现有 sink
        Logger::instance().clear_sinks();

        // 添加控制台输出
        auto console_sink = std::make_shared<ConsoleSink>();
        console_sink->setFormat(factory.create("console"));
        Logger::instance().add_sink(console_sink);

        // 生产环境添加文件输出
        if (is_production) {
            auto file_sink = std::make_shared<FileSink>(
                "app.log",
                OpenMode::Append
            );
            file_sink->setFormat(factory.create("file"));
            Logger::instance().add_sink(file_sink);
        }

        // 设置日志级别
        Logger::instance().setMininumLevel(
            is_production ? level::INFO : level::DEBUG
        );
    }
};

int main(int argc, char** argv) {
    // 根据命令行参数配置
    bool is_prod = (argc > 1 && std::string(argv[1]) == "--prod");
    AdvancedLoggerConfig::initialize(is_prod);

    using namespace cf::log;
    info("应用程序启动");

    // 你的应用逻辑...

    Logger::instance().flush_sync();
    return 0;
}
```

## 下一步

- 学习 [格式化器详解](./formatters.md)
- 学习 [Sink 详解](./sinks.md)
- 阅读 [最佳实践](./best_practices.md)

## 相关文档

- [格式化器详解](./formatters.md)
- [Sink 详解](./sinks.md)
- [配置选项](./configuration.md)
- [API 参考](../../../../desktop/base/logger/api_reference.md)
