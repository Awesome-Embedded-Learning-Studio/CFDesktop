# CheckBox - Material 复选框

`CheckBox` 是 Material Design 3 复选框控件的完整实现，支持选中、未选中和不确定（indeterminate）三种状态。包含水波纹效果、状态层动画和焦点指示器，严格遵循 Material Design 3 规范。

## 类参考

```cpp
namespace cf::ui::widget::material;

class CheckBox : public QCheckBox {
    Q_OBJECT
    Q_PROPERTY(bool error READ hasError WRITE setError NOTIFY errorChanged)
};
```

头文件：`ui/widget/material/widget/checkbox/checkbox.h`

## 基本用法

```cpp
#include "widget/material/widget/checkbox/checkbox.h"

using namespace cf::ui::widget::material;

// 创建不带文本的复选框
auto* cb1 = new CheckBox(this);

// 创建带文本的复选框
auto* cb2 = new CheckBox("Enable notifications", this);

// 设置选中状态
cb2->setChecked(true);

// 设置不确定状态
cb2->setCheckState(Qt::PartiallyChecked);

// 连接信号（与 QCheckBox 兼容）
connect(cb2, &QCheckBox::stateChanged, this, &MyClass::onStateChanged);
```

## 错误状态

CheckBox 支持错误状态，当验证失败时可启用：

```cpp
if (!agreedToTerms) {
    checkBox->setError(true);
} else {
    checkBox->setError(false);
}
```

错误状态下，复选框边框使用 error 颜色，提供明确的视觉反馈。

## 三种选中状态

Material Design 3 的复选框支持三种状态：

| 状态 | 值 | 视觉效果 |
|------|------|----------|
| Unchecked | `Qt::Unchecked` | 仅显示边框 |
| Checked | `Qt::Checked` | 填充背景 + 勾选标记 |
| Indeterminate | `Qt::PartiallyChecked` | 填充背景 + 横线标记 |

选中状态切换时，勾选标记/横线标记有从 0 到 1 的缩放动画，由内部 `CFMaterialAnimationFactory` 驱动。

## 交互状态

| 状态 | 视觉效果 | 透明度 |
|------|----------|--------|
| Normal | 默认外观 | 0% |
| Hovered | 状态层叠加 | 8% |
| Pressed | 状态层叠加 + 水波纹 | 12% |
| Focused | 焦点环 + 状态层 | 12% |
| Disabled | 38% 透明度 | 0% |

这些状态由内部的 `StateMachine` 管理。复选框已经处理好了所有事件，直接使用即可。

## 尺寸规范

复选框图标区域遵循 Material Design 3 的尺寸规范，点击区域满足 48x48dp 的触摸目标要求。`hitButton()` 方法重写了默认行为，使整个控件区域都可点击。

## 绘制流程

复选框的 `paintEvent` 实现了以下绘制步骤：

1. 绘制背景（`drawBackground`）
2. 绘制边框（`drawBorder`）
3. 绘制勾选标记或不确定标记（`drawCheckMark` / `drawIndeterminateMark`）
4. 绘制水波纹（`drawRipple`）
5. 绘制文本（`drawText`）
6. 绘制焦点指示器（`drawFocusIndicator`）

## 主要方法

| 方法 | 说明 |
|------|------|
| `setChecked(bool)` | 设置选中状态，同步动画进度 |
| `setCheckState(Qt::CheckState)` | 设置三种选中状态之一 |
| `hasError()` / `setError(bool)` | 获取/设置错误状态 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 信号

| 信号 | 说明 |
|------|------|
| `errorChanged(bool)` | 错误状态变化时发射 |
| 继承自 QCheckBox | `stateChanged`, `clicked`, `toggled` 等 |

## 相关文档

- [StateMachine - Material 状态机](./state_machine.md)
- [RippleHelper - 水波纹效果](../../base/ripple_helper.md)
- [Material Design 3 复选框规范](https://m3.material.io/components/checkboxes)
