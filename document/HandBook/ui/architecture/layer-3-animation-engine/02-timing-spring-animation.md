---
title: 时间与弹簧——两种动画范式的完整实现
description: 在上一篇文章里，我们讲了动画引擎的抽象架构。这篇文章聊聊两种具体的动画实现：基于时间的动画和基于弹簧
---

# 时间与弹簧——两种动画范式的完整实现

在上一篇文章里，我们讲了动画引擎的抽象架构。这篇文章聊聊两种具体的动画实现：基于时间的动画和基于弹簧的动画。

## ICFTimingAnimation：时间驱动的动画

ICFTimingAnimation 是基于时间的动画，它的核心思想是：给定一个时长和一条缓动曲线，在规定的时间内从 `from` 值过渡到 `to` 值。

```cpp
class ICFTimingAnimation : public ICFAbstractAnimation {
public:
    explicit ICFTimingAnimation(IMotionSpec* spec, QObject* parent = nullptr);

    virtual void setRange(float from, float to) {
        m_from = from;
        m_to = to;
    }

    virtual float currentValue() const = 0;

protected:
    IMotionSpec* motion_spec_ = nullptr;  // 用于获取时长和缓动
    float m_from = 0.0f;
    float m_to = 1.0f;
    int m_elapsed = 0;  // 已过时间（毫秒）
};
```text

注意 `motion_spec_` 是一个原始指针。这是一个设计决策：IMotionSpec 的生命周期必须比动画长。在 CFMaterialAnimationFactory 中，这个条件是满足的，因为工厂持有对主题的引用，而主题拥有 MotionSpec。

## tick() 实现逻辑

ICFTimingAnimation 的 `tick()` 实现大致如下：

```cpp
bool ICFTimingAnimation::tick(int dt) {
    m_elapsed += dt;

    // 从 MotionSpec 获取时长
    int duration = motion_spec_->queryDuration(m_motionToken);
    float progress = std::min(m_elapsed / (float)duration, 1.0f);

    // 从 MotionSpec 获取缓动类型
    int easingType = motion_spec_->queryEasing(m_motionToken);
    QEasingCurve curve = Easing::fromEasingType(static_cast<Easing::Type>(easingType));

    // 应用缓动曲线
    float easedProgress = curve.valueForProgress(progress);

    // 计算当前值
    m_value = lerp(m_from, m_to, easedProgress);

    // 更新进度并发出信号
    m_progress = easedProgress;
    emit progressChanged(m_progress);

    return m_elapsed < duration;  // 返回 false 表示动画结束
}
```text

这里的关键是 `valueForProgress()`，它根据缓动曲线将 [0, 1] 的线性进度映射到非线性进度。

## 缓动曲线的应用

Material Design 3 定义了几种标准缓动：

- **Emphasized**：快速启动，缓慢结束（强调进入）
- **Standard**：适中的加速减速
- **Linear**：匀速运动
- **Legacy**：Material Design 2 的缓动（兼容性考虑）

在 Layer 1 我们实现了这些缓动的 QEasingCurve 封装。动画直接使用这些预定义的曲线，确保整个应用的动画风格一致。

## ICFSpringAnimation：弹簧驱动的动画

ICFSpringAnimation 使用弹簧物理来模拟自然的弹性运动。与时间驱动的动画不同，弹簧动画没有固定的时长——它会"自然地"收敛到目标值。

```cpp
class ICFSpringAnimation : public ICFAbstractAnimation {
public:
    ICFSpringAnimation(const Easing::SpringPreset& easing, QObject* parent = nullptr);

    virtual void setTarget(float target) { m_target = target; }
    virtual void setInitialVelocity(float velocity) { m_velocity = velocity; }
    virtual float currentValue() const = 0;

    bool tick(int dt) override;  // 使用 springStep

protected:
    Easing::SpringPreset easing_;  // 包含 stiffness 和 damping
    float m_position = 0.0f;
    float m_velocity = 0.0f;
    float m_target = 1.0f;
};
```text

## springStep 物理模拟

弹簧动画的核心是 `springStep` 函数，它使用半隐式欧拉积分法模拟弹簧物理：

```cpp
std::pair<float, float> springStep(float position, float velocity, float target,
                                   float stiffness, float damping, float dt) {
    // 计算弹簧力
    float force = (target - position) * stiffness;

    // 计算阻尼力
    float dampingForce = -velocity * damping;

    // 总加速度
    float acceleration = force + dampingForce;

    // 更新速度（半隐式欧拉）
    float newVelocity = velocity + acceleration * dt;

    // 更新位置
    float newPosition = position + newVelocity * dt;

    return {newPosition, newVelocity};
}
```text

这个算法在 Layer 1 讲过，关键点是：

1. **力与加速度成正比**：胡克定律 F = kx
2. **阻尼力与速度成正比**：防止永远振荡
3. **半隐式欧拉**：先更新速度，再更新位置，比标准欧拉法更稳定

## 收敛判断

弹簧动画没有固定的时长，所以需要一个收敛判断：

```cpp
bool ICFSpringAnimation::tick(int dt) {
    // 转换 dt 到秒
    float dtSeconds = dt / 1000.0f;

    // 执行物理步进
    auto [newPos, newVel] = springStep(
        m_position, m_velocity, m_target,
        easing_.stiffness, easing_.damping,
        dtSeconds
    );

    m_position = newPos;
    m_velocity = newVel;

    // 计算当前进度（近似）
    float displacement = m_target - m_from;
    float currentDisp = m_position - m_from;
    m_progress = (displacement != 0) ? currentDisp / displacement : 1.0f;

    emit progressChanged(m_progress);

    // 收敛判断：速度很小且接近目标
    bool isConverged = std::abs(m_velocity) < 0.01f &&
                       std::abs(m_target - m_position) < 0.01f;

    return !isConverged;
}
```text

收敛的条件是速度足够小且距离目标足够近。阈值 0.01 是经验值，可以根据需要调整。

## SpringPreset 弹簧预设

Material Design 3 定义了几种弹簧预设：

```cpp
namespace Easing {
    struct SpringPreset {
        float stiffness;
        float damping;
    };

    SpringPreset springGentle();   // 温和的弹性
    SpringPreset springBouncy();   // 明显的弹性
    SpringPreset springStiff();     // 僵硬的弹性
}
```text

不同的预设适用于不同的场景：按钮点击用 gentle，对话框进入用 bouncy，列表滚动用 stiff。

## 选择哪种动画？

一个常见的问题是：什么时候用 TimingAnimation，什么时候用 SpringAnimation？

粗略的原则是：

- **UI 过渡**（淡入淡出、滑动）：用 TimingAnimation
- **物理交互**（拖拽释放、弹性动画）：用 SpringAnimation
- **标准场景**：优先用 TimingAnimation（更可控）
- **强调效果**：用 SpringAnimation（更生动）

## 实际使用示例

控件中使用这两种动画的方式类似：

```cpp
// TimingAnimation：淡入效果
auto fadeAnim = factory->getAnimation("md.animation.fadeIn");
if (fadeAnim) {
    connect(fadeAnim.get(), &ICFAbstractAnimation::progressChanged,
            this, [this](float progress) {
                m_opacity = progress;
                update();
            });
    fadeAnim->start();
}

// SpringAnimation：弹性缩放
// 需要先注册自定义弹簧动画
// 或者使用预设的弹簧动画 token
```yaml

## 总结

TimingAnimation 和 SpringAnimation 是两种互补的动画范式。前者提供可预测的时间驱动动画，后者提供自然的物理驱动动画。它们共享同一个基类接口，可以无缝切换。

但有了具体的动画类型还不够，我们需要一个系统来创建和管理这些动画——这就是工厂和策略模式的作用。

接下来，我们聊聊动画工厂的设计。

---

**相关文档**

- [动画引擎架构](./01-animation-architecture.md)——动画系统的整体设计
- [工厂与策略](./03-factory-and-strategy.md)——动画创建的灵活组合
- [弹簧物理的数学基础](../layer-1-math-utility/01-why-we-need-own-math-layer.md)——Layer 1 的数学工具
