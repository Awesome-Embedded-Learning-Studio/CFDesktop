# Slider - Material 滑块

`Slider` 是 Material Design 3 滑块控件的完整实现，支持水平/垂直方向、活动/非活动轨道、带海拔的滑块、刻度标记和状态层。

## 类参考

```cpp
namespace cf::ui::widget::material;

class Slider : public QSlider {
    Q_OBJECT
};
```

头文件：`ui/widget/material/widget/slider/slider.h`

## 基本用法

```cpp
#include "widget/material/widget/slider/slider.h"

using namespace cf::ui::widget::material;

// 水平滑块（默认）
auto* slider = new Slider(this);
slider->setRange(0, 100);
slider->setValue(50);

// 垂直滑块
auto* vSlider = new Slider(Qt::Vertical, this);
vSlider->setRange(0, 100);

// 连接信号（与 QSlider 兼容）
connect(slider, &QSlider::valueChanged, this, &MyClass::onValueChanged);
```

## 方向

Slider 支持两种方向：

```cpp
// 水平方向（默认）
auto* horizontal = new Slider(Qt::Horizontal, this);

// 垂直方向
auto* vertical = new Slider(Qt::Vertical, this);
```

## 轨道绘制

滑块将轨道分为两部分：

- **非活动轨道（Inactive Track）**：从起点到滑块位置的部分，使用较淡的颜色
- **活动轨道（Active Track）**：从起点到当前滑块位置的填充部分，使用 Primary 颜色

```cpp
// 轨道高度遵循 Material Design 规范
// 滑块半径遵循 Material Design 规范
```

## 滑块与海拔

滑块（Thumb）使用 `MdElevationController` 提供海拔效果：

- 正常状态：无阴影
- 悬停/按压状态：带阴影的滑块

## 刻度标记

Slider 支持刻度标记（Tick Marks）绘制：

```cpp
// 通过 QSlider 的标准属性控制刻度
slider->setTickPosition(QSlider::TicksBelow);
slider->setTickInterval(10);
```

刻度标记会沿着轨道均匀分布，使用 `inactiveTrackColor` 绘制。

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 非活动轨道 + 活动轨道 + 滑块 |
| Hovered | 状态层叠加 + 滑块阴影 |
| Pressed | 水波纹 + 滑块阴影 |
| Focused | 焦点环 |
| Disabled | 38% 透明度 |

## 拖动行为

Slider 支持鼠标拖动：

- 鼠标按下时记录位置，触发水波纹
- 鼠标移动时更新滑块位置
- 鼠标释放时结束拖动状态

`thumbPosition()` 方法根据当前值计算滑块在轨道上的精确位置。

## 绘制流程

Slider 的 `paintEvent` 实现以下绘制步骤：

1. 计算轨道区域（`trackRect`）
2. 绘制非活动轨道（`drawInactiveTrack`）
3. 绘制活动轨道（`drawActiveTrack`）
4. 绘制刻度标记（`drawTickMarks`）
5. 绘制滑块（`drawThumb`）
6. 绘制水波纹（`drawRipple`）
7. 绘制焦点指示器（`drawFocusIndicator`）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 活动轨道 | Primary |
| 非活动轨道 | SurfaceVariant |
| 滑块 | Primary |
| 状态层 | 根据 hover/press 状态计算 |
| 刻度标记 | InactiveTrackColor |

## 主要方法

| 方法 | 说明 |
|------|------|
| 继承自 QSlider | `setValue()`, `setRange()`, `setOrientation()` 等 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [ProgressBar - Material 进度条](./progressbar.md)
- [MdElevationController - 阴影控制器](../../base/elevation_controller.md)
- [Material Design 3 滑块规范](https://m3.material.io/components/sliders)
