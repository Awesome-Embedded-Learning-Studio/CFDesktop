---
title: "materialfactory.hpp - Material 工厂函数"
description: 提供了一套自由函数风格的工厂 API，用于创建 Material Design 3 的各个组件。相比
---

# material_factory.hpp - Material 工厂函数

`material_factory.hpp` 提供了一套自由函数风格的工厂 API，用于创建 Material Design 3 的各个组件。相比 `MaterialFactory` 类，这套 API 更轻量，不需要实例化工厂对象——直接调用函数就行。

## 颜色方案工厂

创建颜色方案是最常用的功能：

```cpp
#include "material_factory.hpp"

// 默认浅色方案
auto light = cf::ui::core::material::light();

// 默认深色方案
auto dark = cf::ui::core::material::dark();
```text

这两个函数返回 `MaterialColorScheme` 对象（不是指针），可以直接使用或移动。

## 从种子颜色生成

Material You 的核心功能——动态颜色生成：

```cpp
#include "base/color.h"

// 从任意颜色生成
CFColor seed("#6750A4");
auto scheme = cf::ui::core::material::fromKeyColor(seed);

// 生成深色版本
auto darkScheme = cf::ui::core::material::fromKeyColor(seed, true);
```text

种子颜色可以是用户选择的品牌色、墙纸的主色等等。生成的配色会自动计算 26 个颜色角色的值，确保视觉协调。

## 从 JSON 创建（带错误处理）

`fromJson` 返回 `cf::expected` 类型，支持详细的错误处理：

```cpp
QByteArray json = loadFromFile();
auto result = cf::ui::core::material::fromJson(json);

if (result) {
    auto scheme = std::move(*result);
    // 使用 scheme
} else {
    const auto& err = result.error();
    switch (err.kind) {
        case MaterialSchemeError::Kind::InvalidJson:
            qDebug() << "JSON 格式错误:" << err.message.c_str();
            break;
        case MaterialSchemeError::Kind::MissingColor:
            qDebug() << "缺少必需颜色:" << err.message.c_str();
            break;
        case MaterialSchemeError::Kind::InvalidColorFormat:
            qDebug() << "颜色格式错误:" << err.message.c_str();
            break;
        case MaterialSchemeError::Kind::GenerationFailed:
            qDebug() << "颜色生成失败:" << err.message.c_str();
            break;
    }
}
```text

这个错误处理比 `MaterialFactory::fromJson` 的空指针友好得多。

## 导出到 JSON

可以把配色导出为 JSON：

```cpp
auto scheme = material::light();
QByteArray json = material::toJson(scheme);

// 保存到文件
QFile file("theme.json");
file.open(QIODevice::WriteOnly);
file.write(json);
```text

导出的格式兼容 Material Theme Builder。

## 字体工厂

创建默认排版系统：

```cpp
auto typography = cf::ui::core::material::defaultTypography();

QFont titleFont = typography.queryTargetFont("md.typography.titleLarge");
QFont bodyFont = typography.queryTargetFont("md.typography.bodyMedium");
```text

默认字体会根据平台自动选择——Windows 用 Segoe UI，macOS 用 .SF NS Text，Linux 用 Ubuntu。

## 圆角工厂

创建默认圆角系统：

```cpp
auto radius = cf::ui::core::material::defaultRadiusScale();

float small = radius.queryRadiusScale("md.shape.cornerSmall");   // 8.0f
float medium = radius.queryRadiusScale("md.shape.cornerMedium"); // 12.0f
float large = radius.queryRadiusScale("md.shape.cornerLarge");   // 16.0f
```text

## 动画工厂

创建默认运动系统：

```cpp
auto motion = cf::ui::core::material::motion();

int duration = motion.queryDuration("shortEnter");  // 200
auto spec = motion.getMotionSpec("mediumExit");
```text

## 完整工作流

一个典型的使用流程是组合各个组件创建完整主题：

```cpp
#include "material_factory.hpp"

// 创建各个组件
auto colors = material::fromKeyColor("#6750A4");
auto typography = material::defaultTypography();
auto radius = material::defaultRadiusScale();
auto motion = material::motion();

// 使用组件...
QColor primary = colors.queryExpectedColor("md.primary");
QFont titleFont = typography.queryTargetFont("md.typography.titleLarge");
float cardRadius = radius.queryRadiusScale("md.shape.cornerMedium");
auto enterSpec = motion.getMotionSpec("mediumEnter");
```text

如果需要完整的 `MaterialTheme` 对象，还是得用 `MaterialFactory` 类。

## 自定义主题示例

可以从种子颜色生成主题并应用：

```cpp
void applyCustomTheme(const QColor& brandColor) {
    using namespace cf::ui::core::material;

    // 从品牌色生成配色
    CFColor keyColor(brandColor);
    auto scheme = fromKeyColor(keyColor, isDarkMode());

    // 获取其他默认组件
    auto typography = defaultTypography();
    auto radius = defaultRadiusScale();
    auto motion = motion();

    // 应用到应用...
    updateColorScheme(&scheme);
    updateTypography(&typography);
    // ...
}
```bash

这种方式让应用可以轻松实现"品牌色换肤"功能。

## 与 MaterialFactory 的选择

两套 API 的区别：

| 特性 | material_factory.hpp | MaterialFactory 类 |
|------|---------------------|-------------------|
| 使用方式 | 自由函数 | 类实例 |
| 错误处理 | cf::expected | 空指针 |
| 主题创建 | 不支持 | 支持 |
| 适用场景 | 独立组件创建 | 完整主题管理 |

如果只需要创建颜色/字体/圆角等单个组件，用自由函数 API 更简洁。如果需要创建完整 `MaterialTheme` 对象，得用 `MaterialFactory` 类。

## 相关文档

- [MaterialFactory - 主题工厂类](./material_factory_class.md)
- [MaterialColorScheme - 颜色方案](./cfmaterial_scheme.md)
- [MaterialTheme - 主题实现](./cfmaterial_theme.md)
