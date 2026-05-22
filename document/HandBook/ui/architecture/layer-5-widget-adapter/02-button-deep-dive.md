---
title: Button 控件深度解析——七步绘制流程的完整实现
description: "在上一篇文章里，我们聊了控件适配层的\"薄包装\"设计理念。这篇文章以 Button 控件为例，完整展示"
---

# Button 控件深度解析——七步绘制流程的完整实现

在上一篇文章里，我们聊了控件适配层的"薄包装"设计理念。这篇文章以 Button 控件为例，完整展示这个理念如何付诸实践。

Button 是我们实现最完整的控件，可以作为所有其他控件的参考模板。

## 五种按钮变体

Material Design 3 定义了五种按钮变体，每种有不同的视觉用途：

| 变体 | 用途 | 视觉特点 |
|---|---|---|
| Filled | 主要操作 | 填充 Primary 颜色 |
| Tonal | 次要操作 | 填充 SecondaryContainer 颜色 |
| Outlined | 次要操作 | 无填充，有边框 |
| Text | 最低优先级 | 无填充，无边框，仅文本 |
| Elevated | 悬浮元素 | 有阴影的海拔效果 |

代码中用枚举表示：

```cpp
enum class ButtonVariant {
    Filled,
    Tonal,
    Outlined,
    Text,
    Elevated
};
```text

## 构造函数中的组件初始化

Button 的构造函数创建所有行为组件并连接信号：

```cpp
Button::Button(ButtonVariant variant, QWidget* parent)
    : QPushButton(parent), variant_(variant) {

    // 获取动画工厂
    m_animationFactory = Application::animationFactory();

    // 创建行为组件
    m_stateMachine    = new StateMachine(m_animationFactory, this);
    m_ripple          = new RippleHelper(m_animationFactory, this);
    m_elevation       = new MdElevationController(m_animationFactory, this);
    m_focusIndicator  = new MdFocusIndicator(m_animationFactory, this);

    // 设置初始海拔
    if (variant_ == ButtonVariant::Elevated) {
        m_elevation->setElevation(1);  // Elevated 按钮有 1dp 海拔
    }

    // 连接信号
    connect(m_ripple, &RippleHelper::repaintNeeded,
            this, QOverload<>::of(&Button::update));
    connect(m_stateMachine, &StateMachine::stateLayerOpacityChanged,
            this, [this](float) { update(); });

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, [this](const ICFTheme&) { update(); });
}
```text

## 事件处理的标准实现

每个事件处理函数都遵循相同的模式：先调用父类方法，再转发给行为组件。

```cpp
void Button::mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);
    m_stateMachine->onPress(event->pos());
    m_ripple->onPress(event->pos(), rect());
}

void Button::mouseReleaseEvent(QMouseEvent* event) {
    QPushButton::mouseReleaseEvent(event);
    m_stateMachine->onRelease();
    m_ripple->onRelease();
}

void Button::enterEvent(QEnterEvent* event) {
    QPushButton::enterEvent(event);
    m_stateMachine->onHoverEnter();
}

void Button::leaveEvent(QEvent* event) {
    QPushButton::leaveEvent(event);
    m_stateMachine->onHoverLeave();
}

void Button::focusInEvent(QFocusEvent* event) {
    QPushButton::focusInEvent(event);
    m_stateMachine->onFocusIn();
    m_focusIndicator->onFocusIn();
}

void Button::focusOutEvent(QFocusEvent* event) {
    QPushButton::focusOutEvent(event);
    m_stateMachine->onFocusOut();
    m_focusIndicator->onFocusOut();
}
```text

## 七步绘制流程

paintEvent 是整个控件的核心，它按七个固定步骤绘制所有视觉元素：

```cpp
void Button::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // 准备工作
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算按压偏移
    float pressOffset = m_pressEffectEnabled ? m_elevation->pressOffset() : 0.0f;
    QRectF contentRect = rect().adjusted(-margin, -margin, margin, margin);
    contentRect.translate(0, pressOffset);

    // 创建形状路径
    QPainterPath shape = geometry::roundedRect(contentRect, cornerRadius());

    // 七步绘制管道
    drawShadow(painter, contentRect, shape);      // 1. 阴影层
    drawBackground(painter, shape);              // 2. 背景色
    drawStateLayer(painter, shape);              // 3. 状态叠加层
    drawRipple(painter, shape);                  // 4. 涟漪效果
    if (variant_ == ButtonVariant::Outlined) {
        drawOutline(painter, shape);            // 5. 边框（仅Outlined）
    }
    drawContent(painter, contentRect);           // 6. 内容（图标+文本）
    drawFocusIndicator(painter, shape);         // 7. 焦点环
}
```text

让我们逐步拆解这个流程。

### 第一步：绘制阴影

阴影位于最底层，需要在背景之前绘制。对于 Elevated 变体，使用 `MdElevationController` 的阴影绘制；对于其他变体，可以跳过或使用更简单的阴影。

```cpp
void Button::drawShadow(QPainter& p, const QRectF& contentRect, const QPainterPath& shape) {
    if (variant_ == ButtonVariant::Elevated) {
        m_elevation->paintShadow(&p, shape);
    }
    // 其他变体没有阴影（或只有轻微阴影）
}
```text

### 第二步：绘制背景

背景色根据变体类型从主题获取：

```cpp
void Button::drawBackground(QPainter& p, const QPainterPath& shape) {
    QColor bgColor = containerColor();
    p.fillPath(shape, bgColor);
}

QColor Button::containerColor() const {
    auto& theme = ThemeManager::instance().currentTheme();
    switch (variant_) {
        case ButtonVariant::Filled:
            return theme.color_scheme().queryColor("md.primary");
        case ButtonVariant::Tonal:
            return theme.color_scheme().queryColor("md.secondaryContainer");
        case ButtonVariant::Outlined:
        case ButtonVariant::Text:
            return Qt::transparent;  // 无背景
        case ButtonVariant::Elevated:
            return theme.color_scheme().queryColor("md.surfaceContainerLow");
    }
    return Qt::black;  // fallback
}
```text

### 第三步：绘制状态层

状态层是一个半透明的叠加层，用于表示交互状态（Hover、Pressed 等）：

```cpp
void Button::drawStateLayer(QPainter& p, const QPainterPath& shape) {
    float opacity = m_stateMachine->stateLayerOpacity();
    if (opacity > 0.001f) {
        QColor stateColor = stateLayerColor();
        stateColor.setAlphaF(opacity);
        p.fillPath(shape, stateColor);
    }
}

QColor Button::stateLayerColor() const {
    auto& theme = ThemeManager::instance().currentTheme();
    switch (variant_) {
        case ButtonVariant::Filled:
        case ButtonVariant::Elevated:
            return theme.color_scheme().queryColor("md.onPrimary");
        case ButtonVariant::Tonal:
            return theme.color_scheme().queryColor("md.onSecondaryContainer");
        case ButtonVariant::Outlined:
            return theme.color_scheme().queryColor("md.primary");
        case ButtonVariant::Text:
            return theme.color_scheme().queryColor("md.primary");
    }
    return Qt::white;
}
```text

### 第四步：绘制涟漪

涟漪由 `RippleHelper` 绘制，它会处理多个涟漪的叠加和动画：

```cpp
void Button::drawRipple(QPainter& p, const QPainterPath& shape) {
    m_ripple->paint(&p, shape);
}
```text

### 第五步：绘制边框

只有 Outlined 变体有边框：

```cpp
void Button::drawOutline(QPainter& p, const QPainterPath& shape) {
    if (variant_ != ButtonVariant::Outlined) {
        return;
    }

    QColor outlineColor = this->outlineColor();
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float borderWidth = helper.dpToPx(1.0f);

    QPen pen(outlineColor, borderWidth);
    pen.setCosmetic(true);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(shape);
}
```text

### 第六步：绘制内容

内容包括文本和可选的前置图标：

```cpp
void Button::drawContent(QPainter& p, const QRectF& contentRect) {
    // 设置字体
    p.setFont(labelFont());
    p.setPen(labelColor());

    // 计算内容区域
    QRectF textRect = contentRect.adjusted(paddingH(), 0, -paddingH(), 0);

    // 如果有图标，绘制图标
    if (!leadingIcon_.isNull()) {
        // ... 绘制图标
        textRect.adjust(iconSize + spacing, 0, 0, 0);  // 调整文本区域
    }

    // 绘制文本
    p.drawText(textRect, Qt::AlignCenter, text());
}
```text

### 第七步：绘制焦点环

焦点环由 `MdFocusIndicator` 绘制，位于最顶层：

```cpp
void Button::drawFocusIndicator(QPainter& p, const QPainterPath& shape) {
    m_focusIndicator->paint(&p, rect(), cornerRadius());
}
```yaml

## 总结

Button 控件的实现展示了"薄包装"设计的精髓：继承 Qt 原生控件，通过组合行为组件获得 Material 特性，在 paintEvent 中按固定顺序绘制各层。

这个模式可以复用到其他控件：CheckBox、TextField、ComboBox 等都遵循相同的设计思路，只是组合的行为组件和绘制的元素有所不同。

但单个控件的实现只是第一步，我们还需要考虑整体绘制管道的性能优化。

接下来，我们聊聊绘制管道的性能考量。

---

**相关文档**

- [适配器模式](./01-adapter-pattern.md)——控件适配层的设计理念
- [绘制管道优化](./03-painting-pipeline.md)——性能优化技巧
- [涟漪与阴影](../layer-4-material-behavior/02-ripple-and-elevation.md)——行为组件的使用
