# ScrollView - Material 滚动视图

`ScrollView` 是 Material Design 3 滚动区域控件的完整实现，具有自定义滚动条、淡入淡出效果和主题集成。支持水平和垂直滚动。

## 类参考

```cpp
namespace cf::ui::widget::material;

class ScrollView : public QScrollArea {
    Q_OBJECT
    Q_PROPERTY(bool scrollbarFadeEnabled READ scrollbarFadeEnabled WRITE setScrollbarFadeEnabled)
    Q_PROPERTY(int scrollbarFadeDelay READ scrollbarFadeDelay WRITE setScrollbarFadeDelay)
    Q_PROPERTY(bool scrollbarHoverExpansion READ scrollbarHoverExpansion WRITE setScrollbarHoverExpansion)
};
```

头文件：`ui/widget/material/widget/scrollview/scrollview.h`

## 滚动条状态

Material Design 3 定义了三种滚动条交互状态：

```cpp
enum class ScrollbarState {
    Idle,    // 空闲 - 40% 透明度，12dp 宽度
    Hovered, // 悬停 - 100% 透明度，16dp 宽度
    Dragged  // 拖拽 - 100% 透明度，16dp 宽度，状态层叠加
};
```

## 基本用法

```cpp
#include "widget/material/widget/scrollview/scrollview.h"

using namespace cf::ui::widget::material;

// 创建滚动视图
auto* scroll = new ScrollView(this);

// 设置内容控件
auto* content = new QWidget();
auto* layout = new QVBoxLayout(content);
// ... 添加子控件到布局中
scroll->setWidget(content);

// 启用滚动条淡入淡出
scroll->setScrollbarFadeEnabled(true);
scroll->setScrollbarFadeDelay(500);  // 500ms 后淡出

// 启用悬停扩展
scroll->setScrollbarHoverExpansion(true);
```

## 自定义滚动条

ScrollView 完全重写了默认滚动条渲染，使用自定义绘制：

- 滚动条宽度：12dp（空闲）/ 16dp（悬停/拖拽）
- 圆角：完全圆角
- 淡入淡出动画：基于不透明度插值
- 宽度动画：基于宽度插值

```cpp
// 滚动条动画参数：
// ANIMATION_FRAME_MS = 16ms (~60fps)
// ANIMATION_SPEED_WIDTH = 0.3f
// ANIMATION_SPEED_OPACITY = 0.2f
```

## 淡入淡出效果

```cpp
// 启用淡入淡出效果（滚动停止后自动隐藏滚动条）
scroll->setScrollbarFadeEnabled(true);

// 设置淡出延迟（默认 500ms）
scroll->setScrollbarFadeDelay(500);

// 滚动时自动显示滚动条
// 停止滚动后延迟隐藏
// 鼠标悬停时保持显示
```

## 悬停扩展

```cpp
// 启用悬停扩展（12dp -> 16dp）
scroll->setScrollbarHoverExpansion(true);

// 悬停时：
// 1. 宽度从 12dp 平滑过渡到 16dp
// 2. 透明度从 40% 过渡到 100%
```

## 滑块拖动

ScrollView 支持直接拖动滚动条滑块：

```cpp
// 鼠标按下在滑块区域：开始拖动
// 鼠标移动：更新滚动位置
// 鼠标释放：结束拖动

// 通过以下方法检测点击区域：
// isPointOverVerticalThumb()
// isPointOverHorizontalThumb()
// isPointOverVerticalTrack()
// isPointOverHorizontalTrack()
```

## 滚动条覆盖层

ScrollView 使用内部 `ScrollbarOverlay` 小部件在视口上绘制滚动条：

```cpp
// ScrollbarOverlay 是内部实现，定义在 .cpp 中
// 通过 eventFilter 跟踪视口几何变化
// 保持滚动条与视口同步
```

## 绘制流程

ScrollView 的自定义滚动条绘制步骤：

1. 计算轨道和滑块几何形状
2. 绘制轨道（`drawScrollbarTrack`）
3. 绘制滑块（`drawScrollbarThumb`）
4. 绘制状态层（`drawStateLayer`）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 背景 | Surface |
| 滚动条轨道 | 透明 |
| 滚动条滑块 | OnSurface (40%/100% 透明度) |
| 状态层 | OnSurfaceVariant |

## 主要方法

| 方法 | 说明 |
|------|------|
| `scrollbarFadeEnabled()` / `setScrollbarFadeEnabled(bool)` | 获取/设置淡入淡出效果 |
| `scrollbarFadeDelay()` / `setScrollbarFadeDelay(int)` | 获取/设置淡出延迟（ms） |
| `scrollbarHoverExpansion()` / `setScrollbarHoverExpansion(bool)` | 获取/设置悬停扩展 |
| `sizeHint()` | 获取推荐尺寸 |

## 相关文档

- [Material Design 3 滚动条规范](https://m3.material.io/components/scrollbars)
