---
title: 快速入门
description: 本文档将引导您从零开始使用 ConfigStore 配置管理中心。
---

# 快速入门

本文档将引导您从零开始使用 ConfigStore 配置管理中心。

## 安装与初始化

### 包含头文件

```cpp
#include "cfconfig.hpp"                   // 主接口
#include "cfconfig/cfconfig_path_provider.h" // 自定义路径提供器（可选）
#include <QString>                         // Qt 字符串类型

using namespace cf::config;
```text

### 获取单例实例

ConfigStore 使用单例模式，首次访问时自动初始化：

```cpp
// 获取单例实例
auto& config = ConfigStore::instance();
```text

### 自定义路径配置

如需自定义配置文件路径，可以实现 `IConfigStorePathProvider` 接口：

```cpp
class CustomPathProvider : public IConfigStorePathProvider {
public:
    QString system_path() const override {
        return "{app_dir}/system.ini";  // CFDesktop 自管理目录内
    }

    QString user_dir() const override {
        QString home = getenv("HOME") ? getenv("HOME") : "/tmp";
        return home + "/.config/myapp";
    }

    QString user_filename() const override {
        return "user.ini";
    }

    QString app_dir() const override {
        return "/var/lib/myapp/config";
    }

    QString app_filename() const override {
        return "app.ini";
    }

    bool is_layer_enabled(int layer_index) const override {
        return true; // 启用所有层
    }
};

// 在首次使用前初始化
auto provider = std::make_shared<CustomPathProvider>();
ConfigStore::instance().initialize(provider);
```text

## 基础操作

### 读取配置

#### 使用默认值

最常用的方式是提供默认值，当配置不存在时返回默认值：

```cpp
// 读取整数配置
int window_width = ConfigStore::instance().query<int>(
    KeyView{.group = "ui.window", .key = "width"},  // 键名
    800                                              // 默认值
);

// 读取字符串配置
std::string theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "theme"},
    "default"
);

// 读取浮点数配置
double scale = ConfigStore::instance().query<double>(
    KeyView{.group = "display", .key = "scale"},
    1.0
);

// 读取布尔配置
bool auto_save = ConfigStore::instance().query<bool>(
    KeyView{.group = "app", .key = "auto_save"},
    true
);
```text

#### Optional 查询

使用 `std::optional` 可以判断配置是否存在：

```cpp
// 查询可能不存在的配置
auto port = ConfigStore::instance().query<int>(
    KeyView{.group = "network", .key = "port"}
);

if (port.has_value()) {
    std::cout << "端口: " << port.value() << std::endl;
} else {
    std::cout << "端口未配置，使用默认值 8080" << std::endl;
}

// 或者使用结构化绑定（C++17）
if (auto value = ConfigStore::instance().query<int>(
        KeyView{.group = "network", .key = "port"})) {
    std::cout << "端口: " << *value << std::endl;
}
```text

#### 指定层查询

直接查询特定层的配置值，不经过优先级合并：

```cpp
// 只查询 User 层的配置
auto user_theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "theme"},
    Layer::User  // 指定层
);

if (user_theme.has_value()) {
    std::cout << "用户设置的主题: " << user_theme.value() << std::endl;
}
```text

### 写入配置

#### 基本写入

默认写入 App 层：

```cpp
// 写入整数
ConfigStore::instance().set(
    KeyView{.group = "ui.window", .key = "width"},
    1024
);

// 写入字符串
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark")
);

// 写入浮点数
ConfigStore::instance().set(
    KeyView{.group = "display", .key = "scale"},
    1.5
);

// 写入布尔值
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "auto_save"},
    false
);
```text

#### 选择目标层

将配置写入指定层：

```cpp
// 写入用户配置
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::User  // 写入 User 层
);

// 写入临时配置（不持久化）
ConfigStore::instance().set(
    KeyView{.group = "session", .key = "id"},
    std::string("abc123"),
    Layer::Temp  // 写入 Temp 层，重启后丢失
);
```text

#### 通知策略

控制变更通知的时机：

```cpp
// 立即通知（默认）
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::App,
    NotifyPolicy::Immediate  // 立即触发 Watcher
);

// 手动通知（批量操作）
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "a"},
    1,
    Layer::App,
    NotifyPolicy::Manual  // 不立即触发
);
ConfigStore::instance().set(
    KeyView{.group = "batch", .key = "b"},
    2,
    Layer::App,
    NotifyPolicy::Manual
);
// 批量操作完成后，一次性触发所有 Watcher
ConfigStore::instance().notify();
```text

### 键管理

#### 注册键

注册键可以显式声明配置项，并设置初始值：

```cpp
// 定义键
Key theme_key{
    .full_key = "app.theme.name",
    .full_description = "应用程序主题名称 (default/dark/light)"
};

// 注册键并设置初始值
auto result = ConfigStore::instance().register_key(
    theme_key,
    std::string("default"),  // 初始值
    Layer::App,
    NotifyPolicy::Immediate
);

if (result == RegisterResult::KeyRegisteredSuccess) {
    std::cout << "键注册成功" << std::endl;
} else {
    std::cout << "键已存在" << std::endl;
}
```text

#### 注销键

删除不再使用的配置项：

```cpp
Key theme_key{
    .full_key = "app.theme.name",
    .full_description = "应用程序主题名称"
};

// 注销键
auto result = ConfigStore::instance().unregister_key(
    theme_key,
    Layer::App
);

if (result == UnRegisterResult::KeyUnRegisteredSuccess) {
    std::cout << "键注销成功" << std::endl;
}
```text

#### 检查键存在

```cpp
// 检查键是否存在（递归查询所有层）
bool exists = ConfigStore::instance().has_key(
    KeyView{.group = "app", .key = "theme"}
);

// 检查特定层是否存在该键
bool in_user = ConfigStore::instance().has_key(
    KeyView{.group = "app", .key = "theme"},
    Layer::User
);
```text

## 监听配置变更

### 基本监听

```cpp
// 监听单个键的变更
auto handle = ConfigStore::instance().watch(
    "app.theme.name",
    [](const Key& k, const std::any* old_value, const std::any* new_value, Layer layer) {
        std::cout << "配置变更: " << k.full_key << std::endl;

        if (old_value && new_value) {
            auto old = std::any_cast<std::string>(*old_value);
            auto new_v = std::any_cast<std::string>(*new_value);
            std::cout << "  " << old << " -> " << new_v << std::endl;
        }
    }
);
```text

### 通配符监听

使用通配符 `*` 监听多个键：

```cpp
// 监听所有 app.* 的变更
auto handle = ConfigStore::instance().watch(
    "app.*",
    [](const Key& k, const std::any* old_value, const std::any* new_value, Layer layer) {
        std::cout << "应用配置变更: " << k.full_key << std::endl;
    }
);

// 监听所有 theme 相关的配置
auto theme_watcher = ConfigStore::instance().watch(
    "*.theme.*",
    [](const Key& k, const std::any* old_value, const std::any* new_value, Layer layer) {
        std::cout << "主题配置变更: " << k.full_key << std::endl;
    }
);
```text

### 取消监听

```cpp
// 保存监听句柄
WatcherHandle handle = ConfigStore::instance().watch("app.*", callback);

// 取消监听
ConfigStore::instance().unwatch(handle);
```text

### 手动通知模式

```cpp
// 使用 Manual 策略添加 Watcher
auto handle = ConfigStore::instance().watch(
    "batch.*",
    callback,
    NotifyPolicy::Manual  // 手动通知模式
);

// 执行批量修改
ConfigStore::instance().set(KeyView{.group = "batch", .key = "a"}, 1,
                            Layer::App, NotifyPolicy::Manual);
ConfigStore::instance().set(KeyView{.group = "batch", .key = "b"}, 2,
                            Layer::App, NotifyPolicy::Manual);

// 手动触发通知
ConfigStore::instance().notify();
```text

## 持久化操作

### 同步到磁盘

```cpp
// 异步同步（默认，不阻塞调用方）
ConfigStore::instance().sync(SyncMethod::Async);

// 同步同步（阻塞直到写入完成）
ConfigStore::instance().sync(SyncMethod::Sync);
```text

### 重新加载配置

```cpp
// 从磁盘重新加载所有配置
// 注意：这会清空 Temp 层的所有临时配置
ConfigStore::instance().reload();
```text

### 查看待写入变更

```cpp
// 获取待同步的变更数量
size_t pending = ConfigStore::instance().pending_changes();
std::cout << "待同步变更数: " << pending << std::endl;
```text

## 完整示例：应用程序配置管理

以下是一个完整的应用程序配置管理示例：

```cpp
#include "cfconfig.hpp"
#include <iostream>

using namespace cf::config;

class AppConfig {
public:
    // 初始化配置
    void init() {
        auto& config = ConfigStore::instance();

        // 注册默认配置
        config.register_key(
            Key{.full_key = "app.theme", .full_description = "应用主题"},
            std::string("default"),
            Layer::App
        );

        config.register_key(
            Key{.full_key = "app.language", .full_description = "应用语言"},
            std::string("zh_CN"),
            Layer::App
        );

        config.register_key(
            Key{.full_key = "ui.window.width", .full_description = "窗口宽度"},
            1024,
            Layer::App
        );

        config.register_key(
            Key{.full_key = "ui.window.height", .full_description = "窗口高度"},
            768,
            Layer::App
        );

        // 监听配置变更
        watch_changes();
    }

    // 加载配置
    void load() {
        auto& config = ConfigStore::instance();

        theme_ = config.query<std::string>(
            KeyView{.group = "app", .key = "theme"}, "default");
        language_ = config.query<std::string>(
            KeyView{.group = "app", .key = "language"}, "zh_CN");
        window_width_ = config.query<int>(
            KeyView{.group = "ui.window", .key = "width"}, 1024);
        window_height_ = config.query<int>(
            KeyView{.group = "ui.window", .key = "height"}, 768);

        print_config();
    }

    // 保存配置
    void save() {
        auto& config = ConfigStore::instance();

        config.set(KeyView{.group = "app", .key = "theme"}, theme_);
        config.set(KeyView{.group = "app", .key = "language"}, language_);
        config.set(KeyView{.group = "ui.window", .key = "width"}, window_width_);
        config.set(KeyView{.group = "ui.window", .key = "height"}, window_height_);

        // 同步到磁盘
        config.sync(SyncMethod::Sync);
    }

    // 打印当前配置
    void print_config() {
        std::cout << "当前配置:" << std::endl;
        std::cout << "  主题: " << theme_ << std::endl;
        std::cout << "  语言: " << language_ << std::endl;
        std::cout << "  窗口: " << window_width_ << "x" << window_height_ << std::endl;
    }

    // 设置主题
    void set_theme(const std::string& theme) {
        auto& config = ConfigStore::instance();
        config.set(KeyView{.group = "app", .key = "theme"}, theme);
        theme_ = theme;
    }

private:
    void watch_changes() {
        auto& config = ConfigStore::instance();

        // 监听主题变更
        theme_watcher_ = config.watch(
            "app.theme",
            [this](const Key& k, const std::any* old_value, const std::any* new_value, Layer layer) {
                if (new_value) {
                    theme_ = std::any_cast<std::string>(*new_value);
                    std::cout << "主题已变更为: " << theme_ << std::endl;
                    apply_theme();
                }
            }
        );
    }

    void apply_theme() {
        std::cout << "应用主题: " << theme_ << std::endl;
        // 实际应用主题的逻辑...
    }

    std::string theme_;
    std::string language_;
    int window_width_;
    int window_height_;
    WatcherHandle theme_watcher_;
};

int main() {
    AppConfig app_config;

    // 初始化配置
    app_config.init();

    // 加载配置
    app_config.load();

    // 修改配置
    app_config.set_theme("dark");

    return 0;
}
```text

## 下一步

- 查看 [最佳实践](./02-best-practices.md) 学习推荐的使用模式
- 浏览 [常见问题](./03-faq.md) 解答使用中的疑问
- 阅读 [架构详解](./04-architecture.md) 深入理解内部实现
