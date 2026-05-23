---
title: Qt 窗口行为解析：QWidget 到 DesktopBehaviors
description: Qt WindowFlags 到 DesktopBehaviors 的映射关系及跨平台差异
---

# Qt 窗口行为解析：QWidget 到 DesktopBehaviors -- 设计意图

## 为什么选择这种方案

CFDesktop 定义了平台无关的 `DesktopBehaviors` 标志来描述窗口行为意图，但实际执行必须映射到 Qt 的 `QWidget` API。由于 Qt 的窗口状态分散在 `windowFlags()`、`isFullScreen()`、`minimumSize()/maximumSize()` 等多个 API 中，且部分行为（如 AvoidSystemUI）根本没有直接对应的 Qt API，需要通过多条件推断，因此需要一份明确的映射表作为所有平台策略实现的参考契约。

## 关键决策

**核心映射表** -- `DesktopBehaviorFlag` 到 Qt API 的查询方式：

| DesktopBehaviorFlag | Qt API / 状态 | 获取方式 |
|---------------------|---------------|---------|
| **Fullscreen** | `isFullScreen()` | `widget->isFullScreen()` |
| **Frameless** | `Qt::FramelessWindowHint` | `widget->windowFlags() & Qt::FramelessWindowHint` |
| **StayOnBottom** | `Qt::WindowStaysOnBottomHint` | `widget->windowFlags() & Qt::WindowStaysOnBottomHint` |
| **AllowResize** | `minimumSize()` / `maximumSize()` | min/max 尺寸比较推断 |
| **AvoidSystemUI** | 多条件推断 | 全屏 / SplashScreen+ToolTip+Popup 类型 / Tool+Frameless / X11BypassWM |

**跨平台差异** -- 关键限制：

| 行为 | Windows | X11 | Wayland | EGLFS |
|------|---------|-----|---------|-------|
| Fullscreen | 完全支持 | 支持 | 受限 | 支持 |
| Frameless | 完全支持 | 支持 | 需 CSD | 支持 |
| StayOnBottom | 完全支持 | 不稳定 | 不支持 | 不支持 |
| AvoidSystemUI | 完全支持 | 受限 | 严格限制 | 支持 |

## 当前状态

映射逻辑通过各平台的 `IDesktopDisplaySizeStrategy` 实现落地：`desktop/ui/platform/windows/windows_display_size_policy.{h,cpp}`（Windows）和 `desktop/ui/platform/linux_wsl/linux_wsl_display_size_policy.{h,cpp}`（WSL/X11）。
