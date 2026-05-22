---
title: Qt 窗口行为解析：QWidget 到 DesktopBehaviors
description: 1. 引言：为什么需要从 Qt 状态反推行为
---

# Qt 窗口行为解析：QWidget 到 DesktopBehaviors

## 目录

1. [引言：为什么需要从 Qt 状态反推行为](#引言为什么需要从-qt-状态反推行为)
2. [行为与 Qt API 映射表](#行为与-qt-api-映射表)
3. [从 QWidget 查询行为的标准实现](#从-qwidget-查询行为的标准实现)
4. [跨平台行为差异分析](#跨平台行为差异分析)
5. [不可直接获取的行为推断](#不可直接获取的行为推断)
6. [平台特定的行为查询策略](#平台特定的行为查询策略)
7. [实践示例与最佳实践](#实践示例与最佳实践)

---

## 引言：为什么需要从 Qt 状态反推行为

### 问题背景

在桌面应用程序开发中，我们经常面临以下场景：

1. **日志记录需求**：需要记录窗口当前的行为状态用于调试
2. **状态同步**：多个组件需要了解窗口的当前行为配置
3. **动态决策**：根据当前窗口状态决定后续操作
4. **策略融合**：多个策略模块需要基于当前状态进行组合判断

在 CFDesktop 项目中，我们定义了 `DesktopBehaviors` 作为抽象的行为描述层，而 Qt 的 `QWidget` 则是底层实现。如何从 `QWidget` 的当前状态准确反推出对应的 `DesktopBehaviors`，是一个需要深入理解 Qt 窗口系统的问题。

### Qt 窗口系统概述

根据 [Qt 6.11 QWidget 官方文档](https://doc.qt.io/qt-6/qwidget.html)：

> "Window flags are a combination of a type (e.g. Qt::Dialog) and zero or more hints to the window system (e.g. Qt::FramelessWindowHint)."

Qt 的窗口标志（Window Flags）由两部分组成：

1. **窗口类型（Window Type）**：定义窗口的基本性质，只能有一个
2. **窗口提示（Window Hints）**：定制窗口外观和行为的可选标志，可以有多个

```cpp
Qt::WindowFlags flags = Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
```cpp

### 反推行为的挑战

从 Qt 状态反推行为存在以下挑战：

| 挑战 | 描述 | 示例 |
|------|------|------|
| **状态分散** | 同一行为可能由多个 API 共同决定 | `AllowResize` 需要检查 `minimumSize()` 和 `maximumSize()` |
| **平台差异** | 同一标志在不同平台表现不同 | `WindowStaysOnBottomHint` 在 X11 上不稳定 |
| **隐式行为** | 某些行为无法直接查询 | `AvoidSystemUI` 是隐式的，需要推断 |
| **状态动态变化** | 窗口状态可能在运行时改变 | `isFullScreen()` 状态可能与创建时的标志不同 |
| **标志组合冲突** | 某些标志组合会产生意外结果 | `FramelessWindowHint` + `WindowTitleHint` |

---

## 行为与 Qt API 映射表

### 核心映射表

这是 `DesktopBehaviorFlag` 到 Qt API 的完整映射关系：

| DesktopBehaviorFlag | Qt API / 状态 | 获取方式 | 平台兼容性 |
|---------------------|---------------|---------|-----------|
| **Fullscreen** | `isFullScreen()` | `widget->isFullScreen()` | 全平台 |
| **Frameless** | `Qt::FramelessWindowHint` | `widget->windowFlags() & Qt::FramelessWindowHint` | 全平台 |
| **StayOnTop** | `Qt::WindowStaysOnTopHint` | `widget->windowFlags() & Qt::WindowStaysOnTopHint` | 全平台（Wayland 限制） |
| **StayOnBottom** | `Qt::WindowStaysOnBottomHint` | `widget->windowFlags() & Qt::WindowStaysOnBottomHint` | 全平台（X11 限制） |
| **AllowResize** | `minimumSize()` / `maximumSize()` | 尺寸比较 | 全平台 |
| **AvoidSystemUI** | 多种标志组合 | 推断 | 平台相关 |

### 详细 API 映射

#### 1. Fullscreen（全屏模式）

```cpp
// 直接查询 API
bool isFullscreen = widget->isFullScreen();
```text

**注意事项**：
- `isFullScreen()` 返回窗口当前是否处于全屏状态
- 与 `showFullScreen()` / `showNormal()` 相关
- 可能与窗口标志的初始设置不同

**平台差异**：
- **Windows**：完全支持，可以隐藏任务栏
- **X11**：支持，依赖窗口管理器
- **Wayland**：受限，需要 compositor 支持
- **macOS**：支持，有自己的全屏动画

#### 2. Frameless（无边框窗口）

```cpp
// 通过窗口标志查询
Qt::WindowFlags flags = widget->windowFlags();
bool isFrameless = (flags & Qt::FramelessWindowHint) != 0;
```text

**相关标志**：
- `Qt::FramelessWindowHint`：无边框窗口
- `Qt::CustomizeWindowHint`：自定义窗口标题栏
- `Qt::WindowTitleHint`：窗口有标题栏
- `Qt::WindowSystemMenuHint`：窗口有系统菜单

根据 [Window Flags Example 文档](https://doc.qt.io/qt-6/qtwidgets-widgets-windowflags-example.html)：

> "A window flag is either a type or a hint. A type is used to specify various window-system properties for the widget."

**平台差异**：
- **Windows**：完全支持，需要自己实现窗口拖动
- **X11**：支持，但窗口管理器可能添加装饰
- **Wayland**：支持，但需要客户端装饰
- **macOS**：受限，系统可能强制添加某些元素

#### 3. StayOnTop（置顶窗口）

```cpp
bool isStayOnTop = (widget->windowFlags() & Qt::WindowStaysOnTopHint) != 0;
```text

**相关标志**：
- `Qt::WindowStaysOnTopHint`：保持在其他窗口之上
- `Qt::WindowStaysOnBottomHint`：保持在其他窗口之下（与置顶互斥）

**平台差异**：
- **Windows**：完全支持
- **X11**：支持，但依赖窗口管理器
- **Wayland**：**不支持**，这是 Wayland 的安全限制
- **macOS**：支持，但等级系统不同

#### 4. StayOnBottom（置底窗口）

```cpp
bool isStayOnBottom = (widget->windowFlags() & Qt::WindowStaysOnBottomHint) != 0;
```text

**平台差异**：
- **Windows**：完全支持
- **X11**：**不稳定**，很多窗口管理器不完全支持
- **Wayland**：**不支持**
- **macOS**：支持有限

#### 5. AllowResize（允许调整大小）

```cpp
// 通过尺寸约束查询
QSize minSize = widget->minimumSize();
QSize maxSize = widget->maximumSize();
bool isResizable = (minSize.isEmpty() || minSize.width() == 0) &&
                   (maxSize.isEmpty() || maxSize.width() == QWIDGETSIZE_MAX);
```text

**更精确的判断**：

```cpp
bool isResizable(QWidget* widget) {
    QSize minSize = widget->minimumSize();
    QSize maxSize = widget->maximumSize();

    // 如果最小和最大尺寸相等，则不可调整
    if (minSize.isValid() && maxSize.isValid() && minSize == maxSize) {
        return false;
    }

    // 检查宽度和高度是否都可以调整
    bool widthResizable = (minSize.width() == 0 || maxSize.width() == QWIDGETSIZE_MAX ||
                          (maxSize.width() > minSize.width()));
    bool heightResizable = (minSize.height() == 0 || maxSize.height() == QWIDGETSIZE_MAX ||
                           (maxSize.height() > minSize.height()));

    return widthResizable && heightResizable;
}
```text

**相关 API**：
- `setFixedSize()`：设置固定大小
- `setMinimumSize()`：设置最小尺寸
- `setMaximumSize()`：设置最大尺寸
- `sizePolicy()`：尺寸策略

#### 6. AvoidSystemUI（避开系统 UI）

这是一个**推断性**的行为标志，没有直接的 API 可以查询。

**推断方法**：

```cpp
bool hasAvoidSystemUI(QWidget* widget) {
    Qt::WindowFlags flags = widget->windowFlags();

    // 方法 1：检查是否全屏
    if (widget->isFullScreen()) {
        return true;
    }

    // 方法 2：检查是否为 Splash/ToolTip/Popup 类型
    Qt::WindowType type = static_cast<Qt::WindowType>(flags & Qt::WindowType_Mask);
    if (type == Qt::SplashScreen || type == Qt::ToolTip || type == Qt::Popup) {
        return true;
    }

    // 方法 3：检查是否为 Tool 类型 + Frameless
    if (type == Qt::Tool && (flags & Qt::FramelessWindowHint)) {
        return true;
    }

    // 方法 4：检查是否有 X11BypassWindowManagerHint
    if (flags & Qt::X11BypassWindowManagerHint) {
        return true;
    }

    return false;
}
```yaml

---

## 从 QWidget 查询行为的标准实现

### queryFromWidget 函数实现

以下是完整的 `queryFromWidget` 函数实现，用于从 `QWidget` 查询所有行为标志：

```cpp
#include <QWidget>
#include <QGuiApplication>
#include "IDesktopDisplaySizeStrategy.h"

namespace cf::desktop::platform_strategy {

/**
 * @brief 从 QWidget 查询当前的桌面行为
 *
 * 该函数通过检查 QWidget 的各种状态和属性，
 * 推断出当前窗口的 DesktopBehaviors 标志。
 *
 * @param widget 要查询的窗口部件
 * @return DesktopBehaviors 查询到的行为标志组合
 */
DesktopBehaviors queryFromWidget(QWidget* widget) {
    if (!widget) {
        return DesktopBehaviorFlag::None;
    }

    DesktopBehaviors behaviors = DesktopBehaviorFlag::None;
    Qt::WindowFlags flags = widget->windowFlags();

    // 1. 检查全屏状态
    if (widget->isFullScreen()) {
        behaviors |= DesktopBehaviorFlag::Fullscreen;
    }

    // 2. 检查无边框状态
    if (flags & Qt::FramelessWindowHint) {
        behaviors |= DesktopBehaviorFlag::Frameless;
    }

    // 3. 检查置底状态
    // 注意：StayOnBottom 与 StayOnTop 互斥，优先检查 StayOnBottom
    if (flags & Qt::WindowStaysOnBottomHint) {
        behaviors |= DesktopBehaviorFlag::StayOnBottom;
    }

    // 4. 检查是否允许调整大小
    QSize minSize = widget->minimumSize();
    QSize maxSize = widget->maximumSize();

    bool canResizeWidth = (minSize.width() == 0 || maxSize.width() == QWIDGETSIZE_MAX ||
                          (maxSize.width() > minSize.width()));
    bool canResizeHeight = (minSize.height() == 0 || maxSize.height() == QWIDGETSIZE_MAX ||
                           (maxSize.height() > minSize.height()));

    if (canResizeWidth && canResizeHeight) {
        behaviors |= DesktopBehaviorFlag::AllowResize;
    }

    // 5. 推断 AvoidSystemUI
    behaviors |= inferAvoidSystemUI(widget);

    // 6. 平台特定的行为过滤
    behaviors = filterPlatformBehaviors(behaviors);

    return behaviors;
}

/**
 * @brief 推断是否避开系统 UI
 *
 * 避开系统 UI 是一个隐式行为，需要综合多个条件判断。
 */
DesktopBehaviors inferAvoidSystemUI(QWidget* widget) {
    DesktopBehaviors result = DesktopBehaviorFlag::None;
    Qt::WindowFlags flags = widget->windowFlags();

    // 获取窗口类型
    Qt::WindowType type = static_cast<Qt::WindowType>(flags & Qt::WindowType_Mask);

    // 检查全屏状态
    if (widget->isFullScreen()) {
        result |= DesktopBehaviorFlag::AvoidSystemUI;
    }

    // 检查特殊窗口类型
    switch (type) {
        case Qt::SplashScreen:
        case Qt::ToolTip:
        case Qt::Popup:
            result |= DesktopBehaviorFlag::AvoidSystemUI;
            break;
        case Qt::Tool:
            // Tool 类型 + Frameless 通常表示小工具
            if (flags & Qt::FramelessWindowHint) {
                result |= DesktopBehaviorFlag::AvoidSystemUI;
            }
            break;
        default:
            break;
    }

    // 检查 X11 Bypass Window Manager 标志
    if (flags & Qt::X11BypassWindowManagerHint) {
        result |= DesktopBehaviorFlag::AvoidSystemUI;
    }

    return result;
}

/**
 * @brief 过滤平台不支持的行为标志
 *
 * 不同平台对窗口行为标志的支持程度不同，
 * 这个函数根据当前平台过滤掉不支持的标志。
 */
DesktopBehaviors filterPlatformBehaviors(DesktopBehaviors behaviors) {
#ifdef Q_OS_WIN
    // Windows 平台：完全支持所有标志
    return behaviors;
#elif defined(Q_OS_LINUX)
    // Linux 平台：需要区分 X11 和 Wayland
    QString platformName = QGuiApplication::platformName();

    if (platformName == QLatin1String("wayland")) {
        // Wayland 平台限制
        behaviors &= ~DesktopBehaviorFlag::StayOnBottom;  // Wayland 不支持

        // StayOnTop 在 Wayland 上的支持有限
        // 某些 compositor 可能通过 layer-shell 协议支持
        // 但标准 Wayland 不支持
    }
    // X11 平台：大部分支持，但 StayOnBottom 可能不稳定
    return behaviors;
#elif defined(Q_OS_MACOS)
    // macOS 平台：大部分支持，但有特殊限制
    return behaviors;
#else
    // 其他平台：保守处理
    return behaviors;
#endif
}

/**
 * @brief 获取行为的描述字符串（用于调试）
 */
QString behaviorDescription(const DesktopBehaviors& behaviors) {
    QStringList descriptions;

    if (behaviors.testFlag(DesktopBehaviorFlag::Fullscreen)) {
        descriptions << QStringLiteral("Fullscreen");
    }
    if (behaviors.testFlag(DesktopBehaviorFlag::Frameless)) {
        descriptions << QStringLiteral("Frameless");
    }
    if (behaviors.testFlag(DesktopBehaviorFlag::StayOnBottom)) {
        descriptions << QStringLiteral("StayOnBottom");
    }
    if (behaviors.testFlag(DesktopBehaviorFlag::AllowResize)) {
        descriptions << QStringLiteral("AllowResize");
    }
    if (behaviors.testFlag(DesktopBehaviorFlag::AvoidSystemUI)) {
        descriptions << QStringLiteral("AvoidSystemUI");
    }

    return descriptions.isEmpty() ?
           QStringLiteral("None") :
           descriptions.join(QStringLiteral(", "));
}

} // namespace cf::desktop::platform_strategy
```bash

---

## 跨平台行为差异分析

### 平台支持矩阵

| 行为标志 | Windows | X11 | Wayland | macOS | EGLFS | Framebuffer |
|---------|---------|-----|---------|-------|-------|-------------|
| Fullscreen | 完全支持 | 支持 | 受限 | 支持 | 支持 | 支持 |
| Frameless | 完全支持 | 支持 | 需客户端装饰 | 受限 | 支持 | 支持 |
| StayOnTop | 完全支持 | 支持 | **不支持** | 支持 | 不支持 | 不支持 |
| StayOnBottom | 完全支持 | **不稳定** | **不支持** | 有限 | 不支持 | 不支持 |
| AllowResize | 完全支持 | 支持 | 支持 | 支持 | 支持 | 受限 |
| AvoidSystemUI | 完全支持 | 受限 | **严格限制** | 受限 | 支持 | 支持 |

### 平台详细分析

#### Windows 平台

**特点**：Qt 在 Windows 上的原生平台，支持最全面

**完全支持的行为**：
- `Fullscreen`：真正的全屏，可以隐藏任务栏
- `Frameless`：完全无边框，可以创建自定义标题栏
- `StayOnTop`：使用 `HWND_TOPMOST` 实现
- `StayOnBottom`：使用 `HWND_BOTTOM` 实现

**特殊标志**：
- `Qt::MSWindowsFixedSizeDialogHint`：创建固定大小的对话框
- `Qt::WindowSystemMenuHint`：添加系统菜单

**代码示例**：

```cpp
// Windows 平台最佳实践
#ifdef Q_OS_WIN
    // 创建无边框置顶窗口
    Qt::WindowFlags flags = Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
    widget->setWindowFlags(flags);

    // Windows 特有的无边框处理
    // 需要处理 WM_NCHITTEST 消息实现窗口拖动
#endif
```bash

**参考**：[MSDN Window Styles](https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles)

#### X11 平台

**特点**：依赖窗口管理器（Window Manager），行为多样

**支持情况**：
- `Fullscreen`：支持，通过 `_NET_WM_STATE_FULLSCREEN` EWMH 提示
- `Frameless`：支持，通过 `MOTIF_WM_HINTS` 属性
- `StayOnTop`：支持，通过 `_NET_WM_STATE_ABOVE`
- `StayOnBottom`：**不稳定**，通过 `_NET_WM_STATE_BELOW`，但不是所有 WM 都支持

**窗口管理器差异**：

| 窗口管理器 | StayOnBottom | Frameless | 备注 |
|-----------|-------------|-----------|------|
| KWin | 支持 | 支持 | KDE 默认 WM |
| Mutter | 有限支持 | 支持 | GNOME 默认 WM |
| Openbox | 支持 | 支持 | 轻量级 WM |
| i3/sway | 支持 | 支持 | 平铺式 WM |

**X11 特有标志**：
- `Qt::X11BypassWindowManagerHint`：绕过窗口管理器

```cpp
// X11 平台注意事项
#ifdef Q_OS_LINUX
    if (QGuiApplication::platformName() == QLatin1String("xcb")) {
        // X11 特定处理

        // 检查窗口管理器支持
        // StayOnBottom 在某些 WM 上可能不工作
        if (behaviors.testFlag(DesktopBehaviorFlag::StayOnBottom)) {
            qWarning() << "StayOnBottom may not work properly on this window manager";
        }
    }
#endif
```text

#### Wayland 平台

**特点**：安全优先的协议，限制客户端的窗口控制能力

**核心限制**（根据 [Wayland and Qt 文档](https://doc.qt.io/qt-6/wayland-and-qt.html)）：

> "The Wayland protocol is designed with security and isolation in mind, and is strict/conservative about what information and functionality is available to clients."

**不支持的行为**：
- `StayOnTop`：**完全不支持**（标准协议）
- `StayOnBottom`：**完全不支持**
- 窗口位置查询：客户端无法查询自己的窗口位置
- 全局输入捕获：不支持

**受限的行为**：
- `Fullscreen`：需要 compositor 支持，通过 `xdg-shell` 协议
- `Frameless`：需要客户端装饰（CSD），不支持服务端装饰（SSD）

**Wayland 特殊考虑**：

```cpp
// Wayland 平台特殊处理
#ifdef Q_OS_LINUX
    if (QGuiApplication::platformName() == QLatin1String("wayland")) {
        // 移除不支持的标志
        behaviors &= ~DesktopBehaviorFlag::StayOnTop;
        behaviors &= ~DesktopBehaviorFlag::StayOnBottom;

        // Wayland 上的 Frameless 需要 CSD
        if (behaviors.testFlag(DesktopBehaviorFlag::Frameless)) {
            // 需要实现客户端装饰
        }
    }
#endif
```yaml

**Wayland 协议扩展**：
某些 compositor 提供扩展协议：
- `xdg-shell`：标准的 shell 协议
- `layer-shell`：用于面板/覆盖层的协议
- `xdg-decoration`：装饰协商协议

#### EGLFS 平台

**特点**：嵌入式 Linux 平台，无窗口管理器

**支持情况**：
- `Fullscreen`：完全支持（默认行为）
- `Frameless`：支持（默认无装饰）
- `StayOnTop/Bottom`：**不支持**（无窗口管理器）
- `AllowResize`：支持有限

**适用场景**：
- 嵌入式设备
- 信息亭模式
- 数字标牌

#### Framebuffer 平台

**特点**：直接写入帧缓冲，最低级别的平台

**支持情况**：
- `Fullscreen`：完全支持（唯一模式）
- `Frameless`：完全支持（无窗口系统）
- `StayOnTop/Bottom`：**不支持**（无窗口堆栈）
- `AllowResize`：不支持（单一表面）

**适用场景**：
- 极简嵌入式系统
- 启动画面
- 硬件直接访问

---

## 不可直接获取的行为推断

### 问题概述

某些 `DesktopBehaviorFlag` 不能直接从 Qt API 获取，需要通过以下方法推断：

1. **组合条件推断**：多个条件综合判断
2. **状态变化监听**：通过事件推断意图
3. **平台特定 API**：使用原生平台 API
4. **启发式推断**：基于常见模式猜测

### AvoidSystemUI 推断详解

`AvoidSystemUI` 是最复杂的推断性标志，表示窗口希望避开系统 UI 元素（任务栏、Dock 等）。

#### 推断条件

```cpp
bool inferAvoidSystemUI(QWidget* widget) {
    // 条件 1：全屏模式
    if (widget->isFullScreen()) {
        return true;
    }

    Qt::WindowFlags flags = widget->windowFlags();
    Qt::WindowType type = static_cast<Qt::WindowType>(flags & Qt::WindowType_Mask);

    // 条件 2：特殊窗口类型
    if (type == Qt::SplashScreen ||      // 启动画面
        type == Qt::ToolTip ||            // 工具提示
        type == Qt::Popup) {              // 弹出菜单
        return true;
    }

    // 条件 3：Tool + Frameless（小工具模式）
    if (type == Qt::Tool && (flags & Qt::FramelessWindowHint)) {
        return true;
    }

    // 条件 4：X11 Bypass Window Manager
    if (flags & Qt::X11BypassWindowManagerHint) {
        return true;
    }

    // 条件 5：检查位置（如果窗口位于屏幕边缘）
    QRect screenGeometry = widget->screen()->geometry();
    QRect windowGeometry = widget->geometry();

    // 窗口紧贴屏幕边缘，可能是面板/工具栏
    const int edgeThreshold = 10;
    if (windowGeometry.left() <= screenGeometry.left() + edgeThreshold ||
        windowGeometry.top() <= screenGeometry.top() + edgeThreshold ||
        windowGeometry.right() >= screenGeometry.right() - edgeThreshold ||
        windowGeometry.bottom() >= screenGeometry.bottom() - edgeThreshold) {
        return true;
    }

    return false;
}
```text

### AllowResize 推断详解

`AllowResize` 需要检查多个尺寸相关的属性：

```cpp
bool inferAllowResize(QWidget* widget) {
    // 方法 1：检查最小/最大尺寸
    QSize minSize = widget->minimumSize();
    QSize maxSize = widget->maximumSize();

    // 如果设置了固定大小，则不允许调整
    if (minSize.isValid() && maxSize.isValid() && minSize == maxSize) {
        return false;
    }

    // 方法 2：检查 sizePolicy
    QSizePolicy policy = widget->sizePolicy();
    if (policy.horizontalPolicy() == QSizePolicy::Fixed &&
        policy.verticalPolicy() == QSizePolicy::Fixed) {
        return false;
    }

    // 方法 3：检查是否有固定大小的设置
    // 通过 Qt::MSWindowsFixedSizeDialogHint 等标志
    Qt::WindowFlags flags = widget->windowFlags();
    if (flags & Qt::MSWindowsFixedSizeDialogHint) {
        return false;
    }

    // 方法 4：检查全屏状态（全屏时不可调整）
    if (widget->isFullScreen()) {
        return false;
    }

    // 默认允许调整
    return true;
}
```text

### 状态变化监听

对于某些行为，需要监听状态变化来推断：

```cpp
class BehaviorTracker : public QObject {
    Q_OBJECT
public:
    explicit BehaviorTracker(QWidget* parent)
        : QObject(parent), m_widget(parent) {
        // 监听窗口标志变化
        connect(parent, &QWidget::windowTitleChanged,
                this, &BehaviorTracker::onWindowFlagsChanged);
    }

    DesktopBehaviors currentBehaviors() const {
        return m_behaviors;
    }

signals:
    void behaviorsChanged(DesktopBehaviors behaviors);

private slots:
    void onWindowFlagsChanged(const QString&) {
        DesktopBehaviors newBehaviors = queryFromWidget(m_widget);
        if (newBehaviors != m_behaviors) {
            m_behaviors = newBehaviors;
            emit behaviorsChanged(m_behaviors);
        }
    }

private:
    QWidget* m_widget;
    DesktopBehaviors m_behaviors = DesktopBehaviorFlag::None;
};
```yaml

---

## 平台特定的行为查询策略

### 策略模式应用

在 CFDesktop 项目中，使用策略模式处理平台差异：

```cpp
// 平台检测
QString platformName() {
    return QGuiApplication::platformName();
}

// 创建平台特定的策略
std::unique_ptr<IDesktopDisplaySizeStrategy> createPlatformStrategy() {
    QString platform = platformName();

#ifdef Q_OS_WIN
    return std::make_unique<WindowsDisplaySizePolicy>();
#elif defined(Q_OS_LINUX)
    if (platform == QLatin1String("wayland")) {
        return std::make_unique<WaylandDisplaySizePolicy>();
    } else if (platform == QLatin1String("xcb")) {
        return std::make_unique<X11DisplaySizePolicy>();
    } else if (platform.startsWith(QLatin1String("eglfs"))) {
        return std::make_unique<EGLFSDisplaySizePolicy>();
    }
#elif defined(Q_OS_MACOS)
    return std::make_unique<macOSDisplaySizePolicy>();
#endif
    return std::make_unique<DefaultDisplaySizePolicy>();
}
```text

### 平台特定实现

#### Windows 策略

```cpp
class WindowsDisplaySizePolicy : public IDesktopDisplaySizeStrategy {
public:
    DesktopBehaviors query() const override {
        DesktopBehaviors behaviors = DesktopBehaviorFlag::None;

        // Windows 平台完全支持所有标志
        // 可以直接查询

        return behaviors;
    }

    bool action(QWidget* widget) override {
        // Windows 特定的行为应用逻辑
        return true;
    }
};
```text

#### Wayland 策略

```cpp
class WaylandDisplaySizePolicy : public IDesktopDisplaySizeStrategy {
public:
    DesktopBehaviors query() const override {
        DesktopBehaviors behaviors = DesktopBehaviorFlag::None;

        // Wayland 平台限制
        // 自动过滤不支持的标志

        behaviors &= ~DesktopBehaviorFlag::StayOnTop;
        behaviors &= ~DesktopBehaviorFlag::StayOnBottom;

        return behaviors;
    }

    bool action(QWidget* widget) override {
        // Wayland 特定的行为应用逻辑
        // 需要与 compositor 协商

        return true;
    }
};
```yaml

---

## 实践示例与最佳实践

### 完整示例：行为查询工具类

```cpp
/**
 * @file DesktopBehaviorQuery.h
 * @brief 桌面窗口行为查询工具
 */

#pragma once
#include <QWidget>
#include <QScopedPointer>
#include "IDesktopDisplaySizeStrategy.h"

namespace cf::desktop::platform_strategy {

/**
 * @brief 行为查询结果
 */
struct BehaviorQueryResult {
    DesktopBehaviors behaviors;       // 查询到的行为
    QString platformName;             // 平台名称
    QStringList warnings;             // 警告信息
    QStringList unsupported;          // 不支持的标志

    bool isValid() const {
        return behaviors != DesktopBehaviorFlag::None || !unsupported.isEmpty();
    }

    QString toString() const {
        return QStringLiteral("Platform: %1, Behaviors: %2")
            .arg(platformName)
            .arg(behaviorDescription(behaviors));
    }
};

/**
 * @brief 桌面行为查询工具类
 *
 * 提供从 QWidget 查询行为的完整功能，
 * 包括平台检测、行为推断、警告提示等。
 */
class DesktopBehaviorQuery {
public:
    /**
     * @brief 从 widget 查询行为
     *
     * @param widget 要查询的窗口部件
     * @return BehaviorQueryResult 查询结果，包含行为、平台、警告等信息
     */
    static BehaviorQueryResult query(QWidget* widget) {
        BehaviorQueryResult result;

        if (!widget) {
            result.warnings << QStringLiteral("Widget is null");
            return result;
        }

        // 1. 获取平台信息
        result.platformName = QGuiApplication::platformName();

        // 2. 查询基础行为
        result.behaviors = queryFromWidget(widget);

        // 3. 检查平台支持
        checkPlatformSupport(result);

        // 4. 检查行为冲突
        checkBehaviorConflicts(result);

        return result;
    }

private:
    /**
     * @brief 检查平台对行为的支持情况
     */
    static void checkPlatformSupport(BehaviorQueryResult& result) {
        QString platform = result.platformName;

#ifdef Q_OS_LINUX
        if (platform == QLatin1String("wayland")) {
            // Wayland 不支持的标志
            if (result.behaviors.testFlag(DesktopBehaviorFlag::StayOnTop)) {
                result.unsupported << QStringLiteral("StayOnTop");
                result.warnings << QStringLiteral("StayOnTop is not supported on Wayland");
            }
            if (result.behaviors.testFlag(DesktopBehaviorFlag::StayOnBottom)) {
                result.unsupported << QStringLiteral("StayOnBottom");
                result.warnings << QStringLiteral("StayOnBottom is not supported on Wayland");
            }
        } else if (platform == QLatin1String("xcb")) {
            // X11 可能不稳定的标志
            if (result.behaviors.testFlag(DesktopBehaviorFlag::StayOnBottom)) {
                result.warnings << QStringLiteral("StayOnBottom may be unstable on X11");
            }
        }
#endif
    }

    /**
     * @brief 检查行为之间的冲突
     */
    static void checkBehaviorConflicts(BehaviorQueryResult& result) {
        // Fullscreen 和 AllowResize 冲突
        if (result.behaviors.testFlag(DesktopBehaviorFlag::Fullscreen) &&
            result.behaviors.testFlag(DesktopBehaviorFlag::AllowResize)) {
            result.warnings << QStringLiteral("Fullscreen conflicts with AllowResize");
        }

        // StayOnTop 和 StayOnBottom 冲突
        if (result.behaviors.testFlag(DesktopBehaviorFlag::StayOnTop) &&
            result.behaviors.testFlag(DesktopBehaviorFlag::StayOnBottom)) {
            result.warnings << QStringLiteral("StayOnTop conflicts with StayOnBottom");
        }
    }
};

} // namespace cf::desktop::platform_strategy
```text

### 使用示例

```cpp
#include "DesktopBehaviorQuery.h"

void logWidgetBehaviors(QWidget* widget) {
    // 查询行为
    auto result = DesktopBehaviorQuery::query(widget);

    // 输出结果
    qDebug() << "Platform:" << result.platformName;
    qDebug() << "Behaviors:" << behaviorDescription(result.behaviors);

    // 输出警告
    if (!result.warnings.isEmpty()) {
        qWarning() << "Warnings:" << result.warnings;
    }

    // 输出不支持的标志
    if (!result.unsupported.isEmpty()) {
        qDebug() << "Unsupported on this platform:" << result.unsupported;
    }
}
```text

### 最佳实践总结

1. **始终进行平台检测**：在应用任何行为前，检测当前平台
2. **优雅降级**：对于不支持的标志，提供替代方案或警告
3. **监听状态变化**：窗口行为可能在运行时改变
4. **文档化平台限制**：清晰记录每个平台的限制
5. **提供测试工具**：帮助开发者理解不同平台的行为差异

### 调试技巧

```cpp
/**
 * @brief 调试辅助函数：打印窗口的详细信息
 */
void debugWindowInfo(QWidget* widget) {
    qDebug() << "=== Window Debug Info ===";
    qDebug() << "Platform:" << QGuiApplication::platformName();
    qDebug() << "Window flags:" << widget->windowFlags();
    qDebug() << "Is fullscreen:" << widget->isFullScreen();
    qDebug() << "Is visible:" << widget->isVisible();
    qDebug() << "Is top level:" << widget->isWindow();
    qDebug() << "Geometry:" << widget->geometry();
    qDebug() << "Minimum size:" << widget->minimumSize();
    qDebug() << "Maximum size:" << widget->maximumSize();
    qDebug() << "Size policy:" << widget->sizePolicy();
    qDebug() << "========================";
}
```yaml

---

## 总结

### 核心要点

1. **行为反推是必要的**：在日志、同步、决策等场景中需要从 Qt 状态反推行为
2. **API 映射是复杂的**：同一行为可能涉及多个 Qt API
3. **平台差异显著**：不同平台对窗口行为的支持程度不同
4. **某些行为需要推断**：如 `AvoidSystemUI`、`AllowResize` 不能直接查询
5. **策略模式是最佳实践**：使用策略模式处理平台差异

### 参考资源

#### Qt 官方文档

- [QWidget Class | Qt Widgets | Qt 6.11.0](https://doc.qt.io/qt-6/qwidget.html)
- [Window Flags Example | Qt Widgets | Qt 6.11.0](https://doc.qt.io/qt-6/qtwidgets-widgets-windowflags-example.html)
- [Wayland and Qt | Qt 6.11](https://doc.qt.io/qt-6/wayland-and-qt.html)
- [Qt Namespace | Qt Core | Qt 6.11.0](https://doc.qt.io/qt-6/qt.html)
- [QWindow Class | Qt GUI | Qt 6.11.0](https://doc.qt.io/qt-6/qwindow.html)
- [Window and Dialog Widgets](https://doc.qt.io/qt-6/application-windows.html)

#### 相关文档

- [EGLFS - Qt for Linux](https://doc.qt.io/qt-6/embedded-linux.html)
- [LinuxFB Platform Plugin](https://doc.qt.io/qt-6/linuxfb.html)
- [X11 Platform Notes](https://doc.qt.io/qt-6/x11-platform.html)

### 下一步

在下一篇文档中，我们将深入探讨 **桌面策略系统设计**，了解如何使用 Strategy 模式来管理不同平台的行为差异。
