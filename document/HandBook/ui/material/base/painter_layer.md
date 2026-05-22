---
title: "PainterLayer - 绘图层管理器"
description: 是 Material 分层绘制的基础组件。它的职责很简单：持有一个颜色和透明度值，在绘制时将两者叠加
---

# PainterLayer - 绘图层管理器

`PainterLayer` 是 Material 分层绘制的基础组件。它的职责很简单：持有一个颜色和透明度值，在绘制时将两者叠加后填充到指定路径中。虽然看起来只是个颜色存储器，但单独抽出来的意义在于为状态层、遮罩层等提供统一接口，避免在每个控件里重复实现"带透明度的颜色填充"这个操作。

## 基本用法

`PainterLayer` 的使用分为三步：创建、设置属性、绘制：

```cpp
#include "widget/material/base/painter_layer.h"

using namespace cf::ui::widget::material;

class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 创建绘制层
        m_stateLayer = new base::PainterLayer(this);

        // 设置颜色和透明度
        m_stateLayer->setColor(cf::ui::base::CFColor::fromRGB(0, 0, 0));
        m_stateLayer->setOpacity(0.08f);  // 8% 透明度
    }

private:
    base::PainterLayer* m_stateLayer;
};
```text

## 绘制调用

在 `paintEvent` 中调用 `paint()` 方法，传入激活的 `QPainter` 和裁剪路径：

```cpp
void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 先绘制背景
    p.fillPath(shape, backgroundColor());

    // 绘制状态层
    m_stateLayer->paint(&p, shape);

    // 再绘制其他内容...
}
```text

`paint()` 方法内部会处理透明度小于等于零的情况直接返回，所以不需要在外层判断。

## 透明度叠加

最终的绘制颜色是 `setColor()` 的颜色 alpha 值乘以 `setOpacity()` 的值：

```cpp
// 内部实现（简化）
void PainterLayer::paint(QPainter* painter, const QPainterPath& clipPath) {
    if (!painter || opacity_ <= 0.0f) return;

    QColor color = cached_color_.native_color();
    color.setAlphaF(color.alphaF() * opacity_);  // 叠加透明度

    painter->fillPath(clipPath, color);
}
```text

这意味着如果颜色本身是半透明的（比如 alpha = 0.5），再设置 opacity = 0.5，最终 alpha 会是 0.25。这个设计允许你用一个基础颜色控制整体色调，用 opacity 控制当前状态下的强度。

## 状态层使用场景

`PainterLayer` 最常见的用途是实现 Material 的状态层（State Layer）——悬停、按下等交互时叠加的半透明层：

```cpp
class MyWidget : public QWidget {
    // ... 构造函数中初始化 ...

    void mousePressEvent(QMouseEvent* event) override {
        // 按下时增加状态层透明度
        m_stateLayer->setOpacity(0.12f);
        update();
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        // 恢复默认透明度
        m_stateLayer->setOpacity(0.0f);
        update();
    }

    void enterEvent(QEnterEvent* event) override {
        // 悬停时轻微的视觉反馈
        m_stateLayer->setOpacity(0.08f);
        update();
    }

    void leaveEvent(QEvent* event) override {
        m_stateLayer->setOpacity(0.0f);
        update();
    }
};
```text

实际项目中我们通常配合 `StateMachine` 来管理这些状态变化，而不是在每个事件里手动设置。

## 颜色选择

状态层的颜色通常使用文本颜色（label color），这样可以确保对比度：

```cpp
void MyWidget::updateStateLayerColor() {
    auto* theme = getTheme();
    auto* colors = static_cast<MaterialColorScheme*>(theme->color_scheme());

    // 文本在表面色上的颜色
    QColor onSurface = colors->queryExpectedColor("md.onSurface");

    m_stateLayer->setColor(cf::ui::base::CFColor(onSurface));
}
```text

⚠️ 不要用背景色做状态层颜色，那会改变控件的"色调"而不是"深浅"。Material 的状态层是通过叠加一层半透明的文本颜色来模拟"变深"或"变亮"的视觉效果。

## 性能特点

`PainterLayer` 本身不存储任何需要重绘的触发机制——它只是个被动的数据持有者。如果你想监听属性变化来触发重绘，需要自己处理：

```cpp
// 手动触发重绘
m_stateLayer->setOpacity(newOpacity);
update();  // 别忘了这个
```text

这和 `RippleHelper` 的设计不同——后者内部管理动画并主动发出 `repaintNeeded()` 信号，因为涟漪是"主动"的视觉效果，而状态层是"被动"的。

## 多层叠加

Material 控件可能需要多个绘制层，比如同时有状态层和遮罩层：

```cpp
class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 状态层：交互反馈
        m_stateLayer = new base::PainterLayer(this);
        m_stateLayer->setColor(labelColor);
        m_stateLayer->setOpacity(0.0f);

        // 遮罩层：禁用时的半透明遮罩
        m_maskLayer = new base::PainterLayer(this);
        m_maskLayer->setColor(cf::ui::base::CFColor::fromRGB(0, 0, 0));
        m_maskLayer->setOpacity(0.0f);
    }

    void paintEvent(QPaintEvent* event) override {
        QPainter p(this);

        // 背景层
        p.fillPath(shape, backgroundColor());

        // 状态层
        m_stateLayer->paint(&p, shape);

        // 遮罩层（禁用时）
        if (!isEnabled()) {
            m_maskLayer->setOpacity(0.38f);
            m_maskLayer->paint(&p, shape);
        }

        // 内容层
        drawContent(p);
    }

private:
    base::PainterLayer* m_stateLayer;
    base::PainterLayer* m_maskLayer;
};
```text

绘制顺序很重要：背景 → 状态层 → 遮罩层 → 内容。改变顺序会破坏视觉层次。

## 内存管理

`PainterLayer` 继承自 `QObject`，支持 Qt 的父子对象内存管理：

```cpp
// parent 为 this 时，会在控件销毁时自动删除
m_layer = new base::PainterLayer(this);

// 手动管理也是可以的
m_layer = new base::PainterLayer(nullptr);
// ... 使用完毕后
delete m_layer;
```text

## 为什么不直接用 QColor

这是个好问题。既然 `PainterLayer` 只是存储颜色和透明度，为什么不直接在控件里存两个成员变量？

```cpp
// 看起来更简单的方式
QColor m_stateColor;
float m_stateOpacity = 0.0f;
```text

问题在于一致性——当有多个控件需要状态层、多个层需要管理时，每个控件都要自己实现"填充带透明度的颜色到路径"的逻辑，容易出错。抽出 `PainterLayer` 后：

1. 统一的接口，`setColor()` + `setOpacity()` + `paint()`
2. 未来可以方便地添加渐变支持、混合模式等高级特性
3. 便于测试，可以独立验证绘制逻辑

这是我们设计基础组件时的通用原则：简单可以，但一致更重要。

## 相关文档

- [RippleHelper - 涟漪效果助手](./ripple_helper.md)
- [StateMachine - Material 状态机](../widget/state_machine.md)
- [Button - Material 按钮](../widget/button.md)
