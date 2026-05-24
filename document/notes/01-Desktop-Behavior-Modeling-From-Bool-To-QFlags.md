---
title: 桌面行为建模：从 bool 到 QFlags
description: 用 QFlags<DesktopBehaviorFlag> 替代 struct bool 字段，为窗口行为建模
---

# 桌面行为建模：从 bool 到 QFlags -- 设计意图

## 为什么选择这种方案

CFDesktop 需要描述窗口的多种行为特性（全屏、无边框、置底、可调整大小等）。最初考虑使用 `struct { bool fullscreen; bool frameless; ... }` 的朴素方案，但该方案存在根本缺陷：字段不可通过位运算组合、每次扩展字段都破坏 ABI、内存占用高（N 字节 vs 1-4 字节）、且无法与 Qt 元对象系统集成。

最终选择 `QFlags<DesktopBehaviorFlag>` 作为行为描述的统一类型。QFlags 提供编译时类型安全（防止误将不相关枚举混入）、原生位运算符重载（`|`, `&`, `^`, `~`）、与 QVariant/QMetaObject 的无缝集成，以及 `testFlag()` 等便捷 API。使用 `1 << N` 位偏移赋值确保新标志位可以随时追加而不影响现有值。

行为标志位在概念上被定义为**能力集合（Capability Set）**而非状态集合，与 Strategy 系统配合描述"窗口应该具备什么行为"，而非"窗口当前是什么状态"。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| 使用 `QFlags` + `enum class` (bit-shift 赋值) | 类型安全、Qt 集成、可扩展、位运算组合 | `struct { bool ... }`（不可组合、扩展性差） |
| `DesktopBehaviorFlag` 值: `Fullscreen=0x1, Frameless=0x2, StayOnBottom=0x4, AllowResize=0x8, AvoidSystemUI=0x10` | 精简为实际使用的 5 个标志，移除未实现的 StayOnTop/Transparent/ClickThrough/Modal/Popup/Tool/Splash | 原始草案包含 12+ 标志位（多数无对应实现） |
| 通过 `Q_DECLARE_FLAGS` / `Q_DECLARE_OPERATORS_FOR_FLAGS` 宏声明 | 使 `DesktopBehaviorFlag::X \| DesktopBehaviorFlag::Y` 在全局作用域合法 | 手写 `operator|` 重载（冗余且易遗漏） |
| Flag = Capability Set 语义 | 与 Strategy 系统的 `query()` / `action()` 分离对齐 | Flag = State 语义（状态应由 `query()` 运行时检测） |

## 当前状态

已实现。标志枚举和 QFlags 声明位于 `desktop/ui/platform/IDesktopDisplaySizeStrategy.h`，作为 Strategy 接口的一部分对外暴露。
