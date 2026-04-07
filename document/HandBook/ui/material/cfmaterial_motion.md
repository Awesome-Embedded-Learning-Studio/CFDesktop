# MaterialMotionScheme - Material 动画系统

`MaterialMotionScheme` 实现 Material Design 3 的动画规范。Material 把动画分成 9 种预设，每种都有精确的时长和缓动曲线。这套系统的核心思想是"自然运动"——模仿真实物理世界的运动规律，让界面感觉更"真实"。

## 运动预设

Material Design 3 定义了 9 种运动预设：

| 预设 | 时长 | 缓动曲线 | 用途 |
|------|------|----------|------|
| shortEnter | 200ms | EmphasizedDecelerate | 小元素进入屏幕 |
| shortExit | 150ms | EmphasizedAccelerate | 小元素退出屏幕 |
| mediumEnter | 300ms | EmphasizedDecelerate | 中等元素进入 |
| mediumExit | 250ms | EmphasizedAccelerate | 中等元素退出 |
| longEnter | 450ms | Emphasized | 大元素进入 |
| longExit | 400ms | Emphasized | 大元素退出 |
| stateChange | 200ms | Standard | 状态层动画 |
| rippleExpand | 400ms | Standard | 水波纹扩展 |
| rippleFade | 150ms | Linear | 水波纹淡出 |

## 基本用法

通过工厂函数创建默认运动系统：

```cpp
#include "material_factory.hpp"

// 创建默认运动规范
auto motion = cf::ui::core::material::motion();

// 查询时长
int duration = motion.queryDuration("shortEnter");  // 200

// 查询缓动
int easing = motion.queryEasing("shortEnter");

// 获取完整运动规格
MotionSpec spec = motion.getMotionSpec("mediumEnter");
// spec.durationMs = 300
// spec.easing = Easing::Type::EmphasizedDecelerate
```

## 运动规格结构

`MotionSpec` 把时长、缓动和延迟打包在一起：

```cpp
struct MotionSpec {
    int durationMs;                    // 时长（毫秒）
    cf::ui::base::Easing::Type easing; // 缓动类型
    int delayMs = 0;                   // 延迟（毫秒）
};
```

这个结构可以直接用于动画设置：

```cpp
void MyWidget::animateIn() {
    auto* motion = getMotionScheme();
    MotionSpec spec = motion->getMotionSpec("mediumEnter");

    QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");
    anim->setDuration(spec.durationMs);
    anim->setEasingCurve(spec.toEasingCurve());
    anim->setStartValue(offScreenPos());
    anim->setEndValue(targetPos());
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
```

## 静态预设函数

除了通过字符串查询，也可以直接用静态函数：

```cpp
// 短进入动画
MotionSpec spec = MotionPresets::shortEnter();
// durationMs = 200, easing = EmphasizedDecelerate

// 中退出动画
MotionSpec spec = MotionPresets::mediumExit();
// durationMs = 250, easing = EmphasizedAccelerate

// 状态切换动画
MotionSpec spec = MotionPresets::stateChange();
// durationMs = 200, easing = Standard
```

静态函数在编译期就能确定值，没有字符串查找开销，性能更好。

## 获取所有预设

可以用 `presets()` 一次性获取所有预设：

```cpp
MaterialMotionScheme motion = material::motion();
auto all = motion.presets();

// 使用各个预设
useSpec(all.shortEnter);
useSpec(all.mediumExit);
useSpec(all.rippleExpand);
```

这个设计方便在需要同时使用多个预设时减少代码重复。

## 缓动曲线说明

Material 使用自定义缓动曲线，不是标准的 Qt 曲线：

| Material 曲线 | 描述 |
|---------------|------|
| Standard | 标准缓动，类似 EaseOutCubic |
| Emphasized | 强调缓动，快速开始平滑结束 |
| EmphasizedDecelerate | 强调减速，快速开始明显减速 |
| EmphasizedAccelerate | 强调加速，慢速开始快速结束 |
| Linear | 线性，匀速运动 |

我们的 `cf::ui::base::Easing` 模块实现了这些曲线，`MotionSpec::toEasingCurve()` 会转换成 Qt 的 `QEasingCurve`。

## 元素大小对应关系

不同大小的元素应该用不同的运动时长：

```cpp
// 根据元素大小选择预设
MotionSpec spec;
if (size.width() < 200) {
    spec = MotionPresets::shortEnter();   // 小元素
} else if (size.width() < 500) {
    spec = MotionPresets::mediumEnter();  // 中元素
} else {
    spec = MotionPresets::longEnter();    // 大元素
}
```

这个对应关系能保证动画速度和元素大小匹配——大物体运动慢，小物体运动快，符合物理直觉。

## 进场和出场

进入动画通常比退出动画长，这是因为人眼对"出现"的过程更敏感：

```cpp
// 进入：200ms（短）
animateIn(MotionPresets::shortEnter());

// 退出：150ms（短）
animateOut(MotionPresets::shortExit());
```

同样的"短"级别，进入比退出慢 50ms。

## 水波纹效果

Material 的点击水波纹有专门的预设：

```cpp
void RippleEffect::start() {
    auto* motion = getMotionScheme();

    // 扩展阶段
    m_expandSpec = motion->getMotionSpec("rippleExpand");
    // 400ms, Standard

    // 淡出阶段
    m_fadeSpec = motion->getMotionSpec("rippleFade");
    // 150ms, Linear
}
```

水波纹淡出用 Linear 是有原因的——淡出是透明度变化，用线性曲线更自然。

## 自定义运动规格

可以创建自定义 `MotionSpec`：

```cpp
MotionSpec customSpec;
customSpec.durationMs = 500;
customSpec.easing = cf::ui::base::Easing::Type::Emphasized;
customSpec.delayMs = 100;
```

或者基于预设修改：

```cpp
MotionSpec spec = MotionPresets::mediumEnter();
spec.delayMs = 200;  // 添加延迟
spec.durationMs = 400;  // 延长时长
```

## 相关文档

- [MaterialTheme - 主题组合](./cfmaterial_theme.md)
- [MaterialColorScheme - 颜色方案](./cfmaterial_scheme.md)
- [base/easing.h - 缓动曲线](../base/easing.md)
