---
title: "IMotionSpec - 动画规格接口"
description: 定义了按名称查询动画参数的抽象接口，是 Material Design 3 运动系统的 C++ 映射
---

# IMotionSpec - 动画规格接口

`IMotionSpec` 定义了按名称查询动画参数的抽象接口，是 Material Design 3 运动系统的 C++ 映射层。选择把持续时间、缓动类型和延迟分别查询而不是打包成一个结构体，是为了灵活性——有些场景只需要持续时间，有些场景需要完整的三参数组合。

## 基本用法

`IMotionSpec` 提供了三个查询方法，分别对应动画的三个核心参数：

```cpp
#include "ui/core/motion_spec.h"

IMotionSpec& motion = theme.motion_spec();

// 查询动画持续时间（毫秒）
int duration = motion.queryDuration("md.motion.shortEnter");   // 200ms
int standard = motion.queryDuration("md.motion.standard");     // 400ms

// 查询缓动类型（枚举值）
int easing = motion.queryEasing("md.motion.standard");         // 线性/缓入缓出

// 查询动画延迟（毫秒）
int delay = motion.queryDelay("md.motion.shortEnter");         // 通常为 0
```text

这三个参数组合起来可以构造完整的动画配置：

```cpp
// 使用 QPropertyAnimation
auto* animation = new QPropertyAnimation(button, "geometry");
animation->setDuration(motion.queryDuration("md.motion.standard"));
animation->setEasingCurve(static_cast<QEasingCurve::Type>(
    motion.queryEasing("md.motion.standard")
));
animation->setStartValue(startRect);
animation->setEndValue(endRect);
animation->start();
```text

## Token 命名约定

虽然接口本身不规定 token 的命名格式，但我们遵循 Material Design 3 的约定：

```cpp
// Material Design 3 运动规范
"md.motion.standard"        // 标准运动（400ms，缓入缓出）
"md.motion.shortEnter"      // 短进入动画（200ms，缓出）
"md.motion.mediumEnter"     // 中等进入动画（300ms，缓出）
"md.motion.longEnter"       // 长进入动画（500ms，缓出）
"md.motion.shortExit"       // 短退出动画（150ms，缓入）
"md.motion.mediumExit"      // 中等退出动画（250ms，缓入）
"md.motion.longExit"        // 长退出动画（350ms，缓入）
"md.motion.shortDuration"   // 短持续时间（150ms）
"md.motion.mediumDuration"  // 中等持续时间（250ms）
"md.motion.longDuration"    // 长持续时间（350ms）
"md.motion.extraLongDuration" // 超长持续时间（500ms+）
```text

缓动类型返回的是整数值，需要映射到具体的缓动曲线枚举。Qt 的 `QEasingCurve::Type` 是一个常见的映射目标：

```cpp
// 缓动类型映射示例
// 返回值 -> QEasingCurve::Type
// 0      -> Linear
// 1      -> InQuad
// 2      -> OutQuad
// 3      -> InOutQuad
// 32     -> InCubic
// 33     -> OutCubic
// 34     -> InOutCubic
// ...
```text

具体的映射关系由实现类决定，可以在文档中说明。

## 使用示例

在实际 UI 组件中使用动画规格：

```cpp
// 淡入动画
void fadeInWidget(QWidget* widget, const char* motionToken) {
    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    auto* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(motion.queryDuration(motionToken));
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(static_cast<QEasingCurve::Type>(
        motion.queryEasing(motionToken)
    ));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

// 使用
fadeInWidget(myWidget, "md.motion.shortEnter");  // 快速淡入
```text

对于需要延迟的动画序列：

```cpp
void staggeredShow(QList<QWidget*> widgets) {
    auto& motion = getTheme().motion_spec();
    int staggerDelay = motion.queryDelay("md.motion.staggerDelay"); // 50ms

    for (int i = 0; i < widgets.size(); ++i) {
        auto* animation = new QPropertyAnimation(widgets[i], "opacity");
        animation->setDuration(motion.queryDuration("md.motion.shortEnter"));
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
        animation->setStartDelay(i * staggerDelay);  // 错开延迟
        animation->start();
    }
}
```text

## 缓动曲线映射

缓动类型返回的整数值需要映射到具体框架的缓动曲线。这里是一个简单的映射辅助类示例：

```cpp
class EasingCurveMapper {
public:
    static QEasingCurve fromMotionValue(int value) {
        switch (value) {
            case 0:  return QEasingCurve::Linear;
            case 1:  return QEasingCurve::InQuad;
            case 2:  return QEasingCurve::OutQuad;
            case 3:  return QEasingCurve::InOutQuad;
            case 32: return QEasingCurve::InCubic;
            case 33: return QEasingCurve::OutCubic;
            case 34: return QEasingCurve::InOutCubic;
            default: return QEasingCurve::InOutCubic;  // fallback
        }
    }
};

// 使用
auto curve = EasingCurveMapper::fromMotionValue(
    motion.queryEasing("md.motion.standard")
);
animation->setEasingCurve(curve);
```text

## 实现要点

实现 `IMotionSpec` 时需要考虑几个细节：

1. **缓动类型的表示**：接口用 `int` 返回缓动类型，而不是定义自己的枚举。这是为了避免向调用方传播 C++ 类型定义——如果接口要被其他语言调用，整数比枚举更容易处理。实现类可以选择内部用枚举，导出时转换。

2. **默认延迟值**：大多数动画不需要延迟，所以 `queryDelay()` 的默认返回值应该是 0。只有在"交错动画"这类特殊场景下，才需要非零延迟。

3. **无效 token 处理**：不要抛异常或返回负值。对于持续时间，返回一个合理的默认值（比如 300ms）；对于缓动类型，返回一个常用的缓动曲线；对于延迟，返回 0。

4. **线程安全**：动画规格查询通常发生在 UI 线程，但如果实现类支持跨线程访问，需要在各方法内部加锁。考虑到这些都是简单的哈希查找，锁的开销可以接受。

## 设计决策

`IMotionSpec` 把三个参数分开查询，而不是返回一个包含三个字段的结构体。这是为了使用场景的灵活性——很多动画只需要持续时间和缓动，延迟总是 0；而有些场景（比如交错动画）只需要延迟参数。如果打包成结构体，调用方总是要构造或接收一个他们不需要的字段。

另一个设计点是，我们用 `int` 表示时间而不是 `std::chrono::duration`。这是因为 Qt 的动画 API 用整数毫秒，用标准库类型反而需要转换。如果将来要支持更高精度的时间单位，可以在实现类内部用 `std::chrono`，导出时转换成毫秒。

## 相关文档

- [ICFTheme - 主题接口](./theme.md)
- [ICFColorScheme - 颜色方案](./color_scheme.md)
- [IRadiusScale - 圆角规范](./radius_scale.md)
- [IFontType - 字体样式](./font_type.md)
