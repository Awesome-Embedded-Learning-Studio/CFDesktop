# TabView - Material 标签页

`TabView` 是 Material Design 3 标签页控件的完整实现，支持标签文本、带滑动动画的选中指示器和标签滚动。包含状态层、焦点指示器和 Material Design 3 样式。

## 类参考

```cpp
namespace cf::ui::widget::material;

class TabView : public QTabWidget {
    Q_OBJECT
    Q_PROPERTY(int tabHeight READ tabHeight WRITE setTabHeight)
    Q_PROPERTY(int tabMinWidth READ tabMinWidth WRITE setTabMinWidth)
    Q_PROPERTY(bool showIndicator READ showIndicator WRITE setShowIndicator)
};
```

头文件：`ui/widget/material/widget/tabview/tabview.h`

## 基本用法

```cpp
#include "widget/material/widget/tabview/tabview.h"

using namespace cf::ui::widget::material;

// 创建标签页视图
auto* tabs = new TabView(this);

// 添加标签页
tabs->addTab(new QWidget(tr("Home")), "Home");
tabs->addTab(new QWidget(tr("Settings")), "Settings");
tabs->addTab(new QWidget(tr("Profile")), "Profile");

// 设置标签高度
tabs->setTabHeight(48);

// 连接信号
connect(tabs, &QTabWidget::currentChanged, this, &MyClass::onTabChanged);
```

## 标签高度和宽度

```cpp
// 设置标签高度（默认遵循 Material Design 3 规范）
tabs->setTabHeight(48);

// 设置标签最小宽度（默认遵循 Material Design 3 规范）
tabs->setTabMinWidth(90);
```

## 选中指示器

```cpp
// 显示选中指示器（默认启用）
tabs->setShowIndicator(true);

// 选中指示器是一个滑动条，在标签之间平滑过渡
// 使用 Primary 颜色绘制
```

指示器在标签切换时通过滑动动画移动到新的选中标签位置。

## 可关闭标签

```cpp
// 设置标签可关闭
tabs->setTabCloseable(0, true);  // 第一个标签可关闭
tabs->setTabCloseable(1, true);  // 第二个标签可关闭

// 连接关闭信号
connect(tabs, &TabView::tabCloseRequested, this, &MyClass::onTabClose);
```

可关闭的标签会显示关闭按钮。

## 内部 TabBar

TabView 使用内部 `MaterialTabBar` 实现标签栏：

```cpp
// MaterialTabBar 是内部实现，不暴露给外部
// 负责标签的绘制、选中指示器动画和滚动
```

## 绘制流程

TabBar 内部实现以下绘制步骤：

1. 绘制标签背景
2. 绘制状态层
3. 绘制标签文本
4. 绘制选中指示器（带滑动动画）
5. 绘制焦点指示器

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 标签背景 | Surface |
| 标签文本（未选中） | OnSurfaceVariant |
| 标签文本（选中） | Primary |
| 选中指示器 | Primary |
| 状态层 | 根据状态计算 |

## 主要方法

| 方法 | 说明 |
|------|------|
| `tabHeight()` / `setTabHeight(int)` | 获取/设置标签高度 |
| `tabMinWidth()` / `setTabMinWidth(int)` | 获取/设置标签最小宽度 |
| `showIndicator()` / `setShowIndicator(bool)` | 获取/设置选中指示器 |
| `setTabCloseable(int, bool)` | 设置标签是否可关闭 |
| `sizeHint()` | 获取推荐尺寸 |
| 继承自 QTabWidget | `addTab()`, `setCurrentIndex()`, `currentWidget()` 等 |

## 信号

| 信号 | 说明 |
|------|------|
| `tabCloseRequested(int)` | 标签关闭按钮被点击时发射 |
| 继承自 QTabWidget | `currentChanged(int)` 等 |

## 相关文档

- [Material Design 3 标签页规范](https://m3.material.io/components/tabs)
