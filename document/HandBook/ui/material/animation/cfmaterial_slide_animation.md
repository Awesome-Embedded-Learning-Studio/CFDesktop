---
title: "CFMaterialSlideAnimation - Material 滑动动画"
description: 实现了 Material Design 3 规范中的位移动画，控制 widget 在屏幕空间中的位置
---

# CFMaterialSlideAnimation - Material 滑动动画

`CFMaterialSlideAnimation` 实现了 Material Design 3 规范中的位移动画，控制 widget 在屏幕空间中的位置变化。滑动动画是 Material Design 中最重要的过渡方式之一——它利用空间位置的变化来建立元素之间的视觉关联，让用户理解"这个东西从哪里来、到哪里去"。

## 在 Material Design 3 中的定位

在 Material 的运动语言里，滑动代表了"元素从边界进入或离开"。不同方向的滑动有不同的语义：

| 方向 | 典型用途 | 语义 |
|------|----------|------|
| Up | 底部sheet、键盘升起、 Snackbar | 从下方边界进入 |
| Down | 下拉刷新、关闭底部sheet | 向下方边界退出 |
| Left | 侧边菜单收起、向右切换内容 | 向左边界退出/进入 |
| Right | 侧边菜单展开、向左切换内容 | 从右边界进入/退出 |

这个语义映射不是绝对的——具体用哪个方向取决于你的 UI 布局和导航逻辑，但保持一致性很重要。

## 基本用法

滑动动画需要在构造时指定方向，因为方向决定了位移的正负：

```cpp
#include "ui/components/material/cfmaterial_slide_animation.h"

using namespace cf::ui::components::material;

// 创建一个向上滑动的动画（widget 从下方移入）
auto slideAnim = std::make_unique<CFMaterialSlideAnimation>(
    &motionSpec,
    SlideDirection::Up,  // 滑动方向
    this
);

// 设置滑动距离（像素）
slideAnim->setDistance(100.0f);

// 设置目标并启动
slideAnim->setTargetWidget(myCard);
slideAnim->start(Direction::Forward);
```text

方向枚举的命名可能会让你困惑——`SlideDirection::Up` 表示 widget 向上移动，视觉上是"从下方滑入"。反过来想：枚举值描述的是 widget 移动的轨迹方向，而不是来源方向。

## 滑动距离

`setDistance()` 控制动画的位移量，单位是像素。默认值是 100px，但这个值在不同屏幕尺寸下可能不合适。我们建议根据实际使用场景动态计算：

```cpp
// 列表项从右侧滑入，距离为父容器宽度的 30%
float offset = parentWidget->width() * 0.3f;
slideAnim->setDistance(offset);

// 底部 sheet 从屏幕底部滑入，距离为父容器高度
slideAnim->setDistance(parentWidget->height());

// Snackbar 从底部轻微上浮
slideAnim->setDistance(50.0f);
```text

⚠️ 距离值应该是正数。方向由 `SlideDirection` 控制，不需要用负距离来表示反向移动。

## 原始位置保存

动画会在启动时保存 widget 的原始位置，并在动画停止时恢复。这个设计的行为是：

```cpp
// 假设 widget 当前在 (100, 100)
slideAnim->setTargetWidget(widget);
slideAnim->start(Direction::Forward);
// ... 动画运行中，widget 位置被修改 ...
slideAnim->stop();  // widget 回到 (100, 100)
```text

但如果你在动画过程中手动移动了 widget，动画停止时恢复的位置仍然是启动时的原始位置，这可能导致视觉跳跃。如果需要在动画结束后将 widget 定位到新位置，应该在 `finished` 信号中处理：

```cpp
connect(slideAnim.get(), &ICFAbstractAnimation::finished, this, [widget]() {
    // 动画结束后，手动设置到目标位置
    widget->move(200, 100);
});
```text

## 方向详解

四个方向的位移计算逻辑如下：

```cpp
// SlideDirection::Up: widget 向上移动（从下方进入）
// 位移应用到 y 轴，offset 为负值
offset = QPoint(0, -distance)

// SlideDirection::Down: widget 向下移动（从上方进入）
// 位移应用到 y 轴，offset 为正值
offset = QPoint(0, distance)

// SlideDirection::Left: widget 向左移动（从右侧进入）
// 位移应用到 x 轴，offset 为负值
offset = QPoint(-distance, 0)

// SlideDirection::Right: widget 向右移动（从左侧进入）
// 位移应用到 x 轴，offset 为正值
offset = QPoint(distance, 0)
```bash

理解这个逻辑有助于你在调试时判断问题方向——如果动画朝反方向移动，大概率是方向枚举选错了。

## 时序参数

滑动动画通常使用比淡入淡出更长的时长，因为位移变化的视觉冲击力更大：

| 场景 | 推荐时长 | 推荐缓动 |
|------|----------|----------|
| 小元素滑动（按钮、图标） | 200ms | EmphasizedDecelerate |
| 列表项滑入 | 250ms | EmphasizedDecelerate |
| 底部 Sheet / 侧边栏 | 300ms | EmphasizedDecelerate |
| 全屏页面切换 | 350ms | EmphasizedDecelerate |

工厂会根据你选择的动画 token 自动应用这些参数，一般不需要手动干预。

## 常见使用场景

### 底部 Sheet 弹出

```cpp
void showBottomSheet(QWidget* sheet, QWidget* parent) {
    // 初始位置：放在父容器底部之外
    int startY = parent->height();
    sheet->move(0, startY);
    sheet->show();

    // 向上滑动到最终位置
    auto slideAnim = std::make_unique<CFMaterialSlideAnimation>(
        &motionSpec, SlideDirection::Up);
    slideAnim->setDistance(parent->height());
    slideAnim->setTargetWidget(sheet);
    slideAnim->start();
}
```text

### 侧边菜单滑入

```cpp
void showSideMenu(QWidget* menu, QWidget* parent) {
    // 从右侧滑入
    menu->setParent(parent);
    menu->move(parent->width(), 0);
    menu->show();

    auto slideAnim = std::make_unique<CFMaterialSlideAnimation>(
        &motionSpec, SlideDirection::Left);
    slideAnim->setDistance(parent->width());
    slideAnim->setTargetWidget(menu);
    slideAnim->start();
}
```text

### 列表项交错动画

```cpp
void showListItems(const QList<QWidget*>& items) {
    for (int i = 0; i < items.size(); ++i) {
        auto slideAnim = std::make_unique<CFMaterialSlideAnimation>(
            &motionSpec, SlideDirection::Up);

        // 设置交错延迟
        slideAnim->setDelay(i * 50);  // 每项延迟 50ms
        slideAnim->setDistance(30.0f);
        slideAnim->setTargetWidget(items[i]);
        slideAnim->start();
    }
}
```text

### 下拉刷新

```cpp
void onPullDown(float offset) {
    // offset 是下拉的距离，正值
    auto slideAnim = std::make_unique<CFMaterialSlideAnimation>(
        &motionSpec, SlideDirection::Up);
    slideAnim->setDistance(offset);

    // 反向播放：回到原位
    slideAnim->start(Direction::Backward);
}
```text

## 性能优化建议

滑动动画涉及 widget 几何属性的变化，相对淡入淡出有更高的渲染开销：

1. **避免同时动画大量 widget**：如果一个列表有上百项同时滑入，考虑分批或使用交错延迟
2. **硬件加速**：确保 widget 的 `WA_TranslucentBackground` 或 `WA_NoSystemBackground` 属性设置正确
3. **简化布局**：动画期间避免复杂的布局计算，可以考虑用固定几何属性

## 组合使用

滑动动画常与其他动画组合以产生更丰富的效果：

```cpp
// 从右下方滑入，同时淡入
auto slideAnim = factory->getAnimation("md.animation.slideInRight");
auto fadeAnim = factory->getAnimation("md.animation.fadeIn");

slideAnim->setTargetWidget(widget);
fadeAnim->setTargetWidget(widget);

slideAnim->start();
fadeAnim->start();
```yaml

这种组合在卡片、对话框等"重要元素"出现时特别有效。

## 常见问题

**Q: 动画结束后 widget 跳回原位置**

A: 这是因为动画停止时会恢复原始位置。如果需要保持最终位置，监听 `finished` 信号并在其中手动设置 widget 位置。

**Q: 滑动方向与预期相反**

A: 检查 `SlideDirection` 枚举值。枚举表示 widget 移动的方向，不是来源方向。`Up` = 向上移动 = 从下方进入。

**Q: 动画过程中 widget 闪烁**

A: 可能是布局系统在干扰。尝试在动画期间暂停布局更新，或者使用固定位置而不是依赖布局管理器。

## 相关文档

- [CFMaterialAnimationFactory - 动画工厂](./cfmaterial_animation_factory.md)
- [CFMaterialFadeAnimation - 淡入淡出动画](./cfmaterial_fade_animation.md)
- [CFMaterialScaleAnimation - 缩放动画](./cfmaterial_scale_animation.md)
- [Material Design 3 运动规范](https://m3.material.io/styles/motion)
