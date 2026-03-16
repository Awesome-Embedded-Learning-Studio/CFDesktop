# 最佳实践

本文档总结了使用 CFLogger 的推荐做法和常见模式。

## 日志级别使用

### 级别选择指南

| 级别 | 使用场景 | 示例 |
|------|----------|------|
| TRACE | 详细的执行流程 | "进入函数 processRequest()", "循环迭代 i=5" |
| DEBUG | 调试信息 | "变量 x = 42", "缓存未命中" |
| INFO | 重要状态变化 | "服务启动", "用户登录成功" |
| WARNING | 潜在问题 | "配置文件缺失，使用默认值", "内存使用率 80%" |
| ERROR | 错误和异常 | "数据库连接失败", "文件不存在" |

### 推荐原则

```cpp
// ✅ 好的做法
void process_user_login(const std::string& username) {
    debug("尝试登录用户: " + username, "Auth");

    if (authenticate(username)) {
        info("用户登录成功: " + username, "Auth");
    } else {
        warning("用户登录失败: " + username, "Auth");
    }
}

// ❌ 不好的做法
void process_user_login(const std::string& username) {
    trace("开始执行 process_user_login 函数", "Auth");
    trace("参数 username = " + username, "Auth");
    // ... 过度日志
}
```

## 标签使用

### 标签命名规范

```cpp
// ✅ 推荐的标签
info("连接成功", "Database");     // 大写首字母
info("请求处理", "HTTP");         // 全大写缩写
info("缓存更新", "cache");        // 小写也可以，保持一致

// ❌ 不推荐的标签
info("连接成功", "db");           // 过于简短
info("请求处理", "HttpRequestHandler");  // 过于详细
info("缓存更新", "c");            // 意义不明
```

### 按模块划分标签

```cpp
class UserService {
public:
    void login(const std::string& username) {
        info("用户登录: " + username, "User");
    }
};

class DatabaseService {
public:
    void connect() {
        info("数据库连接", "Database");
    }
};

class NetworkService {
public:
    void send(const std::string& data) {
        debug("发送数据: " + data, "Network");
    }
};
```

### 常用标签建议

| 模块类型 | 推荐标签 |
|----------|----------|
| 用户认证 | Auth, User, Login, Session |
| 数据访问 | Database, DB, SQL, Cache, Redis |
| 网络通信 | Network, HTTP, API, WebSocket |
| 文件操作 | FileIO, FileSystem, Storage |
| 配置管理 | Config, Settings, Preferences |
| UI 渲染 | UI, Render, Paint, Widget |
| 后台任务 | Background, Worker, Job, Task |
| 性能监控 | Performance, Metrics, Stats |

## 消息内容

### 消息编写原则

```cpp
// ✅ 好的消息 - 包含上下文
error("无法打开配置文件: " + path + ", 原因: " + strerror(errno), "FileIO");
info("用户 " + username + " 从 " + ip_address + " 登录", "Auth");
warning("查询耗时 " + std::to_string(duration_ms) + "ms 超过阈值", "Database");

// ❌ 差的消息 - 缺少上下文
error("文件打开失败", "FileIO");
info("用户登录", "Auth");
warning("查询慢", "Database");
```

### 结构化消息

```cpp
// ✅ 使用结构化格式
info("请求处理 | method=POST | path=/api/users | duration=50ms | status=200", "API");

// ✅ 使用键值对
error("数据库错误 | code=" + std::to_string(err.code) +
      " | msg=" + err.message + " | query=" + query, "Database");
```

### 避免敏感信息

```cpp
// ❌ 不要记录敏感信息
info("用户登录: user=admin&password=123456", "Auth");

// ✅ 脱敏处理
info("用户登录: user=admin&password=****", "Auth");
info("信用卡支付: ****-****-****-" + last4, "Payment");
```

## 性能考虑

### 避免过度日志

```cpp
// ❌ 在热路径中过度日志
void process_data(const std::vector<Data>& items) {
    for (const auto& item : items) {
        trace("处理项目: " + item.to_string());  // 每个项目都记录
    }
}

// ✅ 批量记录
void process_data(const std::vector<Data>& items) {
    debug("开始处理 " + std::to_string(items.size()) + " 个项目");
    for (const auto& item : items) {
        // 处理...
    }
    info("完成处理 " + std::to_string(items.size()) + " 个项目");
}
```

### 延迟计算

```cpp
// ❌ 每次都计算
trace("调试信息: " + expensive_computation());  // 即使 TRACE 被过滤也会计算

// ✅ 使用条件判断
if (should_log(level::TRACE)) {
    trace("调试信息: " + expensive_computation());
}
```

### 字符串拼接

```cpp
// ❌ 多次拼接
std::string msg = "用户: ";
msg += username;
msg += ", 操作: ";
msg += action;
info(msg);

// ✅ 单次拼接
info("用户: " + username + ", 操作: " + action);
```

## 线程安全

### 多线程环境

```cpp
// ✅ CFLogger 是线程安全的，可以直接使用
void worker_thread(int id) {
    info("工作线程 " + std::to_string(id) + " 启动", "Worker");
    // ... 工作逻辑 ...
    info("工作线程 " + std::to_string(id) + " 完成", "Worker");
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker_thread, i);
    }
    for (auto& t : threads) {
        t.join();
    }
}
```

### 共享资源的日志

```cpp
// ✅ 记录共享资源访问
class ThreadSafeCounter {
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        counter_++;
        trace("计数器递增到 " + std::to_string(counter_), "Counter");
    }

private:
    std::mutex mutex_;
    int counter_ = 0;
};
```

## 错误处理

### 记录异常

```cpp
// ✅ 记录异常信息
try {
    process_data();
} catch (const std::exception& e) {
    error(std::string("处理失败: ") + e.what(), "Process");
    throw;  // 重新抛出
}

// ✅ 记录异常和上下文
try {
    connect_database(url);
} catch (const DatabaseException& e) {
    error("数据库连接失败 | url=" + url +
          " | error=" + std::string(e.what()), "Database");
}
```

### 关键操作前后

```cpp
// ✅ 关键操作前后记录
void save_to_file(const std::string& path, const Data& data) {
    info("开始保存文件: " + path, "FileIO");

    try {
        write_file(path, data);
        info("文件保存成功: " + path, "FileIO");
    } catch (...) {
        error("文件保存失败: " + path, "FileIO");
        throw;
    }
}
```

## 启动和关闭

### 应用启动

```cpp
int main(int argc, char** argv) {
    // 1. 尽早初始化日志
    setup_logging();

    using namespace cf::log;
    info("=" * 50, "Main");
    info("应用程序启动", "Main");
    info("版本: " + std::string(VERSION), "Main");
    info("命令行参数: " + join_args(argc, argv), "Main");

    // 2. 应用逻辑...

    return 0;
}
```

### 应用关闭

```cpp
int main(int argc, char** argv) {
    setup_logging();

    // 应用逻辑...

    using namespace cf::log;
    info("应用程序正常退出", "Main");
    Logger::instance().flush_sync();  // 确保所有日志写入

    return 0;
}
```

## 配置管理

### 环境感知配置

```cpp
enum class Environment {
    Development,
    Testing,
    Production
};

void setup_logging(Environment env) {
    using namespace cf::log;

    switch (env) {
        case Environment::Development:
            Logger::instance().setMininumLevel(level::TRACE);
            break;
        case Environment::Testing:
            Logger::instance().setMininumLevel(level::DEBUG);
            break;
        case Environment::Production:
            Logger::instance().setMininumLevel(level::INFO);
            break;
    }
}
```

### 动态调整

```cpp
class LoggingController {
public:
    void increase_verbosity() {
        auto current = Logger::instance().getMininumLevel();
        int new_level = std::max(0, as<int>(current) - 1);
        Logger::instance().setMininumLevel(static_cast<level>(new_level));
        info("日志级别调整为: " + std::string(to_string(static_cast<level>(new_level))));
    }

    void decrease_verbosity() {
        auto current = Logger::instance().getMininumLevel();
        int new_level = std::min(4, as<int>(current) + 1);
        Logger::instance().setMininumLevel(static_cast<level>(new_level));
    }
};
```

## 日志轮转

### 虽然需要自定义实现，但建议：

```cpp
// ✅ 按大小轮转
class RotatingFileSink : public ISink {
    // 参见 sinks.md 中的实现
};

// ✅ 按时间轮转
class DailyFileSink : public ISink {
    // 每天创建新文件
    // app_20260316.log, app_20260317.log, ...
};
```

## 单元测试

### 测试中的日志

```cpp
// ✅ 测试时可以降低日志级别
TEST(MyTest, TestSomething) {
    // 临时设置更高日志级别用于调试
    Logger::instance().setMininumLevel(level::TRACE);

    // 测试代码...

    // 测试结束恢复
    Logger::instance().setMininumLevel(level::WARNING);
}
```

### Mock Sink

```cpp
// ✅ 使用 Mock Sink 验证日志
class MockSink : public ISink {
public:
    std::vector<std::string> messages;

    bool write(const LogRecord& record) override {
        if (formatable() && formatter_) {
            messages.push_back(formatter_->format_me(record));
        } else {
            messages.push_back(record.msg);
        }
        return true;
    }

    bool flush() override { return true; }
};

TEST(LoggingTest, ErrorLogged) {
    auto mock = std::make_shared<MockSink>();
    Logger::instance().add_sink(mock);

    error("Test error");

    ASSERT_FALSE(mock->messages.empty());
    ASSERT_TRUE(mock->messages[0].find("Test error") != std::string::npos);
}
```

## 检查清单

在发布前检查：

- [ ] 生产环境日志级别不低于 INFO
- [ ] 没有记录敏感信息（密码、令牌等）
- [ ] 热路径中没有过度日志
- [ ] 关键操作前后都有日志
- [ ] 使用一致的标签命名
- [ ] 日志消息包含足够的上下文
- [ ] 文件 sink 使用追加模式
- [ ] 应用退出前调用 flush_sync()

## 下一步

- 学习 [性能优化](./performance.md)
- 阅读 [故障排除](./troubleshooting.md)

## 相关文档

- [配置选项](./configuration.md)
- [性能优化](./performance.md)
- [故障排除](./troubleshooting.md)
