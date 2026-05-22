---
title: ConfigStore 最佳实践
description: ConfigStore 提供四层存储架构，每一层都有其特定的使用场景：
---

# ConfigStore 最佳实践

## 文档信息

| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-03-17 |
| 所属模块 | cf::config (ConfigStore) |
| 前置知识 | 快速入门指南 (01-quick-start.md) |

---

## 一、键命名规范

### 1.1 分层结构建议

ConfigStore 使用点分隔的分层键名结构，建议采用 2-4 层的命名深度：

```bash
[level1].[level2].[level3].[level4]
   |        |        |        |
   |        |        |        +-- 具体属性名 (必需)
   |        |        +----------- 子模块 (可选)
   |        +-------------------- 功能模块 (必需)
   +----------------------------- 命名空间/域 (必需)
```text

**推荐示例：**

```text
app.theme.name              # 应用主题名称
app.theme.dark_mode         # 主题深色模式
ui.window.width             # 窗口宽度
ui.window.height            # 窗口高度
user.preferences.language   # 用户语言偏好
network.proxy.host          # 代理主机地址
network.proxy.port          # 代理端口
editor.font.size            # 编辑器字体大小
editor.font.family          # 编辑器字体族
```bash

### 1.2 命名约定

遵循以下命名约定以确保代码一致性：

| 规则 | 说明 | 示例 |
|------|------|------|
| **小写字母** | 所有层级使用小写字母 | `app.theme.name` |
| **点分隔** | 使用 `.` 作为层级分隔符 | `ui.window.width` |
| **语义清晰** | 使用描述性名称，避免缩写 | `max_file_size` 而非 `mfs` |
| **下划线连接** | 多词属性用 `_` 连接 | `dark_mode` 而非 `darkMode` |
| **避免数字** | 除非是版本或编号 | `schema_v2` 而非 `style1` |
| **全英文** | 键名使用英文，非中文 | `background_color` 而非 `背景颜色` |

### 1.3 避免冲突的建议

为避免键名冲突，推荐为不同模块或组件使用命名空间前缀：

```text
# 为每个子系统预留独立的命名空间
app.*          # 应用程序级配置
ui.*           # 用户界面配置
editor.*       # 编辑器配置
network.*      # 网络配置
database.*     # 数据库配置
logger.*       # 日志配置
```text

**常见命名空间：**

```cpp
// 在代码中定义命名空间常量
namespace ConfigKeys {
    constexpr std::string_view APP_PREFIX = "app";
    constexpr std::string_view UI_PREFIX = "ui";
    constexpr std::string_view EDITOR_PREFIX = "editor";

    // 预定义的 KeyView
    inline constexpr KeyView APP_THEME_NAME{.group = "app.theme", .key = "name"};
    inline constexpr KeyView UI_WINDOW_WIDTH{.group = "ui.window", .key = "width"};
    inline constexpr KeyView EDITOR_FONT_SIZE{.group = "editor.font", .key = "size"};
}
```yaml

---

## 二、层级使用策略

ConfigStore 提供四层存储架构，每一层都有其特定的使用场景：

### 2.1 层级优先级图

```bash
+-----------------------------+
|         Temp 层 (优先级 3)   |  内存临时数据，进程重启后丢失
+-----------------------------+
                    |
                    v 覆盖
+-----------------------------+
|         App 层 (优先级 2)    |  应用运行时配置，会话有效
+-----------------------------+
                    |
                    v 覆盖
+-----------------------------+
|         User 层 (优先级 1)   |  用户个人偏好，跨会话持久化
+-----------------------------+
                    |
                    v 覆盖
+-----------------------------+
|       System 层 (优先级 0)   |  系统默认配置，管理员维护
+-----------------------------+
```text

### 2.2 System 层：系统默认

**用途：** 存储应用程序的系统默认配置，由管理员或安装程序维护。

**使用场景：**
- 默认配置值
- 系统级限制（如最大文件大小）
- 硬件相关的默认设置
- 部署环境配置

**示例：**

```cpp
// 初始化系统默认配置 (通常在安装时或首次运行时设置)
void init_system_defaults() {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    // 系统默认配置应该设置为只读参考
    store.register_key(
        Key{.full_key = "app.version", .full_description = "Application version"},
        std::string("1.0.0"),
        Layer::System
    );

    store.register_key(
        Key{.full_key = "file.max_size_mb", .full_description = "Max file size in MB"},
        100,
        Layer::System
    );

    store.register_key(
        Key{.full_key = "performance.default_dpi", .full_description = "Default DPI setting"},
        96,
        Layer::System
    );

    store.sync(SyncMethod::Sync);  // 同步写入
}
```text

### 2.3 User 层：用户偏好

**用途：** 存储用户的个人偏好设置，在用户配置文件中持久化。

**使用场景：**
- 主题和外观设置
- 语言和区域设置
- 用户习惯（窗口大小、位置）
- 功能开关

**示例：**

```cpp
// 保存用户偏好设置
void save_user_preferences(const UserPreferences& prefs) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    store.set(KeyView{.group = "user.theme", .key = "name"},
              prefs.theme_name, Layer::User);
    store.set(KeyView{.group = "user.theme", .key = "dark_mode"},
              prefs.dark_mode, Layer::User);
    store.set(KeyView{.group = "user.interface", .key = "language"},
              prefs.language, Layer::User);
    store.set(KeyView{.group = "user.interface", .key = "font_size"},
              prefs.font_size, Layer::User);

    // 异步保存，不阻塞 UI
    store.sync(SyncMethod::Async);
}

// 加载用户偏好设置
UserPreferences load_user_preferences() {
    using namespace cf::config;
    auto& store = ConfigStore::instance();
    UserPreferences prefs;

    prefs.theme_name = store.query<std::string>(
        KeyView{.group = "user.theme", .key = "name"}, "default");
    prefs.dark_mode = store.query<bool>(
        KeyView{.group = "user.theme", .key = "dark_mode"}, false);
    prefs.language = store.query<std::string>(
        KeyView{.group = "user.interface", .key = "language"}, "en_US");
    prefs.font_size = store.query<int>(
        KeyView{.group = "user.interface", .key = "font_size"}, 12);

    return prefs;
}
```text

### 2.4 App 层：运行时状态

**用途：** 存储应用程序运行时状态，与应用程序生命周期绑定。

**使用场景：**
- 窗口几何状态
- 最近打开的文件列表
- 会话恢复数据
- 应用程序内部状态

**示例：**

```cpp
// 保存窗口状态
void save_window_state(QWidget* window) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    auto geometry = window->geometry();
    store.set(KeyView{.group = "app.window", .key = "x"},
              geometry.x(), Layer::App);
    store.set(KeyView{.group = "app.window", .key = "y"},
              geometry.y(), Layer::App);
    store.set(KeyView{.group = "app.window", .key = "width"},
              geometry.width(), Layer::App);
    store.set(KeyView{.group = "app.window", .key = "height"},
              geometry.height(), Layer::App);
    store.set(KeyView{.group = "app.window", .key = "maximized"},
              window->isMaximized(), Layer::App);

    store.sync(SyncMethod::Async);
}

// 恢复窗口状态
void restore_window_state(QWidget* window) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    int x = store.query<int>(KeyView{.group = "app.window", .key = "x"}, 100);
    int y = store.query<int>(KeyView{.group = "app.window", .key = "y"}, 100);
    int w = store.query<int>(KeyView{.group = "app.window", .key = "width"}, 800);
    int h = store.query<int>(KeyView{.group = "app.window", .key = "height"}, 600);
    bool maximized = store.query<bool>(
        KeyView{.group = "app.window", .key = "maximized"}, false);

    window->setGeometry(x, y, w, h);
    if (maximized) {
        window->showMaximized();
    }
}
```text

### 2.5 Temp 层：临时数据

**用途：** 存储进程生命周期内的临时数据，不持久化到磁盘。

**使用场景：**
- 单元测试中的临时配置
- 功能演示/预览模式
- 会话令牌
- 调试标志

**示例：**

```cpp
// 使用 Temp 层进行单元测试
TEST(ConfigStoreTest, OverrideWithTempLayer) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    // 设置系统默认值
    store.set(KeyView{.group = "test", .key = "value"},
              "system_default", Layer::System);

    // 使用 Temp 层覆盖 (不持久化)
    store.set(KeyView{.group = "test", .key = "value"},
              "temp_override", Layer::Temp);

    // 查询返回 Temp 层的值
    auto value = store.query<std::string>(
        KeyView{.group = "test", .key = "value"});
    EXPECT_EQ(value, "temp_override");

    // 清除 Temp 层后，回退到 System 层
    store.clear_layer(Layer::Temp);
    value = store.query<std::string>(
        KeyView{.group = "test", .key = "value"});
    EXPECT_EQ(value, "system_default");
}

// 临时启用调试模式
void enable_debug_mode_temporarily() {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    store.set(KeyView{.group = "debug", .key = "enabled"},
              true, Layer::Temp);
    store.set(KeyView{.group = "debug", .key = "log_level"},
              5, Layer::Temp);

    // 不会持久化到磁盘
}
```bash

### 2.6 层级选择决策表

| 场景 | 推荐层级 | 持久化 | 谁可修改 |
|------|---------|--------|----------|
| 应用默认值 | System | 是 | 安装程序/管理员 |
| 用户偏好 | User | 是 | 最终用户 |
| 窗口状态 | App | 是 | 应用程序 |
| 会话令牌 | Temp | 否 | 应用程序 |
| 测试数据 | Temp | 否 | 测试代码 |
| 限制配置 | System | 是 | 管理员 |
| 功能开关 | User | 是 | 最终用户 |

---

## 三、性能优化

### 3.1 缓存利用技巧

ConfigStore 在内存中维护缓存，遵循以下最佳实践以充分利用缓存：

**1. 优先使用优先级查询**

```cpp
// 推荐：让 ConfigStore 自动查找最高优先级的值
auto value = store.query<int>(KeyView{.group = "app", .key = "timeout"}, 5000);

// 不推荐：手动查询各层
int value = 5000;
if (auto v = store.query<int>(KeyView{.group = "app", .key = "timeout"}, Layer::Temp)) {
    value = *v;
} else if (auto v = store.query<int>(KeyView{.group = "app", .key = "timeout"}, Layer::App)) {
    value = *v;
} // ...
```text

**2. 批量读取配置**

```cpp
// 推荐：一次性读取需要的配置
struct AppSettings {
    std::string theme;
    int dpi;
    bool dark_mode;
    double scale;

    static AppSettings load() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();
        return AppSettings{
            .theme = store.query<std::string>(
                KeyView{.group = "app.theme", .key = "name"}, "default"),
            .dpi = store.query<int>(
                KeyView{.group = "app.display", .key = "dpi"}, 96),
            .dark_mode = store.query<bool>(
                KeyView{.group = "app.theme", .key = "dark_mode"}, false),
            .scale = store.query<double>(
                KeyView{.group = "app.display", .key = "scale"}, 1.0)
        };
    }
};

// 使用缓存的配置
auto settings = AppSettings::load();
apply_settings(settings);
```text

**3. 避免频繁查询同一配置**

```cpp
// 不推荐：每次函数调用都查询
void render_frame() {
    auto scale = ConfigStore::instance().query<double>(
        KeyView{.group = "display", .key = "scale"}, 1.0);
    // 使用 scale...
}

// 推荐：缓存配置值，监听变化
class Renderer {
public:
    Renderer() {
        // 初始化时读取
        scale_ = ConfigStore::instance().query<double>(
            KeyView{.group = "display", .key = "scale"}, 1.0);

        // 监听变化
        watcher_handle_ = ConfigStore::instance().watch(
            "display.scale",
            [this](const Key&, const std::any*, const std::any* new_val, Layer) {
                if (new_val) {
                    scale_ = std::any_cast<double>(*new_val);
                    on_scale_changed();
                }
            }
        );
    }

    ~Renderer() {
        ConfigStore::instance().unwatch(watcher_handle_);
    }

    void render_frame() {
        // 直接使用缓存的值
        double scale = scale_;
        // ...
    }

private:
    double scale_;
    WatcherHandle watcher_handle_;

    void on_scale_changed() {
        // 处理缩放变化
    }
};
```text

### 3.2 批量操作建议

当需要修改多个配置项时，使用手动通知策略可以批量处理变更：

```cpp
// 批量修改配置
void update_multiple_settings() {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    // 使用 Manual 策略，避免每次 set 都触发 Watcher
    NotifyPolicy manual = NotifyPolicy::Manual;

    store.set(KeyView{.group = "ui", .key = "theme"}, "dark", Layer::User, manual);
    store.set(KeyView{.group = "ui", .key = "font_size"}, 14, Layer::User, manual);
    store.set(KeyView{.group = "ui", .key = "scale"}, 1.25, Layer::User, manual);

    // 一次性触发所有 Watcher
    store.notify();

    // 一次性同步到磁盘
    store.sync(SyncMethod::Async);
}
```text

### 3.3 异步持久化的使用

对于频繁写入的配置，使用异步持久化避免阻塞主线程：

```cpp
// 异步保存配置示例
class ConfigSaver {
public:
    static ConfigSaver& instance() {
        static ConfigSaver instance;
        return instance;
    }

    void schedule_save() {
        if (!save_pending_.load()) {
            save_pending_.store(true);
            // 延迟 500ms 后保存，避免频繁 I/O
            QTimer::singleShot(500, [this]() {
                if (save_pending_.load()) {
                    ConfigStore::instance().sync(SyncMethod::Async);
                    save_pending_.store(false);
                }
            });
        }
    }

    void save_immediately() {
        ConfigStore::instance().sync(SyncMethod::Sync);
        save_pending_.store(false);
    }

private:
    ConfigSaver() = default;
    std::atomic<bool> save_pending_{false};
};

// 使用示例
void on_config_changed() {
    ConfigStore::instance().set(...);
    ConfigSaver::instance().schedule_save();  // 延迟异步保存
}

void on_application_exit() {
    ConfigSaver::instance().save_immediately();  // 退出时立即保存
}
```yaml

---

## 四、线程安全

### 4.1 读多写少场景的最佳实践

ConfigStore 使用 `std::shared_mutex` 实现读写锁，非常适合读多写少的场景：

```cpp
// 多线程读取配置 - 完全并发
class ConfigReader {
public:
    int get_timeout() const {
        // query() 内部使用 shared_lock，允许多个线程并发读取
        return ConfigStore::instance().query<int>(
            KeyView{.group = "network", .key = "timeout"}, 5000);
    }

    std::string get_server_url() const {
        return ConfigStore::instance().query<std::string>(
            KeyView{.group = "network", .key = "server_url"}, "localhost");
    }
};

// 多线程安全使用
void worker_thread(int id) {
    ConfigReader reader;
    for (int i = 0; i < 1000; ++i) {
        int timeout = reader.get_timeout();
        std::string url = reader.get_server_url();
        // 使用配置...
    }
}
```text

### 4.2 避免死锁的注意事项

**1. 不要在 Watcher 回调中调用 ConfigStore::set()**

```cpp
// 危险：可能导致死锁
auto handle = ConfigStore::instance().watch(
    "app.theme",
    [](const Key&, auto, auto, Layer) {
        // 危险！在回调中再次写入可能导致死锁
        ConfigStore::instance().set(KeyView{.group = "app", .key = "changed"}, true);
    }
);

// 安全：使用标志位延迟处理
std::atomic<bool> theme_changed{false};
auto handle = ConfigStore::instance().watch(
    "app.theme",
    [&theme_changed](const Key&, auto, auto, Layer) {
        theme_changed.store(true);
    }
);

// 在主循环中处理
void main_loop() {
    if (theme_changed.load()) {
        theme_changed.store(false);
        // 安全地处理主题变更
        ConfigStore::instance().set(KeyView{.group = "app", .key = "changed"}, true);
    }
}
```text

**2. 持有其他锁时避免调用 ConfigStore**

```cpp
class MyClass {
    std::mutex data_mutex_;
    SomeData data_;

    void update_config() {
        // 不推荐：在持有 data_mutex_ 时调用 ConfigStore
        std::lock_guard lock(data_mutex_);
        ConfigStore::instance().set(...);  // 可能导致死锁
    }

    // 推荐：分离锁的范围
    void update_config_safe() {
        // 先准备数据
        SomeData new_data;
        {
            std::lock_guard lock(data_mutex_);
            new_data = data_;
        }
        // 释放锁后再调用 ConfigStore
        ConfigStore::instance().set(...);
    }
};
```text

### 4.3 Watcher 回调中的注意事项

```cpp
// Watcher 回调的最佳实践
class SafeWatcher {
public:
    void start_watching() {
        handle_ = ConfigStore::instance().watch(
            "app.*",
            [this](const Key& k, const std::any* old_val,
                   const std::any* new_val, Layer layer) {
                // 1. 避免在回调中执行耗时操作
                // 2. 不要在回调中直接修改 ConfigStore
                // 3. 使用线程安全的方式通知主线程

                // 提取必要的信息
                std::string key = k.full_key;
                std::any new_value = new_val ? *new_val : std::any();

                // 将事件放入队列，由主线程处理
                std::lock_guard lock(queue_mutex_);
                event_queue_.push({key, new_value, layer});
                cv_.notify_one();
            }
        );

        // 启动处理线程
        worker_thread_ = std::thread(&SafeWatcher::process_events, this);
    }

    ~SafeWatcher() {
        {
            std::lock_guard lock(queue_mutex_);
            stop_ = true;
        }
        cv_.notify_one();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        ConfigStore::instance().unwatch(handle_);
    }

private:
    struct Event {
        std::string key;
        std::any value;
        Layer layer;
    };

    void process_events() {
        while (true) {
            std::unique_lock lock(queue_mutex_);
            cv_.wait(lock, [this] {
                return stop_ || !event_queue_.empty();
            });

            if (stop_) break;

            while (!event_queue_.empty()) {
                auto event = event_queue_.front();
                event_queue_.pop();
                lock.unlock();

                // 处理事件 (不在锁内)
                handle_event(event);

                lock.lock();
            }
        }
    }

    void handle_event(const Event& event) {
        // 安全地处理配置变更
        // 可以在这里安全地读取 ConfigStore
    }

    WatcherHandle handle_;
    std::thread worker_thread_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::queue<Event> event_queue_;
    bool stop_ = false;
};
```yaml

---

## 五、错误处理

### 5.1 类型转换的处理

ConfigStore 使用 `std::any` 和 `QVariant` 存储值，类型转换可能失败：

```cpp
// 安全的类型转换处理
class SafeConfigReader {
public:
    // 方法 1：使用默认值
    int get_int(KeyView kv, int default_value = 0) {
        try {
            return ConfigStore::instance().query<int>(kv, default_value);
        } catch (const std::exception& e) {
            log_error("Failed to read int config: ", e.what());
            return default_value;
        }
    }

    // 方法 2：使用 optional 检查
    std::optional<double> get_double(KeyView kv) {
        auto value = ConfigStore::instance().query<double>(kv);
        if (value.has_value()) {
            return value;
        }
        // 尝试从字符串转换
        auto str_value = ConfigStore::instance().query<std::string>(kv);
        if (str_value) {
            try {
                return std::stod(*str_value);
            } catch (...) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    // 方法 3：验证范围
    int get_percentage(KeyView kv) {
        int value = ConfigStore::instance().query<int>(kv, 100);
        // 限制在 0-100 范围
        if (value < 0) return 0;
        if (value > 100) return 100;
        return value;
    }
};
```text

### 5.2 键验证的处理

```cpp
// 键名验证
class ConfigValidator {
public:
    static bool is_valid_key(const std::string& key) {
        // 检查键名格式
        if (key.empty() || key.length() > 256) {
            return false;
        }

        // 检查只包含允许的字符
        for (char c : key) {
            if (!(std::isalnum(c) || c == '_' || c == '.')) {
                return false;
            }
        }

        return true;
    }

    static bool set_safe(const KeyView& kv, const std::string& value) {
        // 先验证键名
        KeyHelper helper;
        Key k;
        if (!helper.fromKeyViewToKey(kv, k)) {
            return false;
        }

        if (!is_valid_key(k.full_key)) {
            return false;
        }

        // 再执行设置
        return ConfigStore::instance().set(kv, value);
    }
};
```text

### 5.3 优雅降级策略

```cpp
// 配置加载的优雅降级
class RobustConfigLoader {
public:
    struct Config {
        std::string server_url;
        int timeout;
        int retry_count;
        bool enable_cache;

        static Config load_with_fallbacks() {
            using namespace cf::config;
            auto& store = ConfigStore::instance();

            Config config;

            // 尝试多个可能的键名
            config.server_url = store.query<std::string>(
                KeyView{.group = "network", .key = "server_url"},
                // 尝试备用键名
                store.query<std::string>(
                    KeyView{.group = "network", .key = "host"},
                    // 最后使用硬编码默认值
                    "localhost"
                )
            );

            // 带验证的加载
            config.timeout = load_int_with_validation(
                KeyView{.group = "network", .key = "timeout"},
                1000,  // 默认值
                100,   // 最小值
                60000  // 最大值
            );

            config.retry_count = load_int_with_validation(
                KeyView{.group = "network", .key = "retry_count"},
                3,  // 默认值
                0,  // 最小值
                10  // 最大值
            );

            config.enable_cache = store.query<bool>(
                KeyView{.group = "network", .key = "enable_cache"},
                true  // 默认启用
            );

            return config;
        }

    private:
        static int load_int_with_validation(KeyView kv, int default_value,
                                            int min_value, int max_value) {
            using namespace cf::config;
            auto& store = ConfigStore::instance();

            int value = store.query<int>(kv, default_value);
            if (value < min_value) {
                log_warning("Config value too low, using minimum: ", min_value);
                return min_value;
            }
            if (value > max_value) {
                log_warning("Config value too high, using maximum: ", max_value);
                return max_value;
            }
            return value;
        }

        static void log_warning(const std::string& msg, int value) {
            // 实现日志记录
        }
    };
};
```yaml

---

## 六、实践模式

### 6.1 模式一：配置管理器封装类

封装 ConfigStore 以提供类型安全的配置访问接口：

```cpp
/**
 * @brief 应用程序配置管理器
 *
 * 封装 ConfigStore，提供类型安全的配置访问接口，
 * 并集中管理配置键名和默认值。
 */
class AppConfigManager {
public:
    // 单例模式
    static AppConfigManager& instance() {
        static AppConfigManager instance;
        return instance;
    }

    // ========== 主题配置 ==========

    struct ThemeConfig {
        std::string name = "default";
        bool dark_mode = false;
        int accent_color = 0x2196F3;
    };

    ThemeConfig get_theme() const {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        return ThemeConfig{
            .name = store.query<std::string>(
                KeyView{.group = "app.theme", .key = "name"}, "default"),
            .dark_mode = store.query<bool>(
                KeyView{.group = "app.theme", .key = "dark_mode"}, false),
            .accent_color = store.query<int>(
                KeyView{.group = "app.theme", .key = "accent_color"}, 0x2196F3)
        };
    }

    void set_theme(const ThemeConfig& config) {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        store.set(KeyView{.group = "app.theme", .key = "name"},
                  config.name, Layer::User);
        store.set(KeyView{.group = "app.theme", .key = "dark_mode"},
                  config.dark_mode, Layer::User);
        store.set(KeyView{.group = "app.theme", .key = "accent_color"},
                  config.accent_color, Layer::User);

        store.sync(SyncMethod::Async);
    }

    // ========== 网络配置 ==========

    struct NetworkConfig {
        std::string server_url = "https://api.example.com";
        int timeout_ms = 5000;
        int max_retries = 3;
        bool enable_proxy = false;
        std::string proxy_host;
        int proxy_port = 8080;
    };

    NetworkConfig get_network() const {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        return NetworkConfig{
            .server_url = store.query<std::string>(
                KeyView{.group = "network", .key = "server_url"},
                "https://api.example.com"),
            .timeout_ms = clamp_range(
                store.query<int>(KeyView{.group = "network", .key = "timeout_ms"}, 5000),
                1000, 30000),
            .max_retries = clamp_range(
                store.query<int>(KeyView{.group = "network", .key = "max_retries"}, 3),
                0, 10),
            .enable_proxy = store.query<bool>(
                KeyView{.group = "network", .key = "enable_proxy"}, false),
            .proxy_host = store.query<std::string>(
                KeyView{.group = "network", .key = "proxy_host"}, ""),
            .proxy_port = store.query<int>(
                KeyView{.group = "network", .key = "proxy_port"}, 8080)
        };
    }

    // ========== 编辑器配置 ==========

    struct EditorConfig {
        std::string font_family = "Monospace";
        int font_size = 12;
        bool line_numbers = true;
        bool word_wrap = false;
        int tab_size = 4;
    };

    EditorConfig get_editor() const {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        return EditorConfig{
            .font_family = store.query<std::string>(
                KeyView{.group = "editor", .key = "font_family"}, "Monospace"),
            .font_size = clamp_range(
                store.query<int>(KeyView{.group = "editor", .key = "font_size"}, 12),
                8, 72),
            .line_numbers = store.query<bool>(
                KeyView{.group = "editor", .key = "line_numbers"}, true),
            .word_wrap = store.query<bool>(
                KeyView{.group = "editor", .key = "word_wrap"}, false),
            .tab_size = clamp_range(
                store.query<int>(KeyView{.group = "editor", .key = "tab_size"}, 4),
                1, 8)
        };
    }

    // ========== 初始化 ==========

    void initialize() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        // 注册所有配置键
        register_theme_keys();
        register_network_keys();
        register_editor_keys();

        store.sync(SyncMethod::Sync);
    }

private:
    AppConfigManager() = default;
    ~AppConfigManager() = default;

    // 禁止拷贝和移动
    AppConfigManager(const AppConfigManager&) = delete;
    AppConfigManager& operator=(const AppConfigManager&) = delete;

    static int clamp_range(int value, int min_val, int max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    void register_theme_keys() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        store.register_key(
            Key{.full_key = "app.theme.name",
                .full_description = "Application theme name"},
            std::string("default"), Layer::User);

        store.register_key(
            Key{.full_key = "app.theme.dark_mode",
                .full_description = "Enable dark mode"},
            false, Layer::User);

        store.register_key(
            Key{.full_key = "app.theme.accent_color",
                .full_description = "Theme accent color (ARGB)"},
            0x2196F3, Layer::User);
    }

    void register_network_keys() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        store.register_key(
            Key{.full_key = "network.server_url",
                .full_description = "API server URL"},
            std::string("https://api.example.com"), Layer::User);

        store.register_key(
            Key{.full_key = "network.timeout_ms",
                .full_description = "Network request timeout in milliseconds"},
            5000, Layer::User);

        store.register_key(
            Key{.full_key = "network.max_retries",
                .full_description = "Maximum number of retry attempts"},
            3, Layer::User);
    }

    void register_editor_keys() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        store.register_key(
            Key{.full_key = "editor.font_family",
                .full_description = "Editor font family"},
            std::string("Monospace"), Layer::User);

        store.register_key(
            Key{.full_key = "editor.font_size",
                .full_description = "Editor font size in points"},
            12, Layer::User);

        store.register_key(
            Key{.full_key = "editor.tab_size",
                .full_description = "Tab size in spaces"},
            4, Layer::User);
    }
};

// 使用示例
void use_app_config() {
    auto& config = AppConfigManager::instance();

    // 获取配置
    auto theme = config.get_theme();
    auto network = config.get_network();
    auto editor = config.get_editor();

    // 修改配置
    ThemeConfig new_theme;
    new_theme.name = "dark";
    new_theme.dark_mode = true;
    config.set_theme(new_theme);
}
```text

### 6.2 模式二：热重载支持

实现配置文件的监听和热重载功能：

```cpp
/**
 * @brief 配置热重载管理器
 *
 * 监听配置文件变化，自动重新加载配置并通知观察者。
 */
class ConfigHotReload {
public:
    using ConfigChangedCallback = std::function<void()>;

    static ConfigHotReload& instance() {
        static ConfigHotReload instance;
        return instance;
    }

    void start_watching() {
        if (watcher_) return;  // 已启动

        // 使用 QFileSystemWatcher 监听配置文件
        watcher_ = std::make_unique<QFileSystemWatcher>();

        // 添加要监听的配置文件路径
        auto paths = get_config_paths();
        for (const auto& path : paths) {
            if (QFile::exists(path)) {
                watcher_->addPath(path);
            }
        }

        // 连接文件变化信号
        QObject::connect(watcher_.get(), &QFileSystemWatcher::fileChanged,
                         this, &ConfigHotReload::on_file_changed);

        // 设置防抖定时器
        debounce_timer_ = std::make_unique<QTimer>();
        debounce_timer_->setSingleShot(true);
        debounce_timer_->setInterval(500);  // 500ms 防抖

        QObject::connect(debounce_timer_.get(), &QTimer::timeout,
                         this, &ConfigHotReload::reload_config);
    }

    void stop_watching() {
        watcher_.reset();
        debounce_timer_.reset();
    }

    void add_observer(ConfigChangedCallback callback) {
        observers_.push_back(std::move(callback));
    }

private:
    ConfigHotReload() = default;
    ~ConfigHotReload() = default;

    QStringList get_config_paths() {
        using namespace cf::config;
        // 获取配置文件路径
        // 这里简化处理，实际应该从 PathProvider 获取
        return {
            QString::fromStdString(get_user_config_path()),
            QString::fromStdString(get_app_config_path())
        };
    }

    std::string get_user_config_path() {
        // 实现获取用户配置路径
        return "~/.config/cfdesktop/user.ini";
    }

    std::string get_app_config_path() {
        // 实现获取应用配置路径
        return "./config/app.ini";
    }

    void on_file_changed(const QString& path) {
        // 文件变化时，启动防抖定时器
        qDebug() << "Config file changed:" << path;
        debounce_timer_->start();
    }

    void reload_config() {
        using namespace cf::config;

        qDebug() << "Reloading configuration...";

        // 重新加载配置
        ConfigStore::instance().reload();

        // 通知所有观察者
        for (auto& observer : observers_) {
            observer();
        }

        qDebug() << "Configuration reloaded";
    }

    std::unique_ptr<QFileSystemWatcher> watcher_;
    std::unique_ptr<QTimer> debounce_timer_;
    std::vector<ConfigChangedCallback> observers_;
};

// 使用示例
class ThemeManager {
public:
    ThemeManager() {
        // 监听配置变化
        ConfigHotReload::instance().add_observer([this]() {
            on_config_reloaded();
        });
    }

    void apply_theme() {
        auto& config = AppConfigManager::instance();
        auto theme = config.get_theme();
        // 应用主题...
        qDebug() << "Theme applied:" << QString::fromStdString(theme.name);
    }

private:
    void on_config_reloaded() {
        qDebug() << "Config reloaded, reapplying theme...";
        apply_theme();
    }
};

// 在应用启动时启用热重载
void initialize_application() {
    // 初始化配置
    AppConfigManager::instance().initialize();

    // 启用热重载 (开发环境)
#ifdef QT_DEBUG
    ConfigHotReload::instance().start_watching();
#endif
}
```text

### 6.3 模式三：配置迁移

处理配置版本升级时的数据迁移：

```cpp
/**
 * @brief 配置迁移管理器
 *
 * 处理不同版本配置之间的数据迁移。
 */
class ConfigMigration {
public:
    static ConfigMigration& instance() {
        static ConfigMigration instance;
        return instance;
    }

    /**
     * 执行配置迁移
     * @param from_version 迁移前的版本号
     * @param to_version 迁移后的版本号
     */
    bool migrate(int from_version, int to_version) {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        for (int v = from_version; v < to_version; ++v) {
            if (!migrate_to(v + 1)) {
                return false;
            }
        }

        // 更新版本号
        store.set(KeyView{.group = "app", .key = "config_version"},
                  to_version, Layer::User);
        store.sync(SyncMethod::Sync);

        return true;
    }

    /**
     * 检查是否需要迁移
     */
    bool needs_migration() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        int current_version = store.query<int>(
            KeyView{.group = "app", .key = "config_version"}, 1);
        int target_version = get_target_version();

        return current_version < target_version;
    }

    int get_current_version() const {
        using namespace cf::config;
        return ConfigStore::instance().query<int>(
            KeyView{.group = "app", .key = "config_version"}, 1);
    }

private:
    ConfigMigration() = default;

    static constexpr int get_target_version() { return 2; }

    bool migrate_to(int version) {
        switch (version) {
            case 2:
                return migrate_v1_to_v2();
            // 添加未来的版本迁移
            // case 3:
            //     return migrate_v2_to_v3();
            default:
                return false;
        }
    }

    /**
     * v1 -> v2 迁移
     *
     * 变更：
     * - app.theme.* 重命名为 ui.theme.*
     * - editor.font.family 重命名为 editor.font_family
     */
    bool migrate_v1_to_v2() {
        using namespace cf::config;
        auto& store = ConfigStore::instance();

        qDebug() << "Migrating config from v1 to v2...";

        // 手动通知策略，避免在迁移过程中触发 Watcher
        NotifyPolicy manual = NotifyPolicy::Manual;

        // 迁移 app.theme.* -> ui.theme.*
        migrate_key(store, "app.theme.name", "ui.theme.name", manual);
        migrate_key(store, "app.theme.dark_mode", "ui.theme.dark_mode", manual);
        migrate_key(store, "app.theme.accent_color", "ui.theme.accent_color", manual);

        // 迁移 editor.font.family -> editor.font_family
        migrate_key(store, "editor.font.family", "editor.font_family", manual);

        // 删除旧键
        store.unregister_key(Key{.full_key = "app.theme.name", .full_description = ""},
                            Layer::User, manual);
        store.unregister_key(Key{.full_key = "app.theme.dark_mode", .full_description = ""},
                            Layer::User, manual);
        store.unregister_key(Key{.full_key = "app.theme.accent_color", .full_description = ""},
                            Layer::User, manual);
        store.unregister_key(Key{.full_key = "editor.font.family", .full_description = ""},
                            Layer::User, manual);

        // 触发通知
        store.notify();

        qDebug() << "Migration v1 -> v2 completed";
        return true;
    }

    void migrate_key(cf::config::ConfigStore& store,
                     const std::string& old_key,
                     const std::string& new_key,
                     cf::config::NotifyPolicy policy) {
        // 读取旧值
        auto old_value = get_any_value(store, old_key);
        if (!old_value.has_value()) {
            return;  // 旧键不存在，跳过
        }

        // 写入新键
        set_any_value(store, new_key, *old_value, Layer::User, policy);
    }

    std::optional<std::any> get_any_value(cf::config::ConfigStore& store,
                                          const std::string& key) {
        // 简化实现，实际需要根据类型获取
        // 这里假设所有值都是字符串
        KeyView kv{.group = key.substr(0, key.rfind('.')),
                   .key = key.substr(key.rfind('.') + 1)};
        auto value = store.query<std::string>(kv);
        if (value) {
            return *value;
        }
        return std::nullopt;
    }

    void set_any_value(cf::config::ConfigStore& store,
                       const std::string& key,
                       const std::any& value,
                       Layer layer,
                       NotifyPolicy policy) {
        // 简化实现
        if (value.type() == typeid(std::string)) {
            KeyView kv{.group = key.substr(0, key.rfind('.')),
                       .key = key.substr(key.rfind('.') + 1)};
            store.set(kv, std::any_cast<std::string>(value), layer, policy);
        }
    }
};

// 应用启动时执行迁移
void check_and_migrate_config() {
    auto& migration = ConfigMigration::instance();

    if (migration.needs_migration()) {
        int current = migration.get_current_version();
        int target = 2;  // ConfigMigration::get_target_version()

        qDebug() << "Migrating config from v" << current << " to v" << target;

        if (migration.migrate(current, target)) {
            qDebug() << "Config migration successful";
        } else {
            qWarning() << "Config migration failed";
        }
    }
}
```text

### 6.4 模式四：多实例隔离（使用自定义路径提供者）

通过自定义路径提供者实现多个 ConfigStore 实例的隔离：

```cpp
/**
 * @brief 自定义路径提供者
 *
 * 为测试或特殊场景提供自定义配置文件路径。
 */
class CustomPathProvider : public IConfigStorePathProvider {
public:
    /**
     * @brief 创建测试用的路径提供者
     * @param base_path 基础路径
     * @param instance_name 实例名称，用于区分不同实例
     */
    CustomPathProvider(const QString& base_path,
                      const QString& instance_name)
        : base_path_(base_path), instance_name_(instance_name) {

        // 确保目录存在
        QDir().mkpath(base_path);
    }

    QString system_path() const override {
        return base_path_ + "/" + instance_name_ + "_system.ini";
    }

    QString user_dir() const override {
        return base_path_;
    }

    QString user_filename() const override {
        return instance_name_ + "_user.ini";
    }

    QString app_dir() const override {
        return base_path_;
    }

    QString app_filename() const override {
        return instance_name_ + "_app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        // 启用所有层
        (void)layer_index;
        return true;
    }

private:
    QString base_path_;
    QString instance_name_;
};

/**
 * @brief 隔离的配置存储包装器
 *
 * 使用不同的配置文件路径创建隔离的配置存储实例。
 */
class IsolatedConfigStore {
public:
    explicit IsolatedConfigStore(const QString& instance_name,
                                 const QString& base_path = "/tmp/test_config")
        : instance_name_(instance_name), base_path_(base_path) {

        // 创建自定义路径提供者
        auto path_provider = std::make_shared<CustomPathProvider>(base_path, instance_name);

        // 初始化 ConfigStore
        cf::config::ConfigStore::instance().initialize(path_provider);
    }

    ~IsolatedConfigStore() {
        // 可选：清理测试文件
        // cleanup();
    }

    void cleanup() {
        QFile::remove(base_path_ + "/" + instance_name_ + "_system.ini");
        QFile::remove(base_path_ + "/" + instance_name_ + "_user.ini");
        QFile::remove(base_path_ + "/" + instance_name_ + "_app.ini");
    }

    cf::config::ConfigStore& store() {
        return cf::config::ConfigStore::instance();
    }

private:
    QString instance_name_;
    QString base_path_;
};

// ========== 使用示例 ==========

// 示例 1：单元测试中的隔离配置
class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试使用独立的配置文件
        test_store_ = std::make_unique<IsolatedConfigStore>(
            QString("test_") + QString::number(std::rand()),  // 随机实例名
            "/tmp/config_tests"
        );
    }

    void TearDown() override {
        test_store_->cleanup();
    }

    cf::config::ConfigStore& config() {
        return test_store_->store();
    }

private:
    std::unique_ptr<IsolatedConfigStore> test_store_;
};

TEST_F(ConfigTest, SetValueAndQuery) {
    auto& store = config();

    store.set(cf::config::KeyView{.group = "test", .key = "value"},
              42, cf::config::Layer::App);

    auto result = store.query<int>(cf::config::KeyView{.group = "test", .key = "value"});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

// 示例 2：多用户配置隔离
class MultiUserConfigManager {
public:
    struct UserConfig {
        std::string username;
        std::string theme;
        int font_size;
    };

    void set_user_config(const std::string& username, const UserConfig& config) {
        auto& store = get_user_store(username);

        store.set(cf::config::KeyView{.group = "user", .key = "theme"},
                  config.theme, cf::config::Layer::User);
        store.set(cf::config::KeyView{.group = "user", .key = "font_size"},
                  config.font_size, cf::config::Layer::User);

        store.sync(cf::config::ConfigStore::SyncMethod::Sync);
    }

    UserConfig get_user_config(const std::string& username) {
        auto& store = get_user_store(username);

        return UserConfig{
            .username = username,
            .theme = store.query<std::string>(
                cf::config::KeyView{.group = "user", .key = "theme"}, "default"),
            .font_size = store.query<int>(
                cf::config::KeyView{.group = "user", .key = "font_size"}, 12)
        };
    }

private:
    cf::config::ConfigStore& get_user_store(const std::string& username) {
        // 每个用户使用独立的配置文件
        auto it = user_stores_.find(username);
        if (it == user_stores_.end()) {
            auto store = std::make_unique<IsolatedConfigStore>(
                QString::fromStdString(username),
                "/tmp/multi_user_config"
            );
            it = user_stores_.emplace(username, std::move(store)).first;
        }
        return it->second->store();
    }

    std::unordered_map<std::string, std::unique_ptr<IsolatedConfigStore>> user_stores_;
};
```bash

---

## 附录：快速参考

### A.1 层级优先级速查

| 层级 | 优先级 | 持久化 | 用途 |
|------|--------|--------|------|
| Temp | 3 (最高) | 否 | 临时数据、测试 |
| App | 2 | 是 | 运行时状态 |
| User | 1 | 是 | 用户偏好 |
| System | 0 (最低) | 是 | 系统默认 |

### A.2 常用代码片段

```cpp
// 设置用户偏好
ConfigStore::instance().set(KeyView{.group = "user.theme", .key = "name"},
                            "dark", Layer::User);

// 读取带默认值
auto theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "user.theme", .key = "name"}, "default");

// 批量修改
store.set(..., Layer::User, NotifyPolicy::Manual);
store.set(..., Layer::User, NotifyPolicy::Manual);
store.notify();
store.sync(SyncMethod::Async);

// 监听变化
auto handle = store.watch("user.theme.*", [](auto... args) {
    // 处理变化
});
store.unwatch(handle);
```yaml

### A.3 错误处理检查清单

- [ ] 查询时是否提供合理的默认值？
- [ ] 类型转换是否考虑了失败情况？
- [ ] 键名是否进行了验证？
- [ ] Watcher 回调是否避免再次调用 ConfigStore？
- [ ] 是否使用了异步持久化避免阻塞？
- [ ] 多线程场景是否考虑了锁的顺序？

---

**文档版本：** v1.0
**最后更新：** 2026-03-17
**维护者：** CFDesktop 团队
