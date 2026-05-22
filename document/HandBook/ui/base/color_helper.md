---
title: "colorhelper - 颜色助手"
description: 提供了 Material Design 3 颜色系统常用的颜色操作函数，包括混合、高程叠加、对比度计
---

# color_helper - 颜色助手

`color_helper` 提供了 Material Design 3 颜色系统常用的颜色操作函数，包括混合、高程叠加、对比度计算和色调板生成。这些本来应该是色彩理论的实现细节，但在做动态主题系统时反复出现，所以抽出来作为独立工具。

## 颜色混合

`blend()` 在两个颜色之间按指定比例混合：

```cpp
#include "ui/base/color_helper.h"

using namespace cf::ui::base;

CFColor base("#1A1A1A");
CFColor overlay("#FFFFFF");

// 50% 混合
CFColor result = blend(base, overlay, 0.5f);

// 半透明遮罩效果
CFColor dimmed = blend(originalColor, CFColor(0, 0, 0), 0.3f);
```text

混合是在 RGB 空间线性进行的，不是 alpha 混合。ratio = 0 返回 base，ratio = 1 返回 overlay，中间值按比例线性组合。

## 高程叠加

Material Design 用"高程"（elevation）来表示层级关系，`elevationOverlay()` 实现了这个效果：

```cpp
CFColor surface("#FFFFFF");
CFColor primary("#6750A4");

// 无高程
CFColor flat = elevationOverlay(surface, primary, 0);

// 高程 4dp 的卡片
CFColor card = elevationOverlay(surface, primary, 4);

// 高程 8dp 的对话框
CFColor dialog = elevationOverlay(surface, primary, 8);
```text

高程越大，surface 颜色会越向 primary 靠近，模拟阴影和光照效果。这是 Material Design 3 的标准做法，比单纯用 alpha 叠加黑色要精致一些。

⚠️ elevation 参数的单位是 dp，不是像素。需要根据屏幕密度转换，或者在高 DPI 环境下调整参数。

## 对比度计算

`contrastRatio()` 计算 WCAG 标准的对比度，用于判断文字和背景的可读性：

```cpp
CFColor text("#000000");
CFColor background("#FFFFFF");

float ratio = contrastRatio(text, background);  // 21.0 (最大对比度)

// WCAG AA 标准要求正文对比度 >= 4.5
if (contrastRatio(foreground, background) < 4.5f) {
    // 需要调整颜色
}

// 自动选择深色/浅色文字
CFColor textColor = contrastRatio(darkText, bgColor) > contrastRatio(lightText, bgColor)
                    ? darkText : lightText;
```text

对比度是基于相对亮度计算的，返回值范围是 1.0 到 21.0。1.0 表示两个颜色亮度相同，21.0 是黑白之间的最大对比度。

⚠️ 这个计算相对昂贵，因为涉及 RGB 到线性空间的转换。在热路径（比如渲染循环）里最好缓存结果，别每帧都算。

## 色调板生成

`tonalPalette()` 从一个关键色生成完整的色调板，返回 13 个色调从浅到深的颜色：

```cpp
CFColor keyColor("#6750A4");  // Material Purple

QList<CFColor> palette = tonalPalette(keyColor);

// palette[0]  是最浅色调（tone ~95）
// palette[12] 是最深色调（tone ~10）

// 用法示例
CFColor surface = palette[4];    // 浅色表面
CFColor surfaceVariant = palette[6];  // 变体表面
CFColor onSurface = palette[10]; // 表面上的文字
```text

色调板是 Material Design 3 主题系统的核心——整个颜色系统都是从单个"种子颜色"衍生出来的。生成的色调遵循 Material 规范，确保视觉和谐度。

这个函数内部用的是 HCT 色彩空间，所以生成的颜色保持了原始色相，只是调整色度和色调。这比直接在 RGB 空间操作要自然得多。

## 相关文档

- [color - HCT 色彩空间](./color.md)
- [math_helper - 数学工具](./math_helper.md)
