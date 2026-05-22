---
title: "MdElevationController - Material 阴影控制器"
description: 是 Material Design 3 高程系统的核心实现，负责管理控件的海拔级别和对应的阴影渲染。
---

# MdElevationController - Material 阴影控制器

`MdElevationController` 是 Material Design 3 高程系统的核心实现，负责管理控件的海拔级别和对应的阴影渲染。在 Material Design 中，高程（Elevation）是表达 UI 层级关系的关键手段，通过阴影和色调变化来传达"哪个元素在上"。我们选择自己实现，是因为 Qt 的图形系统默认不支持 Material 的多光源阴影模型和动画过渡。

## Material Design 高程系统

Material Design 定义了 6 个标准海拔级别（0-5），每个级别对应不同的阴影参数：

| 级别 | 高程值 | 模糊半径 | 偏移量 | 不透明度 |
|------|--------|----------|--------|----------|
| Level 0 | 0dp | 0px | 0px | 0% |
| Level 1 | 1dp | 2px | 1px | 15% |
| Level 2 | 3dp | 4px | 2px | 20% |
| Level 3 | 6dp | 8px | 4px | 25% |
| Level 4 | 8dp | 12px | 6px | 30% |
| Level 5 | 12dp | 16px | 8px | 35% |

这些参数在 `paramsForLevel()` 中定义，阴影效果通过多层叠加实现近似模糊。

## 基本用法

`MdElevationController` 需要配合动画工厂使用，通常在控件构造函数中初始化：

```cpp
#include "widget/material/base/elevation_controller.h"

using namespace cf::ui::widget::material;

class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        auto animationFactory = cf::WeakPtr<components::material::CFMaterialAnimationFactory>::DynamicCast(
            Application::animationFactory()
        );

        // 创建高程控制器
        m_elevation = new base::MdElevationController(animationFactory, this);

        // 设置海拔级别
        m_elevation->setElevation(2);  // Level 2
    }

private:
    base::MdElevationController* m_elevation;
};
```text

## 设置高程级别

直接使用 `setElevation()` 设置高程，这会立即更新阴影效果：

```cpp
// 静态设置
m_elevation->setElevation(0);  // 无阴影
m_elevation->setElevation(3);  // 中等阴影
m_elevation->setElevation(5);  // 最强阴影
```text

高程值会被 clamp 在 [0, 5] 范围内，超出范围的值会自动截断。

## 动画过渡

对于需要动态高程变化的场景（如按钮按压），使用 `animateTo()` 方法：

```cpp
#include "core/material/cfmaterial_motion.h"

// 按压时升高
void MyWidget::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    m_elevation->setPressed(true);

    // 使用标准动画规格
    auto spec = core::MotionSpec::standard();
    m_elevation->animateTo(4, spec);
}

// 释放时恢复
void MyWidget::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    m_elevation->setPressed(false);
    m_elevation->animateTo(2, core::MotionSpec::standard());
}
```text

`MotionSpec` 定义了动画的缓动曲线和时长，使用 Material 标准值可确保动画感觉一致。

## 绘制阴影

阴影应该在 `paintEvent` 中最先绘制，这样它会出现在控件背景下方：

```cpp
void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Step 1: 先绘制阴影
    QPainterPath shape = widgetShape();
    m_elevation->paintShadow(&p, shape);

    // Step 2: 绘制背景（会覆盖阴影的主体部分，只露出边缘）
    p.fillPath(shape, backgroundColor());

    // 其他绘制...
}
```text

⚠️ 阴影必须先绘制，否则会覆盖控件内容。绘制顺序错了视觉效果会很奇怪。

## 光源角度

Material Design 假设光源来自左上方（约 15 度），这形成向右下方的阴影。可以通过 `setLightSourceAngle()` 调整：

```cpp
// 默认值（光源来自左上方）
m_elevation->setLightSourceAngle(15.0f);

// 光源来自正上方
m_elevation->setLightSourceAngle(0.0f);

// 光源来自右侧
m_elevation->setLightSourceAngle(-30.0f);
```text

角度正值表示光源从左侧来，阴影向右投射；负值表示光源从右侧来。这个参数影响阴影的水平偏移量。

## 按压效果

按压效果包含两部分：阴影变化和控件位移。控制器提供了 `setPressed()` 和 `pressOffset()` 来支持这个效果：

```cpp
void MyWidget::mousePressEvent(QMouseEvent* event) {
    m_elevation->setPressed(true);
    update();
}

void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);

    // 应用按压位移
    float offset = m_elevation->pressOffset();
    p.translate(0, offset);

    // 正常绘制...
}
```text

按压时阴影会缩小并靠近控件（约 50%），同时控件向下移动，产生"按下"的视觉反馈。

## 暗色主题支持

在暗色主题中，阴影效果不明显，Material Design 使用色调叠加来表示高程：

```cpp
CFColor MdElevationController::tonalOverlay(CFColor surface, CFColor primary) const {
    // 返回混合后的表面颜色
}
```text

使用方式：

```cpp
bool isDark = theme->isDarkTheme();
CFColor surfaceColor = theme->getColor(MdColorRole::Surface);
CFColor primaryColor = theme->getColor(MdColorRole::Primary);

if (isDark) {
    // 暗色主题使用色调叠加
    backgroundColor = m_elevation->tonalOverlay(surfaceColor, primaryColor);
} else {
    // 亮色主题使用阴影
    backgroundColor = surfaceColor;
}
```bash

色调叠加量与高程级别成正比，级别越高叠加越多。

## 高程层级选择

选择合适的高程级别取决于控件在界面中的层级关系：

| 场景 | 推荐级别 |
|------|----------|
| 平面控件（卡片、表单） | 0-1 |
| 悬浮按钮（FAB） | 3 |
| 对话框 | 4 |
| 底部抽屉 | 5 |
| 菜单、下拉框 | 2-3 |

保持相邻层级之间至少相差 1 级，确保视觉层次清晰。

## 性能考虑

阴影渲染涉及多层绘制和透明度混合，在低端设备上可能成为性能瓶颈。如果遇到性能问题：

1. 降低高程级别（Level 1 和 2 的渲染开销较小）
2. 全局禁用动画（阴影计算仍然进行，但没有插值开销）
3. 在低端设备上禁用阴影（通过条件编译或运行时检测）

## 常见问题

阴影看起来"不对劲"通常是因为：

1. 绘制顺序错误——阴影必须在背景之前绘制
2. 控件没有设置 `WA_TranslucentBackground` 属性（某些平台需要）
3. 设备像素比计算错误（高 DPI 屏幕上阴影会模糊）

## 相关文档

- [StateMachine - Material 状态机](../widget/state_machine.md)
- [MdFocusIndicator - 焦点环](./focus_ring.md)
- [Material Design 3 高程规范](https://m3.material.io/styles/elevation)
