# TreeView - Material 树视图

`TreeView` 是 Material Design 3 树视图控件的完整实现，用于层级数据展示。具有展开/折叠动画、树连接线、正确的层级缩进和 Material Design 3 颜色令牌。

## 类参考

```cpp
namespace cf::ui::widget::material;

class TreeView : public QTreeView {
    Q_OBJECT
    Q_PROPERTY(TreeItemHeight itemHeight READ itemHeight WRITE setItemHeight)
    Q_PROPERTY(TreeIndentStyle indentStyle READ indentStyle WRITE setIndentStyle)
    Q_PROPERTY(bool showTreeLines READ showTreeLines WRITE setShowTreeLines)
    Q_PROPERTY(bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated)
};
```

头文件：`ui/widget/material/widget/treeview/treeview.h`

## 项高度模式

```cpp
enum class TreeItemHeight {
    Compact,  // 48dp - 紧凑模式
    Standard  // 56dp - 标准模式（默认）
};
```

## 缩进样式

```cpp
enum class TreeIndentStyle {
    Material, // 56dp 每级 + 引导线
    Classic   // 传统嵌套缩进
};
```

## 基本用法

```cpp
#include "widget/material/widget/treeview/treeview.h"

using namespace cf::ui::widget::material;

// 创建树视图
auto* tree = new TreeView(this);

// 设置标准项高度
tree->setItemHeight(TreeItemHeight::Standard);

// 使用 Material 缩进样式
tree->setIndentStyle(TreeIndentStyle::Material);

// 显示树连接线
tree->setShowTreeLines(true);

// 装饰根节点（显示展开/折叠控件）
tree->setRootIsDecorated(true);

// 设置模型
auto* model = new QFileSystemModel(this);
tree->setModel(model);

// 连接信号
connect(tree, &QTreeView::clicked, this, &MyClass::onItemClicked);
```

## 树连接线

```cpp
// 启用树连接线（显示层级关系）
tree->setShowTreeLines(true);

// 连接线在父节点和子节点之间绘制
// 使用 OutlineVariant 颜色
```

## 展开/折叠

TreeView 使用 `TreeViewItemDelegate` 处理展开/折叠图标的渲染：

```cpp
// drawBranches() 被重写为空实现
// 所有展开/折叠图标由委托渲染，避免与默认 Qt 渲染冲突
```

## 根节点装饰

```cpp
// 装饰根节点（显示展开/折叠箭头）
tree->setRootIsDecorated(true);

// 隐藏根节点装饰
tree->setRootIsDecorated(false);
```

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认背景 |
| Hovered | 状态层叠加 |
| Pressed | 水波纹 + 状态层 |
| Selected | 选中背景色 |
| Focused | 焦点环 |

通过 `QPersistentModelIndex` 追踪悬停和按下的项目索引，确保模型变更后状态仍然正确。

## 项委托

TreeView 使用内部委托（`TreeViewItemDelegate`）渲染每个项目：

- 展开/折叠图标
- 项目文本和图标
- 悬停/选中状态背景
- 水波纹效果

委托遵循 Qt 的 Model/View 架构。

## 绘制流程

TreeView 的 `paintEvent` 实现：

1. 绘制背景
2. 让 Qt 通过委托渲染树项目
3. `drawBranches()` 为空实现（委托处理所有展开/折叠图标）

## 颜色系统

| 元素 | 颜色角色 |
|------|----------|
| 表面 | Surface |
| 文本 | OnSurface |
| 连接线 | OutlineVariant |

## 主要方法

| 方法 | 说明 |
|------|------|
| `itemHeight()` / `setItemHeight(TreeItemHeight)` | 获取/设置项高度模式 |
| `indentStyle()` / `setIndentStyle(TreeIndentStyle)` | 获取/设置缩进样式 |
| `showTreeLines()` / `setShowTreeLines(bool)` | 获取/设置树连接线 |
| `rootIsDecorated()` / `setRootIsDecorated(bool)` | 获取/设置根节点装饰 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 相关文档

- [ListView - Material 列表视图](./listview.md)
- [TableView - Material 表格视图](./tableview.md)
- [Material Design 3 树视图规范](https://m3.material.io/components/lists)
