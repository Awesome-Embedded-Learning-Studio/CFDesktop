# ICFAnimationManagerFactory - 动画工厂管理器

`ICFAnimationManagerFactory` 是动画的集中创建和管理接口，负责从字符串 token 生成动画实例并管理它们的生命周期。这个设计把动画的创建逻辑从业务代码里抽离出来，通过 token 来间接引用动画——这样可以在不修改调用代码的情况下，统一调整动画参数或者全局关闭动画。

## 工厂模式的核心思路

直接 new 动画对象的问题是调用方需要知道具体的动画类型、时长、缓动曲线等细节。工厂把这些都封装起来，只需要传入一个 token，就能拿到配置好的动画：

```cpp
// 不用工厂：调用方需要知道所有细节
auto* anim = new CFMaterialFadeAnimation(
    motionSpec,  // 从 theme 获取
    "opacity", 0.0f, 1.0f,
    this
);

// 用工厂：只需要一个 token
auto anim = factory->getAnimation("md.animation.fadeIn");
if (anim) {
    anim->start();
}
```

## 动画注册

工厂支持两种注册方式——按类型名称注册，或者直接传入创建函数。

按类型注册比较简单，适合标准动画类型：

```cpp
// 注册一个名为 "buttonPress" 的 fade 动画
auto result = factory->registerOneAnimation("buttonPress", "fade");

if (result == ICFAnimationManagerFactory::RegisteredResult::OK) {
    // 注册成功
} else if (result == ICFAnimationManagerFactory::RegisteredResult::DUP_NAME) {
    // 名称重复
}
```

如果需要更精细的控制，可以用 `registerAnimationCreator` 传入一个 lambda：

```cpp
factory->registerAnimationCreator("customFade", [](QObject* parent) {
    auto* anim = new CFMaterialFadeAnimation(motionSpec, parent);
    anim->setDuration(500);  // 自定义时长
    return anim;
});
```

⚠️ 传入的 lambda 会在每次 `getAnimation()` 被调用时执行，所以不要在这里捕获可能会失效的局部变量。

## Token 查询

`getAnimation()` 是工厂的核心方法，传入 token 字符串，返回动画的弱引用：

```cpp
auto anim = factory->getAnimation("md.animation.fadeIn");
if (anim) {
    // 动画存在且可用
    anim->start();
}
```

返回 `WeakPtr` 而不是原始指针，是因为工厂拥有动画的所有权。如果工厂被销毁，所有返回的 `WeakPtr` 会自动失效，不会变成悬空指针：

```cpp
{
    CFMaterialAnimationFactory factory(*theme);
    auto anim = factory.getAnimation("md.animation.fadeIn");
} // factory 被销毁，anim 变成无效引用

// 使用 anim 前必须检查
if (anim) {
    // 这里不会执行，因为 anim 已经失效
}
```

## 全局开关

工厂支持全局和单独的动画开关，这对性能优化和无障碍支持很有用：

```cpp
// 全局关闭所有动画（比如在性能敏感场景）
factory->setEnabledAll(false);

// 检查全局状态
if (!factory->isAllEnabled()) {
    return;  // 动画被禁用
}

// 单独控制某个动画
factory->setTargetEnabled("md.animation.fadeIn", false);
if (!factory->targetEnabled("md.animation.fadeIn")) {
    // 这个动画被禁用
}
```

⚠️ `setEnabledAll(false)` 只影响**新创建**的动画，已经正在运行的动画会继续直到完成。如果需要立即停止所有动画，需要自己维护一个引用列表并逐个调用 `stop()`。

## 目标帧率

`setTargetFps()` 控制动画更新的频率，影响所有由这个工厂创建的动画：

```cpp
factory->setTargetFps(60.0f);  // 60 FPS
factory->setTargetFps(30.0f);  // 30 FPS（省电）
```

这个值会影响动画 tick 之间的时间间隔，但不会改变动画的总时长——时长是通过 `MotionSpec` 控制的。

## 注册结果处理

`registerOneAnimation()` 和 `registerAnimationCreator()` 都会返回一个 `RegisteredResult` 枚举：

```cpp
enum class RegisteredResult {
    OK,            // 注册成功
    DUP_NAME,      // 名称已存在
    UNSUPPORT_TYPE // 不支持的类型
};
```

遇到 `DUP_NAME` 时，如果要替换已存在的动画，需要先移除再重新注册。目前接口没有直接提供移除方法，这是设计上的取舍——我们假设动画注册是在初始化阶段一次性完成的。

## 相关文档

- [ICFAbstractAnimation - 动画基类](./animation.md)
- [CFMaterialAnimationFactory - Material 动画工厂](../material/animation/cfmaterial_animation_factory.md)
- [ICFTheme - 主题系统](../core/theme.md)
