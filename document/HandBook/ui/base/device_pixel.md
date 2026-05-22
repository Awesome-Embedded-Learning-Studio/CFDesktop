---
title: "devicepixel - 设备像素 (DPI 缩放)"
description: 提供了设备无关像素（dp）、可缩放像素（sp）和物理像素之间的转换工具。在高 DPI 显示器普及的今
---

# device_pixel - 设备像素 (DPI 缩放)

`device_pixel` 提供了设备无关像素（dp）、可缩放像素（sp）和物理像素之间的转换工具。在高 DPI 显示器普及的今天，这个组件是保证 UI 在不同屏幕上显示一致性的基础。我们自己实现而不是完全依赖 Qt 的缩放，是因为需要更精细的控制单位转换逻辑，特别是在响应式布局的断点判断上。

## 基本用法

`CanvasUnitHelper` 是核心工具类，构造时传入设备像素比（DPI），之后就可以进行各种单位转换：

```cpp
#include "ui/base/device_pixel.h"

using namespace cf::ui::base::device;

// 创建一个 2x DPI 的 helper（常见于 Retina 屏幕）
CanvasUnitHelper helper(2.0);

// 将设备无关像素转换为物理像素
qreal buttonWidth = helper.dpToPx(88.0);  // 88dp -> 176px

// 将可缩放像素转换为物理像素（考虑字体缩放）
qreal fontSize = helper.spToPx(14.0);     // 14sp -> 28px

// 反向转换：物理像素转回设备无关像素
qreal dpValue = helper.pxToDp(352.0);     // 352px -> 176dp
```text

设备像素比通常从 Qt 的 `QWindow` 或 `QScreen` 获取：

```cpp
// 在 Qt 窗口中获取当前 DPI
qreal dpi = window()->devicePixelRatio();
CanvasUnitHelper helper(dpi);
```text

## 单位类型

`dp`（device-independent pixel）是设备无关像素，用于布局尺寸。在 160 DPI 的屏幕上，1dp 等于 1 物理像素。在 2x 屏幕上，1dp 等于 2 物理像素。

`sp`（scalable pixel）是可缩放像素，专门用于字体大小。它和 dp 类似，但会额外受到用户字体大小设置的影响。当前实现中 `spToPx()` 和 `dpToPx()` 行为一致，如果需要支持字体缩放，可以在未来扩展。

⚠️ 不要用物理像素硬编码尺寸。你写的 10px 宽的按钮在 4K 显示器上会细得看不见，用 10dp 就能保持一致。

## 响应式断点

`BreakPoint` 枚举定义了响应式布局的宽度断点，参考 Material Design 的分类标准：

```cpp
// 判断当前窗口宽度属于哪个断点
auto bp = helper.breakPoint(widthDp);

if (bp == CanvasUnitHelper::BreakPoint::Compact) {
    // 手机/窄屏布局：< 600dp
    // 使用单列布局，隐藏次要元素
} else if (bp == CanvasUnitHelper::BreakPoint::Medium) {
    // 平板/折叠屏布局：600dp - 839dp
    // 使用两列布局，显示侧边栏
} else {
    // 桌面布局：>= 840dp
    // 使用多列布局，显示完整导航
}
```text

断点判断使用的是 dp 值，而不是物理像素。这样不管屏幕 DPI 如何，布局行为保持一致。

## DPI 缩放的注意事项

处理 DPI 缩放时，有几个容易踩的坑：

第一，混用不同单位。比如 dp 值和 px 值直接相加，在高 DPI 下会得到错误结果。所有计算要么统一在 dp 空间，要么统一在 px 空间。

第二，缓存 DPI 值。如果用户在运行时移动窗口到不同 DPI 的显示器，或者修改系统缩放设置，缓存的 DPI 值会过时。需要在 DPI 变化时重新创建 `CanvasUnitHelper`。

第三，整型截断。`dpToPx()` 返回 `qreal`，但很多绘制 API 需要 `int`。截断前要四舍五入，否则会产生累积误差：

```cpp
// 正确的做法
int rounded = qRound(helper.dpToPx(16.0));

// 错误的做法：直接截断
int truncated = static_cast<int>(helper.dpToPx(16.0));
```text

## 性能考虑

`CanvasUnitHelper` 是一个轻量级的值类型，构造和拷贝都很便宜。如果在一个局部作用域内频繁转换，可以直接按值传递，不需要用指针或引用。但如果在全局或成员变量里存储，确保在 DPI 变化时更新。

## 相关文档

- [math_helper - 数学工具](./math_helper.md)
- [easing - 缓动曲线](./easing.md)
