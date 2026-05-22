---
title: "MaterialRadiusScale - Material 圆角系统"
description: 实现 Material Design 3 的圆角规范。Material 用 7 个标准圆角值覆盖几乎
---

# MaterialRadiusScale - Material 圆角系统

`MaterialRadiusScale` 实现 Material Design 3 的圆角规范。Material 用 7 个标准圆角值覆盖几乎所有 UI 场景，从完全方形到超大圆角都有定义。我们把它们存在嵌入式 Token 注册表中，每个圆角规格实例独立管理。

## 圆角规格

Material Design 3 定义了 7 个标准圆角：

| Token | 值 (dp) | 用途 |
|-------|---------|------|
| cornerNone | 0dp | 无圆角（矩形） |
| cornerExtraSmall | 4dp | Chip、小卡片 |
| cornerSmall | 8dp | 文本框、复选框 |
| cornerMedium | 12dp | 卡片 |
| cornerLarge | 16dp | 警告对话框 |
| cornerExtraLarge | 28dp | FAB、模态框 |
| cornerExtraExtraLarge | 32dp | 抽屉 |

这个设计很有意思——只要记住 7 个数字，就能保证整个 UI 的形状一致性。不需要每个组件单独设计圆角。

## 基本用法

通过工厂函数创建默认圆角系统：

```cpp
#include "material_factory.hpp"

// 创建默认圆角规格
auto radiusScale = cf::ui::core::material::defaultRadiusScale();

// 查询圆角
float smallRadius = radiusScale.queryRadiusScale("md.shape.cornerSmall");   // 8.0f
float mediumRadius = radiusScale.queryRadiusScale("md.shape.cornerMedium"); // 12.0f
float largeRadius = radiusScale.queryRadiusScale("md.shape.cornerLarge");   // 16.0f
```text

圆角名称采用 `md.shape.` 前缀，后跟 Material 官方定义的规格名称。

## 在 Qt 中使用

获取的圆角值可以直接用于 Qt 组件：

```cpp
void MyCard::setupAppearance() {
    auto* theme = getTheme();
    auto* radius = static_cast<MaterialRadiusScale*>(theme->radius_scale());

    float cardRadius = radius->queryRadiusScale("md.shape.cornerMedium");

    // 应用到样式表
    QString style = QString("QWidget { border-radius: %1px; }").arg(cardRadius);
    setStyleSheet(style);

    // 或者用于 QPainter
    m_borderRadius = cardRadius;
}

void MyCard::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect().adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);
}
```text

## 组件对应关系

不同组件对应不同圆角规格，Material 有推荐值：

```cpp
// 按钮用小圆角
float buttonRadius = radiusScale.queryRadiusScale("md.shape.cornerSmall"); // 8dp

// 卡片用中圆角
float cardRadius = radiusScale.queryRadiusScale("md.shape.cornerMedium"); // 12dp

// 对话框用大圆角
float dialogRadius = radiusScale.queryRadiusScale("md.shape.cornerLarge"); // 16dp

// FAB 用超大圆角
float fabRadius = radiusScale.queryRadiusScale("md.shape.cornerExtraLarge"); // 28dp

// 侧边栏用超大超大圆角
float drawerRadius = radiusScale.queryRadiusScale("md.shape.cornerExtraExtraLarge"); // 32dp
```text

## 自定义圆角

如果需要自定义圆角，可以直接修改 registry：

```cpp
MaterialRadiusScale radiusScale = material::defaultRadiusScale();

// 添加自定义圆角
radiusScale.registry().set("md.shape.cornerCustom", 20.0f);

// 覆盖现有圆角
radiusScale.registry().set("md.shape.cornerMedium", 16.0f); // 改成和 Large 一样
```text

这个设计让系统既支持标准 Material 规范，也允许根据品牌需求调整。

## 缓存机制

圆角查询有缓存层，避免重复解析字符串：

```cpp
// 第一次查询会查找并缓存
float r1 = radiusScale.queryRadiusScale("md.shape.cornerLarge");

// 后续查询从缓存返回
float r2 = radiusScale.queryRadiusScale("md.shape.cornerLarge");
```text

缓存在 `MaterialRadiusScale` 对象生命周期内有效。

## 单位说明

圆角值以 `dp`（Density-independent Pixels）为单位。在我们的实现中直接返回浮点数，调用方负责根据 DPI 缩放：

```cpp
// 获取 dp 值
float radiusDp = radiusScale.queryRadiusScale("md.shape.cornerMedium");

// 转换为物理像素
qreal scaleFactor = devicePixelRatioF();
float radiusPx = radiusDp * scaleFactor;
```text

大多数情况下直接用 dp 值传入 Qt 函数就可以，Qt 会自动处理 DPI 缩放。

## 高 DPI 注意事项

在高 DPI 屏幕上，圆角值可能需要特殊处理：

```cpp
// 确保 QPainter 开启抗锯齿
QPainter painter(this);
painter.setRenderHint(QPainter::Antialiasing);

// 使用 float 精度的绘制函数
painter.drawRoundedRect(rect, radius, radius);
```text

小圆角值（如 4dp 的 ExtraSmall）在高 DPI 下可能会被平滑成几乎直线，这是正常行为。

## 相关文档

- [MaterialTheme - 主题组合](./cfmaterial_theme.md)
- [MaterialColorScheme - 颜色方案](./cfmaterial_scheme.md)
- [MaterialTypography - 字体系统](./cfmaterial_fonttype.md)
