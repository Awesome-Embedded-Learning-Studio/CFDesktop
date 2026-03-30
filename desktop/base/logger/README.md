# CFLog -- Logger Components

CFDesktop 的日志系统模块，提供高性能、多级别、异步的日志记录功能。

## Module Overview

CFLog 是 CFDesktop 的核心日志库，为整个桌面应用程序提供统一的日志基础设施。该模块设计为独立组件，不依赖 Qt，可单独用于任何需要日志功能的场景。

主要特点：
- **多级别日志** -- 支持 TRACE / DEBUG / INFO / WARNING / ERROR 五个日志级别
- **异步写入** -- 基于内部异步队列，日志写入不会阻塞调用线程
- **多 Sink 架构** -- 支持同时输出到多个目标（控制台、文件等），可自由扩展
- **格式化输出** -- 可配置的格式化器，支持自定义日志格式
- **线程安全** -- Logger 实例全局单例，所有操作线程安全
- **自动源码定位** -- 基于 `std::source_location`，自动捕获调用位置

## Key Features

### Log Levels

| 级别 | 用途 |
|------|------|
| `level::TRACE` | 最详细的诊断输出 |
| `level::DEBUG` | 调试诊断信息 |
| `level::INFO` | 一般信息性消息（Release 构建默认级别） |
| `level::WARNING` | 潜在问题警告 |
| `level::ERROR` | 错误事件 |

在 Debug / Develop 构建中，默认级别为 `TRACE`；Release 构建中默认为 `INFO`。

### Sinks

内置 Sink 实现：
- **ConsoleSink** -- 输出到标准控制台
- **FileSink** -- 输出到文件

可通过实现 `ISink` 接口扩展自定义 Sink。

### Formatters

内置 Formatter 实现：
- **ConsoleFormatter** -- 控制台友好的彩色输出格式
- **DefaultFormatter** -- 通用默认格式
- **FileFormatter** -- 适合文件持久化的格式

可通过实现 `IFormatter` 接口扩展自定义 Formatter。

## Usage Example

### Basic Logging

```cpp
#include "cflog.h"

// 简单字符串日志
cf::log::info("Application started");
cf::log::error("Something went wrong", "NetworkModule");

// 格式化日志（std::format 风格）
cf::log::infof("Connected to {}:{}", host, port);
cf::log::warningftag("Auth", "Login failed for user {}", username);
```

### Advanced Configuration

```cpp
#include "cflog/cflog.hpp"
#include "cflog/sinks/console_sink.h"
#include "cflog/sinks/file_sink.h"
#include "cflog/formatter/default_formatter.h"

auto& logger = cf::log::Logger::instance();

// 添加自定义 Sink
auto console_sink = std::make_shared<cf::log::ConsoleSink>();
auto file_sink = std::make_shared<cf::log::FileSink>("app.log");
logger.add_sink(console_sink);
logger.add_sink(file_sink);

// 设置最低日志级别
cf::log::set_level(cf::log::level::DEBUG);

// 刷新日志
cf::log::flush();
```

## Directory Structure

```
desktop/base/logger/
├── CMakeLists.txt
├── README.md                   # This file
├── include/
│   ├── cflog.h                 # Convenient logging functions (trace/debug/info/...)
│   └── cflog/
│       ├── cflog.hpp           # Logger class (main interface)
│       ├── cflog_export.h      # Export macros
│       ├── cflog_level.hpp     # Log level enum and utilities
│       ├── cflog_record.h      # Log record structure
│       ├── cflog_sink.h        # ISink interface
│       ├── cflog_format.h      # IFormatter interface
│       ├── cflog_format_config.h
│       ├── cflog_format_factory.h
│       ├── cflog_format_flags.h
│       ├── formatter/          # Built-in formatters
│       └── sinks/              # Built-in sinks
└── src/
    ├── async_queue/            # Async message queue
    ├── formatter/              # Formatter implementations
    ├── impl/                   # Internal implementation
    ├── logger/                 # Logger core
    └── sinks/                  # Sink implementations
```

## HandBook Documentation

详细文档请参阅 HandBook：

- [Overview](../../../document/HandBook/desktop/base/logger/overview.md)
- [Quick Start](../../../document/HandBook/desktop/base/logger/quick_start.md)
- [Basic Logging](../../../document/HandBook/desktop/base/logger/basic_logging.md)
- [Advanced Usage](../../../document/HandBook/desktop/base/logger/advanced_usage.md)
- [Sinks](../../../document/HandBook/desktop/base/logger/sinks.md)
- [Formatters](../../../document/HandBook/desktop/base/logger/formatters.md)
- [Configuration](../../../document/HandBook/desktop/base/logger/configuration.md)
- [Performance](../../../document/HandBook/desktop/base/logger/performance.md)
- [Best Practices](../../../document/HandBook/desktop/base/logger/best_practices.md)
- [Troubleshooting](../../../document/HandBook/desktop/base/logger/troubleshooting.md)
