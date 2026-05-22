---
title: "RippleHelper - Material 涟漪效果助手"
description: 是 Material Design 水波纹效果的完整实现。当用户点击 Material 控件时，从点
---

# RippleHelper - Material 涟漪效果助手

`RippleHelper` 是 Material Design 水波纹效果的完整实现。当用户点击 Material 控件时，从点击位置扩散出的圆形涟漪能提供即时的触觉反馈。我们选择单独实现这个组件，是因为 Qt 的默认效果反馈太"僵硬"，而 Material 的涟漪需要精确的扩散半径计算和淡入淡出时序控制。

## 效果原理

Material 涟漪效果的本质是一个从点击中心向外扩散的圆形渐变。按下时涟漪从零半径扩展到最大半径（覆盖整个控件），松开时透明度逐渐淡出直到消失。扩散和淡出两个动画同时进行，松开越早淡出越明显。

最大半径的计算方式是从点击中心到控件最远角点的距离，这样确保无论点击哪里，涟漪都能完全覆盖控件：

```cpp
float maxRadius(const QRectF& rect, const QPointF& center) {
    // 计算到四个角点的距离，取最大值
    float d1 = std::hypot(center.x() - rect.topLeft().x(), center.y() - rect.topLeft().y());
    float d2 = std::hypot(center.x() - rect.topRight().x(), center.y() - rect.topRight().y());
    float d3 = std::hypot(center.x() - rect.bottomLeft().x(), center.y() - rect.bottomLeft().y());
    float d4 = std::hypot(center.x() - rect.bottomRight().x(), center.y() - rect.bottomRight().y());
    return std::max({d1, d2, d3, d4});
}
```text

## 基本用法

在控件构造函数中初始化 `RippleHelper`，需要传入动画工厂：

```cpp
#include "widget/material/base/ripple_helper.h"

using namespace cf::ui::widget::material;

class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 获取动画工厂
        auto animationFactory = cf::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
            Application::animationFactory()
        );

        // 创建涟漪助手
        m_rippleHelper = new base::RippleHelper(animationFactory, this);

        // 设置涟漪颜色（通常使用控件的主色调）
        m_rippleHelper->setColor(cf::ui::base::CFColor::fromRGB(100, 100, 255));

        // 监听重绘请求
        connect(m_rippleHelper, &base::RippleHelper::repaintNeeded,
                this, QOverload<>::of(&MyWidget::update));
    }

private:
    base::RippleHelper* m_rippleHelper;
};
```text

## 事件处理

将鼠标事件转发给 `RippleHelper` 来驱动涟漪动画：

```cpp
void MyWidget::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    m_rippleHelper->onPress(event->pos(), rect());
    update();
}

void MyWidget::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    m_rippleHelper->onRelease();
    update();
}
```text

`onCancel()` 用于取消未释放的涟漪，比如鼠标拖出控件范围时：

```cpp
void MyWidget::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    m_rippleHelper->onCancel();  // 清除所有涟漪
    update();
}
```text

⚠️ `onCancel()` 会立即清除所有涟漪，不做淡出动画。这是有意为之的设计——当用户明确"取消"交互时，不需要延迟反馈。

## 渲染模式

`RippleHelper` 支持两种渲染模式，区别在于是否裁剪涟漪到控件边界：

```cpp
enum class Mode {
    Bounded,   // 涟漪被裁剪到控件形状内（默认）
    Unbounded  // 涟漪可以超出控件边界
};

// 设置模式
m_rippleHelper->setMode(base::RippleHelper::Mode::Bounded);
```text

大多数控件应该使用 `Bounded` 模式，让涟漪限制在圆角矩形内。`Unbounded` 模式适用于特殊场景，比如浮动操作按钮（FAB）的涟漪可以扩散到圆形边界外。

## 绘制涟漪

在 `paintEvent` 中，涟漪应该绘制在状态层之上、内容之下：

```cpp
void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    p.fillPath(shape, backgroundColor());

    // 绘制状态层（如果有的话）
    // drawStateLayer(p, shape);

    // 绘制涟漪
    QPainterPath clipPath;
    clipPath.addRoundedRect(rect(), cornerRadius, cornerRadius);
    m_rippleHelper->paint(&p, clipPath);

    // 绘制内容
    // drawContent(p);
}
```text

## 颜色设置

涟漪颜色通常使用控件内容颜色（label color）的半透明版本：

```cpp
// 从主题获取颜色
auto* theme = getTheme();
auto* colors = static_cast<MaterialColorScheme*>(theme->color_scheme());
QColor labelColor = colors->queryExpectedColor("md.onSurface");

// 设置涟漪颜色
m_rippleHelper->setColor(cf::ui::base::CFColor(labelColor));
```text

在浅色主题上使用深色涟漪，深色主题上使用浅色涟漪，这是确保对比度的基本要求。

## 渲染细节

`RippleHelper` 内部使用径向渐变（`QRadialGradient`）绘制涟漪，中心实心、边缘柔和渐变到透明：

```cpp
// 内部实现（简化）
QRadialGradient gradient(ripple.center, ripple.radius);
gradient.setColorAt(0.0f, color);      // 中心完全实色
gradient.setColorAt(0.7f, color);      // 70% 半径处仍是实色
gradient.setColorAt(1.0f, transparent); // 边缘渐变到透明
```text

这种处理避免了锯齿边缘，使涟漪看起来更自然。

## 性能考虑

涟漪效果依赖动画工厂，如果全局动画被禁用，`onPress()` 会直接返回，不创建任何涟漪：

```cpp
// 禁用所有动画（包括涟漪）
auto factory = Application::animationFactory();
if (factory) {
    factory->setEnabledAll(false);
}
```bash

这对于低端设备或性能敏感的场景很有用。另外，`hasActiveRipple()` 可以用来判断是否需要重绘，避免无效的 `paintEvent` 调用。

## 动画名称

`RippleHelper` 使用动画工厂中注册的以下动画：

| 名称 | 用途 | 范围 |
|------|------|------|
| `md.animation.rippleExpand` | 涟漪扩散 | 0.0 → 1.0 |
| `md.animation.rippleFade` | 涟漪淡出 | 1.0 → 0.0 |

确保动画工厂中注册了这两个动画，否则涟漪不会显示。

## 相关文档

- [StateMachine - Material 状态机](../widget/state_machine.md)
- [PainterLayer - 绘图层管理器](./painter_layer.md)
- [CFMaterialAnimationFactory - 动画工厂](../animation/cfmaterial_animation_factory.md)
