---
title: "MaterialColorScheme - Material Design 3 颜色方案"
description: 是我们对 Material Design 3 颜色系统的完整实现。Material You 的核心就
---

# MaterialColorScheme - Material Design 3 颜色方案

`MaterialColorScheme` 是我们对 Material Design 3 颜色系统的完整实现。Material You 的核心就是动态颜色系统——从一个种子颜色自动生成 26 个语义化颜色，确保整个 UI 在视觉上协调一致。我们把这个系统做成了嵌入式 Token 注册表的形式，每个颜色方案实例都独立管理自己的颜色值。

## Material Design 3 颜色系统

Material Design 3 的颜色系统是基于"色调调色板"（Tonal Palette）设计的，而不是传统 HSL/HSV。每个颜色角色都有对应的"On"颜色用于显示在其上方的文本。系统把颜色分成几个组：

- **Primary**：主色，用于关键组件如按钮、活跃状态
- **Secondary**：辅助色，用于次要组件和强调
- **Tertiary**：第三色，用于平衡和表达品牌独特性
- **Error**：错误色，用于错误状态和破坏性操作
- **Surface**：表面色，包括背景、表面及其变体
- **Utility**：工具色，支持阴影、遮罩和反色状态

## 基本用法

通过工厂函数创建颜色方案是最简单的方式：

```cpp
#include "material_factory.hpp"

// 创建默认浅色主题（Material 基准紫色）
auto lightScheme = cf::ui::core::material::light();

// 创建默认深色主题
auto darkScheme = cf::ui::core::material::dark();

// 查询颜色
QColor primary = lightScheme.queryExpectedColor("md.primary");
QColor onPrimary = lightScheme.queryExpectedColor("md.onPrimary");
QColor surface = lightScheme.queryExpectedColor("md.surface");
```text

颜色名称采用 `md.` 前缀，后跟 Material 官方定义的 token 名称。这样设计是为了和其他主题系统（比如我们未来可能实现的 Fluent）做区分。

## 颜色组访问

虽然可以用字符串查询，但类型安全的方式是通过颜色组访问器：

```cpp
using namespace cf::ui::core;

MaterialColorScheme scheme = material::light();

// 获取主色组——返回的是包含 Token 类型的结构体
PrimaryColors primary = scheme.primary();
// 这些 Token 类型可以配合我们的 Token Registry 使用

// 需要实际颜色值时还是通过查询
QColor primaryColor = scheme.queryExpectedColor("md.primary");
QColor containerColor = scheme.queryExpectedColor("md.primaryContainer");
```text

颜色组结构体（`PrimaryColors`、`SecondaryColors` 等）主要是为了类型安全和文档目的，实际颜色值还是从 registry 中查询。

## 从种子颜色生成

Material You 的特色是"动态颜色"——用户选一个喜欢的颜色，系统自动生成完整的配色：

```cpp
#include "base/color.h"

// 从任意颜色生成配色
CFColor seedColor("#6750A4");
auto scheme = cf::ui::core::material::fromKeyColor(seedColor);

// 生成深色版本
auto darkScheme = cf::ui::core::material::fromKeyColor(seedColor, true);
```text

这个功能内部使用 HCT 色彩空间和 Material 的色调调色板算法。HCT（Hue-Chroma-Tone）是 Material 团队专门开发的色彩空间，比 HSL 更符合人眼对颜色的感知——这也是为什么 Material 的配色看起来特别和谐的原因。

## 从 JSON 导入

Material Theme Builder 是 Google 官方的配色在线工具，导出的 JSON 我们可以直接解析：

```cpp
QByteArray json = R"({
  "schemes": {
    "light": {
      "primary": "#6750A4",
      "onPrimary": "#FFFFFF",
      "primaryContainer": "#EADDFF",
      ...
    }
  }
})";

// 从 JSON 创建（指定使用 light 方案）
auto result = cf::ui::core::material::fromJson(json, false);

// cf::expected 风格的错误处理
if (result) {
    auto scheme = std::move(*result);
    // 使用 scheme
} else {
    const auto& err = result.error();
    if (err.kind == MaterialSchemeError::Kind::InvalidJson) {
        qDebug() << "JSON 解析失败:" << err.message.c_str();
    }
}
```text

也支持直接传入颜色对象的方式（没有 `schemes` 包装的扁平结构）。

## 导出到 JSON

可以把当前配色导出为 JSON 格式，方便保存或分享：

```cpp
MaterialColorScheme scheme = material::light();
QByteArray json = material::toJson(scheme);

// 可以保存到文件或传输
QFile file("my_theme.json");
file.open(QIODevice::WriteOnly);
file.write(json);
```text

## Token 注册表

每个 `MaterialColorScheme` 内部都有一个 `EmbeddedTokenRegistry`，用来存储所有颜色 Token：

```cpp
MaterialColorScheme scheme = material::light();

// 直接访问底层注册表
auto& registry = scheme.registry();

// 可以手动修改或添加 Token
registry.set("md.customColor", QColor("#FF5722"));
```text

这个设计让系统既支持预定义的 Material 颜色，也允许扩展自定义颜色。

## 缓存机制

颜色查询有缓存层，避免重复解析字符串：

```cpp
// 第一次查询会查找并缓存
QColor color1 = scheme.queryColor("md.primary");

// 后续查询从缓存返回
QColor color2 = scheme.queryColor("md.primary");
```text

缓存在 `MaterialColorScheme` 对象生命周期内有效。如果需要修改某个颜色后立即生效，直接修改 registry 即可——查询接口每次都会从 registry 读取最新值。

## 完整颜色列表

Material Design 3 定义了 26 个颜色角色，对应的查询名称如下：

```text
主色组：
  md.primary, md.onPrimary, md.primaryContainer, md.onPrimaryContainer

辅助色组：
  md.secondary, md.onSecondary, md.secondaryContainer, md.onSecondaryContainer

第三色组：
  md.tertiary, md.onTertiary, md.tertiaryContainer, md.onTertiaryContainer

错误色组：
  md.error, md.onError, md.errorContainer, md.onErrorContainer

表面色组：
  md.background, md.onBackground, md.surface, md.onSurface,
  md.surfaceVariant, md.onSurfaceVariant, md.outline, md.outlineVariant

工具色组：
  md.shadow, md.scrim, md.inverseSurface, md.inverseOnSurface, md.inversePrimary
```text

## 相关文档

- [MaterialTheme - 主题组合](./cfmaterial_theme.md)
- [MaterialFactory - 主题工厂](./material_factory_class.md)
- [MaterialTypography - 字体系统](./cfmaterial_fonttype.md)
