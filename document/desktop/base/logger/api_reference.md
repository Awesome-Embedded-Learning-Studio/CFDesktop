# CFLogger API 参考

本文档提供 CFLogger 的完整 API 参考。

## 目录

- [简单 API](#简单-api-cflogh)
- [高级 API](#高级-api-cflogcfloghpp)
- [数据类型](#数据类型)
- [Formatter API](#formatter-api)
- [Sink API](#sink-api)
- [FormatterFactory API](#formatterfactory-api)

## 简单 API (cflog.h)

简单 API 提供了一组便捷函数，适合快速上手使用。

### 头文件

```cpp
#include "cflog/cflog.h"
```

### 函数

#### trace()

```cpp
void trace(std::string_view msg,
           std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());
```

记录 TRACE 级别的日志。

**参数**：
- `msg`: 日志消息
- `tag`: 日志标签（默认 "CFLog"）
- `loc`: 源代码位置（自动捕获）

#### debug()

```cpp
void debug(std::string_view msg,
           std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());
```

记录 DEBUG 级别的日志。

**参数**：
- `msg`: 日志消息
- `tag`: 日志标签（默认 "CFLog"）
- `loc`: 源代码位置（自动捕获）

#### info()

```cpp
void info(std::string_view msg,
          std::string_view tag = "CFLog",
          std::source_location loc = std::source_location::current());
```

记录 INFO 级别的日志。

**参数**：
- `msg`: 日志消息
- `tag`: 日志标签（默认 "CFLog"）
- `loc`: 源代码位置（自动捕获）

#### warning()

```cpp
void warning(std::string_view msg,
             std::string_view tag = "CFLog",
             std::source_location loc = std::source_location::current());
```

记录 WARNING 级别的日志。

**参数**：
- `msg`: 日志消息
- `tag`: 日志标签（默认 "CFLog"）
- `loc`: 源代码位置（自动捕获）

#### error()

```cpp
void error(std::string_view msg,
           std::string_view tag = "CFLog",
           std::source_location loc = std::source_location::current());
```

记录 ERROR 级别的日志。

**参数**：
- `msg`: 日志消息
- `tag`: 日志标签（默认 "CFLog"）
- `loc`: 源代码位置（自动捕获）

#### set_level()

```cpp
void set_level(level lvl);
```

设置全局最低日志级别。

**参数**：
- `lvl`: 最低日志级别，低于此级别的日志将被过滤

#### flush()

```cpp
void flush();
```

异步刷新日志缓冲区，立即返回。

---

## 高级 API (cflog/cflog.hpp)

高级 API 提供对 Logger 的完整控制。

### 头文件

```cpp
#include "cflog/cflog.hpp"
```

### Logger 类

#### 获取单例

```cpp
static Logger& instance();
```

获取 Logger 单例实例。

**返回值**：Logger 单例的引用

#### log()

```cpp
bool log(level log_level,
         std::string_view msg,
         std::string_view tag,
         std::source_location loc);
```

记录一条日志消息。

**参数**：
- `log_level`: 日志级别
- `msg`: 日志消息
- `tag`: 日志标签
- `loc`: 源代码位置

**返回值**：如果日志被接受返回 `true`，被过滤返回 `false`

#### setMininumLevel()

```cpp
void setMininumLevel(const level lvl);
```

设置最低日志级别。

**参数**：
- `lvl`: 最低日志级别

#### add_sink()

```cpp
void add_sink(std::shared_ptr<ISink> sink);
```

添加一个输出目标。

**参数**：
- `sink`: 要添加的 sink

#### remove_sink()

```cpp
void remove_sink(ISink* sink);
```

移除一个输出目标。

**参数**：
- `sink`: 要移除的 sink 指针

#### clear_sinks()

```cpp
void clear_sinks();
```

清除所有输出目标。

#### flush()

```cpp
void flush();
```

异步刷新，立即返回。

#### flush_sync()

```cpp
void flush_sync();
```

同步刷新，等待所有日志写入完成。

---

## 数据类型

### level 枚举

```cpp
enum class level {
    TRACE,   // 最详细的输出
    DEBUG,   // 调试信息
    INFO,    // 一般信息
    WARNING, // 警告信息
    ERROR    // 错误信息
};
```

**默认级别**：`kDEFAULT_LEVEL = level::DEBUG`

### to_string()

```cpp
constexpr std::string_view to_string(level lvl) noexcept;
```

将日志级别转换为字符串。

### as()

```cpp
template <typename T = int>
constexpr T as(level lvl) noexcept;
```

将日志级别转换为整数。

**示例**：
```cpp
int value = as<int>(level::INFO);  // value == 2
```

### LogRecord 结构体

```cpp
struct LogRecord {
    level lvl;                       // 日志级别
    std::string tag;                 // 标签
    std::string msg;                 // 消息
    cflog_timestamp_t timestamp;     // 时间戳
    std::thread::id tid;             // 线程 ID
    std::source_location loc;        // 源代码位置
};
```

### OpenMode 枚举

```cpp
enum class OpenMode {
    Append,   // 追加模式
    Truncate  // 覆盖模式
};
```

用于 FileSink 的文件打开模式。

---

## Formatter API

### IFormatter 接口

```cpp
class IFormatter {
public:
    virtual ~IFormatter() = default;

    virtual std::string format_me(const LogRecord& r) = 0;
    virtual bool configurable() const;
    virtual bool set_config(std::shared_ptr<FormatterConfig> config);
    virtual std::shared_ptr<FormatterConfig> get_config() const;
};
```

### FormatterFlag 枚举

```cpp
enum FormatterFlag : uint32_t {
    NONE = 0,
    TIMESTAMP = 1 << 0,       // 时间戳
    LEVEL = 1 << 1,           // 日志级别
    TAG = 1 << 2,             // 标签
    THREAD_ID = 1 << 3,       // 线程 ID
    SOURCE_LOCATION = 1 << 4, // 源代码位置
    MESSAGE = 1 << 5,         // 消息
    COLOR = 1 << 6,           // ANSI 颜色

    // 预设组合
    DEFAULT = TIMESTAMP | LEVEL | TAG | SOURCE_LOCATION | MESSAGE,
    MINIMAL = LEVEL | MESSAGE,
    VERBOSE = TIMESTAMP | LEVEL | TAG | THREAD_ID | SOURCE_LOCATION | MESSAGE,
};
```

### FormatterConfig 类

```cpp
class FormatterConfig {
public:
    explicit FormatterConfig(
        FormatterFlag flags = FormatterFlag::DEFAULT,
        std::string timestamp_format = "%H:%M:%S"
    );

    // 标志操作
    FormatterFlag get_flags() const noexcept;
    void set_flags(FormatterFlag flags) noexcept;
    void enable(FormatterFlag flag) noexcept;
    void disable(FormatterFlag flag) noexcept;
    bool is_enabled(FormatterFlag flag) const noexcept;

    // 时间戳格式
    void set_timestamp_format(std::string fmt);
    const std::string& get_timestamp_format() const noexcept;
};
```

**时间戳格式**：使用 `strftime` 格式字符串，默认 `"%H:%M:%S"`

### AsciiColorFormatter 类

```cpp
class AsciiColorFormatter : public IFormatter {
public:
    explicit AsciiColorFormatter(FormatterFlag flags = FormatterFlag::DEFAULT);

    std::string format_me(const LogRecord& r) override;
    bool configurable() const override;
    bool set_config(std::shared_ptr<FormatterConfig> config) override;
    std::shared_ptr<FormatterConfig> get_config() const override;
};
```

**颜色映射**：
- TRACE: 青色 `\033[96m`
- DEBUG: 蓝色 `\033[94m`
- INFO: 绿色 `\033[92m`
- WARNING: 黄色 `\033[93m`
- ERROR: 红色 `\033[91m`

### FileFormatter 类

```cpp
class FileFormatter : public IFormatter {
public:
    explicit FileFormatter(FormatterFlag flags = FormatterFlag::DEFAULT);

    std::string format_me(const LogRecord& r) override;
    bool configurable() const override;
    bool set_config(std::shared_ptr<FormatterConfig> config) override;
    std::shared_ptr<FormatterConfig> get_config() const override;
};
```

与 `AsciiColorFormatter` 相同，但忽略 `COLOR` 标志。

### DefaultFormatter 类

```cpp
class DefaultFormatter : public IFormatter {
public:
    DefaultFormatter() = default;

    std::string format_me(const LogRecord& r) override;
    bool configurable() const override;
};
```

最简单的格式化器，只输出消息内容。不可配置。

---

## Sink API

### ISink 接口

```cpp
class ISink {
public:
    virtual ~ISink() = default;

    virtual bool write(const LogRecord& record) = 0;
    virtual bool flush() = 0;

    virtual bool setFormat(std::shared_ptr<IFormatter> formatter);

protected:
    virtual bool formatable() const;
    virtual bool actFormat();
    std::shared_ptr<IFormatter> formatter_;
};
```

### ConsoleSink 类

```cpp
class ConsoleSink : public ISink {
public:
    bool write(const LogRecord& record) override;
    bool flush() override;
};
```

将日志输出到控制台（stdout）。

### FileSink 类

```cpp
class FileSink : public ISink {
public:
    explicit FileSink(const std::string& filepath,
                      OpenMode mode = OpenMode::Append);
    ~FileSink() override;

    bool write(const LogRecord& record) override;
    bool flush() override;

private:
    std::ofstream file_;
    std::string filepath_;
    OpenMode mode_;
};
```

将日志输出到文件。

**构造函数参数**：
- `filepath`: 文件路径
- `mode`: 打开模式（默认 `OpenMode::Append`）

**注意**：不可拷贝，删除了拷贝构造和赋值运算符。

---

## FormatterFactory API

```cpp
class FormatterFactory {
public:
    using IFormatterPtr = std::shared_ptr<IFormatter>;
    using IFormatterTag = std::string;
    using IFormatterTagView = std::string_view;
    using MakeFormatter = std::function<IFormatterPtr()>;

    // 注册格式化器创建函数
    void register_formatter(IFormatterTagView tag, MakeFormatter creator);

    // 注册单例格式化器
    void register_formatter(IFormatterTagView tag, IFormatterPtr instance);

    // 注销格式化器
    bool unregister_formatter(IFormatterTagView tag);

    // 创建新实例（不缓存）
    IFormatterPtr create(IFormatterTagView formatter_tag) const;

    // 获取或创建（带缓存）
    IFormatterPtr get_or_create(IFormatterTagView formatter_tag);

    // 清除缓存
    void clear_cache();
};
```

**注意**：`FormatterFactory` 不可拷贝或移动（包含 `std::mutex`）。

---

## 使用示例

### 简单 API 使用

```cpp
#include "cflog/cflog.h"

int main() {
    using namespace cf::log;

    // 设置级别
    set_level(level::TRACE);

    // 记录日志
    trace("详细跟踪信息");
    debug("调试信息");
    info("程序正常运行");
    warning("发现潜在问题");
    error("发生错误");

    // 刷新
    flush();

    return 0;
}
```

### 高级 API 使用

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

    // 注册格式化器
    factory.register_formatter("console", []() {
        return std::make_shared<AsciiColorFormatter>();
    });
    factory.register_formatter("file", []() {
        return std::make_shared<FileFormatter>();
    });

    // 创建并配置控制台 sink
    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink);

    // 创建并配置文件 sink
    auto file_sink = std::make_shared<FileSink>("app.log");
    file_sink->setFormat(factory.create("file"));
    Logger::instance().add_sink(file_sink);

    // 设置最低级别
    Logger::instance().setMininumLevel(level::INFO);
}
```

### 自定义 Formatter

```cpp
#include "cflog/cflog_format.h"
#include "cflog/cflog_record.h"

class MyCustomFormatter : public IFormatter {
public:
    std::string format_me(const LogRecord& r) override {
        return "[MY LOG] " + r.msg;
    }

    bool configurable() const override { return false; }
};

// 使用
auto formatter = std::make_shared<MyCustomFormatter>();
sink->setFormat(formatter);
```

### 自定义 Sink

```cpp
#include "cflog/cflog_sink.h"
#include <iostream>

class MyCustomSink : public ISink {
public:
    bool write(const LogRecord& record) override {
        if (formatable() && formatter_) {
            auto formatted = formatter_->format_me(record);
            std::cout << "[CUSTOM] " << formatted << std::endl;
            return true;
        }
        return false;
    }

    bool flush() override {
        std::cout << std::flush;
        return true;
    }
};

// 使用
auto sink = std::make_shared<MyCustomSink>();
sink->setFormat(std::make_shared<AsciiColorFormatter>());
Logger::instance().add_sink(sink);
```

---

## 相关文档

- [概述](./overview.md) - 系统概述
- [架构详解](./architecture.md) - 详细架构说明
- [HandBook/快速入门](../../../HandBook/desktop/base/logger/quick_start.md) - 5 分钟入门
- [HandBook/高级用法](../../../HandBook/desktop/base/logger/advanced_usage.md) - 高级特性详解
