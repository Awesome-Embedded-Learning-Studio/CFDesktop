---
title: 绘制管道优化——从单一控件到批量渲染的性能考量
description: 在上一篇文章里，我们深入分析了 Button 控件的七步绘制流程。每个步骤看起来都很简单，但当页面上
---

# 绘制管道优化——从单一控件到批量渲染的性能考量

在上一篇文章里，我们深入分析了 Button 控件的七步绘制流程。每个步骤看起来都很简单，但当页面上有几十个控件时，性能问题就会显现。

这篇文章聊聊绘制管道的性能优化。

## QPainter 的初始化配置

在 paintEvent 开始时，我们通常需要配置 QPainter：

```cpp
void Button::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform);  // 平滑图像变换

    // ... 绘制代码
}
```text

这两个设置会影响绘制质量：

- `Antialiasing`：让边缘更平滑，但会有性能开销
- `SmoothPixmapTransform`：让图像缩放更平滑，同样有性能开销

对于大多数控件，这两个设置是值得的。但在性能敏感的场景，可以考虑关闭。

## 避免不必要的重绘

重绘（`update()`）触发的 paintEvent 是昂贵的操作。我们应该只在真正需要时才触发重绘：

```cpp
// 不要这样做
void setChecked(bool checked) {
    checked_ = checked;
    update();  // 总是重绘
}

// 更好的做法
void setChecked(bool checked) {
    if (checked_ != checked) {
        checked_ = checked;
        update();  // 只在状态改变时重绘
    }
}
```text

状态机已经做了这个优化——只有当透明度值真正改变时，才会发出 `stateLayerOpacityChanged` 信号。

## 状态缓存策略

很多值在绘制时需要从主题获取，但频繁查询主题有开销。我们可以缓存这些值：

```cpp
class Button {
private:
    void refreshThemeCache() {
        auto& theme = ThemeManager::instance().currentTheme();
        cachedContainerColor_ = theme.color_scheme().queryColor("md.primary");
        cachedLabelColor_ = theme.color_scheme().queryColor("md.onPrimary");
        cachedCornerRadius_ = theme.radius_scale().queryRadiusScale("cornerFull");
        // ...
    }

    QColor cachedContainerColor_;
    QColor cachedLabelColor_;
    float cachedCornerRadius_;
};
```text

在 `themeChanged` 信号处理中刷新缓存：

```cpp
connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
        this, [this](const ICFTheme&) {
            refreshThemeCache();
            update();
        });
```text

这样每次绘制时就不需要查询主题了。

## 裁剪区域的优化

绘制涟漪时，我们需要裁剪到控件边界：

```cpp
void Button::drawRipple(QPainter& p, const QPainterPath& shape) {
    QPainterPath clipPath = geometry::roundedRect(rect(), cornerRadius());
    m_ripple->paint(&p, clipPath);
}
```text

Qt 的 `setClipPath` 操作比较昂贵，因为需要计算复杂的几何。如果控件形状简单（比如矩形），可以考虑用 `setClipRect` 替代：

```cpp
painter.setClipRect(rect(), Qt::IntersectClip);  // 更高效的矩形裁剪
```text

## 静态内容的预渲染

对于完全不变化的内容（比如某些背景图案），可以考虑预渲染为 QPixmap：

```cpp
void Button::cacheBackground() {
    QPixmap cache(size());
    cache.fill(Qt::transparent);

    QPainter painter(&cache);
    // ... 绘制静态背景
    painter.end();

    cachedBackground_ = cache;
}

void Button::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, cachedBackground_);  // 直接绘制缓存的图像
    // ... 绘制动态内容
}
```text

但对于大多数控件来说，这个优化可能不值得——因为背景色会随主题切换而改变。

## 批量重绘的考虑

当主题切换时，所有订阅了主题的控件都会重绘。如果页面上有大量控件，这会导致卡顿。

一个优化策略是"批量重绘"：主题切换时，先标记所有控件为"脏"，然后在下一个事件循环中统一重绘：

```cpp
// 主题切换时
for (auto* widget : installed_widgets) {
    widget->setAttribute(Qt::WA_WState_InPaintEvent, true);  // 标记为脏
}
QTimer::singleShot(0, this, []() {
    for (auto* widget : installed_widgets) {
        widget->setAttribute(Qt::WA_WState_InPaintEvent, false);
        widget->update();
    }
});
```text

这样可以将多次重绘合并为一次。

## 嵌入式环境的特殊考虑

在嵌入式环境中，可能需要禁用某些视觉效果来提升性能：

```cpp
enum class PerformanceProfile {
    Desktop,     // 全功能：60fps，阴影，Ripple，所有动画
    Embedded,    // 精简：30fps，无阴影，无 Ripple，仅状态切换动画
    UltraLow     // 极简：无动画，无阴影，仅颜色变化
};

void Button::paintEvent(QPaintEvent* event) {
    auto profile = Application::performanceProfile();

    if (profile == PerformanceProfile::UltraLow) {
        // 跳过所有动画和特效
        drawBackground();
        drawContent();
        return;
    }

    if (profile == PerformanceProfile::Embedded) {
        // 跳过阴影和涟漪
        drawBackground();
        drawStateLayer();
        drawContent();
        return;
    }

    // Desktop 配置：绘制所有效果
    // ... 完整的七步流程
}
```yaml

## 总结

绘制管道优化的核心思想是"避免不必要的工作"：避免不必要的重绘、缓存不变的值、使用更高效的 API。

但也要注意"过早优化是万恶之源"。在实际问题出现之前，保持代码的清晰和可维护性更重要。只有当性能测试显示确实存在瓶颈时，才应该考虑这些优化。

到这里，我们的"桌面主题架构设计系列"博客就全部完成了。我们从基础数学工具开始，一层一层地构建了完整的 Material Design 3 实现：

- **Layer 1**：HCT 色彩空间、数学工具、几何和 DPI 适配
- **Layer 2**：主题系统、Token 设计、颜色方案、字体形状动效
- **Layer 3**：动画引擎、时间与弹簧动画、工厂与策略
- **Layer 4**：状态机、涟漪与阴影、焦点指示器
- **Layer 5**：适配器模式、Button 控件、绘制管道优化

希望这个系列能帮助你理解 Material Design 3 桌面主题的完整架构，以及我们为什么做出这样的设计决策。

---

**相关文档**

- [Button 控件深度解析](./02-button-deep-dive.md)——控件实现的完整案例
- [适配器模式](./01-adapter-pattern.md)——控件适配层的设计理念
- [动画引擎架构](../layer-3-animation-engine/01-animation-architecture.md)——动画系统的性能考量
