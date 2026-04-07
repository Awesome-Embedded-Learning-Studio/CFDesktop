# CFMaterialAnimationFactory - Material 动画工厂

`CFMaterialAnimationFactory` 是 Material Design 3 动画系统的核心入口，负责创建和管理符合 Material 规范的动画实例。我们设计这个工厂的初衷是让使用者不需要直接面对复杂的动画参数配置，而是通过一个语义化的 token（如 `"md.animation.fadeIn"`）就能获取到完整配置好的动画。

## Material Design 3 运动规范映射

工厂内部完整实现了 Material Design 3 的 Motion System 规范。每个动画都对应一个 motion token，这个 token 决定了动画的持续时间和缓动曲线：

| Motion Token | 对应规范 | 典型用途 |
|--------------|----------|----------|
| `md.motion.shortEnter` | 200ms + EmphasizedDecelerate | 小元素淡入、缩放入场 |
| `md.motion.shortExit` | 150ms + EmphasizedAccelerate | 小元素淡出、缩放退场 |
| `md.motion.mediumEnter` | 300ms + EmphasizedDecelerate | 列表项滑入、旋转入场 |
| `md.motion.mediumExit` | 250ms + EmphasizedAccelerate | 列表项滑出、旋转退场 |

这些映射关系在 `animation_token_mapping.h` 中定义，添加新动画类型时需要同步更新。

## 基本用法

工厂需要一个 Theme 实例来初始化，因为动画的时序参数（duration、easing）是从 MotionSpec 中查询的：

```cpp
#include "ui/components/material/cfmaterial_animation_factory.h"
#include "ui/core/theme.h"

using namespace cf::ui::components::material;
using namespace cf::ui::core;

// 1. 准备主题（通常在应用初始化时完成）
MaterialFactory themeFactory;
auto theme = themeFactory.fromName("theme.material.light");

// 2. 创建动画工厂
auto factory = std::make_unique<CFMaterialAnimationFactory>(*theme);

// 3. 通过 token 获取动画
auto fadeIn = factory->getAnimation("md.animation.fadeIn");
if (fadeIn) {
    fadeIn->setTargetWidget(myWidget);
    fadeIn->start();
}
```

工厂拥有创建的动画实例，返回的是 `WeakPtr`。这个设计是为了避免悬空指针——如果工厂被销毁，所有返回的 WeakPtr 都会自动失效。

## 可用动画 Token

当前支持的预定义动画 token 都在 `cf::ui::components::material::token_literals` 命名空间下：

```cpp
using namespace cf::ui::components::material::token_literals;

// 淡入淡出
factory->getAnimation(ANIMATION_FADE_IN);   // md.animation.fadeIn
factory->getAnimation(ANIMATION_FADE_OUT);  // md.animation.fadeOut

// 滑动
factory->getAnimation(ANIMATION_SLIDE_UP);    // md.animation.slideUp
factory->getAnimation(ANIMATION_SLIDE_DOWN);  // md.animation.slideDown
factory->getAnimation(ANIMATION_SLIDE_LEFT);  // md.animation.slideLeft
factory->getAnimation(ANIMATION_SLIDE_RIGHT); // md.animation.slideRight

// 缩放
factory->getAnimation(ANIMATION_SCALE_UP);   // md.animation.scaleUp
factory->getAnimation(ANIMATION_SCALE_DOWN); // md.animation.scaleDown

// 旋转
factory->getAnimation(ANIMATION_ROTATE_IN);  // md.animation.rotateIn
factory->getAnimation(ANIMATION_ROTATE_OUT); // md.animation.rotateOut
```

## 动画策略模式

工厂支持通过策略模式在不同组件类型间定制动画行为。策略允许你修改动画的参数而不需要改变工厂的接口：

```cpp
// 自定义策略：按钮使用更短的动画时间
class ButtonAnimationStrategy : public AnimationStrategy {
public:
    AnimationDescriptor adjust(const AnimationDescriptor& desc,
                               QWidget* widget) override {
        AnimationDescriptor adjusted = desc;

        // 按钮的滑动动画也用 shortEnter 而不是 mediumEnter
        if (qobject_cast<QPushButton*>(widget)) {
            if (strcmp(desc.motionToken, "md.motion.mediumEnter") == 0) {
                adjusted.motionToken = "md.motion.shortEnter";
            }
        }
        return adjusted;
    }
};

// 设置策略
factory->setStrategy(std::make_unique<ButtonAnimationStrategy>());
```

策略在动画创建之前被调用，所以你可以基于 widget 的状态、尺寸或者任何你关心的条件来调整动画参数。

## 自定义动画描述符

如果预定义的 token 不能满足需求，你可以用 `AnimationDescriptor` 手动构建动画配置：

```cpp
// 创建一个自定义的淡入动画，使用 longEnter 时长
AnimationDescriptor desc{
    "fade",                  // 动画类型
    "md.motion.longEnter",   // motion token（需要在主题中定义）
    "opacity",               // 目标属性
    0.0f,                    // 起始值
    1.0f,                    // 结束值
    100                      // 延迟 100ms
};

auto customFade = factory->createAnimation(desc, myWidget);
if (customFade) {
    customFade->start();
}
```

⚠️ 注意：`createAnimation()` 总是创建新的动画实例，而 `getAnimation()` 会复用已存在的实例。如果你需要多个独立的动画实例（比如同时控制多个 widget），应该用 `createAnimation()`。

## 全局开关

工厂提供了全局启用/禁用动画的能力，这在性能敏感场景或无障碍需求下很有用：

```cpp
// 沉重计算期间禁用动画提升性能
factory->setEnabledAll(false);
// ... 做一些耗时操作 ...
factory->setEnabledAll(true);

// 或者监听系统的"减少动画"设置
if (QGuiApplication::styleHints()->showIsFullScreen() == false) {
    factory->setEnabledAll(false);
}
```

`setEnabledAll()` 只影响新创建的动画，已经运行的动画不会被中断。

## 生命周期管理

工厂使用 `unique_ptr` 拥有所有创建的动画，返回给用户的是 `WeakPtr`。这个设计的含义是：

```cpp
auto anim = factory->getAnimation("md.animation.fadeIn");
// ... 工厂被销毁 ...
if (anim) {  // 这里会失败，WeakPtr 已经失效
    anim->start();
}
```

⚠️ 这个坑在异步代码里尤其容易出现——如果你把 WeakPtr 存起来延迟使用，一定要检查有效性。

## 相关文档

- [AnimationStrategy - 动画策略](./cfmaterial_animation_strategy.md)
- [动画 Token 映射](../../core/token/motion/cfmaterial_motion_token_literals.md)
- [Material Design 3 运动规范](https://m3.material.io/styles/motion)
