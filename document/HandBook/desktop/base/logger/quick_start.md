# 快速入门

欢迎使用 CFLogger！本指南将帮助你在 5 分钟内上手使用 CFLogger。

## 第一步：引入头文件

CFLogger 提供两套 API：简单 API 和高级 API。

### 简单 API（推荐初学者）

```cpp
#include "cflog/cflog.h"
```

### 高级 API（需要更多控制）

```cpp
#include "cflog/cflog.hpp"
```

## 第二步：Hello World

最简单的 CFLogger 程序：

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    // 记录一条日志
    info("Hello, CFLogger!");

    // 确保日志写入
    flush();

    return 0;
}
```

编译运行后，你将看到：

```
[INFO] Hello, CFLogger!
```

## 第三步：使用不同日志级别

CFLogger 提供五个日志级别：

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    trace("最详细的跟踪信息");
    debug("调试信息");
    info("一般信息");
    warning("警告信息");
    error("错误信息");

    flush();
    return 0;
}
```

### 日志级别对照表

| 函数 | 级别 | 用途 | 颜色（控制台） |
|------|------|------|---------------|
| `trace()` | TRACE | 追踪函数调用、详细流程 | 青色 |
| `debug()` | DEBUG | 开发调试信息 | 蓝色 |
| `info()` | INFO | 一般运行信息 | 绿色 |
| `warning()` | WARNING | 潜在问题警告 | 黄色 |
| `error()` | ERROR | 错误和异常 | 红色 |

## 第四步：设置日志级别

使用 `set_level()` 过滤不需要的日志：

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    // 只记录 INFO 及以上级别
    set_level(level::INFO);

    trace("这条不会显示");   // 被过滤
    debug("这条也不会显示"); // 被过滤
    info("这条会显示");      // ✓
    warning("这条会显示");   // ✓
    error("这条会显示");     // ✓

    flush();
    return 0;
}
```

### 级别过滤规则

```
设置为 TRACE：显示所有日志
设置为 DEBUG：显示 DEBUG, INFO, WARNING, ERROR
设置为 INFO：  显示 INFO, WARNING, ERROR
设置为 WARNING：显示 WARNING, ERROR
设置为 ERROR：只显示 ERROR
```

## 第五步：使用标签组织日志

标签可以帮助你区分不同模块的日志：

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    // 不同模块使用不同标签
    info("数据库连接成功", "Database");
    info("用户登录成功", "Auth");
    warning("内存使用率 80%", "Monitor");
    error("文件写入失败", "FileIO");

    flush();
    return 0;
}
```

### 常用标签建议

| 标签 | 用途 |
|------|------|
| "Network" | 网络相关日志 |
| "Database" | 数据库操作 |
| "UI" | 界面渲染 |
| "Config" | 配置加载 |
| "Performance" | 性能数据 |

## 第六步：输出到文件

使用高级 API 将日志同时输出到控制台和文件：

```cpp
#include "cflog/cflog.hpp"
#include "cflog/cflog_format_factory.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/sinks/console_sink.h"
#include "cflog/sinks/file_sink.h"

void init_logger() {
    using namespace cf::log;

    // 创建格式化器工厂
    FormatterFactory factory;

    // 注册控制台格式化器（带颜色）
    factory.register_formatter("console", []() {
        return std::make_shared<AsciiColorFormatter>();
    });

    // 注册文件格式化器（纯文本）
    factory.register_formatter("file", []() {
        return std::make_shared<FileFormatter>();
    });

    // 添加控制台输出
    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink);

    // 添加文件输出（追加模式）
    auto file_sink = std::make_shared<FileSink>("app.log");
    file_sink->setFormat(factory.create("file"));
    Logger::instance().add_sink(file_sink);

    // 设置最低级别
    Logger::instance().setMininumLevel(level::INFO);
}

int main() {
    init_logger();

    using namespace cf::log;
    info("程序启动");
    info("这条日志会同时输出到控制台和文件");

    Logger::instance().flush_sync();  // 等待写入完成
    return 0;
}
```

## 完整示例

以下是一个完整的 CFLogger 使用示例：

```cpp
#include "cflog/cflog.h"
#include <thread>
#include <vector>

void worker_task(int id) {
    using namespace cf::log;
    debug("Worker " + std::to_string(id) + " 开始工作", "Worker");
    info("Worker " + std::to_string(id) + " 完成任务", "Worker");
}

int main() {
    using namespace cf::log;

    // 配置日志
    set_level(level::INFO);

    info("应用程序启动", "Main");

    // 创建多个工作线程
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back(worker_task, i);
    }

    // 等待所有线程完成
    for (auto& w : workers) {
        w.join();
    }

    info("所有工作完成", "Main");

    // 确保日志写入
    flush();

    return 0;
}
```

## CMake 配置

在 CMakeLists.txt 中添加 CFLogger 依赖：

```cmake
# 查找 CFDesktop
find_package(CFDesktop REQUIRED)

# 添加你的可执行文件
add_executable(my_app main.cpp)

# 链接 CFLogger
target_link_libraries(my_app PRIVATE CFDesktop::logger)
```

## 编译运行

```bash
# 配置
cmake -B build

# 编译
cmake --build build

# 运行
./build/my_app
```

## 常见问题

### Q: 日志没有输出？

A: 检查以下几点：
1. 是否调用了 `flush()` 或 `flush_sync()`
2. 日志级别是否设置过高
3. 是否正确添加了 sink

### Q: 程序崩溃后日志丢失？

A: CFLogger 使用异步队列，崩溃可能导致未写入的日志丢失。在关键操作前调用 `flush_sync()` 确保日志写入。

### Q: 如何禁用颜色输出？

A: 使用 `FileFormatter` 或在 `AsciiColorFormatter` 中禁用 `COLOR` 标志。

## 下一步

你已经掌握了 CFLogger 的基础知识！接下来可以：

- 阅读 [基础日志详解](./basic_logging.md) 了解更多 API
- 阅读 [高级用法](./advanced_usage.md) 学习自定义组件
- 阅读 [最佳实践](./best_practices.md) 了解推荐用法

## 相关文档

- [基础日志详解](./basic_logging.md)
- [高级用法](./advanced_usage.md)
- [配置选项](./configuration.md)
- [API 参考](../../../../desktop/base/logger/api_reference.md)
