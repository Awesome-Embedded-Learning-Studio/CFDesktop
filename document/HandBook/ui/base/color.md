---
title: "color - HCT 色彩空间"
description: "是对  的扩展，增加了 HCT（Hue-Chroma-Tone）色彩空间支持。HCT 是 Mater"
---

# color - HCT 色彩空间

`CFColor` 是对 `QColor` 的扩展，增加了 HCT（Hue-Chroma-Tone）色彩空间支持。HCT 是 Material Design 3 采用的色彩空间，比 RGB 或 HSL 更符合人类对颜色的直觉感知——色相、鲜艳度、明度三个维度正交，调整其中一个不会影响其他两个。这在做动态主题系统时特别重要。

## 为什么需要 HCT

RGB 空间的问题是"不直观"——你想把颜色变亮一点，结果色相也跟着变了；想把蓝色调得更鲜艳，结果明度也变了。HCT 解决了这个问题：

```cpp
#include "ui/base/color.h"

using namespace cf::ui::base;

// RGB 方式：想要一个更亮的蓝色...
QColor rgbBlue(0, 0, 255);
QColor brighterBlue = rgbBlue.lighter(120);  // 结果可能偏紫了

// HCT 方式：保持色相和色度，只调整色调
CFColor hctBlue(240.0f, 80.0f, 50.0f);  // 蓝色，中高饱和度，中等亮度
CFColor brighterHCT = CFColor(hctBlue.hue(), hctBlue.chroma(), 70.0f);
// 色相 240° 不变，鲜艳度不变，只是更亮了
```text

这就是 Material Design 3 能从单个"种子颜色"生成整套主题的原因——在 HCT 空间里，你可以独立控制颜色的各个维度。

## 构造颜色

`CFColor` 可以从多种方式构造：

```cpp
// 从 RGB 值
CFColor red(255, 0, 0);

// 从 QColor
QColor qColor("#FF0000");
CFColor fromQColor(qColor);

// 从十六进制字符串
CFColor fromHex("#FF0000");
CFColor fromHexWithAlpha("#80FF0000");  // 50% 透明的红色

// 从 HCT 值
CFColor fromHCT(240.0f, 80.0f, 50.0f);  // 蓝色，饱和度 80，明度 50
```text

HCT 值的范围：
- `hue`：0° 到 360°，绕色相环一圈
- `chroma`：0 到 150，值越大越鲜艳（超过某个值可能无法表示）
- `tone`：0 到 100，0 是纯黑，100 是纯白

⚠️ 并非所有 (hue, chroma, tone) 组合都能映射到有效的 RGB。超出 sRGB 色域的组合会被裁剪，所以设置 chroma 时要保守一点，一般别超过 100。

## 获取 HCT 分量

构造后可以随时获取 HCT 值：

```cpp
CFColor color("#6750A4");  // Material Purple

float h = color.hue();     // 262.5°（紫色偏蓝）
float c = color.chroma();  // 约 52（中等鲜艳度）
float t = color.tone();    // 约 42（中等偏暗）

// 基于这些值生成变体
CFColor lighterVariant(color.hue(), color.chroma() * 0.8f, color.tone() + 20);
CFColor darkerVariant(color.hue(), color.chroma() * 1.2f, color.tone() - 20);
```text

HCT 值在构造时计算并缓存，所以访问是 O(1) 的。别担心性能问题。

## 相对亮度

`relativeLuminance()` 计算 WCAG 标准的相对亮度，用于对比度计算：

```cpp
CFColor text("#000000");
CFColor bg("#FFFFFF");

float lumText = text.relativeLuminance();  // ~0.0（黑色）
float lumBg = bg.relativeLuminance();      // ~1.0（白色）

// 对比度 = (Lmax + 0.05) / (Lmin + 0.05)
float ratio = (std::max(lumText, lumBg) + 0.05) /
              (std::min(lumText, lumBg) + 0.05);
```text

这个函数在 `color_helper.h` 的 `contrastRatio()` 里被使用，一般不需要直接调用。但如果你想自己实现一些可访问性相关的逻辑，可以用它。

## 与 QColor 互操作

`CFColor` 内部维护了一个 `QColor`，可以随时获取：

```cpp
CFColor myColor("#FF0000");

// 获取原生 QColor 用于 Qt API
QColor qColor = myColor.native_color();

// 直接用在 QPainter 里
QPainter painter(this);
painter.setBrush(myColor.native_color());
painter.drawEllipse(center, 50, 50);
```text

`CFColor` 不是 `QColor` 的子类，而是组合关系。这样设计是为了避免 `QColor` 的隐式转换带来的意外——`QColor(const char*)` 会把字符串当成颜色名，而我们希望 hex 字符串有明确的解析行为。

## 使用建议

做主题系统时，推荐的工作流：

1. 选择一个"种子颜色"（通常是品牌色）
2. 转换到 HCT 空间
3. 在 HCT 空间生成各种变体（调整 tone 生成深浅色，调整 chroma 生成去饱和变体）
4. 用 `color_helper.h` 的 `tonalPalette()` 自动完成这个步骤

```cpp
// 种子颜色
CFColor seed("#6750A4");

// 生成完整色调板
QList<CFColor> palette = tonalPalette(seed);

// 使用预定义的色调
CFColor primary = palette[6];      // 主要色
CFColor onPrimary = palette[10];   // 主要色上的文字
CFColor surface = palette[4];      // 表面色
```text

这样生成的主题保证色彩和谐，避免了手工调整 RGB 时容易出现的"脏色"问题。

## 相关文档

- [color_helper - 颜色助手](./color_helper.md)
- [math_helper - 数学工具](./math_helper.md)
