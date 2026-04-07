# AnimationStrategy - Material 动画策略

`AnimationStrategy` 是 Material 动画系统的定制化接口，允许不同组件类型拥有自己的动画行为而不需要修改工厂代码。我们引入策略模式的背景是：在实际开发中发现，不同类型的 widget 对动画时长的敏感度差异很大——按钮需要快速响应，对话框可以更从容，而列表项则需要错落的节奏。

## 策略的作用时机

策略在动画创建流程的中间位置介入，时序如下：

1. 用户通过 token 或 descriptor 请求动画
2. 工厂解析 token 得到基础 `AnimationDescriptor`
3. **策略的 `adjust()` 被调用，可以修改 descriptor**
4. 工厂根据调整后的 descriptor 创建动画实例
5. 返回动画的 WeakPtr 给用户

这个介入点让你有机会在动画真正创建之前改变它的任何参数：motion token、动画类型、属性、数值范围，甚至是延迟时间。

## 动画描述符

`AnimationDescriptor` 是策略的核心操作对象，它包含了创建一个动画所需的全部信息：

```cpp
struct AnimationDescriptor {
    const char* animationType;  // "fade", "slide", "scale", "rotate"
    const char* motionToken;    // "md.motion.shortEnter", "md.motion.mediumEnter" 等
    const char* property;       // "opacity", "positionX", "positionY", "scale"
    float fromValue;           // 起始值
    float toValue;             // 结束值
    int delayMs = 0;           // 延迟时间（毫秒）
};
```

策略可以修改其中任何一个字段。比如你可以把一个 `slideUp` 改成 `fadeIn`，或者把 `mediumEnter` 时长替换为 `shortEnter`。

## 基础策略实现

最简单的策略是什么都不做，直接返回原始 descriptor：

```cpp
#include "ui/components/material/cfmaterial_animation_strategy.h"

using namespace cf::ui::components::material;

class NoOpStrategy : public AnimationStrategy {
public:
    AnimationDescriptor adjust(const AnimationDescriptor& desc,
                               QWidget* widget) override {
        // 不做任何修改，原样返回
        return desc;
    }
};
```

这个实现其实就是 `DefaultAnimationStrategy` 的做法——当你不设置策略时，工厂默认使用的就是这个。

## 修改运动时长

最常见的定制需求是调整动画时长。Material Design 3 定义了四个标准时长等级，但不同组件可能需要不同的选择：

```cpp
class FastButtonStrategy : public AnimationStrategy {
public:
    AnimationDescriptor adjust(const AnimationDescriptor& desc,
                               QWidget* widget) override {
        AnimationDescriptor adjusted = desc;

        // 按钮的动画全部使用 shortEnter/shortExit
        if (qobject_cast<QPushButton*>(widget) ||
            qobject_cast<QToolButton*>(widget)) {

            if (strcmp(desc.motionToken, "md.motion.mediumEnter") == 0) {
                adjusted.motionToken = "md.motion.shortEnter";
            } else if (strcmp(desc.motionToken, "md.motion.mediumExit") == 0) {
                adjusted.motionToken = "md.motion.shortExit";
            }
        }
        return adjusted;
    }
};
```

Material Design 3 的时长标准是：shortEnter=200ms、mediumEnter=300ms、longEnter=400ms，对应的 exit 时长稍短一些。

## 条件禁用动画

策略还可以基于 widget 的状态决定是否启用动画，这对无障碍支持很有用：

```cpp
class AccessibilityAwareStrategy : public AnimationStrategy {
public:
    bool shouldEnable(QWidget* widget) const override {
        // 检查系统的"减少动画"设置
        if (QGuiApplication::styleHints()->showIsFullScreen() == false) {
            return false;
        }

        // 或者基于 widget 特定条件
        if (widget && widget->property("suppressAnimation").toBool()) {
            return false;
        }

        return globalEnabled_;
    }
};
```

`shouldEnable()` 返回 false 时，工厂的 `getAnimation()` 和 `createAnimation()` 会返回无效的 WeakPtr。

## 组合多个条件

实际项目中，一个策略往往需要处理多种情况。我们建议用分步处理的方式保持代码清晰：

```cpp
class SmartStrategy : public AnimationStrategy {
public:
    AnimationDescriptor adjust(const AnimationDescriptor& desc,
                               QWidget* widget) override {
        AnimationDescriptor adjusted = desc;

        // 步骤1：根据组件类型调整
        if (auto* btn = qobject_cast<QPushButton*>(widget)) {
            adjusted = adjustForButton(adjusted, btn);
        } else if (auto* combo = qobject_cast<QComboBox*>(widget)) {
            adjusted = adjustForComboBox(adjusted, combo);
        }

        // 步骤2：根据 widget 尺寸调整
        if (widget && widget->width() < 50) {
            // 特别小的组件用更快的动画
            if (strcmp(adjusted.motionToken, "md.motion.mediumEnter") == 0) {
                adjusted.motionToken = "md.motion.shortEnter";
            }
        }

        return adjusted;
    }

private:
    AnimationDescriptor adjustForButton(const AnimationDescriptor& desc,
                                        QPushButton* btn) {
        AnimationDescriptor result = desc;
        // 按钮的滑动动画改为淡入（更轻量）
        if (strcmp(desc.animationType, "slide") == 0) {
            result.animationType = "fade";
            result.property = "opacity";
            result.fromValue = 0.0f;
            result.toValue = 1.0f;
        }
        return result;
    }

    AnimationDescriptor adjustForComboBox(const AnimationDescriptor& desc,
                                          QComboBox* combo) {
        // ComboBox 的定制逻辑...
        return desc;
    }
};
```

## 全局启用状态

策略有一个 `globalEnabled_` 成员，可以通过 `setGlobalEnabled()` 修改：

```cpp
strategy->setGlobalEnabled(false);  // 禁用所有使用此策略的动画
```

这个设置不会影响 `shouldEnable()` 的其他逻辑——你的实现仍然可以在 `globalEnabled_` 为 false 时返回 true。

⚠️ 不要在策略中存储 widget 指针。`adjust()` 和 `shouldEnable()` 接收的 widget 指针只在调用期间有效，存储它会导致悬空指针。如果需要持久化 widget 相关信息，应该用 QObject 的 property 机制或者其他方式关联。

## 性能考虑

策略的 `adjust()` 方法在每次创建动画时都会被调用，所以应该保持轻量。避免在策略中进行：

- 复杂的计算或 I/O 操作
- 动态内存分配
- 耗时的查询操作

如果你需要基于一些复杂条件来做决策，建议预先计算好结果，用某种缓存机制存储起来，策略中只做简单的查找。

## 相关文档

- [CFMaterialAnimationFactory - 动画工厂](./cfmaterial_animation_factory.md)
- [AnimationDescriptor - 动画描述符](../../core/token/motion/cfmaterial_motion_token_literals.md)
- [Material Design 3 运动规范](https://m3.material.io/styles/motion)
