# DoubleSpinBox - Material 双精度数值调节框

`DoubleSpinBox` 是 Material Design 3 双精度数值调节框控件的完整实现，支持浮点数输入、增减按钮、描边样式、焦点指示器和状态层效果。

## 类参考

```cpp
namespace cf::ui::widget::material;

class DoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT
};
```

头文件：`ui/widget/material/widget/doublespinbox/doublespinbox.h`

## 基本用法

```cpp
#include "widget/material/widget/doublespinbox/doublespinbox.h"

using namespace cf::ui::widget::material;

// 创建双精度数值调节框
auto* spin = new DoubleSpinBox(this);
spin->setRange(0.0, 100.0);
spin->setValue(50.0);
spin->setSingleStep(0.1);
spin->setDecimals(2);

// 设置前缀和后缀
spin->setPrefix("$ ");
spin->setSuffix(" mm");

// 连接信号（与 QDoubleSpinBox 兼容）
connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MyClass::onValueChanged);
```

## 与 SpinBox 的区别

| 特性 | SpinBox | DoubleSpinBox |
|------|---------|---------------|
| 基类 | `QSpinBox` | `QDoubleSpinBox` |
| 数值类型 | `int` | `double` |
| 小数位数 | 不适用 | `setDecimals(int)` |
| 步长精度 | 整数步长 | 浮点步长 |

其余视觉和行为完全一致。

## 增减按钮

与 SpinBox 相同，提供增减按钮：

```cpp
// 增加按钮（incrementButtonRect）
// 减少按钮（decrementButtonRect）
// 每个按钮有独立的 hover/pressed 状态追踪
```

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

DoubleSpinBox 的 `paintEvent` 实现 7 步 Material Design 绘制流程：

1. 绘制背景（`drawBackground`）
2. 绘制状态层（`drawStateLayer`）
3. 绘制水波纹（`drawRipple`）
4. 绘制描边（`drawOutline`）
5. 绘制文本（`drawText`）
6. 绘制增减按钮（`drawButtons`）
7. 绘制焦点指示器（`drawFocusIndicator`）

## 内部 LineEdit 约束

重写了 `resizeEvent` 以约束内部 LineEdit：

```cpp
// 将内部 lineEdit 限制在文本区域
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
| 继承自 QDoubleSpinBox | `setValue()`, `setRange()`, `setSingleStep()`, `setDecimals()`, `setPrefix()`, `setSuffix()` 等 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [SpinBox - Material 数值调节框](./spinbox.md)
- [TextField - Material 文本输入框](./textfield.md)
