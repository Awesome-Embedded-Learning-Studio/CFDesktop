# TableView - Material 表格视图

`TableView` 是 Material Design 3 表格视图控件的完整实现，用于二维数据展示。具有自定义表头渲染、网格线、带水波纹的行选择、排序指示器、列调整大小反馈和交替行颜色。

## 类参考

```cpp
namespace cf::ui::widget::material;

class TableView : public QTableView {
    Q_OBJECT
    Q_PROPERTY(TableRowHeight rowHeight READ rowHeight WRITE setRowHeight)
    Q_PROPERTY(TableGridStyle gridStyle READ gridStyle WRITE setGridStyle)
    Q_PROPERTY(bool showHeader READ showHeader WRITE setShowHeader)
    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors)
    Q_PROPERTY(bool rippleEnabled READ rippleEnabled WRITE setRippleEnabled)
};
```

头文件：`ui/widget/material/widget/tableview/tableview.h`

## 行高度模式

```cpp
enum class TableRowHeight {
    Compact,  // 48dp - 紧凑模式，适合密集数据
    Standard  // 56dp - 标准模式（默认）
};
```

## 网格线样式

```cpp
enum class TableGridStyle {
    None,       // 无网格线
    Horizontal, // 仅水平线
    Vertical,   // 仅垂直线
    Both        // 水平和垂直线（默认）
};
```

## 基本用法

```cpp
#include "widget/material/widget/tableview/tableview.h"

using namespace cf::ui::widget::material;

// 创建表格视图
auto* table = new TableView(this);

// 设置标准行高
table->setRowHeight(TableRowHeight::Standard);

// 设置网格线样式
table->setGridStyle(TableGridStyle::Both);

// 显示表头
table->setShowHeader(true);

// 启用交替行颜色
table->setAlternatingRowColors(true);

// 启用水波纹效果
table->setRippleEnabled(true);

// 设置模型（使用标准 Qt Model/View 架构）
auto* model = new QStandardItemModel(this);
model->setHorizontalHeaderLabels({"Name", "Age", "City"});
// ... 添加数据行
table->setModel(model);

// 连接信号
connect(table, &QTableView::clicked, this, &MyClass::onCellClicked);
```

## 交替行颜色

```cpp
// 启用交替行颜色（SurfaceVariant 5% 透明度）
table->setAlternatingRowColors(true);

// 禁用交替行颜色
table->setAlternatingRowColors(false);
```

交替行颜色使用 `SurfaceVariant` 的 5% 透明度，提供轻微的视觉区分而不影响阅读。

## 行选择

行选择使用 `PrimaryContainer` 颜色（12% 透明度）覆盖：

```cpp
// 选中行有 PrimaryContainer 叠加层
// 水波纹效果从点击位置扩散
// 通过 m_hoveredRow 和 m_pressedRow 追踪状态
```

## 表头

表头使用 48dp 高度，使用 `OnSurface` 颜色绘制：

```cpp
// 显示/隐藏表头
table->setShowHeader(true);
```

表头支持排序指示器和列调整大小的视觉反馈。

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认背景 |
| Hovered | 行状态层叠加 |
| Pressed | 行水波纹 + 状态层 |
| Selected | PrimaryContainer 覆盖（12%） |
| Focused | 焦点环 |

## 绘制流程

TableView 完全重写了默认的表格渲染：

1. 绘制背景和网格线（`drawGridLines`）
2. 绘制行背景（基于 hover/pressed/selected 状态）
3. 绘制内容（由模型和委托提供）
4. 绘制焦点指示器（`drawFocusIndicator`）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 文本 | OnSurface |
| 网格线 | OutlineVariant |
| 选中行背景 | PrimaryContainer |
| 交替行 | SurfaceVariant (5% 透明度) |
| 表头文本 | OnSurface |

## 主要方法

| 方法 | 说明 |
|------|------|
| `rowHeight()` / `setRowHeight(TableRowHeight)` | 获取/设置行高模式 |
| `gridStyle()` / `setGridStyle(TableGridStyle)` | 获取/设置网格线样式 |
| `showHeader()` / `setShowHeader(bool)` | 获取/设置是否显示表头 |
| `alternatingRowColors()` / `setAlternatingRowColors(bool)` | 获取/设置交替行颜色 |
| `rippleEnabled()` / `setRippleEnabled(bool)` | 获取/设置水波纹效果 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [ListView - Material 列表视图](./listview.md)
- [TreeView - Material 树视图](./treeview.md)
- [Material Design 3 数据表规范](https://m3.material.io/components/data-table)
