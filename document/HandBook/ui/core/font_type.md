---
title: "IFontType - 字体样式接口"
description: 定义了按名称查询字体样式的抽象接口，是 Material Design 3 排版系统的 C++ 映射
---

# IFontType - 字体样式接口

`IFontType` 定义了按名称查询字体样式的抽象接口，是 Material Design 3 排版系统的 C++ 映射层。选择用字符串键值而不是枚举来访问字体，是为了支持动态字体配置和运行时扩展——我们可以在不重新编译的情况下加载新的字体方案。

## 基本用法

通过 `queryTargetFont()` 查询字体样式：

```cpp
#include "ui/core/font_type.h"

IFontType& fonts = theme.font_type();

// 查询 Material Design 字体 token
QFont bodyLarge = fonts.queryTargetFont("bodyLarge");
QFont headlineMedium = fonts.queryTargetFont("headlineMedium");
QFont titleSmall = fonts.queryTargetFont("titleSmall");
QFont labelLarge = fonts.queryTargetFont("labelLarge");
```text

`queryTargetFont()` 返回的是 `QFont` 的副本，适合直接使用。Qt 的 `QFont` 采用隐式共享（写时拷贝）机制，所以即使返回副本，实际拷贝开销也很小——只有在修改字体时才会真正复制数据。

## Token 命名约定

虽然接口本身不规定 token 的命名格式，但我们遵循 Material Design 3 的约定：

```cpp
// 正文样式
"bodyLarge"    // 正文大号
"bodyMedium"   // 正文中号
"bodySmall"    // 正文小号

// 标题样式
"headlineLarge"   // 标题大号
"headlineMedium"  // 标题中号
"headlineSmall"   // 标题小号

"titleLarge"      // 次级标题大号
"titleMedium"     // 次级标题中号
"titleSmall"      // 次级标题小号

// 标签样式
"labelLarge"      // 标签大号
"labelMedium"     // 标签中号
"labelSmall"      // 标签小号
```text

具体的 token 列表由实现类决定，可以在运行时动态扩展。如果查询不存在的 token，实现类的行为取决于具体实现——我们推荐返回一个默认的 fallback 字体，比如系统等宽字体，方便调试。

## 使用示例

在实际 UI 组件中使用字体：

```cpp
// 在自定义 widget 中应用主题字体
class MyWidget : public QWidget {
protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        auto& fonts = getTheme().font_type();

        // 使用主题字体绘制文本
        QFont titleFont = fonts.queryTargetFont("headlineMedium");
        painter.setFont(titleFont);
        painter.drawText(rect(), "My Title");

        // 使用正文字体
        QFont bodyFont = fonts.queryTargetFont("bodyMedium");
        painter.setFont(bodyFont);
        painter.drawText(rect().adjusted(0, 30, 0, 0), "Body text");
    }
};
```text

⚠️ 不要在每次绘制时都查询字体——把字体对象缓存起来更好。`QFont` 的隐式共享机制让缓存几乎没有开销：

```cpp
class MyWidget : public QWidget {
private:
    QFont m_titleFont;  // 缓存字体对象

public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        auto& fonts = getTheme().font_type();
        m_titleFont = fonts.queryTargetFont("headlineMedium");
    }
};
```text

## 实现要点

实现 `IFontType` 时需要考虑几个细节：

1. **字符串比较开销**：每次查询都做字符串比较确实不快，但字体查询通常不是性能热点。如果确实需要优化，可以用 `std::unordered_map` 而不是线性查找。

2. **线程安全**：如果实现类支持跨线程访问，需要在 `queryTargetFont()` 内部加锁。考虑到 `QFont` 的拷贝开销很小，也可以在加锁后直接返回副本，避免返回引用带来的生命周期问题。

3. **无效 token 处理**：不要返回 `QFont()` —— 默认构造的 QFont 是应用程序默认字体，很难发现错误。推荐返回一个显眼的 fallback 字体，比如等宽字体或高亮样式，方便调试。

4. **字体家族回退**：如果配置的字体家族在系统上不存在，Qt 会自动回退到默认字体。实现类可以选择在初始化时验证字体可用性，或者依赖 Qt 的回退机制。

## 设计决策

`IFontType` 只提供了一个查询方法，而不是像 `ICFColorScheme` 那样提供引用和拷贝两种变体。这是因为 `QFont` 本身就是值类型的——隐式共享让拷贝开销可以忽略，而引用语义反而会带来生命周期管理的复杂度。

另一个设计点是，我们用 `const char*` 而不是 `QString` 作为参数类型。这是为了让接口更容易在 C 和其他语言中调用，同时避免 `QString` 带来的 Qt 依赖传播。如果实现类内部用 `QString` 存储，可以在方法入口处做一次转换。

## 相关文档

- [ICFTheme - 主题接口](./theme.md)
- [ICFColorScheme - 颜色方案](./color_scheme.md)
- [IMotionSpec - 动画规格](./motion_spec.md)
- [IRadiusScale - 圆角规范](./radius_scale.md)
