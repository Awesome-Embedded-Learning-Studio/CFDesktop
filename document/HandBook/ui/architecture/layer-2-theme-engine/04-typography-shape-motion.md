---
title: 字体、形状与动效——完整的设计规范系统
description: 前面几篇文章我们聊了主题系统的架构、Token 的设计、颜色的实现。但 Material Desig
---

# 字体、形状与动效——完整的设计规范系统

前面几篇文章我们聊了主题系统的架构、Token 的设计、颜色的实现。但 Material Design 3 不只是颜色，它是一套完整的设计规范系统，包括字体、形状、动效等维度。

这篇文章聊聊主题系统的其他组件。

## ICFTheme 的四个组件

回顾一下 ICFTheme 接口，它包含四个组件：

```cpp
struct ICFTheme {
    ICFColorScheme& color_scheme() const;
    IMotionSpec& motion_spec() const;
    IRadiusScale& radius_scale() const;
    IFontType& font_type() const;

protected:
    std::unique_ptr<ICFColorScheme> color_scheme_;
    std::unique_ptr<IMotionSpec> motion_spec_;
    std::unique_ptr<IRadiusScale> radius_scale_;
    std::unique_ptr<IFontType> font_type_;
};
```bash

这四个组件相互独立，但通过主题聚合在一起。控件可以根据需要选择性地使用这些组件。

## Material Type Scale 字体分级体系

Material Design 3 定义了一套完整的字体分级体系，每个等级都有明确的用途：

| 名称 | 用途 | 字重 | 大小(sp) |
|---|---|---|---|
| Display Large | 超大标题 | Regular 400 | 57 |
| Display Medium | 大标题 | Regular 400 | 45 |
| Display Small | 中标题 | Regular 400 | 36 |
| Headline Large | 一级标题 | Regular 400 | 32 |
| Headline Medium | 二级标题 | Regular 400 | 28 |
| Headline Small | 三级标题 | Regular 400 | 24 |
| Title Large | 大标题 | Medium 500 | 22 |
| Title Medium | 中标题 | Medium 500 | 16 |
| Title Small | 小标题 | Medium 500 | 14 |
| Body Large | 大正文 | Regular 400 | 16 |
| Body Medium | 中正文 | Regular 400 | 14 |
| Body Small | 小正文 | Regular 400 | 12 |
| Label Large | 大标签 | Medium 500 | 14 |
| Label Medium | 中标签 | Medium 500 | 12 |
| Label Small | 小标签 | Medium 500 | 11 |

这套分级体系确保了整个应用的文字大小、字重保持一致。

## IFontType 接口

IFontType 是一个简单的接口，只有一个方法：

```cpp
struct IFontType {
    virtual QFont queryTargetFont(const char* name) = 0;
};
```text

使用方式：

```cpp
// 获取 Body Large 字体
QFont bodyLarge = theme->font_type().queryTargetFont("bodyLarge");

// 设置到控件
label->setFont(bodyLarge);
```bash

MaterialTypography 的实现内部维护了一个 QFont 的映射表，用 Token 名称作为键。为了保证性能，QFont 对象被缓存起来，避免重复创建。

## IRadiusScale 圆角分级

Material Design 3 的圆角分为几个等级：

| 名称 | 值(dp) |
|---|---:|
| None | 0 |
| ExtraSmall | 4 |
| Small | 8 |
| Medium | 12 |
| Large | 16 |
| ExtraLarge | 28 |
| Full | 50% (动态计算) |

这个分级和我们在 Layer 1 提到的 ShapeScale 枚举是对应的。

IRadiusScale 接口也很简单：

```cpp
struct IRadiusScale {
    virtual float queryRadiusScale(const char* name) = 0;
};
```text

返回值是 dp 单位，控件需要根据 devicePixelRatio 转换成像素：

```cpp
float radiusDp = theme->radius_scale().queryRadiusScale("cornerMedium");
CanvasUnitHelper helper(qApp->devicePixelRatio());
float radiusPx = helper.dpToPx(radiusDp);
```text

注意这里有个设计细节：IRadiusScale 返回的是 dp，不是 px。原因是圆角应该在所有 DPI 下保持一致的"视觉大小"，所以应该由控件根据当前的 devicePixelRatio 进行转换。

## IMotionSpec 动画规范

Material Design 3 的动画系统基于"时长 + 缓动"的二维配置。每个动画都有三个参数：

- **Duration**：动画持续时间（毫秒）
- **Easing**：缓动曲线类型
- **Delay**：延迟时间（毫秒，可选）

IMotionSpec 接口提供了三个查询方法：

```cpp
struct IMotionSpec {
    virtual int queryDuration(const char* name) = 0;
    virtual int queryEasing(const char* name) = 0;
    virtual int queryDelay(const char* name) = 0;
};
```text

使用方式：

```cpp
// 获取 "md.motion.shortEnter" 的动画参数
int duration = theme->motion_spec().queryDuration("md.motion.shortEnter");
int easing = theme->motion_spec().queryEasing("md.motion.shortEnter");
int delay = theme->motion_spec().queryDelay("md.motion.shortEnter");
```bash

Material Design 3 定义了几种标准动画：

| 名称 | Duration | Easing |
|---|---:|---|
| shortEnter | 200ms | Emphasized |
| mediumEnter | 250ms | Emphasized |
| longEnter | 300ms | Emphasized |
| shortExit | 150ms | Standard |
| mediumExit | 200ms | Standard |
| longExit | 250ms | Standard |

easing 返回的是一个整数，对应我们在 Layer 1 定义的 Easing::Type 枚举。这样做的原因是保持接口的跨语言兼容性——QFont、QColor 等类型在绑定到其他语言时可能有问题，而整数是通用的。

## 组合使用

控件在实际使用时，会同时访问多个组件：

```cpp
void Button::paintEvent(QPaintEvent* event) {
    // 获取颜色
    QColor containerColor = theme->color_scheme().queryColor("md.primaryContainer");
    QColor labelColor = theme->color_scheme().queryColor("md.onPrimaryContainer");

    // 获取字体
    QFont labelFont = theme->font_type().queryTargetFont("labelLarge");

    // 获取圆角
    float radiusDp = theme->radius_scale().queryRadiusScale("cornerFull");

    // 获取动画参数（用于状态切换）
    int duration = theme->motion_spec().queryDuration("md.motion.shortEnter");

    // ... 使用这些值绘制
}
```yaml

## 扩展性

这套设计的一个好处是扩展性。如果你想添加一个新的主题类型，只需要：

1. 实现 ICFColorScheme、IFontType、IRadiusScale、IMotionSpec 接口
2. 实现一个 ThemeFactory 来创建主题
3. 将工厂注册到 ThemeManager

不需要修改任何控件代码，因为控件只依赖接口，不依赖具体实现。

## 总结

到这里，Layer 2（Theme Engine Layer）的内容就基本覆盖了。我们有了：

- 主题系统的整体架构（ICFTheme + ThemeFactory + ThemeManager）
- Token 系统的设计（StaticToken + DynamicToken + TokenRegistry）
- 颜色方案的具体实现（MaterialColorScheme + tonalPalette）
- 字体、圆角、动画的接口定义

这套系统让控件能够以一致的方式访问主题数据，支持动态主题切换，并且有良好的扩展性。

但有了主题只是第一步，控件还需要行为层的支持——状态管理、涟漪效果、阴影绘制等。

接下来，我们进入 Layer 3：Animation Engine Layer，看看统一的动画系统是如何设计的。

---

**相关文档**

- [主题系统架构设计](./01-theme-system-design.md)——主题系统的整体设计
- [Token 系统设计](./02-token-system.md)——字符串字面量的编译时魔法
- [颜色方案实现](./03-color-scheme.md)——从种子颜色到完整主题
- [动画引擎架构](../layer-3-animation-engine/01-animation-architecture.md)——下一层的入口
