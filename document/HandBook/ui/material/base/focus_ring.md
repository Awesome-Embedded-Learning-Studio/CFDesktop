---
title: "MdFocusIndicator - Material 焦点环"
description: 是 Material Design 3 键盘焦点可视化的核心组件。它负责在控件获得键盘焦点时绘制聚焦
---

# MdFocusIndicator - Material 焦点环

`MdFocusIndicator` 是 Material Design 3 键盘焦点可视化的核心组件。它负责在控件获得键盘焦点时绘制聚焦环，并提供流畅的进出动画。我们单独实现这个组件，是因为 Qt 自带的焦点样式（`QWidget::setFocusPolicy` 配合样式表）无法实现 Material 规范的动画效果和精确尺寸控制。

## 可访问性要求

聚焦环不是装饰，是可访问性的核心组成部分。Material Design 要求所有可通过键盘交互的控件都必须有明显的焦点指示器，这对于视障用户和键盘导航用户至关重要。WCAG 2.1 AAA 级别要求焦点指示器与周围背景的对比度至少为 3:1，Material 的聚焦环设计满足了这一要求。

## Material Design 规范

Material Design 3 对聚焦环的尺寸有明确约定：

- **环宽**: 3dp
- **内边距**: 3dp（距离控件边界）
- **动画时长**: 250ms（淡入淡出）
- **颜色**: 使用 `onSurface` 色彩角色

这些尺寸在代码中被硬编码为常量，改动会破坏与其他 Material 控件的视觉一致性。

## 基本用法

`MdFocusIndicator` 需要配合动画工厂使用，通常在控件构造函数中初始化：

```cpp
#include "widget/material/base/focus_ring.h"

using namespace cf::ui::widget::material;

class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 获取全局动画工厂
        auto animationFactory = cf::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
            Application::animationFactory()
        );

        // 创建焦点指示器
        m_focusIndicator = new base::MdFocusIndicator(animationFactory, this);
    }

private:
    base::MdFocusIndicator* m_focusIndicator;
};
```text

## 事件处理

焦点事件的转发非常直接，只需要在控件的事件处理中调用对应方法：

```cpp
void MyWidget::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    m_focusIndicator->onFocusIn();
    update();
}

void MyWidget::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    m_focusIndicator->onFocusOut();
    update();
}
```text

⚠️ 记得在事件处理函数中先调用父类实现，否则 Qt 的焦点系统可能无法正常工作。

## 绘制聚焦环

聚焦环应该在绘制的最后阶段进行，确保它出现在所有内容的上层：

```cpp
void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 先绘制背景、内容等...
    drawBackground(p);
    drawContent(p);

    // 最后绘制聚焦环（如果控件有焦点）
    if (hasFocus()) {
        CFColor indicatorColor = theme->getColor(MdColorRole::OnSurface);
        m_focusIndicator->paint(&p, shape(), indicatorColor);
    }
}
```text

聚焦环的颜色通常使用 `onSurface` 角色获取，这样可以与控件内容保持一致的对比度。

## 形状处理

`paint()` 方法接受一个 `QPainterPath` 参数，这使得它能够适应各种控件形状：

```cpp
// 圆角矩形按钮
QPainterPath shape;
shape.addRoundedRect(rect(), cornerRadius, cornerRadius);
m_focusIndicator->paint(&p, shape, indicatorColor);

// 圆形 FAB 按钮
QPainterPath shape;
shape.addEllipse(rect());
m_focusIndicator->paint(&p, shape, indicatorColor);

// 自定义形状
QPainterPath shape = customShape();
m_focusIndicator->paint(&p, shape, indicatorColor);
```text

环会自动沿着形状的边界向内偏移绘制，不需要手动计算偏移量。

## 动画性能

聚焦环使用工厂的 `md.animation.fadeIn` 和 `md.animation.fadeOut` 动画。如果全局动画被禁用，会直接设置透明度值而跳过动画：

```cpp
// 在应用层面禁用动画
auto factory = Application::animationFactory();
if (factory) {
    factory->setEnabledAll(false);
}
```text

这对于低端设备或性能敏感的场景很有用。

## 键盘导航建议

 Material Design 推荐的焦点策略是 `Qt::StrongFocus`，这样控件既可以通过点击获得焦点，也可以通过 Tab 键导航：

```cpp
MyWidget::MyWidget(QWidget* parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    // ...
}
```text

对于纯装饰性的控件，使用 `Qt::NoFocus` 避免干扰键盘导航流。

## 常见问题

如果聚焦环没有显示，检查以下几点：

1. 控件是否真的有焦点（`hasFocus()` 返回 true）
2. 是否正确调用了 `onFocusIn()`
3. `paint()` 方法是否在 `paintEvent` 中被调用
4. 传入的形状路径是否有效（非空）

## 相关文档

- [StateMachine - Material 状态机](../widget/state_machine.md)
- [MdElevationController - 阴影控制器](./elevation_controller.md)
- [Material Design 3 焦点规范](https://m3.material.io/foundations/accessible-design/focus)
