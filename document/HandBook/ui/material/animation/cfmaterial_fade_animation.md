---
title: "CFMaterialFadeAnimation - Material 淡入淡出动画"
description: 是 Material Design 3 运动系统中的透明度动画实现，负责按照 Material 规范
---

# CFMaterialFadeAnimation - Material 淡入淡出动画

`CFMaterialFadeAnimation` 是 Material Design 3 运动系统中的透明度动画实现，负责按照 Material 规范控制 widget 的不透明度变化。淡入淡出是 UI 中最基础的过渡效果，我们实现这个动画不仅是为了视觉美观，更重要的是在元素出现、消失或状态切换时给用户清晰的心理预期。

## 在 Material Design 3 中的定位

Material Design 3 的 Motion System 将淡入淡出归类为"强调性进入"和"强调性退出"的标准实现。与其他动画类型相比，淡入淡出不涉及位置或尺寸变化，是最"安静"的过渡方式，适合用于：

- 模态对话框的出现和消失
- 列表项的批量加载
- 状态指示器的切换
- 与其他动画组合使用时作为"透明度维度"

## 基本用法

淡入淡出动画需要通过 `IMotionSpec` 获取时序参数，这个 spec 通常从主题的 MotionScheme 中获得：

```cpp
#include "ui/components/material/cfmaterial_fade_animation.h"
#include "ui/core/theme.h"

using namespace cf::ui::components::material;

// 从主题获取 motion spec
auto& motionSpec = theme->motion_spec();

// 创建淡入动画
auto fadeAnim = std::make_unique<CFMaterialFadeAnimation>(&motionSpec, this);

// 设置目标 widget
fadeAnim->setTargetWidget(myDialog);

// 开始动画（Forward = 淡入，Backward = 淡出）
fadeAnim->start(Direction::Forward);
```text

动画通过 `QGraphicsOpacityEffect` 作用于 widget，这意味着它适用于所有继承自 `QWidget` 的控件，包括那些本身不直接支持透明度属性的组件。

## 透明度范围

默认情况下，动画会在完全透明（0.0）和完全不透明（1.0）之间插值。如果你需要自定义范围（比如从半透明到完全不透明），可以通过继承或修改动画配置来实现：

```cpp
// 动画内部使用 0.0 ~ 1.0 的标准范围
// 如果需要部分淡入效果，需要在动画完成后手动设置最终状态
connect(fadeAnim.get(), &ICFAbstractAnimation::finished, this, [widget]() {
    // 设置为某个中间透明度值
    widget->setWindowOpacity(0.8);
});
```bash

## 时序控制

动画的持续时间和缓动曲线由 MotionSpec 决定，工厂会根据动画类型自动选择合适的 motion token：

| 场景 | 推荐时长 | 推荐缓动 |
|------|----------|----------|
| 小元素淡入（按钮、图标） | 200ms | EmphasizedDecelerate |
| 小元素淡出 | 150ms | EmphasizedAccelerate |
| 中等元素淡入（卡片、列表项） | 250ms | EmphasizedDecelerate |
| 大元素淡入（对话框、面板） | 300ms | EmphasizedDecelerate |

## 与 QGraphicsOpacityEffect 的交互

动画会自动为 target widget 创建并管理 `QGraphicsOpacityEffect`。这个设计带来两个需要注意的点：

第一，如果 widget 已经有 graphics effect，动画会尝试复用它而不是覆盖。这意味着你可以在同一个 effect 上组合多个属性动画：

```cpp
// widget 已有一个模糊效果
auto* blurEffect = new QGraphicsBlurEffect(widget);
widget->setGraphicsEffect(blurEffect);

// 淡入动画会创建独立的 opacity effect
// 不会影响已有的模糊效果
fadeAnim->setTargetWidget(widget);
```text

第二，effect 的生命周期由动画管理。当动画销毁时，如果 effect 是它创建的，也会一并销毁。如果你需要在动画结束后保留最终的透明度状态，需要注意 effect 的所有权问题：

```cpp
// 动画结束后 effect 会被清理，widget 会恢复到不透明状态
// 如果需要保持半透明，考虑监听 finished 信号并手动设置样式
```text

## 生命周期控制

动画支持暂停、恢复、停止和反向操作：

```cpp
fadeAnim->start(Direction::Forward);  // 开始淡入

// ... 动画进行中 ...

fadeAnim->pause();   // 暂停在当前状态

fadeAnim->reverse(); // 从当前状态反向播放（淡出）

fadeAnim->stop();    // 停止并重置到初始状态
```text

`reverse()` 是一个很方便的操作，它会让动画从当前进度开始反向播放，而不是跳回起点。这在实现"鼠标悬停显示，移开隐藏"这类交互时特别好用。

## 性能考量

淡入淡出动画本身的开销很小，主要消耗在 `QGraphicsEffect` 的渲染上。如果你需要同时为大量 widget 应用淡入淡出（比如一个包含上百项的列表），考虑以下优化：

1. 批量启动时使用不同的延迟，避免所有动画在同一帧更新
2. 对于简单的透明度需求，考虑直接用 `QWidget::setWindowOpacity()` 或样式表
3. 在低端设备上可以通过工厂的全局开关禁用动画

## 常见场景

### 模态对话框淡入

```cpp
void showModalDialog(QWidget* dialog) {
    dialog->setWindowOpacity(0.0);
    dialog->show();

    auto fadeAnim = std::make_unique<CFMaterialFadeAnimation>(&motionSpec);
    fadeAnim->setTargetWidget(dialog);
    fadeAnim->start(Direction::Forward);
}
```text

### 加载状态切换

```cpp
void showLoadingIndicator() {
    loadingLabel->show();
    auto fadeAnim = factory->getAnimation("md.animation.fadeIn");
    fadeAnim->setTargetWidget(loadingLabel);
    fadeAnim->start();
}

void hideLoadingIndicator() {
    auto fadeAnim = factory->getAnimation("md.animation.fadeOut");
    connect(fadeAnim.get(), &ICFAbstractAnimation::finished, loadingLabel, &QWidget::hide);
    fadeAnim->setTargetWidget(loadingLabel);
    fadeAnim->start();
}
```text

### 组合动画

淡入淡出常与其他动画组合使用，比如从下方滑入的同时淡入：

```cpp
// 滑动动画和淡入动画同时启动
auto slideAnim = factory->getAnimation("md.animation.slideUp");
auto fadeAnim = factory->getAnimation("md.animation.fadeIn");

slideAnim->setTargetWidget(widget);
fadeAnim->setTargetWidget(widget);

slideAnim->start();
fadeAnim->start();
```text

## 相关文档

- [CFMaterialAnimationFactory - 动画工厂](./cfmaterial_animation_factory.md)
- [CFMaterialSlideAnimation - 滑动动画](./cfmaterial_slide_animation.md)
- [CFMaterialScaleAnimation - 缩放动画](./cfmaterial_scale_animation.md)
- [Material Design 3 运动规范](https://m3.material.io/styles/motion)
