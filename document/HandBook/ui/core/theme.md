---
title: "ICFTheme - 主题接口"
description: 是 UI 主题系统的顶层抽象，提供对颜色方案、动画规格、圆角规范和字体样式的统一访问入口。选择把这几
---

# ICFTheme - 主题接口

`ICFTheme` 是 UI 主题系统的顶层抽象，提供对颜色方案、动画规格、圆角规范和字体样式的统一访问入口。选择把这几个分散的设计维度聚合成一个接口，是因为在实际使用中它们总是作为一个整体被切换——我们很少需要"只换颜色但不换动画"的场景。

## 接口结构

`ICFTheme` 本身是一个抽象接口，包含四个子组件的访问器：

```cpp
#include "ui/core/theme.h"

// 获取主题的各个子组件
const ICFTheme& theme = getTheme();  // 通过 ThemeFactory 获取

auto& colors = theme.color_scheme();  // ICFColorScheme&
auto& motion = theme.motion_spec();   // IMotionSpec&
auto& radius = theme.radius_scale();  // IRadiusScale&
auto& fonts = theme.font_type();      // IFontType&
```text

所有访问器都返回引用，这是因为子组件的生命周期由 `ICFTheme` 的实现类管理，调用方不需要关心所有权问题。

## 获取主题实例

`ICFTheme` 的构造函数是 protected 的，不允许直接构造。这是有意为之——强制使用 `ThemeFactory` 来创建主题实例，保证所有主题对象都经过统一的创建流程：

```cpp
#include "ui/core/theme_factory.h"

// 通过工厂创建主题
ThemeFactory* factory = getFactory();  // 获取具体工厂实现
auto theme = factory->fromName("default");  // std::unique_ptr<ICFTheme>

// 从 JSON 创建主题
QByteArray json = loadThemeJson();
auto custom_theme = factory->fromJson(json);

// 序列化主题
QByteArray serialized = factory->toJson(theme.get());
```text

使用工厂模式的好处是可以在运行时动态切换主题实现，比如从文件加载的 JSON 主题和硬编码的默认主题可以共存。

## 子组件使用

拿到主题对象后，通过它访问各个子组件：

```cpp
// 获取颜色
QColor primary = theme.color_scheme().queryColor("md.primary");
QColor surface = theme.color_scheme().queryColor("md.surface");

// 获取动画参数
int duration = theme.motion_spec().queryDuration("md.motion.shortEnter");
int easing = theme.motion_spec().queryEasing("md.motion.standard");

// 获取圆角值
float corner = theme.radius_scale().queryRadiusScale("cornerSmall");

// 获取字体
QFont body_font = theme.font_type().queryTargetFont("bodyLarge");
```text

注意这里每个子组件都有自己的查询语法——这是为兼容 Material Design 3 的 token 命名设计的。具体的 token 名称由各个子组件的实现决定，`ICFTheme` 不做统一规定。

## 设计决策

把四个子组件聚合到 `ICFTheme` 里，而不是让使用者分别管理它们，主要是为了简化主题切换逻辑。如果没有这个聚合层，每次切换主题就需要同步替换四个不同的对象，很容易出现不一致的状态——比如颜色方案已经是 dark 模式了，但字体还是 light 模式的。

另一个值得注意的设计点是，子组件用 `unique_ptr` 持有而不是直接嵌入。这样做允许实现类在运行时选择具体的子组件实现，而不需要在编译期固定类型。对于支持插件式主题扩展的场景，这个灵活性很重要。

## 线程安全

`ICFTheme` 接口本身不提供线程安全保证。如果需要在多线程环境下访问同一个主题对象，调用方需要自己加锁。实际使用中，我们通常为每个线程维护独立的主题访问入口，或者通过 TLS 缓存主题查询结果，避免频繁的跨线程访问。

## 相关文档

- [ICFColorScheme - 颜色方案](./color_scheme.md)
- [IMotionSpec - 动画规格](./motion_spec.md)
- [IRadiusScale - 圆角规范](./radius_scale.md)
- [IFontType - 字体样式](./font_type.md)
- [ThemeFactory - 主题工厂](./theme_factory.md)
