---
title: "Phase 2: Base 库核心详细设计文档"
description: "主题引擎五层流水线、DPI 适配、四层 ConfigStore、异步日志系统的设计意图"
---

# Phase 2: Base 库核心 -- 设计意图

## 为什么选择这种方案

Phase 2 是整个框架的基础设施层，为上层 UI 和桌面环境提供主题、动画、分辨率适配、配置和日志五大核心服务。这些服务必须在设计阶段就考虑嵌入式设备的资源约束——任何模块都不能在低端设备上成为性能瓶颈。

**主题引擎**最终演化为五层流水线架构（Math & Utility -> Theme Engine -> Animation Engine -> Material Behavior -> Widget Adapter）。这一分层并非预先设计，而是在实现过程中发现：颜色运算（CFColor、GeometryHelper）与主题切换逻辑（ThemeManager、token system）职责不同，动画策略（CFMaterialAnimationFactory）与 Material Design 状态机行为（StateMachine、RippleHelper）也应该分离。五层流水线让每一层可以独立测试和替换，Low 档设备可以跳过高层（Layer 4/5）直接使用简化渲染。

**DPI 策略**采用 Android 风格的 dp/sp 单位系统，以 160dpi 为基准密度。选择这套方案而非 Qt 原生的 `QScreen::logicalDotsPerInch()` 直接缩放，是因为嵌入式设备的屏幕参数差异极大（从 7 寸 800x480 到 15 寸 1920x1080），dp/sp 提供了更可控的物理尺寸一致性。DPIManager 支持模拟器注入接口，便于在没有真实屏幕的开发机上调试布局。

**ConfigStore** 采用四层优先级存储（Temp > App > User > System）。Temp 层纯内存，用于运行时临时状态；App 层随应用安装，提供默认配置；User 层存储用户偏好；System 层为系统管理员预留全局策略。四层覆盖而非单一配置文件，是因为嵌入式场景下需要区分「出厂默认」和「用户修改」，且系统管理员可能需要锁死某些策略（如禁用动画以节省电量）。

**Logger** 采用异步 MPSC（多生产者单消费者）队列设计。日志写入是高频操作，如果在主线程同步写文件会严重影响 UI 流畅度。多线程通过 lock-free 队列提交日志消息，独立的后台消费者线程负责格式化和写入多个 Sink（文件、控制台、网络 UDP）。这确保了日志的性能影响始终低于 1%。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| 主题引擎五层流水线 | 各层独立测试、Low 档可跳过高层的 Material 效果 | 单体 ThemeEngine 类（职责过重、难以裁剪） |
| dp/sp 密度无关单位 (160dpi 基准) | 物理尺寸一致性好、嵌入式屏幕参数差异大时仍可控 | 直接使用 Qt DPI 缩放（不同设备表现不一致） |
| ConfigStore 四层覆盖 (Temp/App/User/System) | 区分出厂默认与用户修改、支持管理员锁定策略 | 单一 JSON 配置文件（无法表达优先级和锁定） |
| Logger 异步 MPSC 队列 | 日志写入不阻塞主线程、多线程安全、性能影响 <1% | 同步写文件（主线程卡顿）、spdlog（引入外部依赖） |
| 主题降级由 HWTier 驱动 | Low 档自动禁用阴影/模糊/动画，Mid 档简化效果 | 手动配置每个视觉效果（用户负担重、易遗漏） |

## 当前状态

部分实现 -- ThemeEngine、AnimationManager、ConfigStore、Logger 已完成；DPIManager 尚未实现。UI 层已从 Phase 2 中分离为独立的 `ui/` 模块并完成 95%（P0/P1 widgets）。详见 `document/design_stage/status/`。
