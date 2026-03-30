# Separator - Material 分隔线

`Separator` 是 Material Design 3 分隔线（Divider）控件的完整实现，支持水平和垂直方向，以及全出血（Full-bleed）、内缩（Inset）和中内缩（Middle-inset）三种间距模式。

## 类参考

```cpp
namespace cf::ui::widget::material;

class Separator : public QFrame {
    Q_OBJECT
    Q_PROPERTY(SeparatorMode mode READ mode WRITE setMode)
};
```

头文件：`ui/widget/material/widget/separator/separator.h`

## 间距模式

Material Design 3 定义了三种分隔线间距模式：

```cpp
enum class SeparatorMode {
    FullBleed,  // 贯穿整个宽度/高度
    Inset,      // 两侧各缩进 16dp
    MiddleInset // 仅在起始侧缩进 16dp
};
```

## 基本用法

```cpp
#include "widget/material/widget/separator/separator.h"

using namespace cf::ui::widget::material;

// 水平分隔线（默认全出血模式）
auto* hSep = new Separator(Qt::Horizontal, this);

// 垂直分隔线
auto* vSep = new Separator(Qt::Vertical, this);

// 内缩模式（两侧 16dp 边距）
auto* inset = new Separator(Qt::Horizontal, this);
inset->setMode(SeparatorMode::Inset);

// 中内缩模式（起始侧 16dp 边距）
auto* middleInset = new Separator(Qt::Horizontal, this);
middleInset->setMode(SeparatorMode::MiddleInset);
```

## 方向控制

```cpp
// 设置方向
separator->setOrientation(Qt::Horizontal);  // 水平分隔线
separator->setOrientation(Qt::Vertical);    // 垂直分隔线

// 获取当前方向
Qt::Orientation orient = separator->orientation();
```

## 视觉规格

Separator 遵循 Material Design 3 的视觉规范：

| 属性 | 值 |
|------|------|
| 线条粗细 | 1dp |
| 颜色 | OutlineVariant |
| 透明度 | 12%（注释中提到 40%） |
| 内缩边距 | 16dp |

## 使用场景

分隔线常用于以下场景：

```cpp
// 在列表项之间
auto* layout = new QVBoxLayout();
layout->addWidget(new Label("Section 1"));
layout->addWidget(new Separator(Qt::Horizontal));
layout->addWidget(new Label("Section 2"));

// 在卡片内容之间（使用内缩模式）
auto* cardLayout = new QVBoxLayout();
cardLayout->addWidget(title);
cardLayout->addWidget(new Separator(Qt::Horizontal, SeparatorMode::Inset));
cardLayout->addWidget(content);

// 垂直分隔（侧边栏和内容之间）
auto* hLayout = new QHBoxLayout();
hLayout->addWidget(sidebar);
hLayout->addWidget(new Separator(Qt::Vertical));
hLayout->addWidget(content);
```

## 绘制

Separator 的 `paintEvent` 绘制一条 1dp 的线：

1. 计算分隔线区域（`separatorRect()`）- 根据模式和方向应用缩进
2. 使用 `OutlineVariant` 颜色绘制线条

## 主要方法

| 方法 | 说明 |
|------|------|
| `mode()` / `setMode(SeparatorMode)` | 获取/设置间距模式 |
| `orientation()` / `setOrientation(Qt::Orientation)` | 获取/设置方向 |
| `sizeHint()` | 获取推荐尺寸（1dp 粗细，长度填充父容器） |

## 相关文档

- [ListView - Material 列表视图](./listview.md)（使用分隔线分隔列表项）
- [Material Design 3 分隔线规范](https://m3.material.io/components/dividers)
