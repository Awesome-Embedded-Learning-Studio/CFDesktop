# ICFSpringAnimation - 弹簧动画

`ICFSpringAnimation` 是基于物理弹簧模型的动画基类，用真实的弹簧动力学来驱动动画。与时间动画不同，弹簧动画没有固定的持续时间——它从当前值向目标值"弹"过去，根据刚度和阻尼参数决定运动轨迹。这种方式能产生更自然、更有"重量感"的交互反馈。

## 基本原理

弹簧动画模拟了一个连接在目标值上的物理弹簧：

```
加速度 = (目标值 - 当前值) * 刚度 - 速度 * 阻尼
速度 += 加速度 * dt
位置 += 速度 * dt
```

系统会一直迭代，直到位置足够接近目标值且速度足够小。这意味着动画的实际时长取决于初始距离和物理参数，而不是预先设定的时间。

## 创建弹簧动画

创建时需要指定一个弹簧预设，包含刚度和阻尼参数：

```cpp
#include "ui/components/spring_animation.h"
#include "ui/base/easing.h"

// 使用预设的弹簧参数
auto gentle = cf::ui::base::Easing::springGentle();
auto* anim = new CFSpringAnimation(gentle, this);

// 或者用高弹性预设
auto bouncy = cf::ui::base::Easing::springBouncy();
auto* anim2 = new CFSpringAnimation(bouncy, this);
```

框架提供了三种预设：
- `springGentle()`：柔和的弹簧，回弹较少，适合常规过渡
- `springBouncy()`：高弹性，有明显的回弹，适合强调交互
- `springStiff()`：高刚度低阻尼，快速到位但几乎无回弹

## 设置目标值

弹簧动画的核心是"追逐目标值"，设置新目标会立即改变弹簧的行为：

```cpp
// 设置目标为 1.0
anim->setTarget(1.0f);

// 运行过程中改变目标，弹簧会立即转向
anim->setTarget(0.5f);  // 会立即向新目标加速
```

这种"追逐"特性使得弹簧动画非常适合交互式场景——比如拖拽释放后，元素会"弹"回原位，或者在拖拽过程中跟随手指。

## 初始速度

可以设置动画的初始速度，这在某些场景下很有用：

```cpp
// 给一个向右的初速度
anim->setInitialVelocity(100.0f);

// 配合负目标值，可以做出"甩出去"的效果
anim->setTarget(-1.0f);
anim->setInitialVelocity(500.0f);
```

如果没有设置初始速度，默认从静止开始。

## 获取当前值

具体实现类需要实现 `currentValue()` 方法，返回弹簧当前的物理位置：

```cpp
float position = anim->currentValue();
```

这个值会随着弹簧振荡而变化，直到收敛到目标值。

## 弹簧 vs 时间动画

弹簧动画的优势在于"跟随感"和"自然感"：

```cpp
// 时间动画：固定的缓动曲线，可预测但呆板
tick(int dt) {
    elapsed += dt;
    float t = elapsed / duration;
    float progress = easing(t);
    value = from + progress * (to - from);
    return elapsed < duration;
}

// 弹簧动画：物理驱动，不可预测但生动
tick(int dt) {
    float acceleration = (target - position) * stiffness - velocity * damping;
    velocity += acceleration * dt;
    position += velocity * dt;
    return !isSettled();  // 是否已稳定
}
```

如果你在做一个按钮的点击反馈：
- 时间动画：按下时缩小，松开时放大，像在"播放一段视频"
- 弹簧动画：按下时被"压缩"，松开时"弹开"，像在按压真实的物体

后者的感觉要自然得多。

## 何时使用弹簧动画

适合弹簧动画的场景通常有这些特征：

1. **有物理隐喻**：按钮、开关、卡片拖拽释放
2. **需要跟随交互**：手势驱动的动画，需要能随时改变目标
3. **需要强调感**：微交互、反馈动画，需要一点"弹性"来吸引注意

不适合的场景：
1. **需要精确控制时长**：编排好的多阶段动画序列
2. **需要严格同步**：多个元素必须同时到达终点
3. **需要线性过渡**：进度条、数据可视化

## 稳定条件

弹簧动画在以下条件时被认为"完成"：

```cpp
bool isSettled() {
    return abs(position - target) < epsilon
        && abs(velocity) < epsilon;
}
```

⚠️ 这意味着弹簧可能在理论上"永远"不结束（阻尼过低时会无限振荡）。实际使用中，框架会在若干帧后强制结束，避免空耗 CPU。

## 线程安全

弹簧动画的状态更新（`tick()`）必须在单线程上进行，通常是主线程。如果在其他线程修改 `target` 或 `velocity`，需要手动同步。

## 相关文档

- [ICFAbstractAnimation - 动画基类](./animation.md)
- [ICFTimingAnimation - 时间动画](./timing_animation.md)
- [Easing - 缓动曲线和弹簧预设](../base/easing.md)
