---
title: "MaterialTheme - Material Design 3 主题"
description: 是 Material Design 3 的完整主题实现。它把颜色方案、字体、圆角和动画规范组合成一个
---

# MaterialTheme - Material Design 3 主题

`MaterialTheme` 是 Material Design 3 的完整主题实现。它把颜色方案、字体、圆角和动画规范组合成一个整体，代表一个"完整"的 Material 主题。这个类本身不对外暴露构造函数，只能通过 `MaterialFactory` 创建——这样设计是为了确保主题组件之间的协调一致性。

## 主题组成

一个 Material 主题由四个部分组成：

- **MaterialColorScheme**：26 个语义化颜色
- **MaterialTypography**：15 个字体样式（Type Scale）
- **MaterialRadiusScale**：7 个圆角规格
- **MaterialMotionScheme**：动画时长和缓动曲线

`MaterialTheme` 实现了 `ICFTheme` 接口，所以可以和我们的主题系统无缝集成。

## 创建主题

主题必须通过 `MaterialFactory` 创建：

```cpp
#include "material_factory_class.h"

MaterialFactory factory;

// 按名称创建预定义主题
auto lightTheme = factory.fromName("theme.material.light");
auto darkTheme = factory.fromName("theme.material.dark");

// 从 JSON 创建
QByteArray json = loadThemeJson();
auto customTheme = factory.fromJson(json);
```text

支持的预定义主题名称：
- `theme.material.light`：默认浅色主题
- `theme.material.dark`：默认深色主题

## 访问主题组件

拿到 `ICFTheme` 指针后，可以通过接口访问各个组件：

```cpp
std::unique_ptr<ICFTheme> theme = factory.fromName("theme.material.light");

// 访问颜色方案
auto* colorScheme = static_cast<MaterialColorScheme*>(theme->color_scheme());
QColor primary = colorScheme->queryExpectedColor("md.primary");

// 访问字体
auto* typography = static_cast<MaterialTypography*>(theme->font_type());
QFont titleFont = typography->queryTargetFont("md.typography.titleLarge");

// 访问圆角
auto* radiusScale = static_cast<MaterialRadiusScale*>(theme->radius_scale());
float cardRadius = radiusScale->queryRadiusScale("md.shape.cornerMedium");

// 访问动画
auto* motion = static_cast<MaterialMotionScheme*>(theme->motion_spec());
int duration = motion->queryDuration("shortEnter");
```text

这里需要 `static_cast` 是因为 `ICFTheme` 接口返回的是基类指针。在实际使用中，既然我们明确知道是 Material 主题，这样转换是安全的。

## 主题应用

主题通常配合 UI 组件使用。典型的使用模式是在应用启动时设置主题：

```cpp
void Application::setupTheme() {
    MaterialFactory factory;

    // 根据系统设置选择主题
    bool isDark = isSystemDarkMode();
    const char* themeName = isDark ? "theme.material.dark" : "theme.material.light";

    auto theme = factory.fromName(themeName);
    setTheme(std::move(theme));
}

// 在组件中使用主题
void MyWidget::paintEvent(QPaintEvent*) {
    auto* theme = getTheme();
    auto* colors = static_cast<MaterialColorScheme*>(theme->color_scheme());

    QColor backgroundColor = colors->queryExpectedColor("md.surface");
    // 使用背景色绘制...
}
```text

## 组件协调性

`MaterialTheme` 的设计确保各组件之间是协调的。比如工厂创建浅色主题时，会同时创建浅色的颜色方案、和浅色匹配的字体对比度、适配的圆角大小等。这种协调性通过工厂内部的预设配置保证，而不是在运行时计算。

⚠️ 不要自己手动拼凑 `MaterialTheme` 的各个组件然后期望它们协调工作——组件间的协调性只有通过工厂创建的主题才能保证。

## 不可移动/不可复制

`MaterialTheme` 禁止了拷贝和移动操作。这是因为基类 `ICFTheme` 存储了 `unique_ptr` 成员，移动语义在继承体系下会带来一些复杂问题。既然主题应该通过工厂创建并由智能指针管理，禁用移动反而能防止一些误用：

```cpp
// 编译错误：MaterialTheme 不可移动
MaterialTheme theme2 = std::move(theme1);

// 正确做法：使用工厂创建新实例
auto theme2 = factory.fromName("theme.material.light");
```bash

## Material Design 3 规范对应

我们的实现严格遵循 Material Design 3 规范：

| 组件 | MD3 规范 | 我们的实现 |
|------|----------|-----------|
| 颜色 | Dynamic Color / Tonal Palette | `MaterialColorScheme` |
| 字体 | Type Scale (15 styles) | `MaterialTypography` |
| 形状 | Shape Scheme (7 corner radii) | `MaterialRadiusScale` |
| 动画 | Motion Duration & Easing | `MaterialMotionScheme` |

每个组件的手册文档都会详细说明其对应的 MD3 规范部分。

## 相关文档

- [MaterialColorScheme - 颜色方案](./cfmaterial_scheme.md)
- [MaterialTypography - 字体系统](./cfmaterial_fonttype.md)
- [MaterialRadiusScale - 圆角系统](./cfmaterial_radius_scale.md)
- [MaterialMotionScheme - 动画系统](./cfmaterial_motion.md)
- [MaterialFactory - 主题工厂](./material_factory_class.md)
