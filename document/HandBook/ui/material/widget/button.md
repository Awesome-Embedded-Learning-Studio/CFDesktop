# Button - Material 按钮

`Button` 是 Material Design 3 按钮控件的完整实现，支持五种视觉变体、水波纹效果、状态层动画、阴影和焦点指示器。我们选择自己实现而不是继承 QPushButton 的样式，是因为 Material 的交互规范（特别是状态层动画和水波纹）无法通过 Qt 样式表很好地表达。

## 按钮变体

Material Design 3 定义了五种按钮变体，每种有不同的视觉层级：

```cpp
enum class ButtonVariant {
    Filled,    // 填充按钮 - 最高的视觉强调
    Tonal,     // 调和按钮 - 中等强调
    Outlined,  // 描边按钮 - 较低强调
    Text,      // 文本按钮 - 最低强调
    Elevated,  // 浮起按钮 - 带阴影
};
```

选择哪种变体取决于按钮在界面中的层级关系。主操作用 Filled，次要操作用 Tonal 或 Outlined，低优先级操作用 Text 或 Elevated。

## 基本用法

创建按钮最简单的方式是指定文本和变体：

```cpp
#include "widget/material/widget/button/button.h"

using namespace cf::ui::widget::material;

// 默认变体（Filled）
auto* button1 = new Button("Click me", this);

// 指定变体
auto* button2 = new Button("Secondary", ButtonVariant::Outlined, this);
auto* button3 = new Button("Low priority", ButtonVariant::Text, this);

// 连接信号（与 QPushButton 兼容）
connect(button1, &Button::clicked, this, &MyClass::onButtonClick);
```

## 图标按钮

Material 按钮支持在文本前添加前导图标：

```cpp
QIcon icon = QIcon::fromTheme("favorite");
Button* button = new Button("Like", this);
button->setLeadingIcon(icon);

// 或者使用 setIcon（别名）
button->setIcon(icon);
```

图标尺寸固定为 18dp，与文本间距 8dp，这是 Material 规范要求的。

## 交互状态

按钮遵循 Material Design 3 的交互状态规范，每种状态有不同的视觉反馈：

| 状态 | 视觉效果 | 透明度 |
|------|----------|--------|
| Normal | 默认外观 | 0% |
| Hovered | 状态层叠加 | 8% |
| Pressed | 状态层叠加 + 轻微下沉 | 12% |
| Focused | 焦点环 + 状态层 | 12% |
| Disabled | 38% 透明度 | 0% |

这些状态由内部的 `StateMachine` 管理，只需要在事件处理中正确转发即可。按钮已经处理好了所有事件，直接使用就行。

## 尺寸规范

按钮遵循 Material 的尺寸规范，内容区域高度固定为 40dp，水平内边距 24dp：

```cpp
// 尺寸计算（已在 sizeHint 中实现）
CanvasUnitHelper helper(qApp->devicePixelRatio());
float contentHeight = helper.dpToPx(40.0f);  // 固定高度
float hPadding = helper.dpToPx(24.0f);       // 水平内边距
float iconWidth = helper.dpToPx(18.0f);      // 图标宽度
float iconGap = helper.dpToPx(8.0f);         // 图标与文本间距
```

⚠️ 按钮的最小宽度不是固定的，而是由内容决定的。如果需要确保触摸目标大小（至少 48x48dp），需要在布局时留出足够的间距。

## 绘制流程

按钮的 `paintEvent` 实现了 Material 规范的七步绘制流程：

```cpp
void Button::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Step 1: 绘制阴影（仅 Elevated 变体）
    drawShadow(p, contentRect, shape);

    // Step 2: 绘制背景
    drawBackground(p, shape);

    // Step 3: 绘制状态层
    drawStateLayer(p, shape);

    // Step 4: 绘制水波纹
    drawRipple(p, shape);

    // Step 5: 绘制描边（仅 Outlined 变体）
    drawOutline(p, shape);

    // Step 6: 绘制内容（图标 + 文本）
    drawContent(p, contentRect);

    // Step 7: 绘制焦点指示器
    drawFocusIndicator(p, shape);
}
```

这个顺序很重要——状态层在背景之上、内容之下，水波纹在状态层之上，焦点环在最外层。改变顺序会导致视觉效果不符合 Material 规范。

## 阴影处理

Elevated 变体和按压效果需要阴影支持，由 `MdElevationController` 处理：

```cpp
// 设置海拔级别（0-5）
button->setElevation(2);

// 设置光源角度（默认 15 度，来自左上方）
button->setLightSourceAngle(15.0f);
```

海拔级别影响阴影的模糊半径和偏移量。按钮默认使用 level 2，按压时会临时增加，产生"下沉"的视觉效果。

## 按压效果

按压效果包括两部分：状态层透明度变化和海拔变化。可以通过属性禁用：

```cpp
// 禁用按压效果（仅状态层动画保留）
button->setPressEffectEnabled(false);
```

禁用后，按钮的视觉反馈会减弱，但仍然有水波纹和状态层。这在某些自定义场景下有用。

## 颜色获取

按钮的颜色从当前主题中获取，每种变体使用不同的颜色角色：

```cpp
// Filled: container = PRIMARY, label = ON_PRIMARY
// Tonal: container = SECONDARY_CONTAINER, label = ON_SECONDARY_CONTAINER
// Outlined/Text/Elevated: container = SURFACE, label = PRIMARY
```

如果主题不可用，会使用硬编码的 fallback 颜色。这在开发阶段很有用，但生产环境应该总是配置正确的主题。

## 圆角处理

按钮使用完全圆角（圆角半径等于高度的一半）：

```cpp
float cornerRadius = height() / 2.0f;
```

这在视觉上形成了胶囊形状，是 Material 3 的默认样式。如果需要方角按钮，需要子类化并重写 `cornerRadius()` 方法。

## 禁用状态

禁用状态下，按钮的背景和文本透明度降至 38%，这是 Material 规范要求的：

```cpp
if (!isEnabled()) {
    color.setAlphaF(0.38f);
}
```

禁用时状态层不显示，交互事件也不会触发状态变化。

## 布局建议

按钮在布局中的位置需要遵循 Material 的对齐规范：

```cpp
// 对话框操作按钮通常右对齐
auto* layout = new QHBoxLayout(dialog);
layout->addStretch();
layout->addWidget(new Button("Cancel", ButtonVariant::Text, dialog));
layout->addWidget(new Button("OK", ButtonVariant::Filled, dialog));

// 卡片操作按钮通常左对齐
auto* cardLayout = new QHBoxLayout(card);
cardLayout->addWidget(new Button("Action", ButtonVariant::Outlined, card));
cardLayout->addStretch();
```

## 相关文档

- [StateMachine - Material 状态机](./state_machine.md)
- [RippleHelper - 水波纹效果](../base/ripple_helper.md)
- [MdElevationController - 阴影控制器](../base/elevation_controller.md)
- [Material Design 3 按钮规范](https://m3.material.io/components/buttons)
