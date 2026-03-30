# TextField - Material 文本输入框

`TextField` 是 Material Design 3 文本输入框控件的完整实现，支持填充（Filled）和描边（Outlined）两种视觉变体。包含浮动标签、前缀/后缀图标、字符计数器、帮助/错误文本和密码模式。

## 类参考

```cpp
namespace cf::ui::widget::material;

class TextField : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(TextFieldVariant variant READ variant WRITE setVariant)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QString helperText READ helperText WRITE setHelperText)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(bool showCharacterCounter READ showCharacterCounter WRITE setShowCharacterCounter)
    Q_PROPERTY(bool isFloating READ isFloating NOTIFY floatingChanged)
    Q_PROPERTY(QIcon prefixIcon READ prefixIcon WRITE setPrefixIcon)
    Q_PROPERTY(QIcon suffixIcon READ suffixIcon WRITE setSuffixIcon)
};
```

头文件：`ui/widget/material/widget/textfield/textfield.h`

## 变体

Material Design 3 定义了两种文本框变体：

```cpp
enum class TextFieldVariant {
    Filled,    // 填充变体 - 背景填充色，底部指示线
    Outlined,  // 描边变体 - 圆角边框，无背景填充
};
```

选择哪种变体取决于整体设计风格。Filled 变体适合密集表单，Outlined 变体适合突出显示的场景。

## 基本用法

```cpp
#include "widget/material/widget/textfield/textfield.h"

using namespace cf::ui::widget::material;

// 默认 Filled 变体
auto* field1 = new TextField(this);
field1->setLabel("Username");

// 指定 Outlined 变体
auto* field2 = new TextField("Initial text", TextFieldVariant::Outlined, this);
field2->setLabel("Email");

// 带前缀图标的搜索框
auto* search = new TextField(TextFieldVariant::Outlined, this);
search->setPrefixIcon(QIcon::fromTheme("search"));
search->setLabel("Search");

// 连接信号
connect(field1, &QLineEdit::textChanged, this, &MyClass::onTextChanged);
```

## 浮动标签

标签在获得焦点或有内容时会自动浮动到输入框上方：

```cpp
field->setLabel("Password");

// 标签行为：
// - 空内容 + 无焦点：标签在输入区域内（占位符位置）
// - 有内容或有焦点：标签浮动到输入框上方（小字体）
```

浮动动画通过 `m_floatingProgress`（0.0 到 1.0）控制，由 `CFMaterialAnimationFactory` 驱动平滑过渡。

## 帮助文本与错误文本

```cpp
// 设置帮助文本（始终显示在输入框下方）
field->setHelperText("Must be at least 8 characters");

// 设置错误文本（优先级高于帮助文本）
field->setErrorText("Password is too short");

// 清除错误
field->setErrorText("");  // 恢复显示帮助文本
```

错误状态下，输入框边框/指示线使用 error 颜色。

## 图标

```cpp
// 前缀图标（输入区域左侧）
field->setPrefixIcon(QIcon::fromTheme("person"));

// 后缀图标（输入区域右侧）
field->setSuffixIcon(QIcon::fromTheme("visibility_off"));

// 密码模式（使用 QLineEdit 的内置功能）
field->setEchoMode(QLineEdit::Password);
```

## 字符计数器

```cpp
// 启用字符计数器并设置最大长度
field->setMaxLength(100);
field->setShowCharacterCounter(true);

// 显示格式：当前字符数 / 最大长度
// 例如："42 / 100"
```

字符计数器显示在帮助文本区域的右侧。

## 交互状态

| 状态 | 视觉效果 |
|------|----------|
| Normal | 默认外观 |
| Hovered | 状态层叠加 |
| Focused | 边框高亮 + 浮动标签 |
| Error | Error 颜色边框/指示线 |
| Disabled | 38% 透明度 |

## 绘制流程

TextField 的 `paintEvent` 实现以下绘制步骤：

1. 绘制背景（`drawBackground`） - Filled 变体
2. 绘制描边（`drawOutline`） - Outlined 变体
3. 绘制标签（`drawLabel`） - 带浮动动画
4. 绘制文本（`drawText`）
5. 绘制前缀/后缀图标（`drawPrefixIcon` / `drawSuffixIcon`）
6. 绘制清除按钮（`drawClearButton`）
7. 绘制帮助文本（`drawHelperText`）
8. 绘制字符计数器（`drawCharacterCounter`）
9. 绘制焦点指示器（`drawFocusIndicator`）
10. 绘制水波纹（`drawRipple`）

## 主要方法

| 方法 | 说明 |
|------|------|
| `variant()` / `setVariant(TextFieldVariant)` | 获取/设置变体 |
| `label()` / `setLabel(const QString&)` | 获取/设置标签文本 |
| `helperText()` / `setHelperText(const QString&)` | 获取/设置帮助文本 |
| `errorText()` / `setErrorText(const QString&)` | 获取/设置错误文本 |
| `isFloating()` | 标签是否处于浮动状态 |
| `prefixIcon()` / `setPrefixIcon(const QIcon&)` | 获取/设置前缀图标 |
| `suffixIcon()` / `setSuffixIcon(const QIcon&)` | 获取/设置后缀图标 |
| `showCharacterCounter()` / `setShowCharacterCounter(bool)` | 获取/设置字符计数器 |
| `maxLength()` / `setMaxLength(int)` | 获取/设置最大长度（0=无限制） |

## 信号

| 信号 | 说明 |
|------|------|
| `floatingChanged(bool)` | 标签浮动状态变化时发射 |
| 继承自 QLineEdit | `textChanged`, `returnPressed`, `editingFinished` 等 |

## 相关文档

- [TextArea - Material 多行文本框](./textarea.md)
- [Material Design 3 文本框规范](https://m3.material.io/components/text-fields)
