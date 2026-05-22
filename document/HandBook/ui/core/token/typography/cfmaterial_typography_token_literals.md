---
title: Typography Token 字面量
description: 定义了 Material Design 3 排版系统的全部 Token 字面量。Material 的
---

# Typography Token 字面量

`cfmaterial_typography_token_literals.h` 定义了 Material Design 3 排版系统的全部 Token 字面量。Material 的字体规范不是简单的"字号/行高"，而是一套完整的视觉层次系统，每个样式都有明确的用途场景。

这套字面量让我们可以用语义化的方式引用字体样式，而不是硬编码 "16sp" 这种魔法数字。更重要的是，当需要调整字体规范时，只需修改主题配置，不需要改动业务代码。

## 五大类型体系

Material Design 3 把文字样式分为五个类型：Display、Headline、Title、Body、Label。每个类型下有 Large/Medium/Small 三个变体，共 15 个样式：

```cpp
#include "ui/core/token/typography/cfmaterial_typography_token_literals.h"

using namespace cf::ui::core::token::literals;

// Display - 英雄内容，用于非常大的展示性文字
const char* displayLarge = TYPOGRAPHY_DISPLAY_LARGE;   // 57sp, 落地页标题
const char* displayMedium = TYPOGRAPHY_DISPLAY_MEDIUM; // 45sp
const char* displaySmall = TYPOGRAPHY_DISPLAY_SMALL;   // 36sp

// Headline - 应用栏重要文字
const char* headlineLarge = TYPOGRAPHY_HEADLINE_LARGE; // 32sp, 页面主标题
const char* headlineMedium = TYPOGRAPHY_HEADLINE_MEDIUM; // 28sp
const char* headlineSmall = TYPOGRAPHY_HEADLINE_SMALL; // 24sp, 次级标题

// Title - 分区标题
const char* titleLarge = TYPOGRAPHY_TITLE_LARGE;       // 22sp, 区块标题
const char* titleMedium = TYPOGRAPHY_TITLE_MEDIUM;     // 16sp, 卡片标题
const char* titleSmall = TYPOGRAPHY_TITLE_SMALL;       // 14sp, 小标题

// Body - 主要内容
const char* bodyLarge = TYPOGRAPHY_BODY_LARGE;        // 16sp, 正文
const char* bodyMedium = TYPOGRAPHY_BODY_MEDIUM;      // 14sp, 次要正文
const char* bodySmall = TYPOGRAPHY_BODY_SMALL;        // 12sp, 辅助文字

// Label - 次要信息，按钮和标签文字
const char* labelLarge = TYPOGRAPHY_LABEL_LARGE;      // 14sp, 按钮文字
const char* labelMedium = TYPOGRAPHY_LABEL_MEDIUM;    // 12sp, 标签
const char* labelSmall = TYPOGRAPHY_LABEL_SMALL;      // 11sp, 小标签
```bash

## 字体属性说明

每个 Typography Token 不仅仅是字号，还包含字重和行高：

| Token | 字号 | 字重 | 行高 | 典型用途 |
|-------|------|------|------|----------|
| Display Large | 57sp | 400 | 64sp | 落地页 Hero 标题 |
| Headline Large | 32sp | 400 | 40sp | 应用栏标题 |
| Title Large | 22sp | 500 | 28sp | 分区标题 |
| Body Large | 16sp | 400 | 24sp | 正文内容 |
| Label Large | 14sp | 500 | 20sp | 按钮文字 |

注意到 Title 和 Label 系列的字重是 500（Medium），比其他系列的 400（Regular）更重。这是刻意的设计——标题和标签需要更强的视觉权重来引导注意力。

## 在主题系统中使用

排版 Token 的使用方式和颜色 Token 类似，由主题系统解析成实际的字体属性：

```cpp
// 伪代码：主题系统如何解析排版 Token
struct TextStyle {
    float fontSize;    // sp
    int fontWeight;    // 100-900
    float lineHeight;  // sp
};

class MaterialTheme {
    TextStyle resolveTypography(const char* token) const {
        return typographyTable.at(token);
    }
};

// 使用示例
MaterialTheme theme;

// 设置按钮文字
auto buttonStyle = theme.resolveTypography(TYPOGRAPHY_LABEL_LARGE);
button.setFontSize(buttonStyle.fontSize);
button.setFontWeight(buttonStyle.fontWeight);

// 设置正文
auto bodyStyle = theme.resolveTypography(TYPOGRAPHY_BODY_MEDIUM);
textLabel.setTextStyle(bodyStyle);
```text

## 行高 Token

除了排版 Token，还提供了独立的行高 Token。这看起来有点冗余——为什么行高不能包含在 Typography Token 里？

原因是一些平台或渲染引擎可能需要单独设置行高属性。把它拆出来可以提供更灵活的配置：

```cpp
// 行高 Token
const char* lineHeightBody = LINEHEIGHT_BODY_MEDIUM;  // "md.lineHeight.bodyMedium"
const char* lineHeightTitle = LINEHEIGHT_TITLE_LARGE; // "md.lineHeight.titleLarge"

// 使用场景：某些框架需要分别设置
theme.setTypography(TYPOGRAPHY_BODY_MEDIUM);
theme.setLineHeight(LINEHEIGHT_BODY_MEDIUM);
```text

不过在我们的推荐使用方式中，行高应该由 Typography Token 统一管理，单独使用行高 Token 是比较边缘的场景。

## 选择合适的样式

选择哪个样式有时会让人犹豫，这里有一些经验法则：

```cpp
// 场景 1：页面主标题
// 选 Headline Large，而不是 Display Large
// Display 太大了，除非是落地页那种非常突出的 Hero 区域

// 场景 2：卡片标题
// 选 Title Medium（16sp），而不是 Headline Small
// Headline 是给应用栏那种页面级标题用的

// 场景 3：正文内容
// 选 Body Medium（14sp），这是最常用的正文尺寸
// 如果想要更突出的内容，用 Body Large（16sp）

// 场景 4：按钮文字
// 选 Label Large（14sp），这是专门为按钮设计的
// 虽然尺寸和 Body Medium 一样，但字重不同（500 vs 400）

// 场景 5：时间戳、元数据
// 选 Label Small（11sp）或 Body Small（12sp）
// Label Small 字重更重，适合标签；Body Small 更轻，适合辅助信息
```text

## 批量遍历

和颜色 Token 一样，提供了遍历所有排版 Token 的辅助：

```cpp
// 遍历所有 Typography Token
for (size_t i = 0; i < TYPOGRAPHY_TOKEN_COUNT; ++i) {
    printf("Typography Token %zu: %s\n", i, ALL_TYPOGRAPHY_TOKENS[i]);
}

// 遍历所有 LineHeight Token
for (size_t i = 0; i < LINEHEIGHT_TOKEN_COUNT; ++i) {
    printf("LineHeight Token %zu: %s\n", i, ALL_LINEHEIGHT_TOKENS[i]);
}
```text

## 可访问性考虑

Material Design 的字体规范考虑了可访问性。最小字号是 11sp（Label Small），这个尺寸在大多数设备上仍然可读。如果你的用户群体包含视力障碍用户，可以在主题配置中整体放大字号，同时保持样式之间的比例关系。

## 相关文档

- [Material Token 字面量](../material_scheme/cfmaterial_token_literals.md)
- [Radius Scale Token 字面量](../radius_scale/cfmaterial_radius_scale_literals.md)
- [Motion Token 字面量](../motion/cfmaterial_motion_token_literals.md)
