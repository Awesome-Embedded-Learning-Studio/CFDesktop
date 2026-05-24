---
title: ConfigStore 常见问题与故障排查
description: 本文档列出了 ConfigStore 使用中的常见问题、故障排查方法和调试技巧。
---

# ConfigStore 常见问题与故障排查

本文档列出了 ConfigStore 使用中的常见问题、故障排查方法和调试技巧。

## 目录

- [常见问题](#常见问题)
- [平台特定问题](#平台特定问题)

---

## 常见问题

### Q1: 配置修改后没有生效？

修改后必须调用 `sync()` 才能持久化到磁盘：

```cpp
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark");
ConfigStore::instance().sync(SyncMethod::Sync);  // 同步写入

// 或使用异步同步（推荐）
ConfigStore::instance().sync(SyncMethod::Async);  // 不阻塞调用方
```

如果仍不生效，检查层级优先级——查询时返回的是高优先级（Temp > App > User > System）的值：

```cpp
// Temp 层优先级最高，会覆盖其他层的值
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "dark", Layer::Temp);
ConfigStore::instance().set(KeyView{.group = "app", .key = "theme"}, "light", Layer::System);
// query 返回 "dark"

// 解决方案：清除高优先级配置，或查询指定层级
ConfigStore::instance().clear_layer(Layer::Temp);
auto system_theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "theme"}, Layer::System, ""
);
```

---

### Q2: 如何在不同环境使用不同配置？

使用层级分离或自定义路径提供者：

```cpp
// 方式 1：编译期通过宏选择层级
#ifdef DEBUG
    ConfigStore::instance().set(
        KeyView{.group = "api", .key = "endpoint"},
        "https://dev.api.example.com", Layer::System
    );
#else
    ConfigStore::instance().set(
        KeyView{.group = "api", .key = "endpoint"},
        "https://api.example.com", Layer::System
    );
#endif

// 方式 2：自定义 IConfigStorePathProvider
class EnvironmentPathProvider : public IConfigStorePathProvider {
public:
    QString system_path() const override {
        return QString::fromStdString("/etc/myapp/system.ini");
    }
    QString user_dir() const override {
        return QString::fromStdString(std::string(getenv("HOME")) + "/.config/myapp");
    }
    QString user_filename() const override { return "user.ini"; }
    QString app_dir() const override {
        return QCoreApplication::applicationDirPath() + "/config";
    }
    QString app_filename() const override { return "app.ini"; }
    bool is_layer_enabled(int) const override { return true; }
};
ConfigStore::instance().initialize(std::make_shared<EnvironmentPathProvider>());
```

---

### Q3: Watcher 回调没有被触发？

最常见原因是键模式不匹配。使用通配符确保匹配目标键：

```cpp
// 精确匹配 — 只匹配 "app.theme.name"
ConfigStore::instance().watch("app.theme.name", callback);

// 通配符 — 匹配所有 theme 相关键
ConfigStore::instance().watch("*.theme.*", callback);

// 通配符 — 匹配 app 下所有键
ConfigStore::instance().watch("app.*", callback);
```

如果使用了 `NotifyPolicy::Manual`，需手动调用 `notify()` 才会触发回调：

```cpp
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"}, "dark",
    Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().notify();  // 手动触发所有 Manual Watcher
```

确保 `WatcherHandle` 在需要监听的整个生命周期内保持有效：

```cpp
class AppManager {
    WatcherHandle theme_watcher_;
public:
    void init() {
        theme_watcher_ = ConfigStore::instance().watch(
            "app.theme.*", [this](const Key& k, auto old, auto new_v, Layer layer) {
                onThemeChanged(k);
            }
        );
    }
    ~AppManager() { ConfigStore::instance().unwatch(theme_watcher_); }
};
```

---

### Q4: 类型转换失败怎么办？

存储和读取的类型必须与 QVariant 兼容。如果以 `std::string` 存储，读取为 `int` 会失败：

```cpp
// 正确做法：存储时使用目标类型
ConfigStore::instance().set(KeyView{.group = "test", .key = "value"}, 123);  // 存为 int
int value = ConfigStore::instance().query<int>(
    KeyView{.group = "test", .key = "value"}, 0
);  // 返回 123
```

如果类型不确定，先读取为字符串再手动转换：

```cpp
auto str_value = ConfigStore::instance().query<std::string>(
    KeyView{.group = "test", .key = "value"}, ""
);
if (!str_value.empty()) {
    try {
        int int_value = std::stoi(str_value);
    } catch (const std::exception& e) {
        // 处理转换错误
    }
}
```

---

### Q5: 如何迁移旧的配置文件？

使用 `QSettings` 读取旧配置，通过 ConfigStore API 写入新格式：

```cpp
void migrate_old_config(const std::string& old_file_path) {
    QSettings old_settings(
        QString::fromStdString(old_file_path), QSettings::IniFormat
    );
    for (const auto& group : old_settings.childGroups()) {
        old_settings.beginGroup(group);
        for (const auto& key : old_settings.childKeys()) {
            QVariant value = old_settings.value(key);
            KeyView kv{.group = group.toStdString(), .key = key.toStdString()};
            if (value.type() == QVariant::Int)
                ConfigStore::instance().set(kv, value.toInt(), Layer::User);
            else if (value.type() == QVariant::String)
                ConfigStore::instance().set(kv, value.toString().toStdString(), Layer::User);
            else if (value.type() == QVariant::Bool)
                ConfigStore::instance().set(kv, value.toBool(), Layer::User);
        }
        old_settings.endGroup();
    }
    ConfigStore::instance().sync(SyncMethod::Sync);
}
```

---

### Q6: Temp 层的数据什么时候会丢失？

Temp 层为纯内存存储，在以下场景会丢失：程序退出、调用 `reload()`、调用 `clear_layer(Layer::Temp)`。`sync()` 不会清空 Temp 层。

适用场景：会话临时数据、调试配置、运行时缓存。需要持久化的数据请使用 User 或 App 层。

```cpp
// 会话数据 — 使用 Temp
ConfigStore::instance().set(
    KeyView{.group = "session", .key = "id"}, generate_session_id(), Layer::Temp
);

// 用户偏好 — 使用 User
ConfigStore::instance().set(
    KeyView{.group = "user", .key = "preference"}, "dark", Layer::User
);
```

---

### Q7: 如何批量修改配置而不触发多次 Watcher？

使用 `NotifyPolicy::Manual`，批量修改完成后一次性触发：

```cpp
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "a"}, 1, Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "b"}, 2, Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "c"}, 3, Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().notify();  // 一次性触发所有 Watcher
```

也可封装为 RAII 事务：

```cpp
struct ConfigTransaction {
    ~ConfigTransaction() {
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
    ConfigTransaction tx;
    tx.set(KeyView{.group = "ui", .key = "width"}, 800);
    tx.set(KeyView{.group = "ui", .key = "height"}, 600);
    tx.set(KeyView{.group = "ui", .key = "theme"}, "dark");
} // 析构时自动 notify + sync
```

---

### Q8: 配置文件位置在哪里？

| 平台 | 层级 | 路径 |
|------|------|------|
| **Linux** | System | `<应用目录>/config/system.ini` |
| | User | `~/.config/cfdesktop/user.ini` |
| | App | `<应用目录>/config/app.ini` |
| **Windows** | System | `HKEY_LOCAL_MACHINE\Software\CFDesktop` |
| | User | `HKEY_CURRENT_USER\Software\CFDesktop` |
| | App | `<应用目录>\config\app.ini` |
| **macOS** | System | `/Library/Preferences/com.cfdesktop.system.plist` |
| | User | `~/Library/Preferences/com.cfdesktop.user.plist` |
| | App | `<应用目录>/config/app.ini` |

KeyView 中 group 和 key 只允许字母、数字、下划线，包含空格等非法字符会导致 `set()` 返回 false：

```cpp
KeyView invalid_kv{.group = "app theme", .key = "name"};   // 失败
KeyView valid_kv{.group = "app_theme", .key = "name"};     // 成功
```

---

### Q9: KeyView 合法字符规则是什么？

`group` 和 `key` 字段仅允许字母、数字、下划线和点号。包含空格或其他特殊字符会导致键转换失败，`set()` 返回 `false`。

---

### Q10: 如何减少 sync() 的性能开销？

避免每次写入后立即同步。批量写入后调用一次异步同步：

```cpp
// 不推荐：频繁同步 I/O
for (int i = 0; i < 1000; ++i) {
    ConfigStore::instance().set(KeyView{.group = "data", .key = std::to_string(i)}, i);
    ConfigStore::instance().sync(SyncMethod::Sync);
}

// 推荐：批量写入 + 一次异步同步
for (int i = 0; i < 1000; ++i) {
    ConfigStore::instance().set(
        KeyView{.group = "data", .key = std::to_string(i)}, i,
        Layer::App, NotifyPolicy::Manual
    );
}
ConfigStore::instance().notify();
ConfigStore::instance().sync(SyncMethod::Async);
```

频繁变化的临时数据使用 Temp 层（不写磁盘），仅在需要持久化时写入 App/User 层。

---

## 平台特定问题

| 平台 | 问题 | 原因 | 解决方案 |
|------|------|------|----------|
| Windows | 写入 System 层失败 | HKLM 需要管理员权限 | 使用 User 层或以管理员身份运行 |
| Windows | 深层嵌套键名写入失败 | 注册表路径长度限制 | 使用扁平化的 group/key 命名 |
| Windows | 想使用 INI 而非注册表 | QSettings 默认使用注册表 | 实现 `IConfigStorePathProvider`，返回 `.ini` 路径 |
| Linux | 配置目录不存在 | 首次运行未创建目录 | 应用启动时 `QDir::mkpath()` 确保目录存在 |
| Linux | INI 文件中文乱码 | 文件编码非 UTF-8 | `QSettings::setIniCodec("UTF-8")` |
| Linux | System 层写入失败 | CFDesktop 使用应用自管理目录 | 确保应用目录有写权限，无需 root |
| macOS | 想读取系统 plist | ConfigStore 统一使用 INI 格式 | 用 `QSettings(path, NativeFormat)` 读取后手动导入 |

---

## 相关文档

- [快速入门](./01-quick-start.md)
- [最佳实践](./02-best-practices.md)
- [架构详解](./04-architecture.md)
