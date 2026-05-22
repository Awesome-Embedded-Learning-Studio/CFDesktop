---
title: "mathhelper - 数学工具"
description: 提供了 UI 动画和过渡效果常用的数学工具函数，包括插值、映射、三次贝塞尔曲线和弹簧物理模拟。这些函
---

# math_helper - 数学工具

`math_helper` 提供了 UI 动画和过渡效果常用的数学工具函数，包括插值、映射、三次贝塞尔曲线和弹簧物理模拟。这些函数散落在很多地方都有实现，但我们抽出来统一管理，一是避免重复代码，二是方便后续统一调整算法参数。

## 线性插值

`lerp()` 是最基础的插值函数，在两个值之间按比例 t 进行线性过渡：

```cpp
#include "ui/base/math_helper.h"

using namespace cf::ui::math;

// 基础用法：从 0 到 100，取中间值
float value = lerp(0.0f, 100.0f, 0.5f);  // 返回 50.0

// 动画中的淡入效果
float opacity = lerp(0.0f, 1.0f, progress);  // progress 从 0 到 1

// 颜色通道插值
int red = static_cast<int>(lerp(startColor.red(), endColor.red(), t));
```text

插值参数 t 不限制在 [0, 1] 范围内——超出时会产生"外推"效果，这在某些动画场景下是有用的，但要小心使用。

## 数值限制和映射

`clamp()` 用于将数值约束在指定范围内，`remap()` 则将一个区间的值映射到另一个区间：

```cpp
// 限制范围
float x = clamp(value, 0.0f, 1.0f);  // 确保 x 在 [0, 1] 内

// 区间映射：将 [0, 100] 映射到 [-1, 1]
float normalized = remap(input, 0.0f, 100.0f, -1.0f, 1.0f);

// 进度条的实际像素位置
float pixel = remap(progress, 0.0f, 1.0f, 0.0f, trackWidth);
```text

`remap()` 在处理滑块、进度条这类 UI 控件时特别方便——不用自己算比例和偏移了。

## 三次贝塞尔曲线

自定义缓动曲线时，`cubicBezier()` 是核心函数。给定两个控制点，计算曲线在 t 位置的 y 值：

```cpp
// Material Design 的 emphasized 曲线
// 控制点 (0.2, 0.0) 和 (0, 1.0)
float eased = cubicBezier(0.2f, 0.0f, 0.0f, 1.0f, t);

// 自定义曲线
float custom = cubicBezier(0.25f, 0.1f, 0.25f, 1.0f, progress);
```text

这个函数的参数控制点 x 坐标建议限制在 [0, 1] 范围内，否则曲线会出现"回退"效果（一个 t 对应多个 x）。y 坐标可以超出这个范围，产生弹性过冲效果。

⚠️ 这是一个简化的实现，假设 x(t) ≈ t。如果需要精确的时间映射（比如控制点 x 超出 [0,1]），需要求解贝塞尔方程，那个计算量会大很多。对于 UI 动画，当前这个近似足够用了。

## 弹簧物理模拟

`springStep()` 实现了一个阻尼弹簧的单步物理模拟，用半隐式欧拉积分：

```cpp
float pos = 0.0f;    // 当前位置
float vel = 0.0f;    // 当前速度
float target = 100.0f;  // 目标位置

// 每帧调用（假设 60fps）
auto [newPos, newVel] = springStep(pos, vel, target,
                                   300.0f,  // stiffness
                                   20.0f,   // damping
                                   1.0f/60.0f);
pos = newPos;
vel = newVel;
```text

stiffness 控制弹簧的硬度——值越大回弹越快。damping 控制阻尼——值越小震荡越明显。这两个参数的调优有点玄学，我们提供了一些预设值（参见 `easing.h`）。

⚠️ 弹簧模拟的稳定性依赖于 dt 足够小。如果 stiffness 很大但 dt 也很大，可能会数值爆炸。实时动画里一般用固定时间步，别用 frame time 差值。

## 角度插值

`lerpAngle()` 处理角度插值时的 0°/360° 边界问题，总是选择最短路径：

```cpp
// 从 350° 转到 10°，会逆时针转 20°，而不是顺时针转 340°
float angle = lerpAngle(350.0f, 10.0f, 0.5f);  // 返回 0°

// 旋转动画
float currentAngle = lerpAngle(startAngle, targetAngle, progress);
```text

这个函数对旋转动画特别有用——不然你的 UI 元素可能会莫名其妙地绕一大圈。

## 相关文档

- [easing - 缓动曲线](./easing.md)
- [color - HCT 色彩空间](./color.md)
