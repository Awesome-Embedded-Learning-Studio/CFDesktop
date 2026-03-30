# ListView - Material 列表视图

`ListView` 是 Material Design 3 列表视图控件的完整实现，支持单行、双行和三行列表项。包含水波纹效果、状态层、分隔线和焦点指示器。

## 类参考

```cpp
namespace cf::ui::widget::material;

class ListView : public QListView {
    Q_OBJECT
    Q_PROPERTY(int itemHeight READ itemHeight WRITE setItemHeightInt)
    Q_PROPERTY(bool showSeparator READ showSeparator WRITE setShowSeparator)
    Q_PROPERTY(bool rippleEnabled READ rippleEnabled WRITE setRippleEnabled)
};
```

头文件：`ui/widget/material/widget/listview/listview.h`

## 列表项高度

Material Design 3 定义了三种列表项高度：

```cpp
enum class ItemHeight {
    SingleLine = 56,  // 56dp - 单行项目
    TwoLine = 72,     // 72dp - 双行项目
    ThreeLine = 88    // 88dp - 三行项目
};
```

## 基本用法

```cpp
#include "widget/material/widget/listview/listview.h"

using namespace cf::ui::widget::material;

// 创建列表视图
auto* list = new ListView(this);

// 设置列表项高度
list->setItemHeight(ListView::ItemHeight::TwoLine);

// 启用分隔线
list->setShowSeparator(true);

// 启用水波纹效果
list->setRippleEnabled(true);

// 设置模型（使用标准 Qt Model/View 架构）
auto* model = new QStringListModel({"Item 1", "Item 2", "Item 3"}, this);
list->setModel(model);

// 连接信号
connect(list, &QListView::clicked, this, &MyClass::onItemClicked);
```

## 分隔线

```cpp
// 启用分隔线（在列表项之间绘制水平线）
list->setShowSeparator(true);

// 禁用分隔线
list->setShowSeparator(false);
```

分隔线使用 `OutlineVariant` 颜色绘制，遵循 Material Design 的视觉规范。

## 水波纹效果

```cpp
// 启用/禁用水波纹效果（默认启用）
list->setRippleEnabled(true);
```

水波纹在列表项被点击时从点击位置向外扩散，使用 `RippleHelper` 实现。

## 交互状态

列表项有以下交互状态，由内部状态跟踪维护：

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认背景 |
| Hovered | 状态层叠加 |
| Pressed | 水波纹 + 状态层 |
| Selected | 选中背景色叠加 |
| Focused | 焦点环 |

通过 `m_hoveredIndex` 和 `m_pressedIndex` 追踪每个项目的状态。

## 项委托

ListView 使用内部委托（`ListViewDelegate`）控制列表项的大小：

```cpp
// 委托在 .cpp 中定义，自动设置 uniformItemSizes
// 根据 ItemHeight 设置每项的高度
```

## 绘制流程

ListView 的 `paintEvent` 在视口上实现以下绘制步骤：

1. 绘制视口背景（`drawViewportBackground`）
2. 对每个可见项：
   - 绘制项目背景（`drawItemBackground`）
   - 绘制状态层（`drawItemStateLayer`）
   - 绘制水波纹（`drawItemRipple`）
   - 绘制项目内容（`drawItemContent`） - 由模型提供
   - 绘制分隔线（`drawSeparator`）
   - 绘制焦点指示器（`drawFocusIndicator`）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 背景 | Surface |
| 文本 | OnSurface |
| 次要文本 | OnSurfaceVariant |
| 选中背景 | PrimaryContainer (12% 透明度) |
| 选中文本 | OnSurface |
| 分隔线 | OutlineVariant |
| 状态层 | 根据状态计算 |

## 主要方法

| 方法 | 说明 |
|------|------|
| `itemHeight()` / `setItemHeight(ItemHeight)` | 获取/设置列表项高度类型 |
| `showSeparator()` / `setShowSeparator(bool)` | 获取/设置是否显示分隔线 |
| `rippleEnabled()` / `setRippleEnabled(bool)` | 获取/设置水波纹效果 |
| `sizeHint()` | 获取推荐尺寸 |

## 相关文档

- [TreeView - Material 树视图](./treeview.md)
- [TableView - Material 表格视图](./tableview.md)
- [Material Design 3 列表规范](https://m3.material.io/components/lists)
