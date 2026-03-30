# Label - Material 标签

`Label` 是 Material Design 3 标签控件的完整实现，支持 15 种排版样式（Display、Headline、Title、Body、Label）和 9 种颜色变体。完全集成主题系统，自动响应主题变化。

## 类参考

```cpp
namespace cf::ui::widget::material;

class Label : public QLabel {
    Q_OBJECT
    Q_PROPERTY(TypographyStyle typographyStyle READ typographyStyle WRITE setTypographyStyle)
    Q_PROPERTY(LabelColorVariant colorVariant READ colorVariant WRITE setColorVariant)
    Q_PROPERTY(bool autoHiding READ autoHiding WRITE setAutoHiding)
};
```

头文件：`ui/widget/material/widget/label/label.h`

## 排版样式

Material Design 3 定义了 15 种排版样式，分属 5 个类别：

```cpp
enum class TypographyStyle {
    // 展示样式（57sp, 45sp, 36sp）- 用于英雄内容
    DisplayLarge, DisplayMedium, DisplaySmall,

    // 标题样式（32sp, 28sp, 24sp）- 用于应用栏文本
    HeadlineLarge, HeadlineMedium, HeadlineSmall,

    // 标题样式（22sp, 16sp, 14sp）- 用于节标题
    TitleLarge, TitleMedium, TitleSmall,

    // 正文样式（16sp, 14sp, 12sp）- 用于主要内容
    BodyLarge, BodyMedium, BodySmall,

    // 标签样式（14sp, 12sp, 11sp）- 用于辅助信息
    LabelLarge, LabelMedium, LabelSmall
};
```

## 颜色变体

9 种颜色变体对应 Material Design 3 的语义颜色角色：

```cpp
enum class LabelColorVariant {
    OnSurface,        // 默认 on-surface 颜色
    OnSurfaceVariant, // 变体 on-surface 颜色
    Primary,          // 主品牌颜色
    OnPrimary,        // 主颜色上的文本
    Secondary,        // 次品牌颜色
    OnSecondary,      // 次颜色上的文本
    Error,            // 错误颜色
    OnError,          // 错误颜色上的文本
    InverseSurface,   // 反转表面颜色
    InverseOnSurface  // 反转表面上的文本
};
```

## 基本用法

```cpp
#include "widget/material/widget/label/label.h"

using namespace cf::ui::widget::material;

// 创建标签（默认 BodyMedium 样式）
auto* label = new Label("Hello World", this);

// 指定排版样式
auto* title = new Label("Settings", TypographyStyle::HeadlineSmall, this);

// 设置颜色变体
title->setColorVariant(LabelColorVariant::Primary);

// 创建展示文本
auto* hero = new Label("Welcome", TypographyStyle::DisplayLarge, this);
```

## 排版样式选择指南

| 类别 | 使用场景 | 可选值 |
|------|----------|--------|
| Display | 首屏英雄内容、超大标题 | DisplayLarge / Medium / Small |
| Headline | 应用栏、页面主标题 | HeadlineLarge / Medium / Small |
| Title | 卡片标题、列表项标题 | TitleLarge / Medium / Small |
| Body | 正文内容、描述文本 | BodyLarge / Medium / Small |
| Label | 按钮、标签、辅助文本 | LabelLarge / Medium / Small |

默认样式为 `BodyMedium`，适用于大多数正文场景。

## 自动隐藏

当标签文本为空时，可以自动隐藏标签：

```cpp
label->setAutoHiding(true);

// 当 text 为空时，label->hide() 自动调用
// 当 text 非空时，label->show() 自动调用
```

这在动态内容的场景下很有用，比如错误提示标签在没有错误时不占用布局空间。

## 颜色与主题

Label 从当前主题中自动获取颜色和字体：

```cpp
// 颜色根据 colorVariant 从主题获取
// 禁用状态下，文本透明度降至 38%

// 字体根据 typographyStyle 从主题获取对应的排版令牌
// 查询通过 typographyTokenName() 转换样式为令牌名称
```

为了优化性能，Label 缓存了最近查询的颜色值（`cachedColor_`），避免重复的主题查询。

## 交互状态

Label 本身是静态控件，但禁用状态有特殊处理：

| 状态 | 视觉效果 |
|------|----------|
| Enabled | 正常颜色和字体 |
| Disabled | 文本透明度降至 38% |

## 主要方法

| 方法 | 说明 |
|------|------|
| `typographyStyle()` / `setTypographyStyle(TypographyStyle)` | 获取/设置排版样式 |
| `colorVariant()` / `setColorVariant(LabelColorVariant)` | 获取/设置颜色变体 |
| `autoHiding()` / `setAutoHiding(bool)` | 获取/设置自动隐藏 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [Material Design 3 排版规范](https://m3.material.io/styles/typography)
