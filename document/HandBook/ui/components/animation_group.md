---
title: "ICFAnimationGroup - 动画组"
description: 是动画组合容器，用于将多个动画作为一个整体来控制。它支持两种执行模式——并行和顺序，能覆盖大多数需要
---

# ICFAnimationGroup - 动画组

`ICFAnimationGroup` 是动画组合容器，用于将多个动画作为一个整体来控制。它支持两种执行模式——并行和顺序，能覆盖大多数需要协调多个动画的场景。设计这个组件是因为单个动画只能控制一个属性，而实际 UI 效果往往需要多个属性同步变化。

## 执行模式

动画组通过 `Mode` 枚举控制执行方式：

```cpp
enum class Mode { Parallel, Sequential };
```text

`Parallel` 模式下，所有动画同时启动，适合处理多个属性的同步变化。`Sequential` 模式下，动画按添加顺序依次执行，前一个完成后才开始下一个。

## 基本使用

创建动画组并添加动画，然后像操作单个动画一样启动整个组：

```cpp
#include "ui/components/animation_group.h"

// 创建并行动画组
auto* group = new ICFAnimationGroup(this);

// 创建几个独立的动画
auto* fadeAnim = new CFTimingAnimation(fadeSpec, this);
auto* scaleAnim = new CFTimingAnimation(scaleSpec, this);

// 添加到组中
group->addAnimation(fadeAnim->GetWeakPtr());
group->addAnimation(scaleAnim->GetWeakPtr());

// 启动整个组
group->start(ICFAbstractAnimation::Direction::Forward);
```text

## 顺序执行

当需要动画依次出现时，使用 `Sequential` 模式。注意这个模式下动画是严格串行的，如果想做错位进入（stagger）效果，需要自己计算延迟：

```cpp
// 顺序执行的动画组
auto* sequentialGroup = new ICFAnimationGroup(this);
sequentialGroup->setMode(ICFAnimationGroup::Mode::Sequential);

sequentialGroup->addAnimation(anim1->GetWeakPtr());
sequentialGroup->addAnimation(anim2->GetWeakPtr());
sequentialGroup->addAnimation(anim3->GetWeakPtr());

// anim1 完成后执行 anim2，然后 anim3
sequentialGroup->start();
```text

## 弱引用管理

动画组持有的是动画的 `WeakPtr`，而不是直接持有原始指针。这个设计避免了循环引用导致的内存泄漏——如果动画被组持有，组又被某个对象持有，而那个对象又持有动画的引用，就会形成循环。

使用 `WeakPtr` 也意味着如果动画对象在外部被销毁，组里的引用会自动失效，不会变成悬空指针：

```cpp
auto* anim = new CFTimingAnimation(spec, this);
group->addAnimation(anim->GetWeakPtr());

// 如果 anim 在外部被删除，组里的引用会自动失效
delete anim;

// 移除操作也是安全的
group->removeAnimation(invalidWeakPtr);  // 无操作
```text

## 生命周期

动画组本身也是一个 `ICFAbstractAnimation`，所以它继承了一切标准生命周期控制方法：

```cpp
group->pause();   // 暂停组内所有动画
group->stop();    // 停止并重置所有动画
group->reverse(); // 翻转执行方向
```text

`stop()` 会停止组内所有动画并将它们重置到初始状态，而 `pause()` 只是暂停，可以继续恢复。

## 相关文档

- [ICFAbstractAnimation - 动画基类](./animation.md)
- [CFTimingAnimation - 时间动画](./timing_animation.md)
- [CFSpringAnimation - 弹簧动画](./spring_animation.md)
