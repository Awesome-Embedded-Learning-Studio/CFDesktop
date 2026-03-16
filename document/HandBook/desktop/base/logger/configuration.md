# 配置选项

本文档详细介绍 CFLogger 的所有配置选项。

## 日志级别配置

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

### 设置最低级别

```cpp
// 简单 API
set_level(level::INFO);

// 高级 API
Logger::instance().setMininumLevel(level::INFO);
```

### 级别关系

```
TRACE (0) < DEBUG (1) < INFO (2) < WARNING (3) < ERROR (4)
```

设置某级别后，只有该级别及以上的日志会被记录：

| 设置级别 | TRACE | DEBUG | INFO | WARNING | ERROR |
|----------|-------|-------|------|---------|-------|
| TRACE    | ✓      | ✓      | ✓    | ✓        | ✓     |
| DEBUG    | ✗      | ✓      | ✓    | ✓        | ✓     |
| INFO     | ✗      | ✗      | ✓    | ✓        | ✓     |
| WARNING  | ✗      | ✗      | ✗    | ✓        | ✓     |
| ERROR    | ✗      | ✗      | ✗    | ✗        | ✓     |

### 环境相关配置

```cpp
void setup_logging_by_env() {
    const char* env = std::getenv("LOG_LEVEL");

    level lvl = level::INFO;  // 默认

    if (env) {
        std::string env_str(env);
        if (env_str == "TRACE") lvl = level::TRACE;
        else if (env_str == "DEBUG") lvl = level::DEBUG;
        else if (env_str == "INFO") lvl = level::INFO;
        else if (env_str == "WARNING") lvl = level::WARNING;
        else if (env_str == "ERROR") lvl = level::ERROR;
    }

    Logger::instance().setMininumLevel(lvl);
}
```

## FormatterFlag 配置

### 标志详解

```cpp
enum FormatterFlag : uint32_t {
    NONE = 0,
    TIMESTAMP = 1 << 0,       // 时间戳
    LEVEL = 1 << 1,           // 日志级别
    TAG = 1 << 2,             // 标签
    THREAD_ID = 1 << 3,       // 线程 ID
    SOURCE_LOCATION = 1 << 4, // 源代码位置 (file:line)
    MESSAGE = 1 << 5,         // 消息内容
    COLOR = 1 << 6,           // ANSI 颜色
};
```

### 预设组合

```cpp
MINIMAL = LEVEL | MESSAGE;
DEFAULT = TIMESTAMP | LEVEL | TAG | SOURCE_LOCATION | MESSAGE;
VERBOSE = TIMESTAMP | LEVEL | TAG | THREAD_ID | SOURCE_LOCATION | MESSAGE;
```

### 输出组件示例

| 标志 | 示例输出 |
|------|----------|
| TIMESTAMP | `[14:23:45]` |
| LEVEL | `[INFO]` |
| TAG | `[CFLog]` |
| THREAD_ID | `[12345]` |
| SOURCE_LOCATION | `main.cpp:42` |
| MESSAGE | `Hello World` |

### 运行时修改

```cpp
auto formatter = std::make_shared<AsciiColorFormatter>();
auto config = std::make_shared<FormatterConfig>();

// 单个操作
config->enable(FormatterFlag::THREAD_ID);
config->disable(FormatterFlag::SOURCE_LOCATION);

// 检查状态
if (config->is_enabled(FormatterFlag::COLOR)) {
    // 颜色已启用
}

// 批量设置
config->set_flags(FormatterFlag::MINIMAL);

formatter->set_config(config);
```

### 位运算组合

```cpp
// 组合多个标志
auto flags = FormatterFlag::TIMESTAMP | FormatterFlag::LEVEL | FormatterFlag::MESSAGE;

// 移除某个标志
auto flags = FormatterFlag::DEFAULT & ~FormatterFlag::SOURCE_LOCATION;

// 添加某个标志
auto flags = FormatterFlag::MINIMAL | FormatterFlag::TIMESTAMP;
```

## 时间戳格式配置

### strftime 格式

```cpp
auto config = std::make_shared<FormatterConfig>();
config->set_timestamp_format("%Y-%m-%d %H:%M:%S");
```

### 常用格式

| 格式字符串 | 输出示例 | 说明 |
|-----------|----------|------|
| `"%H:%M:%S"` | `14:23:45` | 时:分:秒（默认） |
| `"%Y-%m-%d %H:%M:%S"` | `2026-03-16 14:23:45` | 完整日期时间 |
| `"%m/%d %H:%M:%S"` | `03/16 14:23:45` | 月/日 时间 |
| `"%Y%m%d_%H%M%S"` | `20260316_142345` | 紧凑格式 |
| `"%T"` | `14:23:45` | ISO 8601 时间 |
| `"%F %T"` | `2026-03-16 14:23:45` | ISO 8601 日期时间 |

### strftime 说明符

| 说明符 | 说明 | 示例 |
|--------|------|------|
| `%Y` | 四位年份 | 2026 |
| `%m` | 月份（01-12） | 03 |
| `%d` | 日期（01-31） | 16 |
| `%H` | 小时（00-23） | 14 |
| `%M` | 分钟（00-59） | 23 |
| `%S` | 秒（00-60） | 45 |
| `%F` | 等同于 %Y-%m-%d | 2026-03-16 |
| `%T` | 等同于 %H:%M:%S | 14:23:45 |

## 颜色配置

### ANSI 颜色代码

CFLogger 使用的颜色映射：

| 级别 | ANSI 代码 | 终端效果 |
|------|-----------|----------|
| TRACE | `\033[96m` | 青色 |
| DEBUG | `\033[94m` | 蓝色 |
| INFO | `\033[92m` | 绿色 |
| WARNING | `\033[93m` | 黄色 |
| ERROR | `\033[91m` | 红色 |

### 禁用颜色

```cpp
// 方法1：使用不带 COLOR 的标志
auto formatter = std::make_shared<AsciiColorFormatter>(
    FormatterFlag::DEFAULT  // 不包含 COLOR
);

// 方法2：运行时禁用
auto config = std::make_shared<FormatterConfig>();
config->disable(FormatterFlag::COLOR);
formatter->set_config(config);

// 方法3：使用 FileFormatter（自动忽略颜色）
auto formatter = std::make_shared<FileFormatter>();
```

## 队列配置

### 队列容量

```cpp
// AsyncPostQueue 中的常量
static constexpr size_t kMaxNormalQueueSize = 65536;  // 2^16
```

这是编译时常量，运行时不可修改。

### 队列行为

| 队列类型 | 容量 | 满时行为 |
|----------|------|----------|
| normalQueue_ | 65,536 | 丢弃新日志 |
| errorQueue_ | 无限制 | 永不丢弃 |

### 监控队列溢出

```cpp
// 获取溢出计数
size_t overflow_count = Logger::instance().get_normal_queue_overflow();

// 定期检查
void monitor_queue() {
    static size_t last_count = 0;
    size_t current_count = Logger::instance().get_normal_queue_overflow();

    if (current_count > last_count) {
        std::cout << "警告: 队列溢出 "
                  << (current_count - last_count)
                  << " 条日志" << std::endl;
        last_count = current_count;
    }
}
```

## 文件 Sink 配置

### OpenMode 选项

```cpp
enum class OpenMode {
    Append,   // 追加到文件末尾
    Truncate  // 覆盖现有文件
};
```

### 使用示例

```cpp
// 追加模式（生产环境推荐）
auto sink = std::make_shared<FileSink>("app.log");

// 覆盖模式（测试环境）
auto sink = std::make_shared<FileSink>("app.log", OpenMode::Truncate);
```

## 多环境配置

### 开发环境

```cpp
void setup_dev_environment() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("dev", []() {
        return std::make_shared<AsciiColorFormatter>(
            FormatterFlag::VERBOSE | FormatterFlag::COLOR
        );
    });

    auto console = std::make_shared<ConsoleSink>();
    auto config = std::make_shared<FormatterConfig>();
    config->set_timestamp_format("%H:%M:%S.%ms");  // 毫秒精度

    auto formatter = factory.create("dev");
    formatter->set_config(config);
    console->setFormat(formatter);

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(console);
    Logger::instance().setMininumLevel(level::TRACE);
}
```

### 测试环境

```cpp
void setup_test_environment() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("test", []() {
        return std::make_shared<AsciiColorFormatter>(
            FormatterFlag::DEFAULT | FormatterFlag::COLOR
        );
    });

    auto console = std::make_shared<ConsoleSink>();
    console->setFormat(factory.create("test"));

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(console);
    Logger::instance().setMininumLevel(level::DEBUG);
}
```

### 生产环境

```cpp
void setup_production_environment() {
    using namespace cf::log;

    FormatterFactory factory;
    factory.register_formatter("prod", []() {
        return std::make_shared<FileFormatter>(
            FormatterFlag::DEFAULT
        );
    });

    // 主日志文件
    auto main_log = std::make_shared<FileSink>(
        "/var/log/myapp/app.log",
        OpenMode::Append
    );
    main_log->setFormat(factory.create("prod"));

    // 错误日志文件
    auto error_log = std::make_shared<FileSink>(
        "/var/log/myapp/error.log",
        OpenMode::Append
    );
    error_log->setFormat(factory.create("prod"));

    auto config = std::make_shared<FormatterConfig>();
    config->set_timestamp_format("%Y-%m-%d %H:%M:%S");
    main_log->getFormat()->set_config(config);
    error_log->getFormat()->set_config(config);

    Logger::instance().clear_sinks();
    Logger::instance().add_sink(main_log);
    Logger::instance().add_sink(error_log);
    Logger::instance().setMininumLevel(level::INFO);
}
```

## 配置文件示例

### JSON 配置

```json
{
    "logging": {
        "level": "INFO",
        "sinks": [
            {
                "type": "console",
                "formatter": {
                    "type": "ascii_color",
                    "flags": "DEFAULT",
                    "color": true,
                    "timestamp_format": "%H:%M:%S"
                }
            },
            {
                "type": "file",
                "path": "app.log",
                "mode": "append",
                "formatter": {
                    "type": "file",
                    "flags": "DEFAULT",
                    "timestamp_format": "%Y-%m-%d %H:%M:%S"
                }
            }
        ]
    }
}
```

### 命令行参数

```cpp
#include <getopt.h>

void parse_command_line_args(int argc, char** argv) {
    using namespace cf::log;

    level log_level = level::INFO;
    bool enable_file = false;
    std::string log_path = "app.log";

    static struct option long_options[] = {
        {"log-level", required_argument, 0, 'l'},
        {"log-file", required_argument, 0, 'f'},
        {"enable-file", no_argument, 0, 'e'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "l:f:e", long_options, NULL)) != -1) {
        switch (opt) {
            case 'l':
                if (strcmp(optarg, "TRACE") == 0) log_level = level::TRACE;
                else if (strcmp(optarg, "DEBUG") == 0) log_level = level::DEBUG;
                else if (strcmp(optarg, "INFO") == 0) log_level = level::INFO;
                else if (strcmp(optarg, "WARNING") == 0) log_level = level::WARNING;
                else if (strcmp(optarg, "ERROR") == 0) log_level = level::ERROR;
                break;
            case 'f':
                log_path = optarg;
                enable_file = true;
                break;
            case 'e':
                enable_file = true;
                break;
        }
    }

    Logger::instance().setMininumLevel(log_level);

    if (enable_file) {
        auto file_sink = std::make_shared<FileSink>(log_path);
        file_sink->setFormat(std::make_shared<FileFormatter>());
        Logger::instance().add_sink(file_sink);
    }
}
```

## 配置最佳实践

1. **环境区分**：开发、测试、生产使用不同配置
2. **级别控制**：生产环境不低于 INFO
3. **文件分离**：普通日志和错误日志分开
4. **格式一致**：同类日志使用统一格式
5. **性能平衡**：生产环境避免过于详细的格式

## 下一步

- 阅读 [最佳实践](./best_practices.md)
- 学习 [性能优化](./performance.md)

## 相关文档

- [格式化器详解](./formatters.md)
- [Sink 详解](./sinks.md)
- [最佳实践](./best_practices.md)
