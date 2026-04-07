# Switch - Material 开关

`Switch` 是 Material Design 3 开关（Toggle）控件的完整实现，带有动画滑块位置、轨道颜色过渡和状态层。提供 CheckBox 的替代方案，适用于二进制开/关设置。

## 类参考

```cpp
namespace cf::ui::widget::material;

class Switch : public QCheckBox {
    Q_OBJECT
};
```

头文件：`ui/widget/material/widget/switch/switch.h`

## 基本用法

```cpp
#include "widget/material/widget/switch/switch.h"

using namespace cf::ui::widget::material;

// 创建不带文本的开关
auto* toggle = new Switch(this);

// 创建带文本的开关
auto* wifiSwitch = new Switch("Wi-Fi", this);
wifiSwitch->setChecked(true);

// 连接信号（与 QCheckBox 兼容）
connect(wifiSwitch, &QCheckBox::toggled, this, &MyClass::onWifiToggled);
```

## 开关尺寸

Switch 遵循 Material Design 3 的尺寸规范：

| 元素 | 尺寸 |
|------|------|
| 轨道宽度 | 52dp |
| 轨道高度 | 32dp |
| 滑块直径 | 选中时更大 |
| 轨道圆角 | 完全圆角（高度的一半） |

## 滑块动画

开关切换时，滑块从一侧滑动到另一侧，有平滑的位移动画：

```cpp
// m_thumbPosition 从 0.0（未选中/左侧）到 1.0（选中/右侧）
// 动画由 CFMaterialAnimationFactory 驱动

toggle->setChecked(true);   // 滑块从左滑到右
toggle->setChecked(false);  // 滑块从右滑到左
```

内部使用 `m_inNextCheckState` 标志防止 `setChecked()` 在状态切换时直接跳过动画。

## 轨道颜色

轨道颜色根据选中状态自动切换：

| 状态 | 轨道颜色 | 滑块颜色 |
|------|----------|----------|
| Unchecked | SurfaceVariant | OnSurface (带 outline) |
| Checked | Primary | OnPrimary |

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认轨道和滑块 |
| Hovered | 状态层叠加 + 滑块扩展 |
| Pressed | 水波纹 + 状态层 |
| Focused | 焦点环 |
| Disabled | 38% 透明度 |

## 与 CheckBox 的区别

| 特性 | CheckBox | Switch |
|------|----------|--------|
| 视觉形式 | 方形勾选框 | 圆形滑块 + 轨道 |
| 适用场景 | 多选列表 | 单项开/关设置 |
| 状态切换 | 勾选/取消勾选 | 滑动动画 |
| 触摸目标 | 48x48dp | 52x32dp |

根据 Material Design 3 的建议，开关适用于即时生效的设置项（如 Wi-Fi 开关），而复选框适用于需要确认的多选项。

## 绘制流程

Switch 的 `paintEvent` 实现以下绘制步骤：

1. 绘制轨道（`drawTrack`） - 带颜色过渡
2. 绘制滑块（`drawThumb`） - 带位置动画
3. 绘制水波纹（`drawRipple`）
4. 绘制文本（`drawText`）
5. 绘制焦点指示器（`drawFocusIndicator`）

## 主要方法

| 方法 | 说明 |
|------|------|
| `setChecked(bool)` | 设置选中状态，触发滑块位移动画 |
| `hitButton(const QPoint&)` | 整个控件区域可点击 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 信号

| 信号 | 说明 |
|------|------|
| 继承自 QCheckBox | `toggled`, `clicked`, `stateChanged` 等 |

## 相关文档

- [CheckBox - Material 复选框](./checkbox.md)
- [MdElevationController - 阴影控制器](../base/elevation_controller.md)
- [Material Design 3 开关规范](https://m3.material.io/components/switch)
