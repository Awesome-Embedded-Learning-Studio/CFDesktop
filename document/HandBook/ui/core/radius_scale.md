---
title: "IRadiusScale - 圆角规范接口"
description: 定义了按名称查询圆角值的抽象接口，是 Material Design 3 形状系统的 C++ 映射层
---

# IRadiusScale - 圆角规范接口

`IRadiusScale` 定义了按名称查询圆角值的抽象接口，是 Material Design 3 形状系统的 C++ 映射层。选择用字符串键值而不是枚举来访问圆角，是为了支持动态形状配置和运行时扩展——我们可以在不重新编译的情况下调整圆角方案。

## 基本用法

通过 `queryRadiusScale()` 查询圆角值：

```cpp
#include "ui/core/radius_scale.h"

IRadiusScale& radius = theme.radius_scale();

// 查询 Material Design 圆角 token
float small = radius.queryRadiusScale("cornerSmall");
float medium = radius.queryRadiusScale("cornerMedium");
float large = radius.queryRadiusScale("cornerLarge");
float extraLarge = radius.queryRadiusScale("cornerExtraLarge");
```text

`queryRadiusScale()` 返回的是浮点数值，单位是密度无关像素（dp）。返回值可以直接用于 Qt 组件的圆角设置：

```cpp
// 设置按钮圆角
QPushButton* button = new QPushButton("Click Me");
float cornerRadius = radius.queryRadiusScale("cornerMedium");
button->setStyleSheet(QString("border-radius: %1px;").arg(cornerRadius));
```text

## Token 命名约定

虽然接口本身不规定 token 的命名格式，但我们遵循 Material Design 3 的约定：

```cpp
// 基础圆角规范
"cornerNone"         // 无圆角（0dp）
"cornerExtraSmall"   // 极小圆角（4dp）
"cornerSmall"        // 小圆角（8dp）
"cornerMedium"       // 中等圆角（12dp）
"cornerLarge"        // 大圆角（16dp）
"cornerExtraLarge"   // 极大圆角（28dp）
"cornerFull"         // 完全圆角（形状变成胶囊或圆形）
```text

具体的 token 列表由实现类决定，可以在运行时动态扩展。如果查询不存在的 token，接口规定返回 0——这是一个便于判断的 fallback 值，但可能隐藏配置错误。

⚠️ 返回 0 不总是最佳行为。如果某个圆角 token 配置缺失，你可能更希望看到一个明显的 fallback 值（比如 8dp），而不是静默地变成无圆角。实现类可以选择在查询失败时记录日志或返回默认值。

## 使用示例

在实际 UI 组件中使用圆角：

```cpp
// 自定义圆角矩形按钮
class RoundedButton : public QPushButton {
private:
    float m_cornerRadius;

public:
    RoundedButton(const char* cornerToken, QWidget* parent = nullptr)
        : QPushButton(parent) {
        auto& radius = getTheme().radius_scale();
        m_cornerRadius = radius.queryRadiusScale(cornerToken);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath path;
        path.addRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
        painter.fillPath(path, palette().button());
    }
};
```text

使用时：

```cpp
// 创建不同圆角风格的按钮
auto* smallBtn = new RoundedButton("cornerSmall");     // 轻微圆角
auto* mediumBtn = new RoundedButton("cornerMedium");   // 标准圆角
auto* largeBtn = new RoundedButton("cornerLarge");     // 大圆角
auto* pillBtn = new RoundedButton("cornerFull");       // 胶囊形状
```text

## 与样式表集成

如果使用 Qt 样式表，圆角值可以直接嵌入字符串：

```cpp
void applyCornerRadius(QWidget* widget, const char* cornerToken) {
    auto& radius = getTheme().radius_scale();
    float r = radius.queryRadiusScale(cornerToken);
    widget->setStyleSheet(QString(
        "QWidget { border-radius: %1px; }"
    ).arg(r));
}
```text

⚠️ 样式表中的圆角是按像素计算的，而 `queryRadiusScale()` 返回的是 dp 值。在高 DPI 屏幕上，需要手动乘以设备像素比率：

```cpp
float dpToPx(float dp) {
    return dp * QApplication::devicePixelRatio();
}

// 正确的高 DPI 处理
float r = dpToPx(radius.queryRadiusScale("cornerMedium"));
```text

## 实现要点

实现 `IRadiusScale` 时需要考虑几个细节：

1. **返回 0 的语义**：接口规定未找到 token 时返回 0。这在语义上是合理的——"无圆角"是一个有效的形状选择——但也可能掩盖配置错误。实现类可以选择在查询失败时记录日志。

2. **单位处理**：接口返回的是 dp 值，调用方负责根据设备像素比率转换。这是有意为之——让接口保持平台无关，具体的缩放逻辑由调用方控制。

3. **cornerFull 的实现**：Material Design 3 的 `cornerFull` 表示"完全圆角"，通常用于胶囊按钮或圆形头像。实现类可以选择返回一个足够大的值（比如 9999），让形状看起来是完全圆角，或者返回 `std::numeric_limits<float>::max()`。

4. **线程安全**：如果实现类支持跨线程访问，需要在 `queryRadiusScale()` 内部加锁。考虑到圆角查询通常只发生在 UI 初始化阶段，线程安全开销可以接受。

## 设计决策

`IRadiusScale` 只返回 `float` 而不是 `int`，这是因为圆角值可能需要更精细的粒度——比如 Material Design 3 的某些圆角规范使用非整数值。虽然最终渲染时会四舍五入到像素，但在配置层面保持浮点精度可以避免累积误差。

另一个设计点是，我们没有提供批量查询接口。圆角 token 的数量通常很少（不超过 10 个），批量查询的收益不明显。如果确实需要优化，可以在实现类内部提供缓存层，而不是增加接口复杂度。

## 相关文档

- [ICFTheme - 主题接口](./theme.md)
- [ICFColorScheme - 颜色方案](./color_scheme.md)
- [IMotionSpec - 动画规格](./motion_spec.md)
- [IFontType - 字体样式](./font_type.md)
