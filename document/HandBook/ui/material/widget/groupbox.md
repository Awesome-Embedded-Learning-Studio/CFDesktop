# GroupBox - Material 分组框

`GroupBox` 是 Material Design 3 分组框控件的完整实现，具有圆角、可选的海拔阴影和主题感知颜色。提供带有标题的容器，用于将相关控件分组显示。

## 类参考

```cpp
namespace cf::ui::widget::material;

class GroupBox : public QGroupBox {
    Q_OBJECT
    Q_PROPERTY(int elevation READ elevation WRITE setElevation)
    Q_PROPERTY(float cornerRadius READ cornerRadius WRITE setCornerRadius)
    Q_PROPERTY(bool hasBorder READ hasBorder WRITE setHasBorder)
};
```

头文件：`ui/widget/material/widget/groupbox/groupbox.h`

## 基本用法

```cpp
#include "widget/material/widget/groupbox/groupbox.h"

using namespace cf::ui::widget::material;

// 创建带标题的分组框
auto* group = new GroupBox("Personal Information", this);

// 在分组框内添加布局和子控件
auto* layout = new QVBoxLayout(group);
layout->addWidget(new TextField(TextFieldVariant::Outlined, group));
layout->addWidget(new TextField(TextFieldVariant::Outlined, group));

// 创建不带标题的分组框
auto* group2 = new GroupBox(this);
```

## 海拔与阴影

GroupBox 支持海拔级别（0-5），使用 `MdElevationController` 控制阴影渲染：

```cpp
// 设置海拔级别（默认 0 = 无阴影）
group->setElevation(2);

// 海拔级别越高，阴影越明显
group->setElevation(4);
```

海拔级别影响阴影的模糊半径和偏移量，遵循 Material Design 的标准级别定义。

## 圆角控制

```cpp
// 获取当前圆角（默认自动计算，基于 ShapeSmall）
float radius = group->cornerRadius();

// 自定义圆角半径
group->setCornerRadius(12.0f);

// 重置为默认值
group->setCornerRadius(0);
```

## 边框控制

```cpp
// 启用/禁用边框（默认启用）
group->setHasBorder(true);   // 显示边框
group->setHasBorder(false);  // 仅显示阴影（如果 elevation > 0）
```

禁用边框时，分组框仅依靠阴影来区分层级，适合卡片式布局。

## 绘制流程

GroupBox 的 `paintEvent` 实现以下绘制步骤：

1. 计算内容区域和标题遮罩区域
2. 绘制阴影（`drawShadow`） - 如果 elevation > 0
3. 绘制背景（`drawBackground`）
4. 绘制边框（`drawBorder`） - 如果 hasBorder 为 true
5. 绘制标题（`drawTitle`）

标题区域会自动遮罩背景，形成 Material Design 3 的标题嵌入效果。

## 颜色系统

GroupBox 从主题中获取颜色：

- 背景色：`Surface` 颜色角色
- 边框色：`Outline` 颜色角色
- 标题色：`OnSurface` 颜色角色
- 标题字体：从主题获取对应的排版令牌

## 主要方法

| 方法 | 说明 |
|------|------|
| `elevation()` / `setElevation(int)` | 获取/设置海拔级别（0-5） |
| `cornerRadius()` / `setCornerRadius(float)` | 获取/设置圆角半径 |
| `hasBorder()` / `setHasBorder(bool)` | 获取/设置是否显示边框 |
| `sizeHint()` / `minimumSizeHint()` | 获取推荐/最小尺寸 |

## 布局建议

GroupBox 在布局中使用时，注意为标题留出足够的上方间距：

```cpp
auto* mainLayout = new QVBoxLayout(this);
mainLayout->setSpacing(16);

auto* addressGroup = new GroupBox("Address");
auto* addressLayout = new QVBoxLayout(addressGroup);
addressLayout->addWidget(new TextField("Street", TextFieldVariant::Outlined));
addressLayout->addWidget(new TextField("City", TextFieldVariant::Outlined));

mainLayout->addWidget(addressGroup);
```

## 相关文档

- [MdElevationController - 阴影控制器](../../base/elevation_controller.md)
- [Material Design 3 容器规范](https://m3.material.io/components/cards)
