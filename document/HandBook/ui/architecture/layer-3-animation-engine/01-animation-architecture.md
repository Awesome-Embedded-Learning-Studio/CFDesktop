---
title: 动画引擎架构——统一调度的生命线管理
description: 在 Layer 2 里，我们聊了主题系统的各个组件。但有了颜色和字体还不够，Material Des
---

# 动画引擎架构——统一调度的生命线管理

在 Layer 2 里，我们聊了主题系统的各个组件。但有了颜色和字体还不够，Material Design 3 的灵魂在于动效——那些流畅的过渡、自然的弹性运动。

这篇文章聊聊动画引擎的架构设计。

## 为什么需要统一的动画引擎？

Qt 已经提供了 QPropertyAnimation、QVariantAnimation 等动画类，为什么还要自己实现？

有几个原因：

1. **Material Design 3 的特殊需求**：MD3 定义了一套标准的动画时长和缓动曲线（shortEnter、mediumEnter 等），Qt 的默认配置不匹配。
2. **生命周期管理问题**：如果每个控件都自己创建动画，很容易出现生命周期混乱——控件销毁了但动画还在运行，或者动画结束了但控件已经不存在了。
3. **性能优化**：统一的动画引擎可以全局控制动画开关（比如用户禁用动画、嵌入式环境精简），避免零散的动画难以管理。
4. **弹簧动画支持**：Qt 没有提供弹簧动画的原生支持，而 MD3 大量使用弹簧效果。

## ICFAbstractAnimation：动画基类

所有动画都继承自 `ICFAbstractAnimation`，它定义了动画的核心接口：

```cpp
class ICFAbstractAnimation : public QObject {
public:
    enum class State { Idle, Running, Paused, Finished };
    enum class Direction { Forward, Backward };

    virtual void start(Direction dir = Direction::Forward) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void reverse() = 0;
    virtual bool tick(int dt) = 0;
    virtual cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() = 0;

signals:
    void started();
    void paused();
    void stopped();
    void finished();
    void progressChanged(float progress);

protected:
    float m_progress = 0.0f;
    State m_state = State::Idle;
};
```text

这里有个关键设计：`GetWeakPtr()` 返回的是弱引用。原因我们后面会讲，但核心思想是：动画由工厂拥有所有权，用户只持有弱引用。

## 动画状态机

动画有四种状态：

- **Idle**：动画尚未开始或已停止
- **Running**：动画正在运行
- **Paused**：动画已暂停
- **Finished**：动画已完成

状态转换遵循以下规则：

```text
Idle → Running (start)
Running → Paused (pause)
Paused → Running (start)
Running/Paused → Idle (stop)
Running → Finished (自然结束)
```text

## Direction 方向控制

动画支持两种播放方向：

- **Forward**：从 `from` 值到 `to` 值
- **Backward**：从 `to` 值到 `from` 值

`reverse()` 方法会停止当前动画，然后以相反方向重新播放。这对于"鼠标进入淡入、鼠标离开淡出"的场景非常实用。

## tick() 虚函数

`tick(int dt)` 是动画的核心更新方法，每帧调用一次。`dt` 是距离上一帧的时间间隔（毫秒）。

每个具体的动画类需要实现自己的 `tick()` 逻辑：

- TimingAnimation：根据 `m_elapsed` 和缓动曲线计算进度
- SpringAnimation：使用弹簧物理公式计算位置和速度

`tick()` 返回 `true` 表示动画继续，`false` 表示动画已完成。

## WeakPtr 所有权模型

这是一个关键设计。动画的所有权结构是：

```text
CFMaterialAnimationFactory (owner)
  └── unordered_map<string, unique_ptr<ICFAbstractAnimation>> animations_

Controls
  └── WeakPtr<ICFAbstractAnimation>
```text

工厂拥有动画的所有权（`unique_ptr`），控件只持有弱引用（`WeakPtr`）。这样设计的好处是：

1. **生命周期安全**：控件销毁时，弱引用自动失效，不会访问野指针
2. **统一管理**：工厂销毁时，所有动画自动销毁
3. **共享访问**：多个控件可以共享同一个动画实例

获取动画的典型流程：

```cpp
auto anim = factory->getAnimation("md.animation.fadeIn");
if (anim) {  // 检查 WeakPtr 是否有效
    connect(anim.get(), &ICFAbstractAnimation::progressChanged,
            this, [this](float progress) {
                update();
            });
    anim->start();
}
```text

## progressChanged 信号

每次 `tick()` 更新进度后，动画会发出 `progressChanged` 信号。控件可以连接这个信号来更新 UI。

注意这里用的是信号机制而不是 QProperty。原因是：

1. **性能**：信号机制比 QProperty 更轻量
2. **灵活性**：控件可以选择是否监听进度变化
3. **兼容性**：不依赖 Qt 6 的新特性

## 全局动画开关

ICFAnimationManagerFactory 提供了全局开关：

```cpp
factory->setEnabledAll(false);  // 禁用所有动画
factory->setEnabledAll(true);   // 启用所有动画
```text

禁用时，`getAnimation()` 会返回无效的 WeakPtr，这样就不会创建新动画。已有的正在运行的动画不受影响，会自然完成。

这个功能对以下场景很有用：

- **性能优化**：繁重任务期间禁用动画
- **无障碍**：尊重系统的"减少动画"设置
- **用户偏好**：提供一个"禁用动画"的选项

## 目标 FPS 设置

工厂可以设置目标 FPS：

```cpp
factory->setTargetFps(60.0f);  // 60 FPS
```yaml

这会影响动画的定时器间隔。更高的 FPS 意味着更平滑的动画，但也意味着更多的 CPU 开销。

## 总结

动画引擎的核心设计是"统一调度 + WeakPtr 所有权"。工厂拥有动画，用户持有弱引用，这样既保证了生命周期安全，又提供了灵活的访问方式。

但抽象基类只是定义接口，具体的动画还需要实现。Material Design 3 用两种主要的动画范式：基于时间的动画和基于弹簧的动画。

接下来，我们深入这两种动画的具体实现。

---

**相关文档**

- [字体、形状与动效](../layer-2-theme-engine/04-typography-shape-motion.md)——Layer 2 的动效接口
- [时间与弹簧动画](./02-timing-spring-animation.md)——两种动画范式的实现
- [工厂与策略](./03-factory-and-strategy.md)——动画创建的灵活组合
