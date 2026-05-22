---
title: 几何与设备无关——跨 DPI 适配的完整方案
description: 如果你在 1080p 的屏幕上设计了一个 40 像素高的按钮，放到 4K 屏幕上会怎么样？答案是：按
---

# 几何与设备无关——跨 DPI 适配的完整方案

如果你在 1080p 的屏幕上设计了一个 40 像素高的按钮，放到 4K 屏幕上会怎么样？答案是：按钮会变得非常小，几乎点不到。

这是桌面 UI 开发中最经典的问题之一：不同 DPI 的屏幕上，UI 元素的大小应该如何保持一致？Qt 提供了高 DPI 支持，但它的 API 设计...怎么说呢，有些历史包袱。我们在 Material Framework 里决定自己封装一套更简洁的方案。

## 问题背景：1080p 到 4K

首先说一下基本概念。DPI（Dots Per Inch）是屏幕的像素密度，1080p 通常是 96 DPI，而 4K 可能是 192 DPI 甚至更高。devicePixelRatio 是操作系统报告的缩放因子，96 DPI 时是 1.0，192 DPI 时是 2.0。

如果你直接用"像素"作为单位，那么在 4K 屏幕上（假设 devicePixelRatio=2.0），一个 40 像素的按钮实际上只相当于 1080p 上的 20 像素——因为物理像素更小了。

## 设备无关单位

Material Design 使用 dp（device-independent pixel，设备无关像素）作为基本单位。1dp 在 160 DPI 的屏幕上等于 1 物理像素，在 320 DPI 的屏幕上等于 2 物理像素。

另外还有一个 sp（scalable pixel，可缩放像素），用于字体大小。sp 和 dp 类似，但会额外考虑用户设置的字体缩放偏好。

## CanvasUnitHelper 的实现

我们做了一个 CanvasUnitHelper 结构体，用来在 dp/sp/px 之间转换：

```cpp
struct CanvasUnitHelper {
    CanvasUnitHelper(const qreal devicePixelRatio);
    qreal dpToPx(qreal dp) const;
    qreal spToPx(qreal sp) const;
    qreal pxToDp(qreal px) const;
    qreal dpi() const;

private:
    qreal devicePixelRatio;
};
```text

转换逻辑很简单：

```cpp
qreal dpToPx(qreal dp) const {
    return dp * devicePixelRatio;
}

qreal spToPx(qreal sp) const {
    // sp 会考虑用户字体缩放偏好
    QFont font = QApplication::font();
    qreal fontScale = font.pointSizeF() / 10.0; // 假设默认 10pt
    return sp * devicePixelRatio * fontScale;
}
```text

这里有个坑：Windows 上获取 devicePixelRatio 的方式经历了多次变迁。早期版本用 `QScreen::devicePixelRatio()`，但这个值在 Windows 10 1709 之后的"缩放与布局"设置下可能不准确。现在推荐用 `QScreen::logicalDotsPerInch()` 除以 96 来计算。

## 响应式断点

Material Design 定义了一套响应式断点，根据窗口宽度来决定布局：

```cpp
enum class BreakPoint {
    Compact,  // < 600dp
    Medium,   // 600dp - 839dp
    Expanded  // >= 840dp
};
```text

这个设计很有意思：Material 不是针对具体设备（手机/平板/桌面）分类，而是针对"可用宽度"分类。一个桌面窗口如果缩得很窄，也应该用 Compact 布局。

```cpp
BreakPoint breakPoint(qreal widthDp) {
    if (widthDp < 600.0) {
        return BreakPoint::Compact;
    } else if (widthDp < 840.0) {
        return BreakPoint::Medium;
    } else {
        return BreakPoint::Expanded;
    }
}
```text

## 圆角矩形工具

Qt 的 QPainterPath 确实支持圆角矩形，但 API 有点繁琐。你需要先创建一个 QPainterPath，然后调用 `addRoundedRect()`，而且这个函数的参数是 xRadius 和 yRadius，不太符合 Material 的"统一圆角"语义。

所以我们封装了一下：

```cpp
// 使用 Material 预定义的圆角尺寸
QPainterPath roundedRect(const QRectF& rect, ShapeScale scale);

// 自定义统一圆角
QPainterPath roundedRect(const QRectF& rect, float radius);

// 每个角单独指定
QPainterPath roundedRect(const QRectF& rect, float topLeft, float topRight,
                         float bottomLeft, float bottomRight);
```text

ShapeScale 枚举对应 Material 的标准圆角尺寸：

```cpp
enum class ShapeScale {
    ShapeNone,       // 0dp
    ShapeExtraSmall, // 4dp
    ShapeSmall,      // 8dp
    ShapeMedium,     // 12dp
    ShapeLarge,      // 16dp
    ShapeExtraLarge, // 28dp
    ShapeFull        // 50% of size
};
```text

这里有个需要注意的地方：ShapeFull 是"完全圆角"，也就是变成一个胶囊或圆形。这种情况下圆角半径是矩形短边的一半。我们在实现时需要特殊处理：

```cpp
if (scale == ShapeScale::ShapeFull) {
    radius = std::min(rect.width(), rect.height()) / 2.0f;
}
```text

## 为什么不直接用 QSS？

Qt 支持 QSS（Qt Style Sheets），类似 CSS，可以在样式表里定义圆角、边距等。那为什么我们不直接用 QSS？

有几个原因：

1. **QSS 的功能有限**：它不支持复杂的绘制逻辑，比如涟漪效果、多层阴影。
2. **动态切换困难**：QSS 的重新加载性能不好，而且无法在运行时精确控制某个属性。
3. **Material 规范的复杂度**：Material 的状态层透明度、动画时长等都无法用 QSS 表达。

所以我们选择在 paintEvent 里完全接管绘制，而 QSS 只用于一些全局的、不常变化的属性（如果有的话）。

## 实际使用示例

在控件中，你会这样使用 CanvasUnitHelper：

```cpp
void Button::paintEvent(QPaintEvent* event) {
    CanvasUnitHelper helper(qApp->devicePixelRatio());

    // Material 按钮高度固定 40dp
    qreal buttonHeight = helper.dpToPx(40.0);

    // 水平内边距 24dp
    qreal paddingH = helper.dpToPx(24.0);

    // 圆角半径
    qreal radius = helper.dpToPx(cornerRadius());
}
```yaml

这样无论在什么 DPI 的屏幕上，按钮的视觉大小都是一致的。

## 总结

到这里，ui/base 层的基础工具就介绍完了。我们有了：

- HCT 色彩空间支持
- 数学工具函数（lerp、clamp、贝塞尔、弹簧）
- Material 标准缓动曲线
- 设备无关单位转换
- 圆角矩形工具

这些工具是纯数学运算，不依赖任何 UI 框架的特定 API。它们可以被单独编译、单独测试，也可以在任何需要的地方复用。

但有了工具只是第一步，接下来我们需要把这些工具组织成一个完整的主题系统。Material Design 3 的主题不是简单的颜色集合，而是一套由 Token 驱动的、支持动态切换的、可扩展的架构。

接下来，我们进入 Layer 2：Theme Engine Layer。

---

**相关文档**

- [为什么我们需要自己的数学层](./01-why-we-need-own-math-layer.md)——基础层设计动机
- [HCT 色彩空间实战](./02-color-system-hct.md)——颜色系统的数学基础
- [主题系统架构设计](../layer-2-theme-engine/01-theme-system-design.md)——下一层的入口
