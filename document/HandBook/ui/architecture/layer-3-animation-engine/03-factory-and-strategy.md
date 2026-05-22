---
title: 工厂与策略——动画创建的灵活组合模式
description: 在前面的文章里，我们讲了动画的抽象基类和两种具体实现。但控件怎么获取这些动画？直接  一个吗？
---

# 工厂与策略——动画创建的灵活组合模式

在前面的文章里，我们讲了动画的抽象基类和两种具体实现。但控件怎么获取这些动画？直接 `new` 一个吗？

不，那样会让控件和动画类型耦合，难以扩展和测试。我们用了工厂模式 + 策略模式的组合来解决这个问题。

## CFMaterialAnimationFactory 的职责

CFMaterialAnimationFactory 是 Material Design 3 动画的核心工厂，它负责：

1. **Token 解析**：将 "md.animation.fadeIn" 这样的 token 转换为动画描述
2. **动画创建**：根据描述创建具体的动画实例
3. **生命周期管理**：拥有创建的动画，提供 WeakPtr 访问
4. **全局开关**：支持启用/禁用所有动画
5. **策略应用**：允许控件类型定制动画行为

```cpp
class CFMaterialAnimationFactory : public ICFAnimationManagerFactory {
public:
    explicit CFMaterialAnimationFactory(const ICFTheme& theme,
                                        std::unique_ptr<AnimationStrategy> strategy = nullptr,
                                        QObject* parent = nullptr);

    cf::WeakPtr<ICFAbstractAnimation> getAnimation(const char* animationToken) override;

    void setStrategy(std::unique_ptr<AnimationStrategy> strategy);
    void setEnabledAll(bool enabled) override;

private:
    const ICFTheme& theme_;
    std::unique_ptr<AnimationStrategy> strategy_;
    bool globalEnabled_ = true;
    std::unordered_map<std::string, std::unique_ptr<ICFAbstractAnimation>> animations_;
};
```bash

## Token 到 AnimationDescriptor 的映射

Material Design 3 定义了标准的动画 Token，比如：

| Token | 类型 | MotionSpec |
|---|---|---|
| md.animation.fadeIn | fade | shortEnter |
| md.animation.fadeOut | fade | shortExit |
| md.animation.slideUp | slide | mediumEnter |
| md.animation.slideDown | slide | mediumExit |
| md.animation.scaleUp | scale | shortEnter |
| md.animation.scaleDown | scale | shortExit |

工厂内部维护了一个映射表，将 Token 转换为 AnimationDescriptor：

```cpp
struct AnimationDescriptor {
    QString type;           // "fade", "slide", "scale"
    QString motionToken;    // "md.motion.shortEnter" 等
    QString targetProperty; // "opacity", "position", "scale"
    float fromValue;
    float toValue;
};
```text

## getAnimation() 的完整流程

当控件调用 `getAnimation("md.animation.fadeIn")` 时，工厂会执行以下步骤：

1. 检查 `globalEnabled_`，如果为 false 返回无效 WeakPtr
2. 检查是否已有同名动画，如果有返回缓存的动画
3. 解析 Token 到 AnimationDescriptor
4. 应用策略（如果设置了）调整描述符
5. 根据类型创建具体的动画实例
6. 存储动画（获得所有权）
7. 返回 WeakPtr 给调用者

```cpp
cf::WeakPtr<ICFAbstractAnimation> CFMaterialAnimationFactory::getAnimation(const char* token) {
    // 1. 全局开关检查
    if (!globalEnabled_) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // 2. 查找缓存
    auto it = animations_.find(token);
    if (it != animations_.end()) {
        return it->second->GetWeakPtr();
    }

    // 3. 解析 Token
    AnimationDescriptor desc = resolveToken(token);
    if (desc.type.isEmpty()) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // 4-5. 应用策略并创建动画
    desc = applyStrategy(desc, nullptr);
    auto anim = createAnimation(desc, nullptr);

    if (!anim) {
        return cf::WeakPtr<ICFAbstractAnimation>();
    }

    // 6. 存储动画
    animations_[token] = std::move(anim);

    // 7. 返回 WeakPtr
    return animations_[token]->GetWeakPtr();
}
```text

## 动画实例的创建

工厂根据 `AnimationDescriptor::type` 创建不同类型的动画：

```cpp
std::unique_ptr<ICFAbstractAnimation> CFMaterialAnimationFactory::createFadeAnimation(
    const AnimationDescriptor& desc, QWidget* widget) {

    auto motionSpec = const_cast<IMotionSpec*>(&theme_.motion_spec());
    auto anim = std::make_unique<CFMaterialFadeAnimation>(motionSpec, this);

    anim->setRange(desc.fromValue, desc.toValue);
    anim->setMotionToken(desc.motionToken.toStdString());

    return anim;
}

std::unique_ptr<ICFAbstractAnimation> CFMaterialAnimationFactory::createSlideAnimation(...) {
    // 类似的逻辑，但创建的是 CFMaterialSlideAnimation
}

std::unique_ptr<ICFAbstractAnimation> CFMaterialAnimationFactory::createScaleAnimation(...) {
    // 创建 CFMaterialScaleAnimation
}
```text

## AnimationStrategy 策略模式

有时候，不同类型的控件需要不同的动画行为。比如按钮可能需要更快的动画，对话框可能需要更慢的动画。

策略模式允许在不修改工厂代码的情况下定制动画行为：

```cpp
class AnimationStrategy {
public:
    virtual ~AnimationStrategy() = default;

    virtual AnimationDescriptor adjust(const AnimationDescriptor& desc,
                                      QWidget* widget) {
        return desc;  // 默认不做修改
    }

    virtual bool shouldEnable(QWidget* widget) {
        return true;  // 默认启用动画
    }
};
```text

控件类型可以实现自己的策略：

```cpp
class ButtonAnimationStrategy : public AnimationStrategy {
public:
    AnimationDescriptor adjust(const AnimationDescriptor& desc, QWidget* widget) override {
        // 按钮的动画更快
        AnimationDescriptor adjusted = desc;
        adjusted.fromValue = desc.fromValue * 0.8f;
        adjusted.toValue = desc.toValue * 1.2f;
        return adjusted;
    }
};

class DialogAnimationStrategy : public AnimationStrategy {
public:
    AnimationDescriptor adjust(const AnimationDescriptor& desc, QWidget* widget) override {
        // 对话框的动画更慢、更平滑
        AnimationDescriptor adjusted = desc;
        adjusted.motionToken = "md.motion.longEnter";
        return adjusted;
    }
};
```text

使用策略：

```cpp
// 创建工厂时设置策略
auto buttonStrategy = std::make_unique<ButtonAnimationStrategy>();
auto factory = std::make_unique<CFMaterialAnimationFactory>(theme, std::move(buttonStrategy));

// 控件获取动画时会自动应用策略
auto anim = factory->getAnimation("md.animation.fadeIn");
```text

## 策略的应用时机

策略在动画创建之前应用，这样可以在创建前调整参数。但策略只影响新创建的动画，已经创建的动画不受影响。

如果需要动态更换策略，可以再次调用 `setStrategy()`，之后创建的动画会使用新策略。

## 全局动画开关

工厂提供了全局开关功能：

```cpp
factory->setEnabledAll(false);  // 禁用所有动画
```text

禁用时，`getAnimation()` 会返回无效的 WeakPtr。已有的正在运行的动画不受影响，会自然完成。

这个功能对以下场景很有用：

1. **性能优化**：繁重任务期间禁用动画
2. **无障碍**：尊重系统的"减少动画"设置
3. **用户偏好**：提供一个"禁用动画"的设置选项
4. **嵌入式环境**：资源受限时禁用所有动画

## 单个动画的启用/禁用

工厂也支持单个动画的启用/禁用：

```cpp
factory->setTargetEnabled("md.animation.fadeIn", false);
```yaml

这样只有特定的动画被禁用，其他动画正常运行。

## 动画缓存

工厂会缓存已创建的动画。当多次调用 `getAnimation("md.animation.fadeIn")` 时，会返回同一个动画实例的 WeakPtr。

这意味着如果你需要多个独立的动画实例，需要用不同的 Token 名称或者使用 `createAnimation()` 直接创建。

## 总结

工厂模式 + 策略模式的组合让动画系统既统一又灵活。工厂负责创建和管理动画，策略负责定制行为。控件只需要通过 Token 获取动画，不需要关心具体的实现细节。

到这里，Layer 3（Animation Engine Layer）的内容就基本覆盖了。我们有了统一的动画调度系统，有了两种动画类型，有了工厂和策略模式。

但动画只是"效果"，还需要有东西来触发这些动画——比如鼠标悬停、点击、焦点变化等交互行为。

接下来，我们进入 Layer 4：Material Behavior Layer，看看如何将 Qt 事件映射到 Material 视觉状态。

---

**相关文档**

- [时间与弹簧动画](./02-timing-spring-animation.md)——两种动画范式的实现
- [状态机设计](../layer-4-material-behavior/01-state-machine.md)——下一层的入口
- [动画引擎架构](./01-animation-architecture.md)——动画系统的整体设计
