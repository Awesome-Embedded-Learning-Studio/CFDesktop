---
title: Radius Scale Token 字面量
description: 定义了 Material Design 3 圆角系统的全部 Token 字面量。Material 的
---

# Radius Scale Token 字面量

`cfmaterial_radius_scale_literals.h` 定义了 Material Design 3 圆角系统的全部 Token 字面量。Material 的圆角不是随便选的数字，而是一套精心设计的 7 级标尺，从 0dp（完全直角）到 32dp（大圆角）。

这套字面量的存在是为了保证整个应用的圆角风格一致。与其到处硬编码 `borderRadius: 8`，不如用语义化的 Token，这样调整设计规范时只需改一处。

## 七级圆角标尺

Material Design 3 定义了 7 个标准圆角值：

```cpp
#include "ui/core/token/radius_scale/cfmaterial_radius_scale_literals.h"

using namespace cf::ui::core::token::literals;

// 完全直角 - 0dp
const char* cornerNone = CORNER_NONE;  // "md.shape.cornerNone"

// 超小圆角 - 4dp
const char* cornerXS = CORNER_EXTRA_SMALL;  // "md.shape.cornerExtraSmall"

// 小圆角 - 8dp
const char* cornerSmall = CORNER_SMALL;  // "md.shape.cornerSmall"

// 中圆角 - 12dp
const char* cornerMedium = CORNER_MEDIUM;  // "md.shape.cornerMedium"

// 大圆角 - 16dp
const char* cornerLarge = CORNER_LARGE;  // "md.shape.cornerLarge"

// 超大圆角 - 28dp
const char* cornerXL = CORNER_EXTRA_LARGE;  // "md.shape.cornerExtraLarge"

// 超超大圆角 - 32dp
const char* cornerXXL = CORNER_EXTRA_EXTRA_LARGE;  // "md.shape.cornerExtraExtraLarge"
```text

注意这里用的是 XS/XL 这种缩写，而不是 ExtraSmall/ExtraLarge。常量名为了可读性用的完整拼写，但命名空间内提供别名是个不错的实践（不过当前实现里还没加，需要的话可以自己补）。

## 标准用途映射

每个圆角值都有 Material 推荐的标准用途：

```cpp
// 0dp - 完全直角
// 用于：需要强调边界、或者设计风格偏向硬朗的场景
const char* buttonRadius = CORNER_NONE;  // 某些设计系统用直角按钮

// 4dp - 超小圆角
// 用于：芯片（Chip）、小卡片
const char* chipRadius = CORNER_EXTRA_SMALL;

// 8dp - 小圆角
// 用于：文本框、复选框、小型按钮
const char* textFieldRadius = CORNER_SMALL;
const char* checkboxRadius = CORNER_SMALL;
const char* smallButtonRadius = CORNER_SMALL;

// 12dp - 中圆角
// 用于：卡片（最常用的圆角尺寸）
const char* cardRadius = CORNER_MEDIUM;
const char* buttonRadius = CORNER_MEDIUM;  // Material 标准按钮圆角

// 16dp - 大圆角
// 用于：对话框、提示框
const char* dialogRadius = CORNER_LARGE;
const char* alertDialogRadius = CORNER_LARGE;

// 28dp - 超大圆角
// 用于：悬浮操作按钮（FAB）、底部抽屉
const char* fabRadius = CORNER_EXTRA_LARGE;
const char* modalRadius = CORNER_EXTRA_LARGE;

// 32dp - 超超大圆角
// 用于：导航抽屉、全屏模态
const char* drawerRadius = CORNER_EXTRA_EXTRA_LARGE;
```text

## 在主题系统中使用

圆角 Token 的使用方式和其他 Token 一致，由主题系统解析成实际的数值：

```cpp
// 伪代码：主题系统如何解析圆角 Token
class MaterialTheme {
    float resolveRadius(const char* token) const {
        return radiusTable.at(token);  // 返回 dp 值
    }
};

// 使用示例
MaterialTheme theme;

// 设置按钮圆角
float buttonRadius = theme.resolveRadius(CORNER_MEDIUM);
button.setCornerRadius(buttonRadius);

// 设置卡片圆角
float cardRadius = theme.resolveRadius(CORNER_MEDIUM);
card.setCornerRadius(cardRadius);

// 设置 FAB 圆角
float fabRadius = theme.resolveRadius(CORNER_EXTRA_LARGE);
fab.setCornerRadius(fabRadius);
```text

## 圆角选择指南

选择哪个圆角值有时候不太直观，这里有一些实用的判断依据：

```cpp
// 小型组件（< 40dp 高度）
// 用 4dp 或 8dp，避免圆角占太多比例
const char* smallComponentRadius = CORNER_SMALL;

// 中型组件（40-80dp 高度）
// 用 12dp，平衡视觉和空间
const char* mediumComponentRadius = CORNER_MEDIUM;

// 大型组件（> 80dp 高度）
// 用 16dp 或更大，让圆角和组件尺寸成比例
const char* largeComponentRadius = CORNER_LARGE;

// 正圆形组件（如 FAB）
// 用 28dp 或 32dp，接近半圆
const char* circularComponentRadius = CORNER_EXTRA_LARGE;
```text

⚠️ 一个常见的错误是给小型组件用太大的圆角。比如一个高度只有 32dp 的按钮用 16dp 圆角，结果圆角占了一半高度，看起来会很奇怪。圆角值应该和组件尺寸成比例。

## 自定义圆角值

虽然 Material 定义了这 7 个标准值，但你的设计可能需要其他圆角尺寸。有两种处理方式：

```cpp
// 方式 1：在主题配置中添加自定义 Token
// 这需要修改主题系统，但可以保持代码的语义化

// 方式 2：直接使用数值（不推荐，但现实）
// 对于特别的设计需求，有时候直接传值更实际
button.setCornerRadius(20.0f);  // 自定义值
```text

如果团队有统一的设计规范，建议把常用的自定义圆角也加到 Token 系统里，保持代码的语义化。

## 批量遍历

提供了遍历所有圆角 Token 的辅助：

```cpp
// 遍历所有 Radius Token
for (size_t i = 0; i < RADIUS_TOKEN_COUNT; ++i) {
    printf("Radius Token %zu: %s\n", i, ALL_RADIUS_TOKENS[i]);
}
```text

## 响应式考虑

圆角的 dp 值在不同密度的屏幕上会自动缩放，这是 UI 框架的责任。如果你的框架不支持 dp/sp 单位，需要手动处理密度缩放：

```cpp
// 伪代码：手动处理密度缩放
float scale = getDeviceDensity();
float radiusInPx = theme.resolveRadius(CORNER_MEDIUM) * scale;
view.setCornerRadiusPx(radiusInPx);
```text

## 相关文档

- [Material Token 字面量](../material_scheme/cfmaterial_token_literals.md)
- [Typography Token 字面量](../typography/cfmaterial_typography_token_literals.md)
- [Motion Token 字面量](../motion/cfmaterial_motion_token_literals.md)
