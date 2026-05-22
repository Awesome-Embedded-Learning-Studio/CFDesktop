---
title: "geometryhelper - 几何助手"
description: 提供了创建圆角矩形的工具函数，封装了 Material Design 的形状规范。Qt 原生的  只
---

# geometry_helper - 几何助手

`geometry_helper` 提供了创建圆角矩形的工具函数，封装了 Material Design 的形状规范。Qt 原生的 `QPainterPath::addRoundedRect()` 只能统一设置圆角半径，而我们需要单独控制每个角，所以自己封装了一层。

## Material Design 形状规范

Material Design 定义了标准的圆角尺寸，我们用 `ShapeScale` 枚举表示：

```cpp
#include "ui/base/geometry_helper.h"

using namespace cf::ui::base::geometry;

// 无圆角
QPainterPath path1 = roundedRect(rect, ShapeScale::ShapeNone);

// 小圆角 (8dp) - 适合按钮、卡片
QPainterPath path2 = roundedRect(rect, ShapeScale::ShapeSmall);

// 大圆角 (16dp) - 适合底部表单、对话框
QPainterPath path3 = roundedRect(rect, ShapeScale::ShapeLarge);

// 完全圆角 (50%) - 变成椭圆/胶囊形
QPainterPath path4 = roundedRect(rect, ShapeScale::ShapeFull);
```text

这些预设值来自 Material Design 3 的形状规范，确保整个应用的视觉语言一致。

## 自定义圆角半径

如果预设值不满足需求，可以直接指定像素半径：

```cpp
QRectF buttonRect(0, 0, 120, 40);

// 统一圆角 8px
QPainterPath buttonPath = roundedRect(buttonRect, 8.0f);

// 大圆角，营造现代感
QPainterPath cardPath = roundedRect(cardRect, 16.0f);
```text

⚠️ 半径单位是像素，不是 dp。在高 DPI 屏幕上需要乘以设备像素比，否则看起来会太小。

## 独立控制四个角

某些场景需要单独控制每个角的圆角，比如顶部只有两个圆角的底部表单：

```cpp
// 底部表单：顶部圆角，底部直角
QPainterPath sheetPath = roundedRect(sheetRect,
                                     16.0f,   // topLeft
                                     16.0f,   // topRight
                                     0.0f,    // bottomLeft
                                     0.0f);   // bottomRight

// 气泡对话框风格
QPainterPath bubblePath = roundedRect(bubbleRect,
                                      4.0f,    // topLeft
                                      16.0f,   // topRight
                                      16.0f,   // bottomLeft
                                      4.0f);   // bottomRight
```text

这种用法在做聊天气泡、底部抽屉之类的组件时特别常见。Qt 原生 API 需要手动构建 path，我们的封装简化了这个过程。

⚠️ 如果圆角半径超过矩形尺寸的一半，Qt 会自动裁剪。想做胶囊形状应该用 `ShapeFull`，而不是手动设置一个超大半径。

## 使用场景

圆角路径准备好之后，可以直接用于绘制：

```cpp
void paintEvent(QPaintEvent* event) override {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 创建圆角路径
    QPainterPath path = roundedRect(rect(), ShapeScale::ShapeMedium);

    // 填充
    painter.fillPath(path, QColor("#FFFFFF"));

    // 描边
    painter.setPen(QPen(QColor("#E0E0E0"), 1.0));
    painter.drawPath(path);
}
```text

记得启用 `Antialiasing`，否则圆角会有明显的锯齿。

## 相关文档

- [easing - 缓动曲线](./easing.md)
- [color - HCT 色彩空间](./color.md)
