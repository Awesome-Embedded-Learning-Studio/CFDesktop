# ThemeManager - 主题管理器

`ThemeManager` 是整个 UI 主题系统的入口，负责注册主题工厂、管理主题实例的生命周期，以及向已注册的 widget 分发主题变更事件。采用单例模式是因为整个应用程序只需要一个全局的主题管理点，多处实例化反而会造成状态同步的混乱。

## 获取单例

获取管理器实例的方式很直接，返回的是引用而不是指针——这样能避免空指针检查，同时也符合"全局唯一实例"的语义：

```cpp
#include "ui/core/theme_manager.h"

auto& manager = cf::ui::core::ThemeManager::instance();
```

单例的初始化是线程安全的（C++11 魔法静态变量保证），不用担心多线程竞态。但我们只在 UI 线程里使用它，Qt 的信号机制本身就要求如此。

## 注册主题工厂

主题不是直接创建的，而是通过工厂模式。在使用前需要先注册工厂，名字是后续获取主题的唯一标识：

```cpp
// 注册一个内置主题
manager.insert_one("default", []() {
    return std::make_unique<DefaultThemeFactory>();
});

// 注册自定义主题
manager.insert_one("dark_blue", []() {
    return std::make_unique<DarkBlueFactory>();
});
```

`insert_one` 返回 `bool`，如果名字已经存在会返回 `false`。这个设计挺实用，可以用来检测插件冲突——同一主题被两个插件注册时，至少有一个会失败。

工厂函数是延迟调用的，注册时只是把工厂创建器存起来，真正创建主题实例是在第一次访问时。这意味着你可以提前注册所有可用主题，而不用担心那些可能永远不会被用到的主题占用资源。

## 切换主题

切换主题只需要调用 `setThemeTo`，管理器会通知所有已注册的 widget：

```cpp
// 切换到 dark 主题
manager.setThemeTo("dark_blue");

// 切换时可以选择不广播（特殊场景下使用）
manager.setThemeTo("default", false);
```

默认情况下，切换主题会广播 `themeChanged` 信号到所有通过 `install_widget` 注册的 widget。`doBroadcast` 参数设为 `false` 会跳过广播，这主要用于批量操作时避免重复通知——比如你要连续切换三个主题，中间的两次就不需要广播。

⚠️ 传入的名字必须已经通过 `insert_one` 注册过，否则行为是未定义的。我们当时考虑过抛异常，但 Qt 的信号槽机制和异常配合得不太好，所以选择了静默失败。如果你需要检测错误，建议在调用前先确认主题已注册。

## Widget 注册与通知

widget 需要主动注册才能收到主题变更通知：

```cpp
class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 注册到主题管理器
        cf::ui::core::ThemeManager::instance().install_widget(this);

        // 连接主题变更信号
        connect(&cf::ui::core::ThemeManager::instance(),
                &cf::ui::core::ThemeManager::themeChanged,
                this, &MyWidget::onThemeChanged);
    }

    ~MyWidget() {
        // 记得取消注册
        cf::ui::core::ThemeManager::instance().remove_widget(this);
    }

private slots:
    void onThemeChanged(const cf::ui::core::ICFTheme& new_theme) {
        // 应用新主题
        const auto& colors = new_theme.color_scheme();
        setStyleSheet(QString("background-color: %1;")
                          .arg(colors.background().name()));
    }
};
```

管理器持有的是 `QWidget` 的裸指针观察者，不负责生命周期。widget 析构时必须调用 `remove_widget`，否则管理器里会留下悬空指针。这个设计有点不安全，但 Qt 的对象树机制使得在 widget 析构时自动清理变得复杂，我们选择了简单粗暴的显式管理方式。

## 获取主题实例

有时候你不需要切换主题，只是想读取当前或某个主题的配置：

```cpp
// 获取当前主题名称
const std::string& current = manager.currentThemeName();

// 获取特定主题的引用（延迟创建）
const cf::ui::core::ICFTheme& theme = manager.theme("default");

// 读取颜色配置
const auto& colors = theme.color_scheme();
QColor bg = colors.background();
```

`theme()` 方法返回的是常量引用，主题实例会被缓存起来，后续访问直接从缓存取。第一次访问时才会调用工厂创建，所以第一次可能会有轻微延迟。

## 移除主题

如果一个主题不再需要，可以动态移除：

```cpp
manager.remove_one("old_theme");
```

移除操作不会影响当前正在使用的主题——即使你移除的是当前主题，管理器里仍然保留着它的实例，直到切换到另一个主题。这个行为可能是 feature 也可能是 bug，取决于你从哪个角度看，但它确实避免了一些诡妙的崩溃。

## 线程安全

管理器的内部状态（工厂注册、widget 集合、主题缓存）没有加锁保护。这是有意为之——我们假设所有操作都在 UI 线程上进行，而 Qt 的信号槽机制本身就要求如此。

如果你确实需要在其他线程里访问主题数据，可以：
1. 在后台线程里只读取主题配置（ICFTheme 本身是线程安全的）
2. 通过 `QMetaObject::invokeMethod` 把 UI 操作转发到主线程

⚠️ 不要在多线程环境下同时调用 `insert_one`、`remove_one` 或 `setThemeTo`，一定会翻车。

## 完整示例

这是一个完整的使用流程，展示了从注册到切换的完整链条：

```cpp
// 1. 定义工厂（实际项目里通常在单独文件中）
class LightThemeFactory : public cf::ui::core::ThemeFactory {
public:
    std::unique_ptr<cf::ui::core::ICFTheme> fromName(const char* name) override {
        auto theme = std::make_unique<cf::ui::core::ICFTheme>();
        theme->color_scheme_ = std::make_unique<LightColorScheme>();
        theme->radius_scale_ = std::make_unique<DefaultRadiusScale>();
        // ... 其他组件
        return theme;
    }

    std::unique_ptr<cf::ui::core::ICFTheme> fromJson(const QByteArray& json) override {
        // 解析 JSON 并创建主题
        return nullptr;
    }

    QByteArray toJson(cf::ui::core::ICFTheme* raw_theme) override {
        // 序列化主题到 JSON
        return QByteArray();
    }
};

// 2. 应用初始化时注册所有主题
void App::initThemes() {
    auto& manager = cf::ui::core::ThemeManager::instance();

    manager.insert_one("light", []() {
        return std::make_unique<LightThemeFactory>();
    });

    manager.insert_one("dark", []() {
        return std::make_unique<DarkThemeFactory>();
    });
}

// 3. Widget 连接主题变更
void MyWidget::setupThemeConnection() {
    auto& manager = cf::ui::core::ThemeManager::instance();
    manager.install_widget(this);

    connect(&manager, &cf::ui::core::ThemeManager::themeChanged,
            this, &MyWidget::applyTheme);

    // 立即应用当前主题
    applyTheme(manager.theme(manager.currentThemeName()));
}

// 4. 用户切换主题时
void SettingsDialog::onThemeSelected(const QString& name) {
    cf::ui::core::ThemeManager::instance().setThemeTo(name.toStdString());
}
```

## 相关文档

- [ThemeFactory - 主题工厂](./theme_factory.md)
- [ICFTheme - 主题接口](./theme.md)
- [UI 核心组件概述](./index.md)
