---
title: "CFMaterialScaleAnimation - Material 缩放动画"
description: 是 Material Design 3 运动系统中的尺寸变换实现，控制 widget 以缩放方式出现
---

# CFMaterialScaleAnimation - Material 缩放动画

`CFMaterialScaleAnimation` 是 Material Design 3 运动系统中的尺寸变换实现，控制 widget 以缩放方式出现或消失。与淡入淡出和滑动相比，缩放动画有更强的"强调"意味——它让元素感觉像是"从某个点生长出来"，适合用于需要抓住用户注意力的重要交互。

## 在 Material Design 3 中的定位

Material Design 将缩放归类为"强调性变换"，主要用于：

| 场景 | 效果 | 语义 |
|------|------|------|
| 对话框、菜单弹出 | 从 0.8~0.9 放大到 1.0 | 聚焦，吸引用户注意 |
| 按钮按下效果 | 缩小到 0.95 | 触觉反馈 |
| FAB 展开 | 从小圆点放大成完整按钮 | 状态展开 |
| 图片查看 | 缩小/放大 | 空间导航 |

缩放动画的核心是"中心锚点"——默认情况下，widget 会以其中心点为基准进行缩放，这符合人类对"生长"的直觉认知。

## 基本用法

缩放动画的构造比较简单，只需要 motion spec：

```cpp
#include "ui/components/material/cfmaterial_scale_animation.h"

using namespace cf::ui::components::material;

// 创建一个缩放动画
auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec, this);

// 设置目标 widget
scaleAnim->setTargetWidget(myDialog);

// 开始动画（Forward = 放大，Backward = 缩小）
scaleAnim->start(Direction::Forward);
```text

默认的缩放范围是 0.8 到 1.0（从 80% 放大到 100%），这是 Material 推荐的"弹出"效果参数。

## 中心锚点

缩放动画最关键的设计决策是锚点位置。我们默认从中心点缩放（`scaleFromCenter = true`），这样 widget 的中心位置保持不变，四周均匀缩放：

```cpp
// 默认行为：从中心缩放
auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec);
scaleAnim->setScaleFromCenter(true);  // 这是默认值
scaleAnim->setTargetWidget(widget);
scaleAnim->start();

// widget 会保持中心位置不变，四周同时收缩/扩张
```text

如果需要从左上角或其他位置缩放，可以关闭中心缩放模式：

```cpp
scaleAnim->setScaleFromCenter(false);
// 此时缩放会以 widget 几何原点（左上角）为锚点
```bash

⚠️ 非中心缩放在某些布局下可能会导致 widget 位置发生明显偏移，使用时需要谨慎测试。

## 几何变换实现

我们的缩放是通过修改 widget 的 `setGeometry()` 实现的，而不是使用 `QTransform`。这个选择是经过权衡的：

| 方案 | 优点 | 缺点 |
|------|------|------|
| setGeometry | 布局兼容性好，重绘正确 | 性能稍低，需要手动计算 |
| QTransform | 性能更好，硬件加速 | 可能导致布局错乱，边距问题 |

使用 `setGeometry` 意味着动画过程中 widget 的实际几何属性在变化，布局系统和父容器会正确感知这个变化。如果你在动画期间需要查询 widget 的尺寸，得到的是实时缩放后的值：

```cpp
scaleAnim->start();
// ... 动画进行中 ...
qDebug() << widget->width();   // 当前缩放后的宽度
qDebug() << widget->height();  // 当前缩放后的高度
```bash

## 时序参数

缩放动画的时长选择取决于元素的"重要性"和"预期注意力"：

| 场景 | 推荐时长 | 推荐缓动 |
|------|----------|----------|
| 按钮/小图标 | 150ms | EmphasizedDecelerate |
| 对话框/卡片 | 200ms | EmphasizedDecelerate |
| 菜单/下拉列表 | 250ms | EmphasizedDecelerate |
| 全屏转场 | 300ms | EmphasizedDecelerate |

缩放动画几乎总是用 `EmphasizedDecelerate` 缓动——快速启动、缓慢停止的效果能产生"有力但可控"的感觉。

## 常见使用场景

### 对话框弹出

```cpp
void showDialog(QWidget* dialog) {
    // 初始状态：设置为缩小状态
    dialog->setGeometry(
        dialog->x() + dialog->width() * 0.1,
        dialog->y() + dialog->height() * 0.1,
        dialog->width() * 0.8,
        dialog->height() * 0.8
    );
    dialog->show();

    // 缩放到正常大小
    auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec);
    scaleAnim->setTargetWidget(dialog);
    scaleAnim->start(Direction::Forward);
}
```text

### 按钮按下效果

```cpp
class PressButton : public QPushButton {
public:
    void mousePressEvent(QMouseEvent* event) override {
        // 按下时缩小
        auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec);
        scaleAnim->setTargetWidget(this);
        scaleAnim->start(Direction::Backward);  // 缩小
        QPushButton::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        // 释放时恢复
        auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec);
        scaleAnim->setTargetWidget(this);
        scaleAnim->start(Direction::Forward);  // 放大
        QPushButton::mouseReleaseEvent(event);
    }
};
```text

### 菜单展开

```cpp
void showMenu(QWidget* menu) {
    // 菜单从按钮位置"生长"出来
    // 需要将菜单的缩放中心设置为靠近按钮的位置
    // 这可以通过初始几何属性调整实现

    menu->show();

    auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec);
    scaleAnim->setTargetWidget(menu);
    scaleAnim->start();
}
```text

### 组合动画：弹出 + 淡入

缩放和淡入的组合是"出现"效果的经典组合：

```cpp
void showCard(QWidget* card) {
    card->show();

    auto scaleAnim = factory->getAnimation("md.animation.scaleUp");
    auto fadeAnim = factory->getAnimation("md.animation.fadeIn");

    scaleAnim->setTargetWidget(card);
    fadeAnim->setTargetWidget(card);

    scaleAnim->start();
    fadeAnim->start();
}
```text

两个动画同步运行能产生"从无到有"的完整视觉体验——缩放负责空间维度，透明度负责视觉维度。

## 性能考量

缩放动画涉及 widget 几何属性的变化，可能触发父容器的重新布局：

1. **避免在动画期间修改布局**：如果 widget 的父容器使用复杂布局，动画期间禁用自动更新可以提升性能
2. **固定尺寸 widget 效果更好**：使用固定几何属性的 widget 动画更流畅
3. **考虑淡入淡出替代**：对于简单场景，纯透明度动画性能更好且视觉效果足够

## 缩放值范围

虽然 Material 推荐的缩放范围是 0.8~1.0（弹出）或 1.0~0.8（收起），但你可以根据需要调整：

```cpp
// 更明显的弹出效果：从 0.5 开始
// 这需要修改动画配置或创建自定义动画

// 微妙的强调：从 0.95 开始
// 适合小元素或需要低调的场景
```text

缩放到 1.0 以上（放大效果）在 Material Design 中比较少见，通常用于特殊交互如图片预览。这种场景下可能需要配合淡入淡出来避免突兀。

## 与布局管理器的交互

如果你的 widget 位于布局管理器中，缩放动画可能会与布局系统产生冲突：

```cpp
// widget 在 QVBoxLayout 中
auto layout = new QVBoxLayout(parent);
layout->addWidget(myWidget);

// 缩放动画会改变 widget 的几何属性
// 布局管理器可能会尝试"纠正"这个变化
auto scaleAnim = std::make_unique<CFMaterialScaleAnimation>(&motionSpec);
scaleAnim->setTargetWidget(myWidget);
scaleAnim->start();  // 可能导致布局抖动
```cpp

解决这个问题有几种方案：

1. 动画期间临时移除布局约束
2. 使用固定位置的 widget（不使用布局）
3. 在布局中插入占位 widget 来吸收尺寸变化

## 常见问题

**Q: 动画过程中 widget 闪烁**

A: 可能是 `WA_OpaquePaintEvent` 或 `WA_NoSystemBackground` 属性设置不当。尝试设置 `widget->setAttribute(Qt::WA_OpaquePaintEvent)`。

**Q: 缩放后 widget 边缘模糊**

A: 这是抗锯齿的正常现象。如果需要更清晰的边缘，可以启用 `AA_EnableHighDpiScaling` 或使用更高分辨率的资源。

**Q: 动画结束后布局被破坏**

A: 动画会修改 widget 的几何属性。如果需要恢复布局，监听 `finished` 信号并在其中调用 `widget->setGeometry(originalGeometry)`。

## 相关文档

- [CFMaterialAnimationFactory - 动画工厂](./cfmaterial_animation_factory.md)
- [CFMaterialFadeAnimation - 淡入淡出动画](./cfmaterial_fade_animation.md)
- [CFMaterialSlideAnimation - 滑动动画](./cfmaterial_slide_animation.md)
- [Material Design 3 运动规范](https://m3.material.io/styles/motion)
