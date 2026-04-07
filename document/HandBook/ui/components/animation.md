# ICFAbstractAnimation - 动画基类

`ICFAbstractAnimation` 是所有动画组件的抽象基类，定义了动画的生命周期、状态管理和信号接口。设计这个基类是为了统一时间动画和弹簧动画的行为模式——它们虽然内部插值逻辑不同，但启动、暂停、停止这些外部操作是一致的。

## 动画状态

动画在其生命周期内会经历四种状态：

```cpp
enum class State { Idle, Running, Paused, Finished };
```

- `Idle`：动画尚未启动或已停止，处于初始状态
- `Running`：动画正在运行中
- `Paused`：动画已暂停，可以继续运行
- `Finished`：动画已完成

这些状态由内部管理，外部通过信号监听状态变化。

## 基本使用

动画的典型使用流程是创建、连接信号、启动：

```cpp
#include "ui/components/animation.h"

// 假设有一个具体实现
auto* anim = new CFTimingAnimation(spec, this);

// 连接信号监听动画生命周期
connect(anim, &ICFAbstractAnimation::started, []() {
    qDebug() << "Animation started";
});
connect(anim, &ICFAbstractAnimation::finished, []() {
    qDebug() << "Animation finished";
});
connect(anim, &ICFAbstractAnimation::progressChanged, [](float progress) {
    qDebug() << "Progress:" << progress;
});

// 启动动画
anim->start(ICFAbstractAnimation::Direction::Forward);
```

## 方向控制

动画支持正向和反向播放，`reverse()` 方法会停止当前动画并以相反方向重新启动：

```cpp
// 正向播放
anim->start(ICFAbstractAnimation::Direction::Forward);

// 反向播放
anim->start(ICFAbstractAnimation::Direction::Backward);

// 翻转当前方向
anim->reverse();  // 停止并以相反方向重新开始
```

⚠️ `reverse()` 会停止当前动画并重新启动，而不是简单地改变播放方向。如果需要无缝反向，需要自己管理逻辑。

## 生命周期控制

```cpp
// 暂停正在运行的动画
anim->pause();

// 停止并重置到初始状态
anim->stop();
```

`stop()` 和 `pause()` 的区别在于：`pause()` 可以恢复，而 `stop()` 会将动画重置回初始状态。

## 弱引用获取

每个具体动画类都需要实现 `GetWeakPtr()` 方法，返回指向自己的弱引用。这是为了避免动画对象在异步回调中被意外持有导致内存泄漏：

```cpp
cf::WeakPtr<ICFAbstractAnimation> weak = anim->GetWeakPtr();
```

⚠️ 动画类本身不是线程安全的。如果需要在多线程环境下使用，外部需要自行加锁。

## 相关文档

- [CFTimingAnimation - 时间动画](./timing_animation.md)
- [CFSpringAnimation - 弹簧动画](./spring_animation.md)
- [Easing - 缓动曲线](../base/easing.md)
