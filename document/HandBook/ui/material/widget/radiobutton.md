# RadioButton - Material 单选按钮

`RadioButton` 是 Material Design 3 单选按钮控件的完整实现，具有圆形选择区域、内部圆缩放动画、水波纹效果和焦点指示器。通过 `QButtonGroup` 集成支持互斥选择。

## 类参考

```cpp
namespace cf::ui::widget::material;

class RadioButton : public QRadioButton {
    Q_OBJECT
    Q_PROPERTY(bool error READ hasError WRITE setError)
    Q_PROPERTY(bool pressEffectEnabled READ pressEffectEnabled WRITE setPressEffectEnabled)
};
```

头文件：`ui/widget/material/widget/radiobutton/radiobutton.h`

## 基本用法

```cpp
#include "widget/material/widget/radiobutton/radiobutton.h"

using namespace cf::ui::widget::material;

// 创建单选按钮
auto* option1 = new RadioButton("Option A", this);
auto* option2 = new RadioButton("Option B", this);
auto* option3 = new RadioButton("Option C", this);

// 使用 QButtonGroup 实现互斥选择
auto* group = new QButtonGroup(this);
group->addButton(option1);
group->addButton(option2);
group->addButton(option3);

// 默认选中
option1->setChecked(true);

// 连接信号
connect(group, &QButtonGroup::idClicked, this, &MyClass::onOptionSelected);
```

## 错误状态

当表单验证失败时，可以给 RadioButton 设置错误状态：

```cpp
if (!group->checkedButton()) {
    option1->setError(true);
    option2->setError(true);
    option3->setError(true);
}
```

错误状态下，外环和内部圆使用 error 颜色。

## 按压效果

可以控制按压效果（水波纹动画）的启用/禁用：

```cpp
// 禁用按压效果
radioButton->setPressEffectEnabled(false);
```

禁用后，点击时不会触发水波纹动画，但状态层仍然正常工作。

## 交互状态

| 状态 | 视觉效果 | 透明度 |
|------|----------|--------|
| Normal (未选中) | 空心外环 | 0% |
| Normal (选中) | 填充外环 + 内部实心圆 | 0% |
| Hovered | 状态层叠加 | 8% |
| Pressed | 状态层叠加 + 水波纹 | 12% |
| Focused | 焦点环 + 状态层 | 12% |
| Disabled | 38% 透明度 | 0% |

## 选中动画

选中/取消选中时，内部圆有从 0 到目标尺寸的缩放动画：

```cpp
// setChecked 会同步内部圆的缩放状态
radioButton->setChecked(true);  // 内部圆从 0 缩放到目标尺寸
radioButton->setChecked(false); // 内部圆从目标尺寸缩放到 0
```

## 尺寸规范

RadioButton 遵循 Material Design 3 尺寸规范，触摸目标为 48x48dp。`hitButton()` 方法重写了默认行为，使整个控件区域都可点击。

## 绘制流程

RadioButton 的 `paintEvent` 实现以下绘制步骤：

1. 绘制状态层（`drawStateLayer`）
2. 绘制水波纹（`drawRipple`）
3. 绘制外环（`drawOuterRing`）
4. 绘制内部圆（`drawInnerCircle`） - 带缩放动画
5. 绘制文本（`drawText`）
6. 绘制焦点指示器（`drawFocusIndicator`）

## 主要方法

| 方法 | 说明 |
|------|------|
| `setChecked(bool)` | 设置选中状态，同步内部圆缩放动画 |
| `hasError()` / `setError(bool)` | 获取/设置错误状态 |
| `pressEffectEnabled()` / `setPressEffectEnabled(bool)` | 获取/设置按压效果 |
| `hitButton(const QPoint&)` | 整个控件区域可点击 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [CheckBox - Material 复选框](./checkbox.md)
- [StateMachine - Material 状态机](./state_machine.md)
- [Material Design 3 单选按钮规范](https://m3.material.io/components/radio-buttons)
