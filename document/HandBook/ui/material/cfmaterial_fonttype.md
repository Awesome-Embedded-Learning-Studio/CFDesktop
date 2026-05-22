---
title: "MaterialTypography - Material 字体系统"
description: 实现 Material Design 3 的 Type Scale 系统。Material 把字体规
---

# MaterialTypography - Material 字体系统

`MaterialTypography` 实现 Material Design 3 的 Type Scale 系统。Material 把字体规格分成 5 大类、15 个具体样式，每个样式都有精确的大小、字重和行高规定。我们用嵌入式 Token 注册表存储这些 QFont 对象，每个排版实例独立管理自己的字体配置。

## Type Scale 规格

Material Design 3 定义了完整的 Type Scale：

| 类别 | 样式 | 大小 | 字重 | 行高 | 用途 |
|------|------|------|------|------|------|
| Display | displayLarge | 57sp | Regular | 64sp | 英雄内容 |
| Display | displayMedium | 45sp | Regular | 52sp | 英雄内容 |
| Display | displaySmall | 36sp | Regular | 44sp | 英雄内容 |
| Headline | headlineLarge | 32sp | Regular | 40sp | 应用栏重要文本 |
| Headline | headlineMedium | 28sp | Regular | 36sp | 应用栏重要文本 |
| Headline | headlineSmall | 24sp | Regular | 32sp | 应用栏重要文本 |
| Title | titleLarge | 22sp | Medium | 28sp | 章节标题 |
| Title | titleMedium | 16sp | Medium | 24sp | 章节标题 |
| Title | titleSmall | 14sp | Medium | 20sp | 章节标题 |
| Body | bodyLarge | 16sp | Regular | 24sp | 正文内容 |
| Body | bodyMedium | 14sp | Regular | 20sp | 正文内容 |
| Body | bodySmall | 12sp | Regular | 16sp | 正文内容 |
| Label | labelLarge | 14sp | Medium | 20sp | 次要信息 |
| Label | labelMedium | 12sp | Medium | 16sp | 次要信息 |
| Label | labelSmall | 11sp | Medium | 16sp | 次要信息 |

## 基本用法

通过工厂函数创建默认排版系统：

```cpp
#include "material_factory.hpp"

// 创建默认排版
auto typography = cf::ui::core::material::defaultTypography();

// 查询字体
QFont titleFont = typography.queryTargetFont("md.typography.titleLarge");
QFont bodyFont = typography.queryTargetFont("md.typography.bodyMedium");
```text

字体名称采用 `md.typography.` 前缀，后跟 Material 官方定义的样式名称。

## 字体组访问

可以通过类型安全的访问器获取字体组：

```cpp
MaterialTypography typography = material::defaultTypography();

// 获取标题组
TitleFonts title = typography.title();
// 这些 Token 类型配合 Token Registry 使用

// 获取正文组
BodyFonts body = typography.body();
```text

字体组结构体（`DisplayFonts`、`HeadlineFonts` 等）主要是为了类型安全和代码可读性。

## 行高查询

每个字体样式都有对应的行高规范：

```cpp
MaterialTypography typography = material::defaultTypography();

float lineHeight = typography.getLineHeight("md.typography.bodyLarge");
// 返回 24.0（单位 sp）
```bash

行高在多行文本布局时特别重要，确保行与行之间有合适的呼吸空间。

## 平台字体选择

我们根据平台自动选择合适的系统字体：

| 平台 | 默认字体 | 中文回退 |
|------|----------|----------|
| Windows | Segoe UI | Microsoft YaHei UI |
| macOS | .SF NS Text | PingFang SC |
| Linux | Ubuntu | Noto Sans CJK SC |

这样设计的好处是应用在各平台上看起来"原生"，同时保持一致的排版比例。如果需要自定义字体，可以直接修改 registry：

```cpp
MaterialTypography typography = material::defaultTypography();

QFont customFont("Roboto");
customFont.setPointSize(16);
typography.registry().set("md.typography.bodyMedium", customFont);
```text

## 缓存机制

字体查询有缓存层，避免重复解析字符串：

```cpp
// 第一次查询会查找并缓存
QFont font1 = typography.queryTargetFont("md.typography.titleLarge");

// 后续查询从缓存返回
QFont font2 = typography.queryTargetFont("md.typography.titleLarge");
```text

缓存在 `MaterialTypography` 对象生命周期内有效。

## 在 Qt 中使用

获取的 `QFont` 可以直接用于 Qt 组件：

```cpp
void MyLabel::updateFont() {
    auto* theme = getTheme();
    auto* typography = static_cast<MaterialTypography*>(theme->font_type());

    QFont titleFont = typography->queryTargetFont("md.typography.titleLarge");
    setFont(titleFont);
}
```text

也可以直接设置到 `QPainter`：

```cpp
void paintEvent(QPaintEvent*) {
    QPainter painter(this);

    auto* typography = getMaterialTypography();
    QFont bodyFont = typography->queryTargetFont("md.typography.bodyMedium");
    painter.setFont(bodyFont);

    painter.drawText(rect(), "Hello Material");
}
```text

## 大小单位说明

Material 规范使用 `sp`（Scale-independent Pixels）作为字体大小单位。`sp` 和 `dp` 类似，但会根据用户设置的字体大小缩放。在 Qt 中，我们用 `pointSize` 来近似实现 `sp` 的行为：

```cpp
// Material 规范：16sp
QFont font;
font.setPointSize(16); // Qt 中用 pointSize 近似
```text

这个近似在实际使用中效果足够好，因为 Qt 的字体系统已经有良好的 DPI 处理。

## 相关文档

- [MaterialTheme - 主题组合](./cfmaterial_theme.md)
- [MaterialColorScheme - 颜色方案](./cfmaterial_scheme.md)
- [MaterialRadiusScale - 圆角系统](./cfmaterial_radius_scale.md)
