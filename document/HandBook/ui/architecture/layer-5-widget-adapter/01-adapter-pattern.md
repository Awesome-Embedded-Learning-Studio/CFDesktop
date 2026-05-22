---
title: "适配器模式——Material 控件的\"薄包装\"设计"
description: 在前面的文章里，我们聊完了基础数学工具、主题引擎、动画系统、行为组件。现在所有的基础设施都就绪了，可
---

# 适配器模式——Material 控件的"薄包装"设计

在前面的文章里，我们聊完了基础数学工具、主题引擎、动画系统、行为组件。现在所有的基础设施都就绪了，可以开始构建具体的控件了。

这篇文章聊聊控件适配层的设计理念。

## 继承 vs 组合的选择

一个经典的问题是：为什么必须继承 Qt 原生控件？

有几个重要原因：

1. **Qt 事件体系的要求**：Qt 的信号机制依赖于事件处理函数的正确调用顺序。如果不继承，就需要手动管理信号，复杂且容易出错
2. **布局系统集成**：Qt 的布局管理器（QVBoxLayout、QGridLayout 等）直接与 QWidget 接口交互
3. **QSS 兼容性**：虽然我们不使用 QSS 实现 Material 效果，但保持继承关系让 QSS 仍然可用于某些全局设置
4. **第三方库兼容**：很多 Qt 库假设控件是 QWidget 的子类

所以我们选择继承 Qt 原生控件，但只做最小的修改——事件转发和 paintEvent 重写。

## "只做三件事"的设计原则

Material 控件适配器只做三件事：

1. **事件转发**：在事件 override 中调用父类方法后，转发给行为组件
2. **paintEvent 重写**：按顺序调用渲染层的 paint 方法
3. **主题数据读取**：从 ThemeManager 读取 Token 决定颜色、形状、字体

任何不属于这三件事的逻辑都应该被拒绝——这保持了控件的"薄"特性，让行为组件保持独立。

## 事件转发模式

事件处理的标准模板是：

```cpp
void Button::mousePressEvent(QMouseEvent* event) {
    // 1. 先调用父类方法（必须！）
    QPushButton::mousePressEvent(event);

    // 2. 转发给行为组件
    m_stateMachine->onPress(event->pos());
    m_ripple->onPress(event->pos(), rect());
}
```text

第一步调用父类方法是必须的，原因如下：

- Qt 需要在事件处理中更新内部状态
- `clicked()` 等信号在父类方法中发出
- 跳过父类方法会导致信号不发出、状态不一致等问题

## 组件组合模式

控件通过组合行为组件来获得 Material 特性：

```cpp
Button::Button(ButtonVariant variant, QWidget* parent)
    : QPushButton(parent), variant_(variant) {

    // 获取动画工厂
    m_animationFactory = Application::animationFactory();

    // 创建行为组件
    m_stateMachine    = new StateMachine(m_animationFactory, this);
    m_ripple          = new RippleHelper(m_animationFactory, this);
    m_elevation       = new MdElevationController(m_animationFactory, this);
    m_focusIndicator  = new MdFocusIndicator(m_animationFactory, this);

    // 连接信号
    connect(m_ripple, &RippleHelper::repaintNeeded,
            this, QOverload<>::of(&Button::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged,
            this, [this](float) { update(); });

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, [this](const ICFTheme&) { update(); });
}
```text

注意所有行为组件的 parent 都是 `this`，这意味着它们会随控件一起销毁，无需手动管理生命周期。

## 主题数据读取

控件在绘制时从主题读取数据：

```cpp
QColor Button::containerColor() const {
    auto& theme = ThemeManager::instance().currentTheme();
    switch (variant_) {
        case ButtonVariant::Filled:
            return theme.color_scheme().queryColor("md.primaryContainer");
        case ButtonVariant::Tonal:
            return theme.color_scheme().queryColor("md.secondaryContainer");
        // ...
    }
}
```text

这里使用 `currentTheme()` 获取当前活动的主题。如果主题切换，`themeChanged` 信号会触发 `update()`，控件会重新读取主题数据并重绘。

## 为什么不用 QSS？

这是一个经常被问到的问题。Material Design 3 的视觉需求超出了 QSS 的能力：

1. **多层叠加**：状态层、涟漪层、阴影层需要按特定顺序绘制，QSS 不支持
2. **动态透明度**：状态层的透明度是动画的，QSS 无法表达
3. **复杂动画**：涟漪扩散、弹性动画等需要精确控制，QSS 的 transition 过于简单
4. **性能考虑**：QSS 的解析和应用有性能开销，直接绘制更高效

所以我们选择在 paintEvent 中完全接管绘制。

## 只读主题原则

控件只能读取主题数据，不能修改。这是一个硬性约束：

```cpp
// 错误！不要这样做
auto& theme = ThemeManager::instance().currentTheme();
theme.color_scheme().setColor("md.primary", QColor("#FF0000"));
```bash

修改主题应该通过 ThemeManager 的 `setThemeTo()` 方法，或者在主题创建时指定颜色。这保证了主题的全局一致性。

## 适配器的最小化

理想情况下，控件适配器应该足够"薄"，以至于你可以用一个表格来描述它的事件处理：

| 事件 | 父类调用 | 行为组件调用 |
|---|---:|---|
| mousePressEvent | ✓ | StateMachine::onPress, RippleHelper::onPress |
| mouseReleaseEvent | ✓ | StateMachine::onRelease, RippleHelper::onRelease |
| enterEvent | ✓ | StateMachine::onHoverEnter |
| leaveEvent | ✓ | StateMachine::onHoverLeave |
| focusInEvent | ✓ | StateMachine::onFocusIn, FocusIndicator::onFocusIn |
| focusOutEvent | ✓ | StateMachine::onFocusOut, FocusIndicator::onFocusOut |
| paintEvent | ✗ | 完全重写，按顺序绘制各层 |

这种模式的一致性让新控件的实现变得简单：只需要按照模板填充对应的行为组件调用即可。

## 总结

适配器模式的核心思想是"薄包装"：继承 Qt 原生控件，只做事件转发和自定义绘制，通过组合行为组件获得 Material 特性。这种设计保持了代码的清晰和可维护性。

但适配器模式只是理论框架，具体的实现细节还需要深入探讨。Button 控件作为我们实现最完整的控件，是一个很好的研究案例。

接下来，我们深入 Button 控件的实现。

---

**相关文档**

- [状态机设计](../layer-4-material-behavior/01-state-machine.md)——行为组件之一
- [Button 控件深度解析](./02-button-deep-dive.md)——完整的实现案例
- [主题系统架构设计](../layer-2-theme-engine/01-theme-system-design.md)——Layer 2 的整体设计
