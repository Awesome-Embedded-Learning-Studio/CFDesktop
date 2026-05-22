---
title: 状态机设计——Material 交互状态的核心管理器
description: 在 Layer 3 里，我们讲了动画引擎的完整实现。但动画不会凭空触发——它们需要响应鼠标悬停、点击
---

# 状态机设计——Material 交互状态的核心管理器

在 Layer 3 里，我们讲了动画引擎的完整实现。但动画不会凭空触发——它们需要响应鼠标悬停、点击、焦点变化等交互事件。

这篇文章聊聊 Material 行为层的核心组件：StateMachine。

## 为什么需要独立的状态机？

Qt 有 QStateMachine，为什么还要自己实现？

有几个原因：

1. **Qt QStateMachine 过于重量级**：它是一个通用的状态机框架，对于 Material 的简单状态管理来说太复杂了
2. **Material 的特殊需求**：Material 定义了特定的状态透明度值（0.00、0.08、0.12 等），需要精确控制
3. **状态叠加问题**：Material 的状态可以叠加（比如同时 Hovered 和 Checked），需要位运算处理
4. **动画集成**：状态切换需要触发透明度动画，需要与我们的动画系统无缝集成

## Material 的交互状态

StateMachine 定义了 7 种交互状态：

```cpp
enum class State {
    StateNormal = 0x00,   // 默认状态
    StateHovered = 0x01,  // 鼠标悬停
    StatePressed = 0x02,  // 鼠标按下
    StateFocused = 0x04,  // 键盘焦点
    StateDisabled = 0x08, // 禁用状态
    StateChecked = 0x10,  // 选中状态
    StateDragged = 0x20   // 拖拽状态
};
```text

注意这里使用了位掩码设计，每个状态是一个 2 的幂次方。这样多个状态可以通过按位或运算组合。

## 状态优先级

Material Design 3 定义了状态的优先级顺序：

```text
Disabled > Pressed > Dragged > Focused > Hovered > Normal
```bash

这意味着如果一个控件同时是 Disabled 和 Hovered， Disabled 状态会"赢"，透明度由 Disabled 决定（通常是 0.00）。

## 状态透明度规范

每个状态对应一个特定的透明度值，用于 StateLayer 的叠加：

| 状态 | 透明度 | 说明 |
|---|---:|---|
| Normal | 0.00 | 默认状态，无叠加 |
| Hovered | 0.08 | 鼠标悬停时的视觉反馈 |
| Pressed | 0.12 | 按下时的视觉反馈 |
| Focused | 0.12 | 焦点状态（同 Pressed） |
| Dragged | 0.16 | 拖拽时的视觉反馈 |
| Checked | 0.08 | 选中状态（同 Hovered） |
| Disabled | 0.00 | 禁用状态（无叠加） |

这些透明度值是 Material Design 3 规范的一部分，不应该随意修改。

## 事件处理接口

StateMachine 提供了一系列事件处理方法：

```cpp
void onHoverEnter();
void onHoverLeave();
void onPress(const QPoint& pos);
void onRelease();
void onFocusIn();
void onFocusOut();
void onEnable();
void onDisable();
void onCheckedChanged(bool checked);
```text

这些方法对应 Qt 的各种事件，控件在事件处理函数中调用它们：

```cpp
void Button::enterEvent(QEnterEvent* event) {
    QPushButton::enterEvent(event);  // 先调用父类方法
    m_stateMachine->onHoverEnter();
}

void Button::mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);
    m_stateMachine->onPress(event->pos());
}
```text

注意这里的一个关键点：必须先调用父类方法。否则 Qt 的信号机制会被破坏，比如 `clicked()` 信号可能不会发出。

## 状态切换动画

当状态改变时，StateMachine 会触发透明度动画：

```cpp
void StateMachine::animateOpacityTo(float from, float to) {
    auto anim = m_animator->getAnimation("md.animation.fadeIn");
    if (anim) {
        // 创建一个自定义动画，从 from 到 to
        connect(anim.get(), &ICFAbstractAnimation::progressChanged,
                this, [this, from, to](float progress) {
                    m_opacity = lerp(from, to, progress);
                    emit stateLayerOpacityChanged(m_opacity);
                });
        anim->start();
    }
}
```text

如果动画系统被禁用，会直接设置目标透明度：

```cpp
m_opacity = to;
emit stateLayerOpacityChanged(m_opacity);
```text

## 状态优先级的实现

`targetOpacityForState()` 方法实现了优先级逻辑：

```cpp
float StateMachine::targetOpacityForState(States s) const {
    // 按优先级从高到低检查
    if (s & StateDisabled) return 0.00f;
    if (s & StatePressed) return 0.12f;
    if (s & StateDragged) return 0.16f;
    if (s & StateFocused) return 0.12f;
    if (s & StateHovered) return 0.08f;
    if (s & StateChecked) return 0.08f;
    return 0.00f;  // Normal
}
```text

使用按位与运算（`&`）来检查状态是否被设置。这意味着多个状态可以同时存在，但只有一个决定透明度。

## Checked 状态的特殊处理

Checked 状态有些特殊——它不是由鼠标或键盘事件触发的，而是由控件的逻辑状态决定的。比如 CheckBox 的 `setChecked(bool)` 会调用 `onCheckedChanged()`。

```cpp
void CheckBox::setChecked(bool checked) {
    QCheckBox::setChecked(checked);
    m_stateMachine->onCheckedChanged(checked);
}
```text

## Disabled 状态的特殊处理

Disabled 状态优先级最高。当控件被禁用时，所有其他交互都被忽略：

```cpp
void StateMachine::onPress(const QPoint& pos) {
    if (hasState(StateDisabled)) {
        return;  // 禁用状态下忽略按下事件
    }
    // ... 正常处理
}
```text

## 信号通知

StateMachine 发出两个信号：

```cpp
void stateChanged(States newState, States oldState);
void stateLayerOpacityChanged(float opacity);
```text

控件可以连接这些信号来触发重绘：

```cpp
connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged,
        this, [this](float) { update(); });
```yaml

## 总结

StateMachine 是 Material 行为层的核心，它管理控件的所有交互状态，并驱动状态层透明度的动画。它的设计简单而高效，使用位运算处理状态叠加，用优先级决定最终透明度。

但状态机只是管理状态，状态变化的视觉效果还需要其他组件来实现——比如涟漪效果、阴影绘制等。

接下来，我们聊聊涟漪与阴影的实现。

---

**相关文档**

- [工厂与策略](../layer-3-animation-engine/03-factory-and-strategy.md)——动画工厂设计
- [涟漪与阴影](./02-ripple-and-elevation.md)——视觉效果组件
- [动画引擎架构](../layer-3-animation-engine/01-animation-architecture.md)——Layer 3 的整体设计
