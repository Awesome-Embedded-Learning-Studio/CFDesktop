# StateMachine - Material 状态机

`StateMachine` 是 Material Design 3 交互状态的核心管理组件。它负责处理控件的各种交互状态（悬停、按下、焦点、禁用等），并按照 Material 规范驱动状态层的透明度动画。我们选择单独实现这个组件，是因为 Qt 的状态系统对 Material 的状态层动画支持不够友好，需要更精细的控制。

## 状态定义

Material Design 3 定义了以下几种交互状态，每个状态对应不同的状态层透明度：

```cpp
enum class State {
    StateNormal   = 0x00,  // 默认状态
    StateHovered  = 0x01,  // 鼠标悬停
    StatePressed  = 0x02,  // 鼠标按下
    StateFocused  = 0x04,  // 键盘焦点
    StateDisabled = 0x08,  // 禁用状态
    StateChecked  = 0x10,  // 选中状态（如 ToggleButton）
    StateDragged  = 0x20,  // 拖拽状态
};
```

这些状态可以组合存在（比如同时有焦点和悬停），状态机内部通过位运算处理优先级。

## 透明度规范

状态层的透明度直接影响交互反馈的"分量感"。Material Design 3 的规范如下：

| 状态 | 透明度 | 说明 |
|------|--------|------|
| Normal | 0.00 | 无状态层 |
| Hovered | 0.08 | 轻微的视觉反馈 |
| Pressed | 0.12 | 明确的按下反馈 |
| Focused | 0.12 | 与按下相同 |
| Dragged | 0.16 | 最强的交互状态 |
| Disabled | 0.00 | 禁用时无状态层 |
| Checked | 0.08 | 与悬停相同 |

当多个状态同时存在时，按以下优先级取最高值：`Disabled > Pressed > Dragged > Focused > Hovered > Normal`。这个顺序在 `targetOpacityForState()` 里实现，改动顺序会破坏交互的一致性。

## 基本用法

`StateMachine` 需要配合动画工厂使用，通常在控件的构造函数中初始化：

```cpp
#include "widget/material/base/state_machine.h"

using namespace cf::ui::widget::material;

// 在控件构造函数中
class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 获取全局动画工厂
        auto animationFactory = cf::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
            Application::animationFactory()
        );

        // 创建状态机
        m_stateMachine = new base::StateMachine(animationFactory, this);

        // 监听状态变化
        connect(m_stateMachine, &base::StateMachine::stateLayerOpacityChanged,
                this, QOverload<>::of(&MyWidget::update));
    }

private:
    base::StateMachine* m_stateMachine;
};
```

## 事件处理

状态机提供了一系列事件处理方法，需要将 Qt 的事件转发给它：

```cpp
void MyWidget::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    m_stateMachine->onHoverEnter();
    update();
}

void MyWidget::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    m_stateMachine->onHoverLeave();
    update();
}

void MyWidget::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    m_stateMachine->onPress(event->pos());
    update();
}

void MyWidget::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    m_stateMachine->onRelease();
    update();
}

void MyWidget::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    m_stateMachine->onFocusIn();
    update();
}

void MyWidget::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    m_stateMachine->onFocusOut();
    update();
}
```

禁用状态的监听稍微特殊，因为它通过 `changeEvent` 触发：

```cpp
void MyWidget::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        if (isEnabled()) {
            m_stateMachine->onEnable();
        } else {
            m_stateMachine->onDisable();
        }
        update();
    }
}
```

## 绘制状态层

状态层是一个半透明的叠加层，绘制在控件背景之上、内容之下。在 `paintEvent` 中使用状态机提供的透明度值：

```cpp
void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 先绘制背景
    p.fillPath(shape, backgroundColor());

    // 绘制状态层
    float opacity = m_stateMachine->stateLayerOpacity();
    if (opacity > 0.0f) {
        CFColor stateColor = labelColor();  // 通常与文本颜色相同
        QColor layerColor = stateColor.native_color();
        layerColor.setAlphaF(layerColor.alphaF() * opacity);
        p.fillPath(shape, layerColor);
    }

    // 再绘制其他内容...
}
```

## 选中状态

对于有选中状态的控件（如 RadioButton、CheckBox、ToggleButton），还需要监听选中状态的变化：

```cpp
void MyWidget::setChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        m_stateMachine->onCheckedChanged(checked);
        update();
    }
}
```

⚠️ 选中状态（Checked）只是一种"持久化"的悬停状态，它不应该阻止其他交互状态的叠加。

## 禁用状态保护

状态机内部已经对禁用状态做了保护——在禁用状态下，悬停、按下、焦点等交互事件不会生效。这确保了禁用控件的视觉一致性，不需要在控件层再做额外判断。

## 动画性能

状态机使用动画工厂创建淡入淡出动画，当全局动画被禁用时，会直接设置透明度值而跳过动画。这对于性能敏感的场景很有用：

```cpp
// 在应用层面禁用动画
auto factory = Application::animationFactory();
if (factory) {
    factory->setEnabledAll(false);
}
```

## 相关文档

- [Button - Material 按钮](./button.md)
- [RippleHelper - 水波纹效果](../base/ripple_helper.md)
- [MdElevationController - 阴影控制器](../base/elevation_controller.md)
