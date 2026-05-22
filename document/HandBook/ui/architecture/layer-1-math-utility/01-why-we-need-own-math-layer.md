---
title: 从零开始——为什么我们决定自己造轮子
description: 说实话，当我说我们要自己写一套数学工具库的时候，队友的表情就像看到我决定徒手写 JSON 解析器一样
---

# 从零开始——为什么我们决定自己造轮子

说实话，当我说我们要自己写一套数学工具库的时候，队友的表情就像看到我决定徒手写 JSON 解析器一样——你说你有什么毛病？Qt 不是已经给你配好了 QColor、QEasingCurve，甚至还有 QPropertyAnimation，你这不是吃饱了撑的吗？

这话其实一点没错。如果我们的目标只是"做一个能用的 UI"，那直接用 Qt 的东西确实够了。但问题是，我们要做的不是"能用的 UI"，而是 Material Design 3——那个 Google 把每一个像素、每一个动画曲线都规定得死死的 Design System。而这玩意儿和 Qt 原生提供的工具之间，存在着一条难以跨越的鸿沟。

## 问题一：RGB 空间的"不直觉"

首先来说颜色。Qt 的 QColor 用的是 RGB/HSL，这本来没什么问题——直到你需要根据用户的壁纸自动生成一套主题。

假设用户选了一个品牌色，比如 `#6200EE`（Material 的经典紫色）。现在你需要生成这个颜色的"浅色变体"用于 hover 状态，"深色变体"用于 pressed 状态。用 RGB 的思路，你会怎么做？增加 RGB 值？那就麻烦了——RGB 空间里"变亮"和"变色"是绑定的，你把紫色变亮的时候，它的色相可能会偏移，最后得到一个不再是紫色的"亮紫色"。

Material Design 3 用的是 HCT 色彩空间（Hue-Chroma-Tone），这三个维度是正交的：你调整 Tone（亮度）的时候，Hue（色相）和 Chroma（色度）完全不受影响。这就意味着你可以放心地生成同一个颜色的 13 个亮度等级，而不用担心颜色"跑偏"。

Qt 的 QColor 不支持 HCT，所以我们只能自己实现。

## 问题二：QColor::lighter() 的灾难

有人会说，那用 QColor::lighter() 不就行了？

这里真的要踩个坑。QColor::lighter() 在 HSL 空间里操作，它确实能让颜色"变亮"，但问题是它调整的是 Lightness，而 Lightness 和人类感知的"亮度"并不完全一致。更糟糕的是，这个函数在处理某些颜色的时候会产生奇怪的色相漂移。

你可能会说，那我直接调整 RGB 的每个分量？比如都乘以 1.2？来试试看：`RGB(100, 50, 150)` 乘以 1.2 之后是 `RGB(120, 60, 180)`。看起来没问题，但问题是当某个分量超过 255 的时候，你需要钳位——这会导致颜色向白色漂移，而且漂移的方向和 RGB 三个分量的相对大小有关，完全不可预测。

所以我们决定实现一个 CFColor 类，它内部维护一个 QColor（因为 Qt 绘制 API 只接受 QColor），但同时缓存 HCT 值。当你需要调整亮度时，我们直接在 HCT 空间操作，然后再转回 RGB。这样既保证了颜色一致性，又能和 Qt 的 API 无缝对接。

## 问题三：缓动曲线的"不标准"

再来说动画。Qt 的 QEasingCurve 提供了一堆预定义曲线：InQuad、OutCubic、InOutQuart……但 Material Design 3 用的是一套完全不同的命名和参数：Standard、Emphasized、Legacy。

当然你可以自己用 `QEasingCurve::addCubicBezier()` 手动添加曲线，但问题是 Material 的曲线参数是固定的，你每次都要去查文档。所以我们做了一个 Easing 命名空间，把 Material 的标准曲线预先定义好，你只需要 `Easing::fromEasingType(Type::Standard)` 就能拿到对应的 QEasingCurve。

## 问题四：零 Qt UI 依赖的执念

说到这里可能有人要问了：既然你最后还是要用 QColor、QEasingCurve，那为什么不直接在需要的地方调用 Qt 的 API，非要封装一层？

这个问题触及到了我们设计的核心理念：ui/base 层应该是零 Qt UI 依赖的。

为什么？因为数学工具本身不应该依赖 UI 框架。插值、钳位、贝塞尔曲线——这些是纯粹的数学运算，它们在任何平台上、任何框架里都应该是一样的。如果我们把它们和 Qt 绑定在一起，将来想移植到其他框架，或者想单独测试这些数学函数，就会非常麻烦。

更重要的是，零依赖意味着我们可以把 ui/base 编译成一个独立的静态库，不需要链接 QtWidgets 或 QtGui。这对于单元测试、对于嵌入式环境的精简构建，都有很大的意义。

## 问题五：弹簧动画的缺失

Qt 的动画系统是基于时间的（QPropertyAnimation、QVariantAnimation），但 Material Design 3 大量使用弹簧动画——那种自然的、带一点弹性的运动效果。

Qt 没有提供弹簧动画的实现，所以我们只能自己写。springStep 函数使用半隐式欧拉积分法模拟弹簧物理，给定当前位置、速度、目标位置、劲度系数和阻尼系数，返回下一时刻的位置和速度。

这里有个坑点需要注意：stiffness 和 dt 的乘积不能太大，否则数值积分会不稳定，产生诡异的振荡。我们经过多次测试，发现 stiffness 在 100-500 之间、dt 在 1/60 秒时比较稳定。

## 我们的解决方案

最终，我们在 ui/base 层实现了以下模块：

- `color.h` / `color_helper.h`：HCT 色彩空间支持、颜色混合、对比度计算
- `math_helper.h`：lerp、clamp、remap、cubicBezier、springStep、lerpAngle
- `easing.h`：Material 标准缓动曲线和弹簧预设
- `geometry_helper.h`：圆角矩形路径生成
- `device_pixel.h`：dp/sp/px 单位转换

所有这些模块都只依赖 QtCore（有些甚至完全不依赖 Qt），可以独立编译、独立测试。

## 验证一下

写了这么多，不如直接跑一下看看效果。这里有个简单的例子：我们从一个品牌色生成整套 tonal palette，然后用它来构建一个 Material 主题。

```cpp
// 从品牌色生成 tonal palette
CFColor brandColor("#6200EE");
QList<CFColor> palette = tonalPalette(brandColor);

// palette[0] 是最接近品牌色的 tone 值
// palette[1] 到 palette[13] 是从 Tone 0 到 Tone 100 的 13 个等级
```yaml

如果你打印出这些颜色的 RGB 值，你会发现它们的色相和色度基本保持一致，只有亮度在变化——这正是 HCT 空间的优势。

## 下一步

到这里，基础数学工具层就搭好了。但有了工具只是第一步，接下来我们需要把这些工具组织成一个完整的主题系统。Material Design 3 的主题不是简单的颜色集合，而是一套由 Token 驱动的、支持动态切换的、可扩展的架构。

接下来，我们进入 Layer 2：Theme Engine Layer。

---

**相关文档**

- [HCT 色彩空间实战](./02-color-system-hct.md)——深入了解 HCT 的数学原理
- [几何与设备无关](./03-geometry-and-device-pixel.md)——跨 DPI 适配的完整方案
