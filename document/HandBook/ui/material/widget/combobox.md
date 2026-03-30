# ComboBox - Material 下拉框

`ComboBox` 是 Material Design 3 下拉框控件的完整实现，支持填充（Filled）和描边（Outlined）两种变体，带动画的下拉箭头和自定义列表样式。

## 类参考

```cpp
namespace cf::ui::widget::material;

class ComboBox : public QComboBox {
    Q_OBJECT
};
```

头文件：`ui/widget/material/widget/comboBox/combobox.h`

## 变体

```cpp
enum class ComboBoxVariant {
    Filled,    // 填充背景 + 边框
    Outlined   // 仅描边边框
};
```

## 基本用法

```cpp
#include "widget/material/widget/comboBox/combobox.h"

using namespace cf::ui::widget::material;

// 创建下拉框（默认 Filled 变体）
auto* combo = new ComboBox(this);

// 添加选项
combo->addItem("Option A");
combo->addItem("Option B");
combo->addItem("Option C");

// 使用 Outlined 变体
auto* outlined = new ComboBox(this);
outlined->setVariant(ComboBoxVariant::Outlined);

// 连接信号（与 QComboBox 兼容）
connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MyClass::onSelectionChanged);
```

## 下拉箭头动画

ComboBox 的下拉箭头有旋转动画：

- 关闭状态：箭头朝下（0 度）
- 打开状态：箭头朝上（180 度）

```cpp
// 箭头旋转由 m_arrowRotation 控制
// showPopup() 触发箭头旋转到 180 度
// hidePopup() 触发箭头旋转回 0 度
```

## 自定义弹出列表

`showPopup()` 和 `hidePopup()` 被重写以应用自定义样式：

```cpp
// showPopup() 内部：
// 1. 创建自定义弹出容器
// 2. 应用 Material Design 列表样式
// 3. 使用 QPropertyAnimation 控制弹出动画
// 4. 箭头旋转到上方

// hidePopup() 内部：
// 1. 箭头旋转回下方
// 2. 关闭弹出容器
```

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认外观 |
| Hovered | 状态层叠加 |
| Pressed | 水波纹 + 状态层 |
| Focused | 焦点环 + 状态层 |
| Popup Open | 箭头旋转 + 高亮边框 |
| Disabled | 38% 透明度 |

## 绘制流程

ComboBox 的 `paintEvent` 实现以下绘制步骤：

1. 绘制背景（`drawBackground`）
2. 绘制描边（`drawOutline`） - Outlined 变体
3. 绘制状态层（`drawStateLayer`）
4. 绘制水波纹（`drawRipple`）
5. 绘制文本（`drawText`）
6. 绘制箭头（`drawArrow`） - 带旋转动画
7. 绘制焦点指示器（`drawFocusIndicator`）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 容器背景 | Surface / SurfaceVariant |
| 文本 | OnSurface |
| 描边 | Outline |
| 箭头 | OnSurfaceVariant |
| 状态层 | 根据状态计算 |

## 主要方法

| 方法 | 说明 |
|------|------|
| `variant()` / `setVariant(ComboBoxVariant)` | 获取/设置变体 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |
| 继承自 QComboBox | `addItem()`, `currentText()`, `currentIndex()` 等 |

## 相关文档

- [TextField - Material 文本输入框](./textfield.md)
- [ListView - Material 列表视图](./listview.md)
- [Material Design 3 下拉框规范](https://m3.material.io/components/menus)
