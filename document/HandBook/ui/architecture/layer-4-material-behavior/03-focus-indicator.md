---
title: 焦点指示器——无障碍访问的视觉实现
description: 在前面的文章里，我们聊了状态机、涟漪效果和海拔阴影。这些组件主要响应鼠标交互。但对于键盘导航和无障碍
---

# 焦点指示器——无障碍访问的视觉实现

在前面的文章里，我们聊了状态机、涟漪效果和海拔阴影。这些组件主要响应鼠标交互。但对于键盘导航和无障碍访问来说，还需要一个重要的视觉元素——焦点指示器。

这篇文章聊聊 MdFocusIndicator 的设计。

## Material 焦点环规范

Material Design 3 对焦点指示器有明确的规范：

- **宽度**：3dp
- **内边距**：3dp（距离控件边界）
- **动画**：淡入淡出效果
- **颜色**：通常是 Primary 或 OnPrimary 颜色

焦点环只在控件获得键盘焦点时显示，为键盘用户提供清晰的视觉反馈。

## MdFocusIndicator 的设计

MdFocusIndicator 是一个轻量级组件，只负责绘制焦点环：

```cpp
class MdFocusIndicator : public QObject {
public:
    explicit MdFocusIndicator(
        cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
        QObject* parent = nullptr);

    void onFocusIn();
    void onFocusOut();

    void paint(QPainter* painter, const QRectF& widgetRect, float cornerRadius);

private:
    float m_progress = 0.0f;  // 0 = 隐藏，1 = 完全显示
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;
};
```text

## 焦点进入/离开

当控件获得焦点时，焦点环淡入；失去焦点时，焦点环淡出：

```cpp
void MdFocusIndicator::onFocusIn() {
    auto anim = m_animator->getAnimation("md.animation.fadeIn");
    if (anim) {
        connect(anim.get(), &ICFAbstractAnimation::progressChanged,
                this, [this](float progress) {
                    m_progress = progress;
                    // 触发重绘
                });
        anim->start();
    } else {
        m_progress = 1.0f;
    }
}

void MdFocusIndicator::onFocusOut() {
    auto anim = m_animator->getAnimation("md.animation.fadeOut");
    if (anim) {
        connect(anim.get(), &ICFAbstractAnimation::progressChanged,
                this, [this](float progress) {
                    m_progress = 1.0f - progress;  // 反向
                });
        anim->start();
    } else {
        m_progress = 0.0f;
    }
}
```text

注意淡出动画使用的是 `1.0f - progress`，因为我们希望透明度从 1 变到 0。

## 焦点环的绘制

焦点环是一个圆角矩形，位于控件边界外侧 3dp 的位置：

```cpp
void MdFocusIndicator::paint(QPainter* painter, const QRectF& widgetRect,
                               float cornerRadius) {
    if (m_progress <= 0.001f) {
        return;  // 透明度为 0，跳过绘制
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float inset = helper.dpToPx(3.0f);
    float width = helper.dpToPx(3.0f);

    // 计算焦点环矩形
    QRectF ringRect = widgetRect.adjusted(-inset, -inset, inset, inset);

    // 创建圆角矩形路径
    QPainterPath ringPath;
    ringPath.addRoundedRect(ringRect, cornerRadius + inset, cornerRadius + inset);

    // 设置颜色和透明度
    QColor ringColor = /* 从主题获取 */;
    ringColor.setAlphaF(m_progress * ringColor.alphaF());

    // 绘制
    painter->save();
    QPen pen(ringColor, width);
    pen.setCosmetic(true);  // 不受变换影响
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(ringPath);
    painter->restore();
}
```text

## 与控件的集成

控件需要在焦点事件中调用 MdFocusIndicator：

```cpp
void Button::focusInEvent(QFocusEvent* event) {
    QPushButton::focusInEvent(event);
    m_focusIndicator->onFocusIn();
}

void Button::focusOutEvent(QFocusEvent* event) {
    QPushButton::focusOutEvent(event);
    m_focusIndicator->onFocusOut();
}

void Button::paintEvent(QPaintEvent* event) {
    // ... 其他绘制步骤

    // 最后绘制焦点环
    m_focusIndicator->paint(&painter, rect(), cornerRadius());
}
```yaml

## 无障碍考虑

焦点指示器是无障碍访问的重要组成部分。对于使用键盘、屏幕阅读器或其它辅助技术的用户来说，焦点指示器提供了清晰的视觉反馈，帮助他们理解当前的交互位置。

Material Design 3 要求焦点指示器必须：

1. **始终可见**：当控件有焦点时，焦点环必须清晰可见
2. **高对比度**：焦点环颜色必须与背景有足够的对比度
3. **动画平滑**：焦点切换时的过渡应该流畅自然

## 性能优化

焦点环的绘制相对简单，性能开销不大。但我们可以做一些优化：

1. **透明度为 0 时跳过绘制**：避免无意义的绘制操作
2. **使用 cosmetic pen**：`setCosmetic(true)` 让线宽不受缩放影响
3. **路径缓存**：如果控件形状不变，可以缓存 QPainterPath

## 总结

MdFocusIndicator 是 Material 行为层的最后一个核心组件。它提供了清晰的焦点反馈，支持平滑的淡入淡出动画，是无障碍访问的重要组成部分。

到这里，Layer 4（Material Behavior Layer）的内容就基本覆盖了。我们有了状态管理、涟漪效果、海拔阴影、焦点指示器四个核心组件。

但这些组件不能独立工作——它们需要被组合到具体的控件中，才能发挥作用。

接下来，我们进入 Layer 5：Widget Adapter Layer，看看如何将这些行为组件整合到具体控件中。

---

**相关文档**

- [涟漪与阴影](./02-ripple-and-elevation.md)——Material 的其他视觉组件
- [状态机设计](./01-state-machine.md)——交互状态的管理
- [适配器模式](../layer-5-widget-adapter/01-adapter-pattern.md)——下一层的入口
