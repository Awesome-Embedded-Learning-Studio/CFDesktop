---
title: 桌面行为系统设计：从策略到Window Manager抽象
description: 桌面行为系统的分层架构、行为流转管线、冲突解决与插件化策略的设计意图
---

# 桌面行为系统设计：从策略到Window Manager抽象 -- 设计意图

## 为什么选择这种方案

桌面行为系统需要管理窗口的全屏、无边框、置顶、透明等多种行为，这些行为在不同平台（Windows/X11/Wayland/Embedded）上的支持情况各异，且某些行为之间天然互斥（如 StayOnTop 与 StayOnBottom）。采用分层架构 + 策略模式的核心原因是：将行为的"意图"与"执行"彻底分离，使得应用层代码只描述"想要什么行为"，而由底层各层负责"能不能做、怎么冲突解决、最终怎么映射到平台 API"。

行为流转采用六阶段管线（Strategy -> Query -> Merge -> Resolve -> Filter -> Action -> Validate），每一阶段职责单一、可独立测试。这种管线式设计让冲突检测和平台过滤成为流程的显式步骤而非事后补救，确保行为变更的结果可预测。

插件系统基于 Qt QPluginLoader，策略以动态库形式加载。`IDesktopBehaviorStrategy` 接口让每个插件只暴露 `query()` / `apply()` / `priority()` 三个方法，通过 `StrategyRegistry` 统一注册，避免了插件与核心框架的紧耦合。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| 五层架构（Application / Behavior Abstraction / Strategy / Qt Integration / Platform Abstraction） | 每层职责明确，依赖方向单一，便于跨平台扩展 | 三层架构（将 Qt 集成与平台抽象合并）：平台差异处理会侵入策略层 |
| 行为以 `QFlags<DesktopBehaviorFlag>` 表示 | 位运算天然支持行为的合并与冲突检测，性能优于 set/map | `std::unordered_set<enum>`：每次查询/合并需要遍历，且无法用位运算做互斥检测 |
| 六阶段管线式流转 | 冲突解决和平台过滤成为显式步骤，结果可预测 | 回调链/观察者模式：行为变更的副作用隐式传播，难以追踪和调试 |
| 冲突解决默认使用 `Prioritize` 策略（预定义优先级表） | 自动解决冲突且行为可预测，优先级表可配置 | `Fail` 策略（遇到冲突直接失败）：用户体验差；`RemoveBoth`：丢失合法行为 |
| Query/Modifier 接口分离（ISP） | 只读操作和写操作解耦，便于权限控制和 mock 测试 | 统一 `IDesktopBehavior` 接口：客户端暴露了不需要的修改能力 |
| 插件通过 `QPluginLoader` + `Q_PLUGIN_METADATA` 动态加载 | 支持运行时加载/卸载策略，符合开闭原则 | 静态注册策略：每次新增策略需要重新编译核心框架 |
| 平台能力通过 `PlatformBehaviorFilter` 在管线中显式过滤 | 行为请求先被平台能力裁剪再应用，避免调用不支持的平台 API | 各策略内部判断平台：策略与平台耦合，违反单一职责 |

## 当前状态

架构设计已完成，部分接口已实现。核心接口定义位于 `desktop/` 层，策略注册表与冲突解决器为后续实现重点。
