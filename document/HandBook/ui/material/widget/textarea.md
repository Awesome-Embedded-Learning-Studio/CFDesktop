# TextArea - Material 多行文本框

`TextArea` 是 Material Design 3 多行文本输入框控件的完整实现，支持填充（Filled）和描边（Outlined）两种变体。包含浮动标签、字符计数器、帮助/错误文本和自动调整高度功能。

## 类参考

```cpp
namespace cf::ui::widget::material;

class TextArea : public QTextEdit {
    Q_OBJECT
    Q_PROPERTY(TextAreaVariant variant READ variant WRITE setVariant)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QString helperText READ helperText WRITE setHelperText)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(bool showCharacterCounter READ showCharacterCounter WRITE setShowCharacterCounter)
    Q_PROPERTY(bool isFloating READ isFloating NOTIFY floatingChanged)
    Q_PROPERTY(int minLines READ minLines WRITE setMinLines)
    Q_PROPERTY(int maxLines READ maxLines WRITE setMaxLines)
};
```

头文件：`ui/widget/material/widget/textarea/textarea.h`

## 变体

```cpp
enum class TextAreaVariant {
    Filled,    // 填充变体 - 背景填充色，底部指示线
    Outlined,  // 描边变体 - 圆角边框，无背景填充
};
```

## 基本用法

```cpp
#include "widget/material/widget/textarea/textarea.h"

using namespace cf::ui::widget::material;

// 创建多行文本框
auto* textarea = new TextArea(TextFieldVariant::Outlined, this);
textarea->setLabel("Description");
textarea->setHelperText("Enter a detailed description");

// 设置可见行数范围
textarea->setMinLines(3);   // 最少显示 3 行
textarea->setMaxLines(8);   // 最多显示 8 行，超出后显示滚动条

// 带字符计数器
textarea->setMaxLength(500);
textarea->setShowCharacterCounter(true);

// 连接信号
connect(textarea, &QTextEdit::textChanged, this, &MyClass::onContentChanged);
```

## 自动调整高度

TextArea 支持根据内容自动调整高度：

```cpp
// 设置最少显示行数（默认 1）
textarea->setMinLines(2);

// 设置最多显示行数（0 = 无限制）
textarea->setMaxLines(6);

// 当内容增加时，文本框会从 minLines 增长到 maxLines
// 超过 maxLines 后，内部会出现滚动条
```

`keyPressEvent` 会在达到 `maxLines` 限制时阻止 Enter 键产生新行。

## 浮动标签

与 TextField 相同，标签在获得焦点或有内容时浮动到文本框上方：

```cpp
textarea->setLabel("Comments");

// 浮动动画由 CFMaterialAnimationFactory 驱动
// floatingProgress 从 0.0（静止）到 1.0（浮动）
```

## 帮助文本与错误文本

```cpp
textarea->setHelperText("Maximum 500 characters");

// 错误文本优先级高于帮助文本
textarea->setErrorText("Content exceeds maximum length");
textarea->setErrorText("");  // 清除错误
```

## 与 TextField 的区别

| 特性 | TextField | TextArea |
|------|-----------|----------|
| 基类 | `QLineEdit` | `QTextEdit` |
| 输入类型 | 单行文本 | 多行文本 |
| 自动调整高度 | 不支持 | 支持（minLines/maxLines） |
| 前缀/后缀图标 | 支持 | 不支持 |
| 清除按钮 | 支持 | 不支持 |

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认外观 |
| Hovered | 状态层叠加 |
| Focused | 边框高亮 + 浮动标签 |
| Error | Error 颜色边框/指示线 |
| Disabled | 38% 透明度 |

## 绘制流程

TextArea 的 `paintEvent` 实现以下绘制步骤：

1. 绘制背景（`drawBackground`）
2. 绘制描边（`drawOutline`）
3. 绘制帮助文本（`drawHelperText`）
4. 绘制字符计数器（`drawCharacterCounter`）
5. 绘制焦点指示器（`drawFocusIndicator`）
6. 绘制水波纹（`drawRipple`）

## 主要方法

| 方法 | 说明 |
|------|------|
| `variant()` / `setVariant(TextAreaVariant)` | 获取/设置变体 |
| `label()` / `setLabel(const QString&)` | 获取/设置标签文本 |
| `helperText()` / `setHelperText(const QString&)` | 获取/设置帮助文本 |
| `errorText()` / `setErrorText(const QString&)` | 获取/设置错误文本 |
| `isFloating()` | 标签是否处于浮动状态 |
| `showCharacterCounter()` / `setShowCharacterCounter(bool)` | 获取/设置字符计数器 |
| `maxLength()` / `setMaxLength(int)` | 获取/设置最大长度（0=无限制） |
| `minLines()` / `setMinLines(int)` | 获取/设置最少可见行数 |
| `maxLines()` / `setMaxLines(int)` | 获取/设置最多可见行数（0=无限制） |

## 信号

| 信号 | 说明 |
|------|------|
| `floatingChanged(bool)` | 标签浮动状态变化时发射 |
| 继承自 QTextEdit | `textChanged` 等 |

## 相关文档

- [TextField - Material 文本输入框](./textfield.md)
- [Material Design 3 文本框规范](https://m3.material.io/components/text-fields)
