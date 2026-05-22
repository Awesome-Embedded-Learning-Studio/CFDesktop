---
title: 涟漪与阴影——Material 视觉反馈的完整实现
description: "在上一篇文章里，我们聊了 StateMachine 如何管理交互状态。但状态变化只是\"逻辑\"上的变化"
---

# 涟漪与阴影——Material 视觉反馈的完整实现

在上一篇文章里，我们聊了 StateMachine 如何管理交互状态。但状态变化只是"逻辑"上的变化，用户还需要"视觉"上的反馈——涟漪效果和阴影变化。

这篇文章聊聊 Material 的两个核心视觉组件：RippleHelper 和 MdElevationController。

## RippleHelper：涟漪效果管理器

涟漪是 Material Design 最标志性的视觉元素之一。当用户点击一个控件时，一个圆形的涟漪从点击位置扩散开来，提供即时的视觉反馈。

### 涟漪的生命周期

涟漪有四个关键阶段：

1. **创建（onPress）**：鼠标按下时，在点击位置创建一个新的涟漪
2. **扩散**：涟漪半径从 0 扩散到最大半径
3. **释放（onRelease）**：鼠标释放时，触发淡出动画
4. **消失**：淡出动画完成后，涟漪被移除

### MdRipple 数据结构

每个涟漪由以下数据描述：

```cpp
struct MdRipple {
    QPointF center;    // 涟漪中心
    float radius;      // 当前半径
    float opacity;     // 当前透明度
    bool releasing;    // 是否处于释放阶段
    float maxRadius;   // 最大半径
};
```text

### 最大半径的计算

最大半径取决于渲染模式：

- **Bounded 模式**：从点击位置到控件最远角落的距离
- **Unbounded 模式**：足够大以覆盖整个屏幕（用于 FAB 等控件）

```cpp
float RippleHelper::maxRadius(const QRectF& rect, const QPointF& center) const {
    if (m_mode == Mode::Unbounded) {
        return 1000.0f;  // 足够大
    }

    // 计算到四个角落的距离，取最大值
    float d1 = std::hypot(center.x() - rect.left(), center.y() - rect.top());
    float d2 = std::hypot(center.x() - rect.right(), center.y() - rect.top());
    float d3 = std::hypot(center.x() - rect.left(), center.y() - rect.bottom());
    float d4 = std::hypot(center.x() - rect.right(), center.y() - rect.bottom());
    return std::max({d1, d2, d3, d4});
}
```text

### 多涟漪并存

快速多次点击会产生多个涟漪。RippleHelper 维护一个涟漪列表：

```cpp
QList<MdRipple> m_ripples;
```text

每次 `paint()` 时，所有涟漪都被绘制。当涟漪的透明度降到 0 以下时，它被从列表中移除。

### 涟漪动画

涟漪的半径和透明度分别由两个动画控制：

- 半径动画：从 0 扩散到 maxRadius（使用 md.animation.rippleExpand）
- 透明度动画：释放后从当前值淡出到 0（使用 md.animation.fadeOut）

```cpp
void RippleHelper::onPress(const QPoint& pos, const QRectF& widgetRect) {
    MdRipple ripple;
    ripple.center = pos;
    ripple.radius = 0.0f;
    ripple.opacity = 1.0f;
    ripple.releasing = false;
    ripple.maxRadius = maxRadius(widgetRect, pos);

    m_ripples.append(ripple);

    // 启动半径扩散动画
    auto anim = m_animator->getAnimation("md.animation.rippleExpand");
    if (anim) {
        connect(anim.get(), &ICFAbstractAnimation::progressChanged,
                this, [this](float progress) {
                    // 更新涟漪半径
                    for (auto& ripple : m_ripples) {
                        if (!ripple.releasing) {
                            ripple.radius = ripple.maxRadius * progress;
                        }
                    }
                    emit repaintNeeded();
                });
        anim->start();
    }
}
```text

### 涟漪颜色

涟漪颜色通常是控件的状态颜色（onPrimary、onSurface 等），通过 `setColor()` 设置：

```cpp
void RippleHelper::setColor(const CFColor& color) {
    m_color = color;
}
```text

绘制时，使用这个颜色加上当前的透明度值：

```cpp
QColor rippleColor = m_color.native_color;
rippleColor.setAlphaF(m_ripples[i].opacity);
```text

### 涟漪绘制

```cpp
void RippleHelper::paint(QPainter* painter, const QPainterPath& clipPath) {
    painter->save();
    painter->setClipPath(clipPath);  // Bounded 模式下裁剪

    for (const auto& ripple : m_ripples) {
        QColor rippleColor = m_color.native_color();
        rippleColor.setAlphaF(ripple.opacity);

        painter->setBrush(rippleColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(ripple.center, ripple.radius, ripple.radius);
    }

    painter->restore();
}
```bash

## MdElevationController：海拔阴影控制器

Material Design 3 用"海拔"（elevation）来表示控件的层级关系。海拔越高，阴影越明显。

### 6 级海拔系统

Material 定义了 6 个标准海拔级别：

| Level | dp | Blur | Offset | Opacity |
|---:|---|---|---|---|
| 0 | 0dp | 0px | 0px | 0.00 |
| 1 | 1dp | 2px | 1px | 0.15 |
| 2 | 3dp | 4px | 2px | 0.20 |
| 3 | 6dp | 8px | 4px | 0.25 |
| 4 | 8dp | 12px | 6px | 0.30 |
| 5 | 12dp | 16px | 8px | 0.35 |

### 阴影参数的计算

```cpp
MdElevationController::ShadowParams MdElevationController::paramsForLevel(float level) const {
    // 根据级别返回对应的 blur、offset、opacity
    // ...
}
```text

### 阴影绘制

阴影使用多层叠加来实现更自然的效果：

```cpp
void MdElevationController::paintShadow(QPainter* painter, const QPainterPath& shape) {
    ShadowParams params = paramsForLevel(m_currentLevel);

    // 第一层阴影
    QColor shadowColor(0, 0, 0, params.opacity * 255);
    QPainterPath shadow1 = shape.translated(params.offsetX, params.offsetY);
    // ... 绘制带模糊的阴影

    // 第二层阴影（可选，更明显的海拔效果）
}
```text

实际实现中，Qt 的 QGraphicsDropShadowEffect 可以用来生成模糊阴影，但我们可能需要自定义绘制以获得更精确的控制。

### 按压效果

当控件被按下时，海拔会暂时"降低"，产生"被压下去"的效果：

```cpp
void MdElevationController::setPressed(bool pressed) {
    m_isPressed = pressed;
    if (pressed) {
        // 暂时降低海拔
    } else {
        // 恢复原始海拔
    }
}
```text

这个效果通过调整阴影偏移和透明度来实现。

### pressOffset 计算

按压状态下的垂直偏移量用于实现"按下去"的视觉效果：

```cpp
float MdElevationController::pressOffset() const {
    // 基于当前海拔计算偏移量
    // 海拔越高，按压时的偏移量越大
    return m_currentLevel * 2.0f;  // 简化的公式
}
```text

### Dark Theme 的叠色表示

在暗色主题中，海拔不是用阴影表示的，而是通过向 Surface 颜色叠加 Primary 色调来实现：

```cpp
CFColor MdElevationController::tonalOverlay(CFColor surface, CFColor primary) const {
    // 使用 elevationOverlay 函数计算叠色
    return elevationOverlay(surface, primary, static_cast<int>(m_currentLevel));
}
```yaml

`elevationOverlay` 函数我们在 Layer 1 讲过，它根据海拔级别选择不同的叠加透明度。

## 总结

RippleHelper 和 MdElevationController 是 Material 视觉反馈的两个核心组件。涟漪提供即时的点击反馈，阴影表示控件的层级关系。

但除了涟漪和阴影，Material 还有一个重要的视觉元素——焦点指示器，它对键盘导航和无障碍访问至关重要。

接下来，我们聊聊焦点指示器的设计。

---

**相关文档**

- [状态机设计](./01-state-machine.md)——交互状态的管理
- [焦点指示器](./03-focus-indicator.md)——无障碍访问的视觉实现
- [颜色系统实现](../layer-2-theme-engine/03-color-scheme.md)——Layer 2 的颜色方案
