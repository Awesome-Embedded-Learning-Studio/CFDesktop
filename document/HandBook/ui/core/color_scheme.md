---
title: "ICFColorScheme - 颜色方案接口"
description: 定义了按名称查询颜色的抽象接口，是 Material Design 3 颜色系统的 C++ 映射层。
---

# ICFColorScheme - 颜色方案接口

`ICFColorScheme` 定义了按名称查询颜色的抽象接口，是 Material Design 3 颜色系统的 C++ 映射层。选择用字符串键值对而不是枚举来访问颜色，是为了支持动态主题和运行时扩展——我们可以在不重新编译的情况下加载新的颜色方案。

## 基本用法

通过 `queryColor()` 查询颜色值：

```cpp
#include "ui/core/color_scheme.h"

ICFColorScheme& colors = theme.color_scheme();

// 查询 Material Design 颜色 token
QColor primary = colors.queryColor("md.primary");
QColor onPrimary = colors.queryColor("md.onPrimary");
QColor surface = colors.queryColor("md.surface");
QColor error = colors.queryColor("md.error");
```text

`queryColor()` 返回的是 `QColor` 的副本，适合直接使用。如果需要避免拷贝开销（比如在循环中频繁访问），可以用 `queryExpectedColor()` 获取引用：

```cpp
// 避免拷贝的高频访问场景
const QColor& primary = colors.queryExpectedColor("md.primary");
for (int i = 0; i < 1000; ++i) {
    // 使用 primary，不会触发拷贝
    painter.setBrush(primary);
}
```text

⚠️ `queryExpectedColor()` 返回的引用生命周期和 color scheme 对象绑定。如果 color scheme 被销毁，引用就会悬空——这个坑在异步代码里特别容易出现，务必注意。

## Token 命名约定

虽然接口本身不规定 token 的命名格式，但我们遵循 Material Design 3 的约定：

```cpp
// 基础颜色角色
"md.primary"          // 主色
"md.onPrimary"        // 主色上的内容色
"md.primaryContainer" // 主色容器
"md.onPrimaryContainer" // 主色容器上的内容色

// 语义颜色
"md.background"       // 背景色
"md.onBackground"     // 背景上的内容色
"md.surface"          // 表面色
"md.error"            // 错误色
"md.outline"          // 边框/分割线色
```text

具体的 token 列表由实现类决定，可以在运行时动态扩展。如果查询不存在的 token，实现类的行为取决于具体实现——我们推荐返回一个明显的 fallback 颜色（比如品红色），方便调试。

## 引用 vs 拷贝

接口提供了两个查询方法，返回类型不同：

```cpp
// 返回引用，可修改
QColor& queryExpectedColor(const char* name);

// 返回拷贝，只读便捷方法
QColor queryColor(const char* name) const;
```text

`queryExpectedColor()` 返回非 const 引用是有意为之的——这允许调用方动态修改颜色值，实现运行时主题调整。比如实现"跟随系统 accent color"的功能时，可以直接修改对应的颜色槽位：

```cpp
// 动态更新主色
void updateAccentColor(ICFColorScheme& colors, const QColor& newAccent) {
    colors.queryExpectedColor("md.primary") = newAccent;
    colors.queryExpectedColor("md.primaryContainer") = newAccent.lighter(120%);
}
```text

⚠️ 直接修改颜色会影响所有使用这个 color scheme 的组件。如果只是局部需要特殊颜色，应该在查询后自行调整，而不是修改共享的 scheme。

## 实现要点

实现 `ICFColorScheme` 时需要考虑几个细节：

1. **字符串比较开销**：每次查询都做字符串比较确实不快，但颜色查询通常不是性能热点。如果确实需要优化，可以用 `std::unordered_map` 而不是线性查找。

2. **线程安全**：如果实现类支持跨线程访问，需要在 `queryExpectedColor()` 内部加锁。考虑到锁的粒度很细（一次哈希查找），性能影响通常可接受。

3. **无效 token 处理**：不要返回 `QColor()` —— 默认构造的 QColor 是无效颜色（`isValid() == false`），在渲染时会表现为黑色，很难调试。推荐返回一个显眼的 fallback 颜色。

## 相关文档

- [ICFTheme - 主题接口](./theme.md)
- [IMotionSpec - 动画规格](./motion_spec.md)
- [IRadiusScale - 圆角规范](./radius_scale.md)
- [IFontType - 字体样式](./font_type.md)
