# ProgressBar - Material 进度条

`ProgressBar` 是 Material Design 3 进度条控件的完整实现，支持确定（Determinate）和不确定（Indeterminate）两种模式。包含平滑动画、状态层和焦点指示器。

## 类参考

```cpp
namespace cf::ui::widget::material;

class ProgressBar : public QProgressBar {
    Q_OBJECT
};
```

头文件：`ui/widget/material/widget/progressbar/progressbar.h`

## 基本用法

```cpp
#include "widget/material/widget/progressbar/progressbar.h"

using namespace cf::ui::widget::material;

// 确定模式
auto* progress = new ProgressBar(this);
progress->setRange(0, 100);
progress->setValue(30);

// 不确定模式
auto* loading = new ProgressBar(this);
loading->setRange(0, 0);  // min=max 表示不确定模式

// 连接信号（与 QProgressBar 兼容）
connect(progress, &QProgressBar::valueChanged, this, &MyClass::onProgressChanged);
```

## 确定模式 vs 不确定模式

| 模式 | 设置方式 | 视觉效果 |
|------|----------|----------|
| Determinate | `setRange(0, N)` + `setValue(X)` | 填充从起点到当前比例 |
| Indeterminate | `setRange(0, 0)` | 循环往复的动画条 |

### 确定模式

进度条填充宽度与当前值成正比：

```cpp
progress->setRange(0, 100);
progress->setValue(75);  // 填充 75% 的宽度
```

### 不确定模式

不确定模式使用连续动画表示操作正在进行，但无法确定进度：

```cpp
progress->setRange(0, 0);  // 进入不确定模式
```

动画通过 `m_indeterminatePosition`（0.0 到 1.0）控制位置，以循环方式运行。

## 尺寸规范

ProgressBar 遵循 Material Design 3 的尺寸规范：

- 轨道高度：4dp
- 圆角：完全圆角（高度的一半）
- 活动指示器填充色：Primary 颜色

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 轨道 + 填充 |
| Hovered | 状态层叠加 |
| Focused | 焦点环 |
| Disabled | 38% 透明度 |

## 绘制流程

ProgressBar 的 `paintEvent` 实现以下绘制步骤：

1. 计算轨道区域（`trackRect`）
2. 绘制背景/轨道（`drawBackground`）
3. 确定模式：绘制填充（`drawFill`）
4. 不确定模式：绘制动画条（`drawIndeterminate`）
5. 绘制文本（`drawText`）- 如果启用
6. 绘制焦点指示器（`drawFocusIndicator`）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 轨道背景 | SurfaceVariant |
| 填充 | Primary |
| 文本 | OnSurface |
| 状态层 | 根据状态计算 |

## 不确定动画

不确定动画由内部定时器驱动：

```cpp
// 内部机制：
// m_indeterminatePosition 从 0.0 循环到 1.0
// 动画速度由 CFMaterialAnimationFactory 控制
// startIndeterminateAnimation() / stopIndeterminateAnimation() 管理生命周期
```

## 主要方法

| 方法 | 说明 |
|------|------|
| 继承自 QProgressBar | `setValue()`, `setRange()`, `setOrientation()` 等 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [Slider - Material 滑块](./slider.md)
- [Material Design 3 进度条规范](https://m3.material.io/components/progress-indicators)
