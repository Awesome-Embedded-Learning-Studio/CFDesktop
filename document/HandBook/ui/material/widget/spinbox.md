# SpinBox - Material 数值调节框

`SpinBox` 是 Material Design 3 数值调节框控件的完整实现，支持整数输入、增减按钮、描边样式、焦点指示器和状态层效果。

## 类参考

```cpp
namespace cf::ui::widget::material;

class SpinBox : public QSpinBox {
    Q_OBJECT
};
```

头文件：`ui/widget/material/widget/spinbox/spinbox.h`

## 基本用法

```cpp
#include "widget/material/widget/spinbox/spinbox.h"

using namespace cf::ui::widget::material;

// 创建数值调节框
auto* spin = new SpinBox(this);
spin->setRange(0, 100);
spin->setValue(50);
spin->setSingleStep(1);

// 设置前缀和后缀
spin->setPrefix("$ ");
spin->setSuffix(" px");

// 连接信号（与 QSpinBox 兼容）
connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MyClass::onValueChanged);
```

## 增减按钮

SpinBox 在控件右侧提供增减按钮：

```cpp
// 增加按钮（incrementButtonRect）
// 减少按钮（decrementButtonRect）
// 鼠标悬停在按钮上时有独立的 hover 状态
```

每个按钮有独立的悬停和按压状态追踪：

| 按钮 | 状态变量 |
|------|----------|
| 增加按钮 | `m_hoveringIncrementButton`, `m_pressingIncrementButton` |
| 减少按钮 | `m_hoveringDecrementButton`, `m_pressingDecrementButton` |

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 描边边框 + 文本 + 增减图标 |
| Hovered (整体) | 状态层叠加 |
| Hovered (按钮) | 按钮区域状态层 |
| Pressed (按钮) | 水波纹 + 状态层 |
| Focused | 描边高亮 + 焦点环 |
| Disabled | 38% 透明度 |

## 绘制流程

SpinBox 的 `paintEvent` 实现 7 步 Material Design 绘制流程：

1. 绘制背景（`drawBackground`）
2. 绘制状态层（`drawStateLayer`）
3. 绘制水波纹（`drawRipple`）
4. 绘制描边（`drawOutline`）
5. 绘制文本（`drawText`）
6. 绘制增减按钮（`drawButtons`）
7. 绘制焦点指示器（`drawFocusIndicator`）

## 内部 LineEdit 约束

SpinBox 重写了 `resizeEvent` 以约束内部 LineEdit：

```cpp
// resizeEvent() 将内部 lineEdit 限制在文本区域
// 避免输入框覆盖增减按钮区域
```

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 容器背景 | Surface |
| 文本 | OnSurface |
| 描边 | Outline |
| 焦点描边 | Primary |
| 按钮图标 | OnSurfaceVariant |
| 状态层 | 根据状态计算 |

## 主要方法

| 方法 | 说明 |
|------|------|
| 继承自 QSpinBox | `setValue()`, `setRange()`, `setSingleStep()`, `setPrefix()`, `setSuffix()` 等 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [DoubleSpinBox - Material 双精度数值调节框](./doublespinbox.md)
- [TextField - Material 文本输入框](./textfield.md)
