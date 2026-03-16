# CFLogger 概述

CFLogger 是 CFDesktop 框架的异步日志系统，专为桌面应用程序设计。它提供高性能、线程安全的日志记录功能，支持多种输出目标（控制台、文件）和灵活的格式化选项。

## 为什么选择 CFLogger？

### 核心特性

- **异步处理**：日志消息在单独的工作线程中处理，不会阻塞主线程
- **高性能**：使用无锁 MPSC 队列，支持高并发写入场景
- **线程安全**：所有操作都是线程安全的，可在多线程环境中安全使用
- **灵活扩展**：支持自定义格式化器（Formatter）和输出目标（Sink）
- **优雅降级**：队列满时丢弃普通日志，但保证错误日志不丢失
- **零依赖**：不依赖 Qt 或其他第三方库，可独立使用

## 适用场景

CFLogger 适合以下场景：

- 需要在多线程环境中记录日志的应用程序
- 对性能敏感的桌面应用
- 需要同时输出到多个目标（控制台 + 文件）的场景
- 需要自定义日志格式的项目
- 嵌入式环境或禁用异常的环境

## 系统架构

CFLogger 采用分层架构设计：

```
┌─────────────────────────────────────────────────────────────┐
│                        应用层代码                              │
│  trace() / debug() / info() / warning() / error()           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                      Logger Facade                           │
│                    (Logger::instance())                      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   AsyncPostQueue                             │
│              (无锁 MPSC 队列 + 错误队列)                       │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    工作线程                                   │
│              (异步处理日志消息)                                │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                      Sinks 层                                │
│    ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │
│    │ ConsoleSink  │  │  FileSink    │  │ CustomSink   │    │
│    └──────────────┘  └──────────────┘  └──────────────┘    │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Formatters 层                              │
│    ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │
│    │ AsciiColor   │  │ FileFormatter│  │ Custom       │    │
│    │ Formatter    │  │              │  │ Formatter    │    │
│    └──────────────┘  └──────────────┘  └──────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

## 设计模式

CFLogger 使用了多种设计模式：

### 1. 单例模式 (Singleton)

Logger 类使用单例模式，确保全局只有一个日志实例：

```cpp
auto& logger = Logger::instance();
```

### 2. PIMPL 模式

实现细节隐藏在 `LoggerImpl` 中，减少编译依赖：

```cpp
// Logger 类只声明接口
class Logger {
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
```

### 3. 策略模式 (Strategy)

Formatter 和 Sink 都是可插拔的策略组件：

```cpp
// 可以在运行时切换不同的策略
sink->setFormat(factory.create("console"));
sink->setFormat(factory.create("file"));
```

### 4. 工厂模式 (Factory)

FormatterFactory 负责创建和管理 formatter 实例：

```cpp
FormatterFactory factory;
factory.register_formatter("console", []() {
    return std::make_shared<AsciiColorFormatter>();
});
```

### 5. 观察者模式 (Observer)

多个 Sink 可以"观察"同一个日志消息并各自处理：

```cpp
Logger::instance().add_sink(console_sink);
Logger::instance().add_sink(file_sink);
// 两个 sink 都会收到相同的日志消息
```

## 日志级别

CFLogger 支持五个日志级别（按严重程度递增）：

| 级别 | 说明 | 颜色 | 典型用途 |
|------|------|------|----------|
| TRACE | 最详细的输出 | 青色 | 追踪函数调用流程 |
| DEBUG | 调试信息 | 蓝色 | 开发调试使用 |
| INFO | 一般信息 | 绿色 | 正常运行信息 |
| WARNING | 警告信息 | 黄色 | 潜在问题提示 |
| ERROR | 错误信息 | 红色 | 错误和异常 |

默认级别是 **DEBUG**，低于当前级别的日志消息会被过滤掉。

## 核心组件

### Logger / LoggerImpl

主日志类，提供日志记录的公共 API。使用 PIMPL 模式隐藏实现细节。

**关键文件**：
- [cflog.hpp](../../../../desktop/base/logger/include/cflog/cflog.hpp)
- [cflog_impl.h](../../../../desktop/base/logger/src/impl/cflog_impl.h)

### AsyncPostQueue

异步队列，使用无锁 MPSC（多生产者单消费者）队列实现：

- 正常日志队列容量：65536 条
- 错误日志队列：无限制
- 队列满时：丢弃普通日志，保留错误日志

**关键文件**：
- [async_queue.h](../../../../desktop/base/logger/src/async_queue/async_queue.h)

### Formatter（格式化器）

将日志记录转换为可读文本：

- `DefaultFormatter`：最简单的格式化器，只输出消息本身
- `AsciiColorFormatter`：带 ANSI 颜色的控制台格式化器
- `FileFormatter`：用于文件的纯文本格式化器

### Sink（输出目标）

日志输出的目标位置：

- `ConsoleSink`：输出到控制台（stdout/stderr）
- `FileSink`：输出到文件（支持追加和截断模式）
- `ISink`：自定义 sink 的接口

## 快速示例

### 简单使用

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    // 设置日志级别
    set_level(level::TRACE);

    // 记录不同级别的日志
    trace("程序启动");
    info("初始化完成");
    warning("配置文件未找到，使用默认值");

    flush();  // 确保所有日志都写入
    return 0;
}
```

### 高级使用

```cpp
#include "cflog/cflog.hpp"

void init_logger() {
    using namespace cf::log;

    // 创建格式化器工厂
    FormatterFactory factory;
    factory.register_formatter("console", []() {
        return std::make_shared<AsciiColorFormatter>();
    });
    factory.register_formatter("file", []() {
        return std::make_shared<FileFormatter>();
    });

    // 配置控制台输出
    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink);

    // 配置文件输出
    auto file_sink = std::make_shared<FileSink>("app.log");
    file_sink->setFormat(factory.create("file"));
    Logger::instance().add_sink(file_sink);

    // 设置最低日志级别
    Logger::instance().setMininumLevel(level::INFO);
}
```

## 性能特性

根据基准测试结果：

| 场景 | 性能 |
|------|------|
| 单线程基准 | ≥10,000 条日志/秒 |
| 多线程（16 线程） | 保持基准性能 |
| 目标性能 | 10,000-50,000 条日志/秒 |

详细性能数据请参阅 [HandBook/performance.md](../../../HandBook/desktop/base/logger/performance.md)。

## 线程安全保证

- ✅ 所有公共 API 都是线程安全的
- ✅ 多线程同时写入不会造成数据竞争
- ✅ 动态添加/移除 sink 是线程安全的
- ✅ 运行时修改日志级别是线程安全的

## 错误处理

CFLogger 采用"不抛异常"的设计理念：

- 队列满时静默丢弃普通日志，计数器递增
- 错误级别的日志永远不会被丢弃
- 文件写入失败不会影响其他 sink 的正常工作

## 相关文档

- [架构详解](./architecture.md) - 详细的架构设计和组件交互
- [API 参考](./api_reference.md) - 完整的 API 文档
- [HandBook/快速入门](../../../HandBook/desktop/base/logger/quick_start.md) - 5 分钟入门指南
- [HandBook/最佳实践](../../../HandBook/desktop/base/logger/best_practices.md) - 使用建议
