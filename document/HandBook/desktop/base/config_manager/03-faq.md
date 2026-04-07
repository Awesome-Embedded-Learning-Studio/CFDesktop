# ConfigStore 常见问题与故障排查

本文档列出了 ConfigStore 使用中的常见问题、故障排查方法和调试技巧。

## 目录

- [常见问题](#常见问题)
- [故障排查](#故障排查)
- [平台特定问题](#平台特定问题)
- [性能问题](#性能问题)

---

## 常见问题

### Q1: 配置修改后没有生效？

#### 可能原因 1：未调用 sync()

配置修改后需要调用 `sync()` 才能持久化到磁盘。

```cpp
// 问题代码
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark");
// 程序崩溃或重启后配置丢失

// 解决方案：手动同步
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark");
ConfigStore::instance().sync(SyncMethod::Sync);  // 同步写入

// 或使用异步同步（推荐）
ConfigStore::instance().sync(SyncMethod::Async);  // 不阻塞调用方
```

#### 可能原因 2：层级优先级问题

查询时返回的是高优先级的值，低优先级的修改被覆盖。

```cpp
// 问题场景
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "light", Layer::System);
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark", Layer::Temp);

// 查询时始终返回 "dark"（Temp 优先级高于 System）
auto theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "theme"}, ""
);  // 返回 "dark"

// 解决方案 1：清除高优先级配置
ConfigStore::instance().clear_layer(Layer::Temp);

// 解决方案 2：查询指定层级
auto system_theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "theme"},
    Layer::System,
    ""
);
```

#### 可能原因 3：KeyView 转换失败

KeyView 中包含非法字符导致键转换失败。

```cpp
// 问题代码
KeyView invalid_kv{.group = "app theme", .key = "name"};  // 包含空格
ConfigStore::instance().set(invalid_kv, "value");  // 返回 false

// 解决方案：使用合法字符
KeyView valid_kv{.group = "app_theme", .key = "name"};  // 仅字母、数字、下划线
ConfigStore::instance().set(valid_kv, "value");  // 成功
```

---

### Q2: 如何在不同环境使用不同配置？

#### 方法 1：使用层级分离

```cpp
// 开发环境 - 使用 System 层
#ifdef DEBUG
    ConfigStore::instance().set(
        KeyView{.group = "api", .key = "endpoint"},
        "https://dev.api.example.com",
        Layer::System
    );
#else
    ConfigStore::instance().set(
        KeyView{.group = "api", .key = "endpoint"},
        "https://api.example.com",
        Layer::System
    );
#endif

// 用户可以覆盖（User 层优先级更高）
ConfigStore::instance().set(
    KeyView{.group = "api", .key = "endpoint"},
    "https://custom.api.com",
    Layer::User
);
```

#### 方法 2：自定义路径提供者

```cpp
#include <QCoreApplication>
#include <cstdlib>

class EnvironmentPathProvider : public IConfigStorePathProvider {
public:
    QString system_path() const override {
#ifdef DEBUG
        return QString::fromStdString("/etc/myapp/dev/system.ini");
#else
        return QString::fromStdString("/etc/myapp/prod/system.ini");
#endif
    }

    QString user_dir() const override {
        return QString::fromStdString(std::string(getenv("HOME")) + "/.config/myapp");
    }

    QString user_filename() const override {
        return "user.ini";
    }

    QString app_dir() const override {
        return QCoreApplication::applicationDirPath() + "/config";
    }

    QString app_filename() const override {
        return "app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        return true;  // 启用所有层级
    }
};

// 初始化时使用
ConfigStore::instance().initialize(std::make_shared<EnvironmentPathProvider>());
```

#### 方法 3：使用环境变量

```cpp
#include <QCoreApplication>
#include <cstdlib>

// 从环境变量读取配置路径
const char* env_path = getenv("MYAPP_CONFIG_PATH");
std::string config_dir = env_path ? env_path : "/etc/myapp";

class DynamicPathProvider : public IConfigStorePathProvider {
    QString base_dir_;
public:
    DynamicPathProvider(const QString& dir) : base_dir_(dir) {}

    QString system_path() const override {
        return base_dir_ + "/system.ini";
    }

    QString user_dir() const override {
        return QString::fromStdString(std::string(getenv("HOME")) + "/.config/myapp");
    }

    QString user_filename() const override {
        return "user.ini";
    }

    QString app_dir() const override {
        return QCoreApplication::applicationDirPath() + "/config";
    }

    QString app_filename() const override {
        return "app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        return true;
    }
};

// 使用
ConfigStore::instance().initialize(
    std::make_shared<DynamicPathProvider>(QString::fromStdString(config_dir))
);
```

---

### Q3: Watcher 回调没有被触发？

#### 原因 1：使用了 Manual 通知策略但未调用 notify()

```cpp
// 问题代码
ConfigStore::instance().watch(
    "app.theme",
    [](const Key& k, auto old, auto new_v, Layer layer) {
        std::cout << "Theme changed" << std::endl;
    },
    NotifyPolicy::Manual  // 手动模式
);

ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark");
// 回调不会被触发

// 解决方案：手动触发通知
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark", Layer::App, NotifyPolicy::Manual);
ConfigStore::instance().notify();  // 触发所有 Manual Watcher
```

#### 原因 2：键模式不匹配

```cpp
// 问题代码
ConfigStore::instance().watch(
    "app.theme.name",  // 精确匹配
    callback
);

ConfigStore::instance().set(KeyView{.group = "other", .key = "theme"}, "dark");
// 修改的是 "other.theme"，不会触发 "app.theme.name"

// 解决方案：使用通配符
ConfigStore::instance().watch(
    "*.theme.*",  // 匹配所有 theme 相关键
    callback
);

ConfigStore::instance().watch(
    "app.*",  // 匹配 app 下的所有键
    callback
);
```

#### 原因 3：Watcher 被提前取消

```cpp
// 问题代码
{
    WatcherHandle handle = ConfigStore::instance().watch("app.*", callback);
    ConfigStore::instance().unwatch(handle);  // 立即取消
    // 后续修改不会触发
}

// 解决方案：保持 handle 有效
class AppManager {
    WatcherHandle theme_watcher_;
public:
    void init() {
        theme_watcher_ = ConfigStore::instance().watch(
            "app.theme.*",
            [this](const Key& k, auto old, auto new_v, Layer layer) {
                onThemeChanged(k);
            }
        );
    }

    ~AppManager() {
        ConfigStore::instance().unwatch(theme_watcher_);
    }
};
```

---

### Q4: 类型转换失败怎么办？

#### 问题：读取的类型与存储的类型不匹配

```cpp
// 问题场景
ConfigStore::instance().set(KeyView{.group = "test", .key = "value"}, std::string("123"));

// 尝试读取为 int，但字符串无法直接转换
auto result = ConfigStore::instance().query<int>(KeyView{.group = "test", .key = "value"}, 0);
// 可能返回默认值 0
```

#### 解决方案 1：先读取字符串再转换

```cpp
auto str_value = ConfigStore::instance().query<std::string>(
    KeyView{.group = "test", .key = "value"}, ""
);

if (!str_value.empty()) {
    try {
        int int_value = std::stoi(str_value);
        // 使用 int_value
    } catch (const std::exception& e) {
        // 处理转换错误
    }
}
```

#### 解决方案 2：使用 QVariant 兼容的类型

```cpp
// 存储时使用 QVariant 能正确转换的类型
ConfigStore::instance().set(KeyView{.group = "test", .key = "value"}, 123);  // 存为 int

// 读取时可以直接获取
int value = ConfigStore::instance().query<int>(
    KeyView{.group = "test", .key = "value"}, 0
);  // 正确返回 123
```

#### 解决方案 3：使用 std::any 处理多种类型

```cpp
std::any value = get_raw_value(KeyView{.group = "test", .key = "value"});

if (value.type() == typeid(int)) {
    int int_value = std::any_cast<int>(value);
} else if (value.type() == typeid(std::string)) {
    std::string str_value = std::any_cast<std::string>(value);
    // 手动转换...
}
```

---

### Q5: 如何迁移旧的配置文件？

#### 方法 1：直接复制 INI 文件

```bash
# Linux
cp /old/path/config.ini ~/.config/myapp/user.ini

# Windows
copy C:\OldPath\config.ini %APPDATA%\MyApp\user.ini

# macOS
cp /old/path/config.plist ~/Library/Preferences/com.myapp.plist
```

#### 方法 2：使用 ConfigStore API 迁移

```cpp
void migrate_old_config(const std::string& old_file_path) {
    QSettings old_settings(
        QString::fromStdString(old_file_path),
        QSettings::IniFormat
    );

    // 遍历所有旧配置
    for (const auto& group : old_settings.childGroups()) {
        old_settings.beginGroup(group);

        for (const auto& key : old_settings.childKeys()) {
            QVariant value = old_settings.value(key);

            // 构造 KeyView
            KeyView kv{
                .group = group.toStdString(),
                .key = key.toStdString()
            };

            // 根据类型写入新配置
            if (value.type() == QVariant::Int) {
                ConfigStore::instance().set(kv, value.toInt(), Layer::User);
            } else if (value.type() == QVariant::String) {
                ConfigStore::instance().set(kv, value.toString().toStdString(), Layer::User);
            } else if (value.type() == QVariant::Bool) {
                ConfigStore::instance().set(kv, value.toBool(), Layer::User);
            }
            // 其他类型...
        }

        old_settings.endGroup();
    }

    // 同步到磁盘
    ConfigStore::instance().sync(SyncMethod::Sync);
}
```

#### 方法 3：映射旧键名到新键名

```cpp
struct KeyMapping {
    std::string old_group;
    std::string old_key;
    std::string new_group;
    std::string new_key;
};

std::vector<KeyMapping> mappings = {
    {"ui", "theme", "app.theme", "name"},
    {"ui", "dpi", "display", "dpi"},
    {"network", "server", "api", "endpoint"}
};

void migrate_with_mapping(const std::string& old_file) {
    QSettings old_settings(QString::fromStdString(old_file), QSettings::IniFormat);

    for (const auto& mapping : mappings) {
        old_settings.beginGroup(QString::fromStdString(mapping.old_group));
        QVariant value = old_settings.value(QString::fromStdString(mapping.old_key));
        old_settings.endGroup();

        if (!value.isNull()) {
            KeyView new_kv{
                .group = mapping.new_group,
                .key = mapping.new_key
            };

            // 根据类型写入
            if (value.type() == QVariant::String) {
                ConfigStore::instance().set(
                    new_kv,
                    value.toString().toStdString(),
                    Layer::User
                );
            }
            // 其他类型...
        }
    }

    ConfigStore::instance().sync(SyncMethod::Sync);
}
```

---

### Q6: Temp 层的数据什么时候会丢失？

#### 丢失场景

```cpp
// 1. 程序退出后（Temp 层不持久化）
ConfigStore::instance().set(KeyView{.group = "session", .key = "id"}, "abc123", Layer::Temp);
// 程序重启后，此数据丢失

// 2. 调用 reload() 后
ConfigStore::instance().set(KeyView{.group = "temp", .key = "data"}, "value", Layer::Temp);
ConfigStore::instance().reload();  // Temp 层被清空
// 之前的数据丢失

// 3. 调用 clear_layer(Layer::Temp)
ConfigStore::instance().clear_layer(Layer::Temp);
// Temp 层数据被清空
```

#### 保留场景

```cpp
// Temp 层数据在程序运行期间一直有效
ConfigStore::instance().set(KeyView{.group = "cache", .key = "key"}, "value", Layer::Temp);

// 程序运行期间可以正常读取
auto value = ConfigStore::instance().query<std::string>(
    KeyView{.group = "cache", .key = "key"}, ""
);  // 返回 "value"

// sync() 不会清空 Temp 层
ConfigStore::instance().sync(SyncMethod::Sync);
// Temp 层数据仍然存在
```

#### 最佳实践

```cpp
// Temp 层适合存储：
// 1. 会话临时数据
ConfigStore::instance().set(
    KeyView{.group = "session", .key = "id"},
    generate_session_id(),
    Layer::Temp
);

// 2. 测试/调试配置
#ifdef DEBUG
    ConfigStore::instance().set(
        KeyView{.group = "debug", .key = "verbose"},
        true,
        Layer::Temp
    );
#endif

// 3. 运行时计算的缓存值
ConfigStore::instance().set(
    KeyView{.group = "cache", .key = "computed_value"},
    expensive_computation(),
    Layer::Temp
);

// 需要持久化的数据，不要使用 Temp 层
ConfigStore::instance().set(
    KeyView{.group = "user", .key = "preference"},
    "dark",
    Layer::User  // 使用 User 或 App 层
);
```

---

### Q7: 如何批量修改配置而不触发多次 Watcher？

#### 方法 1：使用 Manual 通知策略

```cpp
// 问题代码：每次修改都触发 Watcher
ConfigStore::instance().set(KeyView{.group = "batch", .key = "a"}, 1);
// Watcher 被触发
ConfigStore::instance().set(KeyView{.group = "batch", .key = "b"}, 2);
// Watcher 被触发
ConfigStore::instance().set(KeyView{.group = "batch", .key = "c"}, 3);
// Watcher 被触发

// 解决方案：使用 Manual 策略
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "a"}, 1,
    Layer::App,
    NotifyPolicy::Manual  // 不立即触发
);
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "b"}, 2,
    Layer::App,
    NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "c"}, 3,
    Layer::App,
    NotifyPolicy::Manual
);

// 批量修改完成后，一次性触发
ConfigStore::instance().notify();  // 所有 Watcher 被触发一次
```

#### 方法 2：使用事务模式

```cpp
class ConfigTransaction {
public:
    ConfigTransaction() {
        // 开始事务：暂存 Watcher 状态
    }

    ~ConfigTransaction() {
        // 提交事务：触发 notify()
        ConfigStore::instance().notify();
        ConfigStore::instance().sync(SyncMethod::Async);
    }

    template<typename T>
    void set(const KeyView& key, const T& value, Layer layer = Layer::App) {
        ConfigStore::instance().set(key, value, layer, NotifyPolicy::Manual);
    }
};

// 使用
{
    ConfigTransaction transaction;

    transaction.set(KeyView{.group = "ui", .key = "width"}, 800);
    transaction.set(KeyView{.group = "ui", .key = "height"}, 600);
    transaction.set(KeyView{.group = "ui", .key = "theme"}, "dark");

    // 事务结束，自动触发通知
}
```

#### 方法 3：先取消 Watcher，批量修改后再添加

```cpp
// 保存 Watcher
auto callback = [](const Key& k, auto old, auto new_v, Layer layer) {
    // 处理变更
};

// 批量修改前取消监听
ConfigStore::instance().unwatch(watcher_handle);

// 批量修改
ConfigStore::instance().set(KeyView{.group = "batch", .key = "a"}, 1);
ConfigStore::instance().set(KeyView{.group = "batch", .key = "b"}, 2);
ConfigStore::instance().set(KeyView{.group = "batch", .key = "c"}, 3);

// 重新添加 Watcher
watcher_handle = ConfigStore::instance().watch("batch.*", callback);
```

---

### Q8: 配置文件位置在哪里？

#### 默认路径

| 平台 | 层级 | 路径 |
|------|------|------|
| **Linux** | System | `/etc/cfdesktop/system.ini` |
| | User | `~/.config/cfdesktop/user.ini` |
| | App | `<应用目录>/config/app.ini` |
| **Windows** | System | `HKEY_LOCAL_MACHINE\Software\CFDesktop` |
| | User | `HKEY_CURRENT_USER\Software\CFDesktop` |
| | App | `<应用目录>\config\app.ini` |
| **macOS** | System | `/Library/Preferences/com.cfdesktop.system.plist` |
| | User | `~/Library/Preferences/com.cfdesktop.user.plist` |
| | App | `<应用目录>/config/app.ini` |

#### 获取当前路径

```cpp
// 方法 1：通过自定义路径提供者获取
#include <QCoreApplication>
#include <iostream>
#include <cstdlib>

class DebugPathProvider : public IConfigStorePathProvider {
public:
    QString system_path() const override {
        QString path = "/etc/cfdesktop/system.ini";
        std::cout << "System path: " << path.toStdString() << std::endl;
        return path;
    }

    QString user_dir() const override {
        QString path = QString::fromStdString(std::string(getenv("HOME")) + "/.config/cfdesktop");
        std::cout << "User dir: " << path.toStdString() << std::endl;
        return path;
    }

    QString user_filename() const override {
        return "user.ini";
    }

    QString app_dir() const override {
        QString path = QCoreApplication::applicationDirPath() + "/config";
        std::cout << "App dir: " << path.toStdString() << std::endl;
        return path;
    }

    QString app_filename() const override {
        return "app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        return true;
    }
};

// 初始化
ConfigStore::instance().initialize(std::make_shared<DebugPathProvider>());

// 方法 2：直接检查文件是否存在
void check_config_files() {
    std::array<std::string, 3> paths = {
        "/etc/cfdesktop/system.ini",
        std::getenv("HOME") + "/.config/cfdesktop/user.ini"s,
        QCoreApplication::applicationDirPath().toStdString() + "/config/app.ini"
    };

    for (const auto& path : paths) {
        QFileInfo file(QString::fromStdString(path));
        if (file.exists()) {
            std::cout << "Found: " << path << std::endl;
            std::cout << "  Size: " << file.size() << " bytes" << std::endl;
            std::cout << "  Modified: " << file.lastModified().toString().toStdString() << std::endl;
        } else {
            std::cout << "Not found: " << path << std::endl;
        }
    }
}
```

---

## 故障排查

### 问题诊断流程

```
                    配置读取异常
                         |
         +---------------+---------------+
         |                               |
    返回默认值？                    返回错误值？
         |                               |
    ↓                               |
 键不存在？                        |
    |                               |
    +---+---+                       |
        |                           |
    是     否                       |
    |       |                       |
    ↓       ↓                       |
检查键名  检查层级                   |
    |       |                       |
    |       +-------+---------------+
    |               |
    |           检查文件
    |               |
    |           +---+---+
    |           |       |
    |        文件    权限
    |        存在    问题
    |           |       |
    +-----------+-------+
                |
            解决问题
```

### 详细诊断步骤

#### 步骤 1：确认键是否存在

```cpp
void diagnose_key(const KeyView& kv) {
    std::cout << "诊断键: " << kv.group << "." << kv.key << std::endl;

    // 检查键是否有效
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(kv, k)) {
        std::cout << "  [错误] 键名无效，包含非法字符" << std::endl;
        std::cout << "  提示: 键名只能包含字母、数字、下划线和点号" << std::endl;
        return;
    }
    std::cout << "  [OK] 完整键名: " << k.full_key << std::endl;

    // 检查各层是否存在
    std::array<Layer, 4> layers = {Layer::System, Layer::User, Layer::App, Layer::Temp};
    std::array<std::string, 4> layer_names = {"System", "User", "App", "Temp"};

    for (size_t i = 0; i < layers.size(); ++i) {
        bool exists = ConfigStore::instance().has_key(kv, layers[i]);
        std::cout << "  [" << (exists ? "X" : " ") << "] " << layer_names[i] << " 层";

        if (exists) {
            auto value = ConfigStore::instance().query<std::string>(
                kv, layers[i], ""
            );
            std::cout << " = \"" << value << "\"";
        }
        std::cout << std::endl;
    }

    // 检查优先级查询结果
    auto final_value = ConfigStore::instance().query<std::string>(kv, "");
    std::cout << "  最终值: \"" << final_value << "\" (优先级查询)" << std::endl;
}

// 使用
diagnose_key(KeyView{.group = "app.theme", .key = "name"});
```

#### 步骤 2：检查配置文件

```cpp
void diagnose_config_files() {
    std::cout << "\n=== 配置文件诊断 ===" << std::endl;

    std::array<std::pair<Layer, std::string>, 3> files = {
        {Layer::System, "/etc/cfdesktop/system.ini"},
        {Layer::User, std::getenv("HOME") + "/.config/cfdesktop/user.ini"s},
        {Layer::App, QCoreApplication::applicationDirPath().toStdString() + "/config/app.ini"}
    };

    for (const auto& [layer, path] : files) {
        std::cout << "\n[" << (layer == Layer::System ? "System" :
                               layer == Layer::User ? "User" : "App") << "] " << path << std::endl;

        QFileInfo file(QString::fromStdString(path));

        // 检查文件是否存在
        if (!file.exists()) {
            std::cout << "  [警告] 文件不存在" << std::endl;
            continue;
        }
        std::cout << "  [OK] 文件存在" << std::endl;

        // 检查文件权限
        if (!file.isReadable()) {
            std::cout << "  [错误] 文件不可读" << std::endl;
        } else {
            std::cout << "  [OK] 文件可读" << std::endl;
        }

        if (!file.isWritable()) {
            std::cout << "  [警告] 文件不可写（可能无法保存配置）" << std::endl;
        } else {
            std::cout << "  [OK] 文件可写" << std::endl;
        }

        // 尝试打开文件
        QSettings settings(QString::fromStdString(path), QSettings::IniFormat);
        if (settings.status() != QSettings::NoError) {
            std::cout << "  [错误] QSettings 错误: "
                      << (settings.status() == QSettings::AccessError ? "访问错误" : "格式错误")
                      << std::endl;
        } else {
            std::cout << "  [OK] 文件格式正确" << std::endl;
        }

        // 显示文件大小和修改时间
        std::cout << "  大小: " << file.size() << " bytes" << std::endl;
        std::cout << "  修改时间: " << file.lastModified().toString().toStdString() << std::endl;
    }
}
```

#### 步骤 3：检查 Watcher 状态

```cpp
void diagnose_watchers() {
    std::cout << "\n=== Watcher 诊断 ===" << std::endl;

    // 添加测试 Watcher
    bool triggered = false;
    auto handle = ConfigStore::instance().watch(
        "diagnostic.test",
        [&triggered](const Key& k, auto old, auto new_v, Layer layer) {
            triggered = true;
            std::cout << "  [OK] Watcher 被触发" << std::endl;
            std::cout << "      键: " << k.full_key << std::endl;
            std::cout << "      层: " << static_cast<int>(layer) << std::endl;
        },
        NotifyPolicy::Immediate
    );

    std::cout << "测试 Watcher: " << handle << std::endl;

    // 触发测试
    std::cout << "触发配置变更..." << std::endl;
    ConfigStore::instance().set(
        KeyView{.group = "diagnostic", .key = "test"},
        std::string("test_value"),
        Layer::Temp
    );

    if (!triggered) {
        std::cout << "  [错误] Watcher 未被触发" << std::endl;
        std::cout << "  可能原因:" << std::endl;
        std::cout << "    1. Watcher 回调中抛出异常" << std::endl;
        std::cout << "    2. 键名不匹配" << std::endl;
        std::cout << "    3. 通知策略设置错误" << std::endl;
    }

    // 清理
    ConfigStore::instance().unwatch(handle);
    ConfigStore::instance().clear_layer(Layer::Temp);
}
```

### 日志输出分析方法

#### 启用详细日志

```cpp
// 在调试模式下，包装 ConfigStore 调用以输出日志
class LoggingConfigStore {
public:
    template<typename T>
    static T query(const KeyView& key, const T& default_value, Layer layer = Layer::System) {
        std::cout << "[ConfigStore::query] " << key.group << "." << key.key
                  << " (layer: " << static_cast<int>(layer) << ")" << std::endl;

        T value = ConfigStore::instance().query(key, layer, default_value);

        std::cout << "  -> returned: " << value << std::endl;
        return value;
    }

    template<typename T>
    static bool set(const KeyView& key, const T& value, Layer layer = Layer::App) {
        std::cout << "[ConfigStore::set] " << key.group << "." << key.key
                  << " = " << value
                  << " (layer: " << static_cast<int>(layer) << ")" << std::endl;

        bool result = ConfigStore::instance().set(key, value, layer);

        std::cout << "  -> result: " << (result ? "success" : "failed") << std::endl;
        return result;
    }
};

// 使用
auto theme = LoggingConfigStore::query<std::string>(
    KeyView{.group = "app", .key = "theme"},
    "default",
    Layer::User
);
```

#### 检查 pending_changes

```cpp
void monitor_pending_changes() {
    size_t before = ConfigStore::instance().pending_changes();
    std::cout << "待写入变更数: " << before << std::endl;

    // 执行一些操作
    ConfigStore::instance().set(KeyView{.group = "test", .key = "a"}, 1, Layer::App, NotifyPolicy::Manual);
    ConfigStore::instance().set(KeyView{.group = "test", .key = "b"}, 2, Layer::App, NotifyPolicy::Manual);

    size_t after = ConfigStore::instance().pending_changes();
    std::cout << "待写入变更数: " << after << " (新增 " << (after - before) << ")" << std::endl;

    // 调用 notify
    ConfigStore::instance().notify();
    size_t after_notify = ConfigStore::instance().pending_changes();
    std::cout << "notify 后待写入变更数: " << after_notify << std::endl;
}
```

### 调试技巧

#### 技巧 1：导出所有配置

```cpp
void dump_all_config() {
    std::cout << "\n=== 配置转储 ===" << std::endl;

    // 导出 Temp 层
    std::cout << "\n[Temp 层]" << std::endl;
    dump_layer(Layer::Temp);

    // 导出 App 层
    std::cout << "\n[App 层]" << std::endl;
    dump_layer(Layer::App);

    // 导出 User 层
    std::cout << "\n[User 层]" << std::endl;
    dump_layer(Layer::User);

    // 导出 System 层
    std::cout << "\n[System 层]" << std::endl;
    dump_layer(Layer::System);
}

void dump_layer(Layer layer) {
    // 由于 ConfigStore 没有遍历 API，这里需要通过 QSettings 直接读取
    std::string path = get_layer_path(layer);
    QSettings settings(QString::fromStdString(path), QSettings::IniFormat);

    dump_group(settings, "", 0);
}

void dump_group(QSettings& settings, const QString& group, int indent) {
    if (!group.isEmpty()) {
        settings.beginGroup(group);
    }

    QString prefix = QString(indent, ' ');

    // 输出所有键值对
    for (const auto& key : settings.childKeys()) {
        QVariant value = settings.value(key);
        std::cout << prefix.toStdString()
                  << key.toStdString()
                  << " = "
                  << value.toString().toStdString()
                  << " ("
                  << value.typeName().toStdString()
                  << ")"
                  << std::endl;
    }

    // 递归输出子组
    for (const auto& child : settings.childGroups()) {
        std::cout << prefix.toStdString() << "[" << child.toStdString() << "]" << std::endl;
        dump_group(settings, child, indent + 2);
    }

    if (!group.isEmpty()) {
        settings.endGroup();
    }
}
```

#### 技巧 2：验证类型转换

```cpp
void test_type_conversion(const KeyView& kv) {
    std::cout << "\n类型转换测试: " << kv.group << "." << kv.key << std::endl;

    // 尝试不同类型的读取
    auto as_string = ConfigStore::instance().query<std::string>(kv, "");
    auto as_int = ConfigStore::instance().query<int>(kv, 0);
    auto as_double = ConfigStore::instance().query<double>(kv, 0.0);
    auto as_bool = ConfigStore::instance().query<bool>(kv, false);

    std::cout << "  as string: \"" << as_string << "\"" << std::endl;
    std::cout << "  as int: " << as_int << std::endl;
    std::cout << "  as double: " << as_double << std::endl;
    std::cout << "  as bool: " << (as_bool ? "true" : "false") << std::endl;
}
```

#### 技巧 3：Watcher 性能分析

```cpp
class PerformanceWatcher {
public:
    PerformanceWatcher(const std::string& pattern)
        : pattern_(pattern)
        , call_count_(0)
        , total_time_(0)
        , max_time_(0)
    {}

    WatcherHandle install() {
        return ConfigStore::instance().watch(
            pattern_,
            [this](const Key& k, auto old, auto new_v, Layer layer) {
                auto start = std::chrono::high_resolution_clock::now();

                // 实际回调逻辑
                this->actual_callback(k, old, new_v, layer);

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                call_count_++;
                total_time_ += duration.count();
                max_time_ = std::max(max_time_, duration.count());
            }
        );
    }

    void print_stats() {
        std::cout << "\nWatcher 性能统计: " << pattern_ << std::endl;
        std::cout << "  调用次数: " << call_count_ << std::endl;
        if (call_count_ > 0) {
            std::cout << "  平均耗时: " << (total_time_ / call_count_) << " μs" << std::endl;
            std::cout << "  最大耗时: " << max_time_ << " μs" << std::endl;
            std::cout << "  总耗时: " << total_time_ << " μs" << std::endl;
        }
    }

private:
    void actual_callback(const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
        // 实际的处理逻辑
        std::cout << "Config changed: " << k.full_key << std::endl;
    }

    std::string pattern_;
    size_t call_count_;
    uint64_t total_time_;
    uint64_t max_time_;
};

// 使用
auto watcher = std::make_unique<PerformanceWatcher>("app.*");
auto handle = watcher->install();

// ... 运行一段时间 ...

watcher->print_stats();
```

---

## 平台特定问题

### Windows

#### 问题 1：注册表路径限制

Windows 版本使用注册表存储配置，路径长度有限制。

```cpp
// 问题：深层嵌套的键名可能超过注册表路径限制
ConfigStore::instance().set(
    KeyView{.group = "a.very.long.group.name.that.exceeds.registry.limit", .key = "key"},
    "value"
);  // 可能失败

// 解决方案：使用扁平化的键名
ConfigStore::instance().set(
    KeyView{.group = "app_short", .key = "long_group_key"},
    "value"
);
```

#### 问题 2：注册表权限

某些注册表路径需要管理员权限。

```cpp
#include <windows.h>

// 检查是否有写入权限
bool check_registry_access() {
    HKEY hKey;
    LPCSTR subKey = "Software\\CFDesktop";

    // 尝试打开注册表键
    LONG result = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,  // System 层使用 HKLM
        subKey,
        0,
        KEY_WRITE,
        &hKey
    );

    if (result == ERROR_ACCESS_DENIED) {
        std::cerr << "错误: 需要管理员权限写入 System 层配置" << std::endl;
        std::cerr << "建议: 使用 User 层或以管理员身份运行" << std::endl;
        return false;
    }

    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }

    return false;
}
```

#### 问题 3：INI 格式与注册表格式差异

```cpp
// 如果想在 Windows 上使用 INI 文件而不是注册表
#include <QCoreApplication>
#include <windows.h>

class WindowsIniPathProvider : public IConfigStorePathProvider {
public:
    QString system_path() const override {
        // 返回 INI 文件路径而不是注册表路径
        return "C:/ProgramData/CFDesktop/system.ini";
    }

    QString user_dir() const override {
        char* appdata = nullptr;
        size_t len = 0;
        _dupenv_s(&appdata, &len, "APPDATA");
        QString path = QString::fromStdString(std::string(appdata)) + "/CFDesktop";
        free(appdata);
        return path;
    }

    QString user_filename() const override {
        return "user.ini";
    }

    QString app_dir() const override {
        return QCoreApplication::applicationDirPath() + "/config";
    }

    QString app_filename() const override {
        return "app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        return true;
    }
};

// QSettings 会根据路径扩展名自动选择格式
// .ini -> INI 格式
// 无扩展名 -> 注册表（Windows）
```

### Linux

#### 问题 1：INI 文件权限

```cpp
// 确保 INI 文件目录存在且可写
void ensure_config_directory(const std::string& path) {
    QFileInfo file(QString::fromStdString(path));
    QDir dir = file.absoluteDir();

    if (!dir.exists()) {
        std::cout << "创建配置目录: " << dir.absolutePath().toStdString() << std::endl;
        if (!dir.mkpath(".")) {
            std::cerr << "错误: 无法创建配置目录" << std::endl;
            std::cerr << "请检查父目录权限" << std::endl;
        }
    }

    // 检查写入权限
    QFileInfo dirInfo(dir.absolutePath());
    if (!dirInfo.isWritable()) {
        std::cerr << "错误: 配置目录不可写" << std::endl;
        std::cerr << "路径: " << dir.absolutePath().toStdString() << std::endl;
    }
}

// 使用
ensure_config_directory("~/.config/cfdesktop/user.ini");
```

#### 问题 2：System 层路径需要 root 权限

```bash
# System 层配置通常位于 /etc，需要 root 权限写入
sudo touch /etc/cfdesktop/system.ini
sudo chown $USER:$USER /etc/cfdesktop/system.ini
# 或者将应用配置放在用户目录
```

#### 问题 3：INI 文件编码

```cpp
// 确保使用 UTF-8 编码
QSettings settings("/path/to/config.ini", QSettings::IniFormat);
settings.setIniCodec("UTF-8");  // 设置编码

// 写入包含非 ASCII 字符的配置
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "title"},
    u8"应用程序标题",  // UTF-8 字符串字面量
    Layer::User
);
```

### macOS

#### 问题 1：plist vs INI

macOS 原生使用 plist 格式，但 ConfigStore 统一使用 INI 格式。

```cpp
// 如果需要与系统 plist 配置交互，可以手动转换
void import_from_plist(const std::string& plist_path) {
    QSettings plist(QString::fromStdString(plist_path), QSettings::NativeFormat);

    // 读取所有键值
    for (const auto& key : plist.allKeys()) {
        QVariant value = plist.value(key);

        // 转换键名格式
        std::string full_key = key.toStdString();
        std::replace(full_key.begin(), full_key.end(), '/', '.');

        // 分离 group 和 key
        size_t pos = full_key.rfind('.');
        std::string group = (pos != std::string::npos) ? full_key.substr(0, pos) : "";
        std::string key_name = (pos != std::string::npos) ? full_key.substr(pos + 1) : full_key;

        // 写入 ConfigStore
        KeyView kv{.group = group, .key = key_name};
        ConfigStore::instance().set(kv, value.toString().toStdString(), Layer::User);
    }
}
```

#### 问题 2：macOS 特殊目录

```cpp
#include <QCoreApplication>
#include <cstdlib>

class MacOSPathProvider : public IConfigStorePathProvider {
public:
    QString system_path() const override {
        // macOS 系统配置
        return "/Library/Preferences/com.cfdesktop.system.ini";
    }

    QString user_dir() const override {
        // 用户配置目录
        char* home = getenv("HOME");
        return QString::fromStdString(std::string(home)) + "/Library/Preferences";
    }

    QString user_filename() const override {
        return "com.cfdesktop.user.ini";
    }

    QString app_dir() const override {
        // 应用内配置（在 Bundle 内）
        return QCoreApplication::applicationDirPath() + "/../Resources/config";
    }

    QString app_filename() const override {
        return "app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        return true;
    }
};
```

---

## 性能问题

### 内存占用过高

#### 原因分析

1. **缓存无限增长**：大量配置项被缓存
2. **Watcher 泄漏**：未正确取消的 Watcher 占用内存
3. **大量临时配置**：Temp 层积累过多数据

#### 解决方案

```cpp
#include <iostream>
#include <fstream>

// 方案 1：定期清理 Temp 层
void cleanup_temp_layer() {
    std::cout << "清理 Temp 层..." << std::endl;
    ConfigStore::instance().clear_layer(Layer::Temp);
}

// 方案 2：监控内存使用
void monitor_memory_usage() {
    // 使用系统 API 获取内存使用情况
#ifdef __linux__
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("VmRSS") != std::string::npos) {
            std::cout << line << std::endl;
        }
    }
#endif
}

// 方案 3：限制缓存大小（需要修改 ConfigStore 实现）
// 在 ConfigStoreImpl 中添加 LRU 缓存
```

### 读写缓慢

#### 诊断

```cpp
#include <chrono>

void benchmark_config_operations() {
    const int iterations = 1000;

    // 写入测试
    auto write_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        ConfigStore::instance().set(
            KeyView{.group = "bench", .key = "key_" + std::to_string(i)},
            i,
            Layer::Temp
        );
    }
    auto write_end = std::chrono::high_resolution_clock::now();
    auto write_duration = std::chrono::duration_cast<std::chrono::microseconds>(write_end - write_start);

    std::cout << "写入 " << iterations << " 次: "
              << write_duration.count() << " μs"
              << " (平均 " << (write_duration.count() / iterations) << " μs/次)"
              << std::endl;

    // 读取测试
    auto read_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        ConfigStore::instance().query<int>(
            KeyView{.group = "bench", .key = "key_" + std::to_string(i)},
            0
        );
    }
    auto read_end = std::chrono::high_resolution_clock::now();
    auto read_duration = std::chrono::duration_cast<std::chrono::microseconds>(read_end - read_start);

    std::cout << "读取 " << iterations << " 次: "
              << read_duration.count() << " μs"
              << " (平均 " << (read_duration.count() / iterations) << " μs/次)"
              << std::endl;

    // 清理
    ConfigStore::instance().clear_layer(Layer::Temp);
}
```

#### 优化建议

1. **减少 sync() 调用频率**
```cpp
// 不推荐：每次写入都同步
for (int i = 0; i < 1000; ++i) {
    ConfigStore::instance().set(KeyView{.group = "data", .key = std::to_string(i)}, i);
    ConfigStore::instance().sync(SyncMethod::Sync);  // 频繁 I/O
}

// 推荐：批量写入后同步
for (int i = 0; i < 1000; ++i) {
    ConfigStore::instance().set(KeyView{.group = "data", .key = std::to_string(i)}, i, Layer::App, NotifyPolicy::Manual);
}
ConfigStore::instance().notify();
ConfigStore::instance().sync(SyncMethod::Async);  // 异步同步
```

2. **使用 Temp 层存储频繁变化的配置**
```cpp
// 频繁更新的计数器，使用 Temp 层避免频繁 I/O
for (int i = 0; i < 10000; ++i) {
    ConfigStore::instance().set(
        KeyView{.group = "counter", .key = "value"},
        i,
        Layer::Temp  // 不写入磁盘
    );
}

// 需要持久化时再写入
ConfigStore::instance().set(
    KeyView{.group = "counter", .key = "final_value"},
    10000,
    Layer::App
);
ConfigStore::instance().sync(SyncMethod::Sync);
```

### Watcher 性能问题

#### 问题：Watcher 回调执行时间过长

```cpp
// 问题代码
ConfigStore::instance().watch(
    "app.*",
    [](const Key& k, auto old, auto new_v, Layer layer) {
        // 耗时操作
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // 这会阻塞所有后续的配置操作
    }
);
```

#### 解决方案

```cpp
// 方案 1：使用异步处理
ConfigStore::instance().watch(
    "app.*",
    [](const Key& k, auto old, auto new_v, Layer layer) {
        // 将处理任务放入队列，异步执行
        std::thread([k]() {
            // 在另一个线程中处理
            handle_config_change(k);
        }).detach();
    }
);

// 方案 2：使用 Debounce（防抖）
class DebouncedWatcher {
    std::string pattern_;
    std::chrono::milliseconds delay_;
    std::thread thread_;
    std::queue<Key> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = true;

public:
    DebouncedWatcher(const std::string& pattern, std::chrono::milliseconds delay)
        : pattern_(pattern), delay_(delay)
    {
        thread_ = std::thread([this]() {
            while (running_) {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this]() { return !queue_.empty() || !running_; });

                if (!running_) break;

                if (!queue_.empty()) {
                    auto key = queue_.front();
                    queue_.pop();

                    // 等待延迟
                    lock.unlock();
                    std::this_thread::sleep_for(delay_);
                    lock.lock();

                    // 检查是否有新元素
                    if (queue_.empty()) {
                        // 没有新元素，执行处理
                        handle_change(key);
                    }
                }
            }
        });
    }

    ~DebouncedWatcher() {
        running_ = false;
        cv_.notify_all();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    WatcherHandle install() {
        return ConfigStore::instance().watch(
            pattern_,
            [this](const Key& k, auto old, auto new_v, Layer layer) {
                std::lock_guard lock(mutex_);
                queue_.push(k);
                cv_.notify_one();
            }
        );
    }

private:
    void handle_change(const Key& k) {
        std::cout << "处理配置变更: " << k.full_key << std::endl;
        // 实际处理逻辑
    }
};

// 使用
auto debounced_watcher = std::make_unique<DebouncedWatcher>("app.*", std::chrono::milliseconds(100));
debounced_watcher->install();
```

---

## 获取帮助

如果以上方法无法解决问题：

1. 参考 [快速入门](./01-quick-start.md)
2. 搜索或提交 Issue 到项目仓库

---

## 相关文档

- [快速入门](./01-quick-start.md)
- [最佳实践](./02-best-practices.md)
- [架构详解](./04-architecture.md)
