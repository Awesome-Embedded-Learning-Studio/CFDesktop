# 基础日志详解

本文档详细介绍 CFLogger 的简单 API 使用方法。

## 简单 API 概述

简单 API 提供了一组便捷函数，适合大多数使用场景。

```cpp
#include "cflog/cflog.h"
```

## 日志函数详解

### trace() - 追踪日志

```cpp
void trace(std::string_view msg,
           std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());
```

**用途**：记录最详细的输出信息，通常用于追踪函数调用流程。

**示例**：

```cpp
void process_request(const std::string& request) {
    trace("收到请求: " + request, "HTTP");
    // 处理请求...
    trace("请求处理完成", "HTTP");
}
```

**建议**：
- 生产环境通常设置为禁用
- 适合开发和调试阶段
- 避免在循环中大量使用（性能影响）

### debug() - 调试日志

```cpp
void debug(std::string_view msg,
           std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());
```

**用途**：记录调试信息，帮助开发者理解程序运行状态。

**示例**：

```cpp
void connect_database(const std::string& url) {
    debug("连接数据库: " + url, "Database");
    // 连接逻辑...
    debug("数据库连接成功", "Database");
}
```

**建议**：
- 开发环境默认级别
- 记录关键的中间状态
- 生产环境可选择性开启

### info() - 信息日志

```cpp
void info(std::string_view msg,
          std::string_view tag = "CFLog",
          std::source_location loc = std::source_location::current());
```

**用途**：记录程序正常运行的重要信息。

**示例**：

```cpp
info("应用程序启动", "App");
info("配置文件加载完成", "Config");
info("服务器启动，监听端口 8080", "Server");
```

**建议**：
- 生产环境推荐的最低级别
- 记录重要的状态变化
- 保持适度，避免信息过载

### warning() - 警告日志

```cpp
void warning(std::string_view msg,
             std::string_view tag = "CFLog",
             std::source_location loc = std::source_location::current());
```

**用途**：记录潜在的问题，不会影响程序继续运行。

**示例**：

```cpp
void load_config(const std::string& path) {
    if (!file_exists(path)) {
        warning("配置文件不存在，使用默认配置", "Config");
        // 使用默认配置...
    }
}
```

**建议**：
- 记录可恢复的异常情况
- 使用有意义的警告信息
- 生产环境应该关注

### error() - 错误日志

```cpp
void error(std::string_view msg,
           std::source_location loc = std::source_location::current());
```

**用途**：记录错误和异常情况。

**示例**：

```cpp
void save_file(const std::string& path) {
    if (!write_file(path, data)) {
        error("文件保存失败: " + path, "FileIO");
    }
}
```

**重要特性**：
- **ERROR 日志永不丢失**：即使队列满，ERROR 日志也会被保留
- 应该记录足够的信息用于排查问题
- 生产环境必须关注

## 日志级别控制

### set_level()

```cpp
void set_level(level lvl);
```

设置全局最低日志级别，低于此级别的日志将被过滤。

### 级别过滤示例

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    // 设置为 INFO 级别
    set_level(level::INFO);

    trace("被过滤");   // TRACE < INFO
    debug("被过滤");   // DEBUG < INFO
    info("显示");      // INFO >= INFO ✓
    warning("显示");   // WARNING >= INFO ✓
    error("显示");     // ERROR >= INFO ✓

    flush();
    return 0;
}
```

### 不同环境的推荐级别

| 环境 | 推荐级别 | 说明 |
|------|----------|------|
| 开发 | `DEBUG` 或 `TRACE` | 详细信息便于调试 |
| 测试 | `INFO` | 关注正常流程 |
| 预发布 | `WARNING` | 关注潜在问题 |
| 生产 | `INFO` 或 `WARNING` | 平衡性能和可观测性 |

## 使用标签

标签用于组织和分类日志。

### 基本用法

```cpp
info("用户登录成功", "Auth");
info("查询数据库", "Database");
warning("缓存未命中", "Cache");
error("连接超时", "Network");
```

### 推荐的标签命名

| 模块 | 推荐标签 |
|------|----------|
| 用户认证 | Auth, User, Login |
| 数据库 | Database, DB, SQL |
| 网络 | Network, HTTP, API |
| 配置 | Config, Settings |
| 缓存 | Cache, Redis |
| 文件操作 | FileIO, FileSystem |
| UI 渲染 | UI, Render, Paint |
| 性能 | Performance, Metrics |

### 标签最佳实践

1. **使用一致的命名**：全大写或驼峰命名
2. **避免过于细分**：`Database` 比 `MySQLQueryExecutorInsert` 更好
3. **关联模块**：相关功能使用相同标签便于过滤

## 日志刷新

### flush()

```cpp
void flush();
```

异步刷新，请求工作线程处理队列，立即返回。

```cpp
info("重要操作开始");
do_something();
flush();  // 确保日志写入
```

**适用场景**：
- 需要确保日志及时写入
- 不想等待写入完成

### flush_sync()

```cpp
void flush_sync();  // 高级 API
```

同步刷新，等待所有日志写入完成。

```cpp
#include "cflog/cflog.hpp"

Logger::instance().flush_sync();
```

**适用场景**：
- 程序退出前
- 关键操作后
- 需要确认日志已写入

## 完整示例

```cpp
#include "cflog/cflog.h"
#include <thread>
#include <chrono>

class DataProcessor {
public:
    void process(const std::string& data) {
        trace("开始处理数据", "Processor");

        if (data.empty()) {
            warning("收到空数据", "Processor");
            return;
        }

        debug("数据大小: " + std::to_string(data.size()), "Processor");

        try {
            // 处理逻辑...
            info("数据处理完成", "Processor");
        } catch (const std::exception& e) {
            error(std::string("处理失败: ") + e.what(), "Processor");
        }
    }
};

int main() {
    using namespace cf::log;

    // 配置
    set_level(level::INFO);

    info("程序启动", "Main");

    DataProcessor processor;
    processor.process("sample data");
    processor.process("");  // 空数据，触发警告

    info("程序结束", "Main");
    flush();

    return 0;
}
```

## 与高级 API 的对比

| 特性 | 简单 API | 高级 API |
|------|----------|----------|
| 易用性 | ✅ 非常简单 | ⚠️ 需要更多代码 |
| 灵活性 | ⚠️ 有限 | ✅ 完全可控 |
| 自定义 Sink | ❌ 不支持 | ✅ 支持 |
| 自定义 Formatter | ❌ 不支持 | ✅ 支持 |
| 同步刷新 | ❌ 不支持 | ✅ 支持 `flush_sync()` |
| 适用场景 | 快速上手、简单应用 | 复杂应用、生产环境 |

## 下一步

- 学习 [高级用法](./advanced_usage.md) 了解更多控制选项
- 阅读 [配置选项](./configuration.md) 了解详细配置
- 参考 [最佳实践](./best_practices.md) 优化你的日志代码

## 相关文档

- [快速入门](./quick_start.md)
- [高级用法](./advanced_usage.md)
- [API 参考](../../../../desktop/base/logger/api_reference.md)
