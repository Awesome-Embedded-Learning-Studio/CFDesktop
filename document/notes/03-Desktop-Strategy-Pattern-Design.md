---
title: 桌面策略系统设计（Strategy Pattern 实战）
description: 使用 Strategy 模式 + CQRS 原则管理跨平台窗口行为
---

# 桌面策略系统设计（Strategy Pattern 实战） -- 设计意图

## 为什么选择这种方案

窗口行为在不同操作系统上的实现截然不同（Windows 用 `WS_EX_TOPMOST`、X11 用 EWMH `_NET_WM_STATE_*`、Wayland 通过 xdg-shell 协议且禁止客户端控制 z-order）。如果直接在业务代码中堆砌 `#ifdef` 条件分支，会导致代码无法维护、无法测试、且每新增一个平台就需要修改所有分支。

Strategy 模式将这些平台特定的行为实现封装为独立的策略类，通过统一接口 `IDesktopDisplaySizeStrategy` 暴露给上层。Context（如 CFDesktop 主窗口）只依赖抽象接口，不关心具体平台实现。这带来了四个直接好处：(1) 新平台只需新增策略类，符合开闭原则；(2) 每个策略类只负责单一平台的单一行为维度，符合单一职责原则；(3) 策略可通过工厂 + DLL 插件化加载；(4) 单元测试中可用 Mock 策略替换真实实现。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| Strategy 模式封装平台行为 | 消除条件分支地狱、支持开闭原则 | `#ifdef` 全局分支（不可测试、不可扩展） |
| 接口继承链: `IDesktopPropertyStrategy` -> `IDesktopDisplaySizeStrategy` | 基类提供 `StrategyType` 枚举和 `name()` ABI 标识；子类添加 `action()`/`query()` | 扁平单接口（无法区分 DisplaySizePolicy vs Extensions） |
| CQRS: `action(QWidget*) -> bool` + `query() const -> DesktopBehaviors` | Action 有副作用（修改窗口状态）、Query 无副作用（纯读取）；分离后 Query 可缓存、可并发调用 | 单一 `apply()` 方法混合读写（无法安全缓存或并发） |
| `WeakPtrFactory` 集成 | 策略可能被多个组件引用，WeakPtr 避免循环引用和悬挂指针 | 原始指针（悬挂风险）、shared_ptr（循环引用风险） |
| 工厂模式 `PlatformFactory` + 自定义删除器 `StrategyDeleter` | 确保跨 DLL 边界的正确分配/释放；`factorize_unique()` / `factorize_shared()` 满足不同所有权需求 | 直接 `new/delete`（跨 DLL 不安全） |
| 冲突检测: `StayOnTop` vs `StayOnBottom` 互斥、`Fullscreen` vs `AllowResize` 冲突 | 某些行为标志语义上不能共存，需在应用前检测并按优先级自动解决 | 静默允许冲突（运行时未定义行为） |
| Action 方法幂等性要求 | `widget->isFullScreen()` 前置检查避免重复操作触发不必要的窗口重建事件 | 无幂等保证（多次调用可能闪烁或丢失状态） |

## 当前状态

已实现。接口定义位于 `desktop/ui/platform/IDesktopDisplaySizeStrategy.h` 和 `IDesktopPropertyStrategy.h`。平台实现位于 `desktop/ui/platform/windows/`（Windows）和 `desktop/ui/platform/linux_wsl/`（WSL/X11）。工厂位于 `desktop/ui/platform/DesktopPropertyStrategyFactory.{h,cpp}`。
