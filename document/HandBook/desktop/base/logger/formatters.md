# 格式化器 (Formatter) 详解

Formatter 负责将 LogRecord 转换为可读的文本格式。

## Formatter 概览

### 什么是 Formatter？

```
LogRecord (结构化数据)
         ↓
    Formatter (格式化器)
         ↓
   std::string (可读文本)
```

### 内置 Formatter

| Formatter | 特点 | 适用场景 |
|-----------|------|----------|
| DefaultFormatter | 只输出消息 | 最简单的需求 |
| AsciiColorFormatter | 带颜色、可配置 | 控制台输出 |
| FileFormatter | 纯文本、可配置 | 文件输出 |

## FormatterFlag 配置

FormatterFlag 控制输出中包含哪些组件。

### 可用标志

```cpp
enum FormatterFlag : uint32_t {
    TIMESTAMP = 1 << 0,       // 时间戳
    LEVEL = 1 << 1,           // 日志级别
    TAG = 1 << 2,             // 标签
    THREAD_ID = 1 << 3,       // 线程 ID
    SOURCE_LOCATION = 1 << 4, // 源代码位置
    MESSAGE = 1 << 5,         // 消息内容
    COLOR = 1 << 6,           // ANSI 颜色
};
```

### 预设组合

```cpp
// 最少：只有级别和消息
MINIMAL = LEVEL | MESSAGE

// 默认：时间戳、级别、标签、位置、消息
DEFAULT = TIMESTAMP | LEVEL | TAG | SOURCE_LOCATION | MESSAGE

// 详细：包含所有组件
VERBOSE = TIMESTAMP | LEVEL | TAG | THREAD_ID | SOURCE_LOCATION | MESSAGE
```

### 输出示例

| 标志组合 | 输出示例 |
|----------|----------|
| MINIMAL | `[INFO] Hello World` |
| DEFAULT | `[14:23:45] [INFO] [CFLog] main.cpp:42 Hello World` |
| VERBOSE | `[14:23:45] [INFO] [CFLog] [12345] main.cpp:42 Hello World` |

## AsciiColorFormatter

### 基本用法

```cpp
#include "cflog/formatter/console_formatter.h"

using namespace cf::log;

// 使用默认配置
auto formatter = std::make_shared<AsciiColorFormatter>();
```

### 自定义配置

```cpp
// 最小输出
auto minimal = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::MINIMAL
);

// 详细输出（带颜色）
auto verbose = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::VERBOSE | FormatterFlag::COLOR
);
```

### 运行时修改配置

```cpp
auto formatter = std::make_shared<AsciiColorFormatter>();
auto config = std::make_shared<FormatterConfig>();

// 禁用颜色
config->disable(FormatterFlag::COLOR);

// 启用线程 ID
config->enable(FormatterFlag::THREAD_ID);

// 自定义时间格式
config->set_timestamp_format("%Y-%m-%d %H:%M:%S");

formatter->set_config(config);
```

### ANSI 颜色映射

| 级别 | ANSI 代码 | 效果 |
|------|-----------|------|
| TRACE | `\033[96m` | 青色 |
| DEBUG | `\033[94m` | 蓝色 |
| INFO | `\033[92m` | 绿色 |
| WARNING | `\033[93m` | 黄色 |
| ERROR | `\033[91m` | 红色 |

### 输出示例

```
[14:23:45] [INFO] [CFLog] 应用启动
         ^^^^^^ 绿色

[14:23:46] [WARNING] [Config] 配置文件未找到
         ^^^^^^^ 黄色

[14:23:47] [ERROR] [Network] 连接失败
         ^^^^^ 红色
```

## FileFormatter

### 基本用法

```cpp
#include "cflog/formatter/file_formatter.h"

using namespace cf::log;

auto formatter = std::make_shared<FileFormatter>();
```

### 特点

FileFormatter 与 AsciiColorFormatter 基本相同，但：

- **忽略 COLOR 标志**：文件中不包含 ANSI 转义码
- 适合日志文件持久化

```cpp
// 即便设置了 COLOR，FileFormatter 也不会输出颜色代码
auto formatter = std::make_shared<FileFormatter>(
    FormatterFlag::DEFAULT | FormatterFlag::COLOR  // COLOR 被忽略
);
```

## DefaultFormatter

### 特点

最简单的 Formatter，只输出消息内容。

```cpp
#include "cflog/formatter/default_formatter.h"

auto formatter = std::make_shared<DefaultFormatter>();
```

**输出**：只包含 `LogRecord.msg`

**不可配置**：`configurable()` 返回 `false`

## FormatterConfig

### 创建配置

```cpp
#include "cflog/cflog_format_config.h"

// 默认配置
auto config = std::make_shared<FormatterConfig>();

// 自定义配置
auto config = std::make_shared<FormatterConfig>(
    FormatterFlag::MINIMAL,
    "%H:%M:%S"  // 时间格式
);
```

### 线程安全操作

```cpp
// 启用组件
config->enable(FormatterFlag::THREAD_ID);

// 禁用组件
config->disable(FormatterFlag::SOURCE_LOCATION);

// 检查是否启用
if (config->is_enabled(FormatterFlag::COLOR)) {
    // ...
}

// 设置所有标志
config->set_flags(FormatterFlag::VERBOSE);
```

### 时间戳格式

使用 `strftime` 格式字符串：

| 格式 | 输出示例 | 说明 |
|------|----------|------|
| `"%H:%M:%S"` | `14:23:45` | 时:分:秒（默认） |
| `"%Y-%m-%d %H:%M:%S"` | `2026-03-16 14:23:45` | 完整日期时间 |
| `"%m/%d %H:%M:%S"` | `03/16 14:23:45` | 月/日 时间 |

## 自定义 Formatter

### 简单自定义

```cpp
#include "cflog/cflog_format.h"

class MyFormatter : public IFormatter {
public:
    std::string format_me(const LogRecord& r) override {
        return "[" + std::string(to_string(r.lvl)) + "] " + r.msg;
    }

    bool configurable() const override {
        return false;  // 不支持配置
    }
};
```

### 可配置自定义

```cpp
class ConfigurableFormatter : public IFormatter {
public:
    ConfigurableFormatter() {
        config_ = std::make_shared<FormatterConfig>();
    }

    std::string format_me(const LogRecord& r) override {
        std::string result;
        auto flags = config_->get_flags();

        if ((flags & FormatterFlag::TIMESTAMP)) {
            result += format_time(r.timestamp) + " ";
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

    std::string format_time(const cflog_timestamp_t& tp) {
        // 实现时间格式化...
        return "";
    }
};
```

## 常见格式示例

### JSON 格式

```cpp
class JsonFormatter : public IFormatter {
public:
    std::string format_me(const LogRecord& r) override {
        std::ostringstream oss;
        oss << "{"
            << "\"timestamp\":\"" << format_timestamp(r) << "\","
            << "\"level\":\"" << to_string(r.lvl) << "\","
            << "\"tag\":\"" << r.tag << "\","
            << "\"message\":\"" << r.msg << "\","
            << "\"file\":\"" << r.loc.file_name() << "\","
            << "\"line\":" << r.loc.line()
            << "}";
        return oss.str();
    }

private:
    std::string format_timestamp(const LogRecord& r) {
        // 实现 ISO 8601 格式...
        return "";
    }
};
```

### Syslog 格式

```cpp
class SyslogFormatter : public IFormatter {
public:
    std::string format_me(const LogRecord& r) override {
        std::ostringstream oss;
        // 优先级代码 (facility * 8 + severity)
        int priority = 1 * 8 + static_cast<int>(r.lvl);
        oss << "<" << priority << ">";

        // 时间戳
        oss << format_timestamp(r) << " ";

        // 主机名和程序
        oss << "myapp" << " ";

        // 消息
        oss << r.tag << ": " << r.msg;

        return oss.str();
    }

private:
    std::string format_timestamp(const LogRecord& r) {
        // 实现 RFC 3164 格式...
        return "";
    }
};
```

## 使用 FormatterFactory

### 注册和使用

```cpp
FormatterFactory factory;

// 注册
factory.register_formatter("custom", []() {
    return std::make_shared<MyFormatter>();
});

// 创建
auto formatter = factory.create("custom");

// 获取或创建（带缓存）
auto cached = factory.get_or_create("custom");
```

## 选择建议

| 场景 | 推荐_formatter |
|------|---------------|
| 控制台输出（开发） | `AsciiColorFormatter` + `COLOR` |
| 控制台输出（生产） | `AsciiColorFormatter` 无 `COLOR` |
| 文件输出 | `FileFormatter` |
| 最小输出 | `AsciiColorFormatter` + `MINIMAL` |
| 详细调试 | `AsciiColorFormatter` + `VERBOSE` |
| 自定义格式 | 继承 `IFormatter` |

## 下一步

- 学习 [Sink 详解](./sinks.md)
- 阅读 [高级用法](./advanced_usage.md)

## 相关文档

- [Sink 详解](./sinks.md)
- [高级用法](./advanced_usage.md)
- [API 参考](../../../../desktop/base/logger/api_reference.md)
