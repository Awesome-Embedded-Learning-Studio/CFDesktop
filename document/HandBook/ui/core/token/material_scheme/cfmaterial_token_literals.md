---
title: Material Token 字面量
description: 定义了 Material Design 3 色彩系统的全部 Token 字面量。Material Y
---

# Material Token 字面量

`cfmaterial_token_literals.h` 定义了 Material Design 3 色彩系统的全部 Token 字面量。Material You 的核心思想是让 UI 从用户的壁纸中提取颜色来生成主题，这些 Token 就是这套动态色彩系统的基础锚点。

我们没有直接用字符串字面量，而是把这些常量抽出来统一管理，原因有两个：一是避免拼写错误（"md.primary" 和 "md.pirmary" 编译器不会报错，但运行时就找不到了），二是方便 IDE 自动补全和重构。

## 色彩角色体系

Material Design 3 的色彩不是简单的调色板，而是基于"角色"（Role）的概念。每个颜色都有它特定的使用场景：

```cpp
#include "ui/core/token/material_scheme/cfmaterial_token_literals.h"

using namespace cf::ui::core::token::literals;

// Primary 色系 - 主品牌色，用于最重要的组件
const char* primary = PRIMARY;                  // "md.primary"
const char* onPrimary = ON_PRIMARY;              // "md.onPrimary"
const char* primaryContainer = PRIMARY_CONTAINER; // "md.primaryContainer"
const char* onPrimaryContainer = ON_PRIMARY_CONTAINER; // "md.onPrimaryContainer"

// Secondary 色系 - 次要强调色
const char* secondary = SECONDARY;
const char* onSecondary = ON_SECONDARY;

// Tertiary 色系 - 第三强调色，用于对比和平衡
const char* tertiary = TERTIARY;
const char* onTertiary = ON_TERTIARY;

// Error 色系 - 错误状态和危险操作
const char* error = ERROR;
const char* onError = ON_ERROR;
```text

注意那个 "On" 前缀——这不是"打开"的意思，而是"绘制在...之上"（On）。`ON_PRIMARY` 就是绘制在 Primary 颜色上的文字和图标的颜色，Material 的配色算法会自动计算对比度，保证可读性。

## Container 色系

Container 色是 Material You 新增的概念。它们是基色的"调色版本"——更浅或更深，用来承载对应颜色的内容：

```cpp
// 场景：一个卡片容器
// card_bg 使用 primaryContainer（浅调的主色）
// card_content 使用 primary（深调的主色）

// 场景：一个按钮
// button_bg 使用 primary
// button_text 使用 onPrimary（高对比度）

// 场景：一个强调区域
// area_bg 使用 tertiaryContainer
// area_icon 使用 onTertiaryContainer
```text

这样设计的好处是，组件的颜色关系由语义决定，而不是由具体的颜色值决定。动态换肤时，整个应用的颜色关系依然保持一致。

## Surface 色系

Surface 色系用于各种"表面"——背景、卡片、菜单等：

```cpp
// 基础背景
const char* background = BACKGROUND;            // 应用背景
const char* onBackground = ON_BACKGROUND;        // 背景上的文字

// 表面颜色
const char* surface = SURFACE;                   // 卡片、表单等表面
const char* onSurface = ON_SURFACE;              // 表面上的文字

// 表面变体
const char* surfaceVariant = SURFACE_VARIANT;    // 微差分的表面色
const char* onSurfaceVariant = ON_SURFACE_VARIANT; // 表面变体上的文字

// 边框颜色
const char* outline = OUTLINE;                   // 边框和分割线
const char* outlineVariant = OUTLINE_VARIANT;    // 微差分的边框色
```text

`surfaceVariant` 和 `outlineVariant` 这两个名字确实有点拗口。它们的作用是在深色模式下提供"不那么突兀"的边框和背景，避免视觉噪音过多。

## 特殊场景色系

还有几个用于特殊场景的颜色：

```cpp
// 阴影和遮罩
const char* shadow = SHADOW;                     // 投影颜色
const char* scrim = SCRIM;                       // 模态背景遮罩

// 反色场景（用于对话框、抽屉等浮层）
const char* inverseSurface = INVERSE_SURFACE;    // 反转的表面色
const char* inverseOnSurface = INVERSE_ON_SURFACE; // 反转表面上的文字
const char* inversePrimary = INVERSE_PRIMARY;     // 反转的主色
```text

`scrim` 是当弹出模态对话框时，后面那层半透明的黑色遮罩。深色模式下它可能是半透明的白色，取决于主题的动态生成逻辑。

## 在主题系统中使用

这些字面量最终会被主题系统解析成实际的颜色值：

```cpp
// 伪代码：主题系统如何使用这些 Token
class MaterialTheme {
    Color resolve(const char* token) const {
        // 实际实现会从动态生成的颜色表中查找
        return colorTable.at(token);
    }
};

// 使用示例
MaterialTheme theme;
auto buttonBg = theme.resolve(PRIMARY);
auto buttonText = theme.resolve(ON_PRIMARY);
auto cardBg = theme.resolve(SURFACE);
auto cardBorder = theme.resolve(OUTLINE);
```text

## 批量遍历

如果你需要遍历所有 Token（比如在主题编辑器中），提供了两个辅助：

```cpp
// 获取所有 Token 的数组
const char* const* allTokens = ALL_TOKENS;
size_t count = TOKEN_COUNT;  // 26 个

// 遍历所有 Token
for (size_t i = 0; i < TOKEN_COUNT; ++i) {
    printf("Token %zu: %s\n", i, ALL_TOKENS[i]);
}
```text

## Token 命名规则

所有 Token 遵循 `md.` 前缀的命名规则。这个 `md` 是 Material Design 的缩写，用来和其他设计系统（比如未来可能引入的自定义设计 Token）做区分。如果你要扩展自己的 Token，建议用不同的前缀避免冲突。

## 相关文档

- [Typography Token 字面量](../typography/cfmaterial_typography_token_literals.md)
- [Radius Scale Token 字面量](../radius_scale/cfmaterial_radius_scale_literals.md)
- [Motion Token 字面量](../motion/cfmaterial_motion_token_literals.md)
