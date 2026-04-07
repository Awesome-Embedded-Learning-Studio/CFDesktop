# ThemeFactory - 主题工厂

`ThemeFactory` 是创建主题实例的抽象接口，负责从名字或 JSON 数据构造 `ICFTheme` 对象，以及将现有主题序列化为 JSON。为什么需要工厂而不是直接 `new` 一个主题？因为主题的创建过程往往比较复杂——需要组合颜色方案、动画规格、圆角尺度等多个组件，工厂模式把这个创建逻辑封装起来，也让加载配置文件成为可能。

## 工厂接口

工厂是一个纯虚基类，你需要继承它并实现三个方法：

```cpp
#include "ui/core/theme_factory.h"

class MyThemeFactory : public cf::ui::core::ThemeFactory {
public:
    // 从名字创建主题
    std::unique_ptr<cf::ui::core::ICFTheme> fromName(const char* name) override;

    // 从 JSON 创建主题
    std::unique_ptr<cf::ui::core::ICFTheme> fromJson(const QByteArray& json) override;

    // 将主题序列化为 JSON
    QByteArray toJson(cf::ui::core::ICFTheme* raw_theme) override;
};
```

注意返回类型是 `std::unique_ptr<ICFTheme>`，调用者获得主题的所有权。这个设计很自然——工厂负责生产，消费者负责销毁。

## 从名字创建

`fromName` 是最常见的创建方式，用于内置主题或预定义的主题变体：

```cpp
std::unique_ptr<cf::ui::core::ICFTheme> MyThemeFactory::fromName(const char* name) {
    auto theme = std::make_unique<cf::ui::core::ICFTheme>();

    // 根据名字配置不同的组件
    if (strcmp(name, "default") == 0) {
        theme->color_scheme_ = std::make_unique<DefaultColorScheme>();
        theme->motion_spec_ = std::make_unique<DefaultMotionSpec>();
        theme->radius_scale_ = std::make_unique<DefaultRadiusScale>();
        theme->font_type_ = std::make_unique<DefaultFontType>();
    } else if (strcmp(name, "compact") == 0) {
        theme->color_scheme_ = std::make_unique<DefaultColorScheme>();
        theme->motion_spec_ = std::make_unique<FasterMotionSpec>();  // 更快的动画
        theme->radius_scale_ = std::make_unique<CompactRadiusScale>();  // 更小的圆角
        theme->font_type_ = std::make_unique<CompactFontType>();
    } else {
        return nullptr;  // 未知主题返回空指针
    }

    return theme;
}
```

`name` 参数来自 `ThemeManager::insert_one` 时注册的名字，但工厂内部可以有自己的命名空间逻辑——你可以用同一个工厂支持多个主题变体，或者完全忽略名字只返回固定配置。

⚠️ 如果无法创建主题（比如名字不认识），返回 `nullptr` 即可，不要抛异常。`ThemeManager` 会检测空指针并优雅处理。

## 从 JSON 创建

`fromJson` 让主题可以从配置文件加载，这是实现用户自定义主题的基础：

```cpp
std::unique_ptr<cf::ui::core::ICFTheme> MyThemeFactory::fromJson(const QByteArray& json) {
    auto theme = std::make_unique<cf::ui::core::ICFTheme>();

    // 使用 QJsonDocument 解析
    QJsonDocument doc = QJsonDocument::fromJson(json);
    if (doc.isNull() || !doc.isObject()) {
        return nullptr;
    }

    QJsonObject root = doc.object();

    // 解析颜色方案
    QJsonObject colors = root["colors"].toObject();
    theme->color_scheme_ = std::make_unique<CustomColorScheme>(
        QColor(colors["background"].toString()),
        QColor(colors["foreground"].toString())
    );

    // 解析动画规格
    QJsonObject motion = root["motion"].toObject();
    theme->motion_spec_ = std::make_unique<CustomMotionSpec>(
        motion["duration"].toInt(200),
        motion["easing"].toString("ease").toStdString()
    );

    // ... 解析其他组件

    return theme;
}
```

JSON 格式完全由你定义，只要工厂能解析就行。我们建议遵循一定的约定，比如 `colors` 对象包含颜色、`motion` 对象包含动画参数，这样不同主题的配置文件可以保持一致性。

## 序列化到 JSON

`toJson` 是 `fromJson` 的逆操作，用于导出当前主题配置：

```cpp
QByteArray MyThemeFactory::toJson(cf::ui::core::ICFTheme* raw_theme) {
    if (!raw_theme) {
        return QByteArray();
    }

    QJsonObject root;

    // 序列化颜色方案
    const auto& colors = raw_theme->color_scheme();
    QJsonObject colorObj;
    colorObj["background"] = colors.background().name();
    colorObj["foreground"] = colors.foreground().name();
    root["colors"] = colorObj;

    // 序列化动画规格
    const auto& motion = raw_theme->motion_spec();
    QJsonObject motionObj;
    motionObj["duration"] = motion.default_duration();
    root["motion"] = motionObj;

    // ... 序列化其他组件

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}
```

`raw_theme` 是裸指针而不是引用，这是为了兼容 Qt 的信号槽参数传递习惯。但在实际使用中，你传进来的应该总是一个有效的主题指针。

⚠️ 序列化时可能会丢失信息——如果你的 `ICFTheme` 实现包含一些无法简单表示的组件（比如引用了外部资源），`fromJson` 和 `toJson` 不必是完全可逆的。只要能保存"用户可配置"的那部分就够了。

## 注册到管理器

工厂本身不做任何事，需要注册到 `ThemeManager` 才能发挥作用：

```cpp
// 注册内置主题
cf::ui::core::ThemeManager::instance().insert_one("builtin", []() {
    return std::make_unique<MyThemeFactory>();
});

// 注册支持 JSON 的主题工厂
cf::ui::core::ThemeManager::instance().insert_one("custom", []() {
    return std::make_unique<JsonThemeFactory>();
});
```

注意这里用的是 lambda 而不是直接传工厂实例。`ThemeManager` 会在需要时调用 lambda 来创建工厂，而不是在注册时就创建。这个延迟创建机制可以减少启动时的开销，特别是当你注册了很多插件主题但大部分都不会被使用时。

## 错误处理

工厂方法的错误处理建议遵循以下原则：

```cpp
std::unique_ptr<cf::ui::core::ICFTheme> MyThemeFactory::fromJson(const QByteArray& json) {
    // 1. 先检查基本有效性
    if (json.isEmpty()) {
        qWarning() << "Empty JSON data";
        return nullptr;
    }

    // 2. 解析失败返回 nullptr，不要抛异常
    QJsonDocument doc = QJsonDocument::fromJson(json);
    if (doc.isNull()) {
        qWarning() << "Invalid JSON format";
        return nullptr;
    }

    // 3. 缺少必要字段可以回退到默认值
    QJsonObject root = doc.object();
    int duration = root.value("duration").toInt(200);  // 默认 200ms

    // 4. 只在真正无法恢复时返回 nullptr
    if (!root.contains("colors")) {
        qWarning() << "Missing required 'colors' field";
        return nullptr;
    }

    // ... 构造主题
}
```

返回 `nullptr` 表示"无法创建"，`ThemeManager` 会把这个情况传达给调用方。我们不建议在工厂方法里抛异常，因为 Qt 的信号槽机制和异常配合得不太好，而且这也让错误处理逻辑更清晰。

## 实现示例

这是一个完整的工厂实现，支持名字创建和 JSON 序列化：

```cpp
class ModernThemeFactory : public cf::ui::core::ThemeFactory {
public:
    std::unique_ptr<cf::ui::core::ICFTheme> fromName(const char* name) override {
        auto theme = std::make_unique<cf::ui::core::ICFTheme>();

        // 现代风格使用鲜艳的accent色和更快的动画
        theme->color_scheme_ = std::make_unique<ModernColorScheme>();
        theme->motion_spec_ = std::make_unique<ModernMotionSpec>();
        theme->radius_scale_ = std::make_unique<ModernRadiusScale>();
        theme->font_type_ = std::make_unique<ModernFontType>();

        return theme;
    }

    std::unique_ptr<cf::ui::core::ICFTheme> fromJson(const QByteArray& json) override {
        QJsonDocument doc = QJsonDocument::fromJson(json);
        if (doc.isNull() || !doc.isObject()) {
            return nullptr;
        }

        QJsonObject root = doc.object();
        auto theme = std::make_unique<cf::ui::core::ICFTheme>();

        // 支持自定义 accent 颜色
        if (root.contains("accent_color")) {
            QColor accent(root["accent_color"].toString());
            theme->color_scheme_ = std::make_unique<ModernColorScheme>(accent);
        } else {
            theme->color_scheme_ = std::make_unique<ModernColorScheme>();
        }

        // 支持自定义动画速度
        int duration = root.value("animation_duration").toInt(150);
        theme->motion_spec_ = std::make_unique<ModernMotionSpec>(duration);

        // 其他组件使用默认值
        theme->radius_scale_ = std::make_unique<ModernRadiusScale>();
        theme->font_type_ = std::make_unique<ModernFontType>();

        return theme;
    }

    QByteArray toJson(cf::ui::core::ICFTheme* raw_theme) override {
        if (!raw_theme) {
            return QByteArray();
        }

        QJsonObject root;

        // 导出关键配置，供后续 fromJson 恢复
        const auto& colors = raw_theme->color_scheme();
        root["accent_color"] = colors.accent().name();

        const auto& motion = raw_theme->motion_spec();
        root["animation_duration"] = motion.default_duration();

        // 添加元数据
        root["theme_name"] = "modern";
        root["version"] = "1.0";

        return QJsonDocument(root).toJson();
    }
};
```

## 相关文档

- [ThemeManager - 主题管理器](./theme_manager.md)
- [ICFTheme - 主题接口](./theme.md)
- [UI 核心组件概述](./index.md)
