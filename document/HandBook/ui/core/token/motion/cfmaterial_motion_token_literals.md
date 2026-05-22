---
title: Motion Token 字面量
description: 定义了 Material Design 3 动效系统的全部 Token 字面量。Material 的
---

# Motion Token 字面量

`cfmaterial_motion_token_literals.h` 定义了 Material Design 3 动效系统的全部 Token 字面量。Material 的动效不仅仅是"让东西动起来"，而是一套精心设计的时间（Duration）和缓动（Easing）体系，用来传达 UI 的层级关系和交互反馈。

这套字面量让我们可以用语义化的方式引用动效参数，而不是到处散落 `animate(300ms, ease-out)` 这种魔法数字。统一管理动效参数的好处是，调整动效风格时只需改主题配置，整个应用的动画节奏会保持一致。

## 动效时长 Token

Material 定义了 6 类标准动效时长，按元素尺寸和进出方向划分：

```cpp
#include "ui/core/token/motion/cfmaterial_motion_token_literals.h"

using namespace cf::ui::core::token::literals;

// 小元素进入 - 200ms
const char* shortEnterDuration = MOTION_SHORT_ENTER_DURATION;
// "md.motion.shortEnter.duration"

// 小元素退出 - 150ms
const char* shortExitDuration = MOTION_SHORT_EXIT_DURATION;
// "md.motion.shortExit.duration"

// 中等元素进入 - 300ms
const char* mediumEnterDuration = MOTION_MEDIUM_ENTER_DURATION;

// 中等元素退出 - 250ms
const char* mediumExitDuration = MOTION_MEDIUM_EXIT_DURATION;

// 大元素进入 - 450ms
const char* longEnterDuration = MOTION_LONG_ENTER_DURATION;

// 大元素退出 - 400ms
const char* longExitDuration = MOTION_LONG_EXIT_DURATION;

// 状态变化 - 200ms
const char* stateChangeDuration = MOTION_STATE_CHANGE_DURATION;

// 涟漪展开 - 400ms
const char* rippleExpandDuration = MOTION_RIPPLE_EXPAND_DURATION;

// 涟漪消散 - 150ms
const char* rippleFadeDuration = MOTION_RIPPLE_FADE_DURATION;
```text

注意退出时长通常比进入时长短，这是个刻意的设计——用户等待东西出现比等待东西消失更不耐烦。

## 动效缓动 Token

缓动函数决定了动画随时间的变化速率，Material 定义了三类标准缓动：

```cpp
// 小元素进入缓动 - EmphasizedDecelerate
const char* shortEnterEasing = MOTION_SHORT_ENTER_EASING;
// "md.motion.shortEnter.easing"

// 小元素退出缓动 - EmphasizedAccelerate
const char* shortExitEasing = MOTION_SHORT_EXIT_EASING;

// 中等元素进入缓动 - EmphasizedDecelerate
const char* mediumEnterEasing = MOTION_MEDIUM_ENTER_EASING;

// 中等元素退出缓动 - EmphasizedAccelerate
const char* mediumExitEasing = MOTION_MEDIUM_EXIT_EASING;

// 大元素进入/退出缓动 - Emphasized
const char* longEnterEasing = MOTION_LONG_ENTER_EASING;
const char* longExitEasing = MOTION_LONG_EXIT_EASING;

// 状态变化缓动 - Standard
const char* stateChangeEasing = MOTION_STATE_CHANGE_EASING;

// 涟漪展开缓动 - Standard
const char* rippleExpandEasing = MOTION_RIPPLE_EXPAND_EASING;

// 涟漪消散缓动 - Linear
const char* rippleFadeEasing = MOTION_RIPPLE_FADE_EASING;
```bash

## 缓动函数说明

Material 的缓动函数名称有点抽象，这里解释一下：

| 缓动名称 | 数学特性 | 适用场景 | 视觉效果 |
|---------|---------|---------|---------|
| EmphasizedDecelerate | 快进慢出 | 元素进入 | 快速入场后柔和减速 |
| EmphasizedAccelerate | 慢进快出 | 元素退出 | 缓慢启动后快速离开 |
| Emphasized | 先加速再减速（明显） | 大元素进入 | 起步和结束都柔和，中间加速明显 |
| Standard | 先加速再减速（轻微） | 状态变化 | 轻微的缓动，不干扰视线 |
| Linear | 匀速 | 涟漪消散 | 无缓动，适合淡出效果 |

Emphasized 系列用于大尺寸元素，是因为大元素的移动更明显，需要更柔和的缓动来避免视觉冲击。Standard 用于状态变化（如按钮按下），不希望太夸张。

## 在主题系统中使用

动效 Token 的使用方式和其他 Token 一致，由主题系统解析成实际的时长和缓动：

```cpp
// 伪代码：主题系统如何解析动效 Token
struct MotionSpec {
    int durationMs;
    EasingFunction easing;
};

class MaterialTheme {
    MotionSpec resolveMotion(const char* durationToken,
                            const char* easingToken) const {
        return {
            durationTable.at(durationToken),
            easingTable.at(easingToken)
        };
    }
};

// 使用示例
MaterialTheme theme;

// 小元素进入动画
auto fadeIn = theme.resolveMotion(
    MOTION_SHORT_ENTER_DURATION,
    MOTION_SHORT_ENTER_EASING
);
element.animate(fadeIn);

// 对话框进入动画（大元素）
auto dialogEnter = theme.resolveMotion(
    MOTION_LONG_ENTER_DURATION,
    MOTION_LONG_ENTER_EASING
);
dialog.animate(dialogEnter);
```text

## 场景选择指南

选择哪个动效参数组合，主要看元素尺寸和动效类型：

```cpp
// 小元素（按钮、开关、标签）
// 进入: 200ms + EmphasizedDecelerate
// 退出: 150ms + EmphasizedAccelerate
auto smallEnter = resolveMotion(
    MOTION_SHORT_ENTER_DURATION,
    MOTION_SHORT_ENTER_EASING
);

// 中等元素（卡片、列表项）
// 进入: 300ms + EmphasizedDecelerate
// 退出: 250ms + EmphasizedAccelerate
auto mediumEnter = resolveMotion(
    MOTION_MEDIUM_ENTER_DURATION,
    MOTION_MEDIUM_ENTER_EASING
);

// 大元素（对话框、抽屉）
// 进入: 450ms + Emphasized
// 退出: 400ms + Emphasized
auto largeEnter = resolveMotion(
    MOTION_LONG_ENTER_DURATION,
    MOTION_LONG_ENTER_EASING
);

// 状态变化（按钮按下、切换开关）
// 200ms + Standard
auto stateChange = resolveMotion(
    MOTION_STATE_CHANGE_DURATION,
    MOTION_STATE_CHANGE_EASING
);

// 涟漪效果
// 展开: 400ms + Standard
// 消散: 150ms + Linear
auto rippleExpand = resolveMotion(
    MOTION_RIPPLE_EXPAND_DURATION,
    MOTION_RIPPLE_EXPAND_EASING
);
```text

⚠️ 一个常见的错误是用错缓动方向。进入动画应该用 Decelerate（减速），这样元素到达终点时会柔和地停下来；退出动画应该用 Accelerate（加速），元素快速离开不留痕迹。用反了会感觉很怪——进入时"哐"一下撞上终点，退出时拖泥带水。

## 批量遍历

提供了遍历所有动效 Token 的辅助：

```cpp
// 遍历所有 Motion Token
for (size_t i = 0; i < MOTION_TOKEN_COUNT; ++i) {
    printf("Motion Token %zu: %s\n", i, ALL_MOTION_TOKENS[i]);
}
```text

## 自定义动效参数

如果你的设计需要非标准的动效参数，有两种处理方式：

```cpp
// 方式 1：在主题配置中添加自定义 Token
// 保持代码语义化，但需要修改主题系统

// 方式 2：直接构造 MotionSpec（灵活但不统一）
// 对于特殊场景，直接传值更实际
MotionSpec custom = {500, CustomEasing};
element.animate(custom);
```text

建议把常用的自定义动效也纳入 Token 管理，保持代码的一致性。

## 性能考虑

动效虽然是视觉体验的重要组成部分，但也要注意性能：

```cpp
// 硬件加速是个好主意
element.setLayerType(LAYER_TYPE_HARDWARE);
element.animate(spec);

// 大量元素同时动画时，考虑简化动效
// 比如用短时长代替长时长，或者取消缓动
```text

Material 的动效时长（最大 450ms）是经过平衡的——既足够传达视觉反馈，又不会让用户等太久。如果你的动效感觉"拖"，可能不是时长问题，而是缓动曲线选择不当。

## 可访问性

对于动效敏感的用户，应该提供"减少动效"选项：

```cpp
// 伪代码：尊重用户的动效偏好
if (userPrefersReducedMotion()) {
    // 跳过动画，或者使用极短时长
    element.setOpacity(1.0f);  // 直接设置最终状态
} else {
    // 正常动画
    element.animate(spec);
}
```text

Material 的动效设计已经考虑了可访问性，但提供降级选项仍然是好实践。

## 相关文档

- [Material Token 字面量](../material_scheme/cfmaterial_token_literals.md)
- [Typography Token 字面量](../typography/cfmaterial_typography_token_literals.md)
- [Radius Scale Token 字面量](../radius_scale/cfmaterial_radius_scale_literals.md)
