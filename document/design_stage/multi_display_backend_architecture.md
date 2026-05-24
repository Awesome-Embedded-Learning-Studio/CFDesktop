---
title: CFDesktop 多显示后端架构设计
description: "多后端运行时选择、三种显示角色模型、组件复用矩阵的设计意图"
---

# CFDesktop 多显示后端架构设计 -- 设计意图

## 为什么选择这种方案

CFDesktop 需要在五种运行场景中工作：Windows 客户端、Linux 有桌面环境客户端、Linux 无桌面 X11 窗口管理器、Linux 无桌面 Wayland 合成器、嵌入式直接渲染。这些场景对显示后端的需求截然不同 -- 有的只需跟踪外部窗口，有的需要自己充当合成器，有的甚至连窗口系统都没有。

核心设计原则是：**同一套 Shell / Panel / WindowManager 代码在所有场景中复用，通过 `IDisplayServerBackend` 抽象层屏蔽平台差异。** 系统定义了三种运行角色（`Client` / `Compositor` / `DirectRender`），每个后端实现一种角色。运行时通过 `DetectDisplayServerMode()` 自动检测环境（环境变量 -> Wayland/X11 socket -> DRM 设备 -> framebuffer），选择合适的后端实例化。上层代码通过 `capabilities()` 查询能力，对不具备的功能优雅降级。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| 三种运行角色（Client/Compositor/DirectRender） | 覆盖所有已知场景，每种角色的能力边界清晰 | 只区分"有/无桌面环境"（无法处理嵌入式场景） |
| 运行时自动检测后端 | 用户无需配置即可适配当前环境，降低使用门槛 | 编译时硬编码后端（每个平台单独编译，维护成本翻倍） |
| `IDisplayServerBackend` 作为顶层抽象 | 角色检测、能力查询、生命周期管理集中在一个接口，上层只需对接这一层 | 每个平台各自暴露不同接口（上层需要大量条件分支） |
| `BackendCapabilities` 能力查询 | 允许上层在运行时根据实际能力做降级决策（如嵌入式不支持多窗口） | 假设所有能力都可用（在功能受限的嵌入式设备上崩溃） |
| `IShellLayer` 接口解耦 QWidget | Wayland 合成器不继承 QWidget，而是实现纯接口 `IShellLayer` | 强制所有后端都基于 QWidget（Wayland 原生合成器无法实现） |

## 平台角色模型

| 场景 | 角色 | 典型技术 |
|------|------|---------|
| Windows 伪桌面 | Client | QWidget + Win32 API + SetWinEventHook |
| Linux 有桌面环境 | Client | QWidget + X11/Wayland 客户端 |
| Linux 无桌面 (X11) | Compositor | XCB SubstructureRedirect + EWMH + XComposite |
| Linux 无桌面 (Wayland) | Compositor | QtWaylandCompositor + xdg-shell + DRM/KMS |
| Linux 嵌入式 | DirectRender | Qt EGLFS/linuxfb + libdrm + evdev |

## 组件复用矩阵

| 组件 | Client | X11 WM | Wayland | DirectRender |
|------|--------|--------|---------|-------------|
| `IWindow` | 直接复用 | XCB Window 适配 | Surface 适配 | QWidget 子控件适配 |
| `IWindowBackend` | QWidget 实现 | XCB WM 实现 | Wayland 实现 | FB 单窗口实现 |
| `WindowManager` | 直接复用 | 直接复用 | 直接复用 | 直接复用 |
| `PanelManager` | 直接复用 | 直接复用 | 直接复用 | 直接复用 |
| `ShellLayer` | QWidget 实现 | QWidget 实现 | IShellLayer 实现 | QWidget 实现 |
| `IShellLayerStrategy` | 直接复用 | 直接复用 | 需新策略 | 直接复用 |

## 当前状态

部分实现。Windows Client 和 WSL X11 Client 后端已完成；X11 WM、Wayland 合成器、EGLFS/linuxfb 后端规划中。详细接口规格参见 HandBook。
