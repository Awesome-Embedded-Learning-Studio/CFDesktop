# CFLogger 使用手册

欢迎使用 CFLogger 使用手册！CFLogger 是 CFDesktop 框架的异步日志系统，专为现代桌面应用程序设计。

## 什么是 CFLogger？

CFLogger 是一个**高性能、异步、线程安全**的 C++ 日志库，具有以下特点：

- **异步处理**：日志在后台线程处理，不阻塞主线程
- **零依赖**：不依赖 Qt 或其他第三方库
- **灵活扩展**：支持自定义格式化器和输出目标
- **优雅降级**：队列满时优先保证错误日志不丢失
- **简单易用**：提供简单 API 和高级 API 两套接口

## 为什么使用 CFLogger？

| 特性 | CFLogger | 其他日志库 |
|------|----------|-----------|
| 异步处理 | ✅ 内置 | ⚠️ 部分支持 |
| Qt 依赖 | ❌ 无依赖 | ⚠️ 常需 Qt |
| 异常机制 | ❌ 不抛异常 | ⚠️ 可能抛异常 |
| 自定义格式 | ✅ 完全可定制 | ⚠️ 有限支持 |
| 线程安全 | ✅ 全线程安全 | ⚠️ 部分场景需额外处理 |
| 错误保护 | ✅ ERROR 日志永不丢失 | ❌ 可能全部丢失 |

## 适用场景

CFLogger 特别适合以下场景：

### ✅ 推荐使用

- 多线程桌面应用程序
- 需要同时输出到控制台和文件
- 对日志性能有要求的应用
- 需要自定义日志格式
- 嵌入式环境或禁用异常的环境

### ⚠️ 谨慎使用

- 简单单线程脚本（可能过度设计）
- 需要同步日志确认的场景（使用 `flush_sync()`）

## 快速预览

### 最简单的使用

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    info("Hello, CFLogger!");
    return 0;
}
```

### 典型配置

```cpp
#include "cflog/cflog.hpp"

void init_logger() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("console", []() {
        return std::make_shared<AsciiColorFormatter>();
    });

    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink);

    Logger::instance().setMininumLevel(level::INFO);
}
```

## 日志输出示例

### 默认格式输出

```
[14:23:45] [INFO] [CFLog] Application started
[14:23:46] [WARNING] [Config] Config file not found
[14:23:47] [ERROR] [Network] Connection failed
```

### 彩色控制台输出

```
[14:23:45] [INFO] [CFLog] Application started
          ^^^^^^ 绿色

[14:23:46] [WARNING] [Config] Config file not found
          ^^^^^^^ 黄色

[14:23:47] [ERROR] [Network] Connection failed
          ^^^^^ 红色
```

## 手册结构

本手册包含以下章节：

| 章节 | 内容 |
|------|------|
| [快速入门](./quick_start.md) | 5 分钟上手 CFLogger |
| [基础日志](./basic_logging.md) | 简单 API 详解 |
| [高级用法](./advanced_usage.md) | 自定义 Sink 和 Formatter |
| [格式化器](./formatters.md) | Formatter 系统详解 |
| [输出目标](./sinks.md) | Sink 系统详解 |
| [配置选项](./configuration.md) | 完整配置参考 |
| [最佳实践](./best_practices.md) | 推荐的使用模式 |
| [性能优化](./performance.md) | 性能数据和优化建议 |
| [故障排除](./troubleshooting.md) | 常见问题解答 |

## 学习路径

```
初学者路径：
overview → quick_start → basic_logging → best_practices

进阶路径：
overview → advanced_usage → formatters → sinks → configuration

深入路径：
overview → architecture → performance → troubleshooting
```

## 相关资源

- **API 参考**：[api_reference.md](../../../../desktop/base/logger/api_reference.md)
- **架构详解**：[architecture.md](../../../../desktop/base/logger/architecture.md)

## 版本要求

- C++17 或更高
- 支持的编译器：GCC 9+, Clang 10+, MSVC 2019+
- 支持的平台：Linux, Windows, macOS

## 获取帮助

如果在使用过程中遇到问题：

1. 查看 [故障排除](./troubleshooting.md) 章节
2. 阅读 [最佳实践](./best_practices.md) 了解推荐用法
3. 查看 [API 参考](../../../../desktop/base/logger/api_reference.md) 了解详细 API

开始使用 CFLogger，请阅读 [快速入门](./quick_start.md)！
