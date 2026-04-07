# ICFTimingAnimation - 时间动画

`ICFTimingAnimation` 是基于时间的动画基类，使用固定的持续时间和缓动曲线来完成插值。这是最常见、最直观的动画方式——我们指定一个起点、一个终点、一段时长，动画就按照缓动曲线从起点走到终点。相比弹簧动画，时间动画的行为更可预测，适合大多数 UI 过渡场景。

## 基本概念

时间动画的核心是"时间驱动"——内部维护一个已逝时间计数器，每帧累加 `dt`，当累计时间超过设定的持续时间时，动画结束。插值过程使用缓动函数将时间进度映射到值进度：

```
progress = easing(elapsed / duration)
value = from + progress * (to - from)
```

## 创建动画

创建时间动画需要提供一个 `IMotionSpec`，它定义了动画的持续时间和缓动类型：

```cpp
#include "ui/components/timing_animation.h"
#include "ui/core/motion_spec.h"

// 假设 theme->motionSpec() 返回有效的 IMotionSpec
auto* motionSpec = theme->motionSpec();
auto* anim = new CFTimingAnimation(motionSpec, this);
```

⚠️ `IMotionSpec` 指针必须在动画的生命周期内保持有效。这个设计是经过权衡的——动画工厂创建动画时持有对主题的引用，而主题拥有 motion spec，所以生命周期是绑定的，不需要额外拷贝。

## 设置值范围

动画需要一个起点和终点：

```cpp
// 从 0 到 1
anim->setRange(0.0f, 1.0f);

// 从透明到不透明（假设是透明度动画）
anim->setRange(0.0f, 255.0f);

// 从位置 A 到位置 B
anim->setRange(startX, endX);
```

`setRange()` 可以在动画运行时调用，会即时改变当前帧的计算基准，但通常建议在 `start()` 前设置好。

## 获取当前值

具体实现类需要实现 `currentValue()` 方法，返回当前帧的插值结果：

```cpp
float current = anim->currentValue();
```

## 时间动画 vs 弹簧动画

选择时间动画还是弹簧动画，取决于场景的"可预测性"要求：

| 场景 | 推荐动画 | 理由 |
|------|----------|------|
| 页面切换、模态框弹出 | 时间动画 | 需要固定时长，便于编排 |
| 按钮点击反馈 | 弹簧动画 | 需要有弹性、跟随手指的感觉 |
| 列表滚动、拖拽 | 时间动画 | 滚动距离和时长成比例 |
| 元素"弹"到位 | 弹簧动画 | 需要物理真实的减速回弹 |

时间动画的好处是"可控"——你知道它确切会在什么时候结束，这对于编排多个连续动画非常重要。弹簧动画则更"自然"，但结束时间取决于物理参数，难以精确预测。

## 典型使用模式

一个完整的淡入动画示例：

```cpp
// 创建淡入动画
auto* fadeIn = new CFOpacityAnimation(theme->motionSpec(), widget);
fadeIn->setRange(0.0f, 1.0f);

// 设置持续时间和缓动（从 motion spec 查询）
int duration = theme->motionSpec()->queryDuration("md.motion.shortEnter");
fadeIn->setDuration(duration);

// 连接完成信号
connect(fadeIn, &ICFAbstractAnimation::finished, []() {
    qDebug() << "Fade in complete";
});

// 启动
fadeIn->start();
```

## 线程安全

时间动画的 `tick()` 方法会被驱动器在主线程上调用，如果需要在其他线程创建或操作动画，必须手动同步。Qt 的信号槽机制可以简化跨线程调用，但动画状态变化仍需要在主线程发生。

⚠️ 不要在 `tick()` 内部执行耗时操作，会阻塞 UI 线程导致掉帧。

## 相关文档

- [ICFAbstractAnimation - 动画基类](./animation.md)
- [ICFSpringAnimation - 弹簧动画](./spring_animation.md)
- [IMotionSpec - 运动规范](../core/motion_spec.md)
