---
title: "easing - 缓动曲线"
description: 提供了 Material Design 规范的预设缓动曲线和弹簧物理参数。缓动曲线决定了动画的速度变
---

# easing - 缓动曲线

`easing` 提供了 Material Design 规范的预设缓动曲线和弹簧物理参数。缓动曲线决定了动画的速度变化——是匀速、先慢后快、还是先快后慢——直接影响动画的"质感"。Material Design 对这个有详细规范，我们直接实现了它的标准曲线。

## 预设缓动类型

`Type` 枚举定义了 Material Design 的标准缓动类型：

```cpp
#include "ui/base/easing.h"

using namespace cf::ui::base::Easing;

// 转换为 Qt 的 QEasingCurve
QEasingCurve curve = fromEasingType(Type::Standard);

// 应用到 QVariantAnimation
QPropertyAnimation* anim = new QPropertyAnimation(this, "geometry");
anim->setEasingCurve(fromEasingType(Type::Emphasized));
anim->setDuration(300);
```text

各种类型的区别：
- `Linear`：匀速，没有加速感
- `Standard`：标准缓动，有轻微的加速和减速
- `Emphasized`：强调型缓动，加减速更明显，适合重要的动画
- `EmphasizedDecelerate` / `EmphasizedAccelerate`：单向的强调缓动
- `StandardDecelerate` / `StandardAccelerate`：单向的标准缓动

选择的原则：**动画越重要，缓动越明显**。比如打开对话框用 `Emphasized`，淡出一个提示用 `Standard` 就够了。

## 自定义贝塞尔曲线

如果预设曲线不满足需求，`custom()` 函数可以创建任意三次贝塞尔曲线：

```cpp
// 自定义曲线：控制点 (0.4, 0.0, 0.2, 1.0)
QEasingCurve myCurve = custom(0.4f, 0.0f, 0.2f, 1.0f);

// 过冲效果：y 值超出 [0, 1]
QEasingCurve overshoot = custom(0.34f, 1.56f, 0.64f, 1.0f);

// 弹性效果
QEasingCurve bounce = custom(0.68f, -0.6f, 0.32f, 1.6f);
```text

控制点参数的含义：
- `x1, y1` 是第一个控制点的坐标
- `x2, y2` 是第二个控制点的坐标
- 曲线起点固定在 (0, 0)，终点固定在 (1, 1)
- x 坐标建议在 [0, 1] 范围内，y 坐标可以超出

⚠️ 如果 x1 或 x2 超出 [0, 1] 范围，曲线会出现"回退"——动画可能会先倒退再前进，或者先快进再退回。这是有意为之的效果，但要小心使用。

## 弹簧物理预设

除了贝塞尔曲线，我们还提供了弹簧物理参数。弹簧动画比贝塞尔曲线更自然，但参数调优更困难：

```cpp
// 温和弹簧：几乎无震荡
SpringPreset gentle = springGentle();
// gentle.stiffness ≈ 200, gentle.damping ≈ 25

// 弹性弹簧：有明显回弹
SpringPreset bouncy = springBouncy();
// bouncy.stiffness ≈ 400, bouncy.damping ≈ 10

// 硬弹簧：快速到位
SpringPreset stiff = springStiff();
// stiff.stiffness ≈ 600, stiff.damping ≈ 30

// 配合 math_helper 的 springStep() 使用
auto [pos, vel] = math::springStep(currentPos, velocity, targetPos,
                                   bouncy.stiffness, bouncy.damping, dt);
```text

弹簧的调优确实有点玄学——stiffness 太小会拖沓，太大容易震荡；damping 太小会一直抖，太大又没有弹性。我们提供的预设是在实际项目中调出来的经验值，基本够用了。

⚠️ 弹簧动画不适合用在有明确时间要求的场景（比如多个动画需要同步）。贝塞尔曲线的时间是确定的，弹簧的"到位时间"取决于目标距离。

## 实际使用建议

选哪种缓动，看场景：

```cpp
// 入场动画：用 Emphasized，给用户明确的视觉反馈
anim->setEasingCurve(fromEasingType(Type::Emphasized));

// 状态切换：用 Standard，不要过度吸引注意力
anim->setEasingCurve(fromEasingType(Type::Standard));

// 背景淡入淡出：用 Linear，不抢戏
anim->setEasingCurve(fromEasingType(Type::Linear));

// 按钮点击反馈：用弹簧，增加触感
SpringPreset preset = springBouncy();
// 在 update 循环中调用 springStep()
```text

总的原则：**用户主动触发的操作用强缓动/弹簧，系统自动的状态变化用弱缓动**。这样既能传达操作的"手感"，又不会让界面显得太花哨。

## 相关文档

- [math_helper - 数学工具](./math_helper.md)
- [geometry_helper - 几何助手](./geometry_helper.md)
