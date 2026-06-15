---
title: CFDesktop 项目 TODO 看板
description: 本目录包含 CFDesktop 项目各模块的详细 TODO 清单，基于设计文档和架构规范整理而成。
---

# CFDesktop 项目 TODO 看板

本目录包含 CFDesktop 项目各模块的详细 TODO 清单，基于 [`design_stage`](../design_stage/) 设计文档与 [`AGENT.md`](../../AGENT.md) 架构规范整理而成。

## 模块索引

| 模块 | 待办清单 | 预计周期 | 依赖 | 状态 |
|------|---------|---------|------|------|
| 工程骨架 / 硬件探针 / Base 库 | [`done/SUMMARY.md`](done/SUMMARY.md)（归档） | — | — | ✅ 完成 |
| 输入抽象层 | [`base/02_input_layer.md`](base/02_input_layer.md) | 1~2 周 | Phase 0, 1 | ⬜ 待开始 |
| 多平台模拟器 | [`base/03_simulator.md`](base/03_simulator.md) | 2~3 周 | Phase 0, 2 | ⬜ 待开始 |
| 测试体系 | [`base/04_testing.md`](base/04_testing.md) | 贯穿全程 | 所有阶段 | 🚧 进行中 |
| UI Material Framework | [`base/99_ui_material_framework.md`](base/99_ui_material_framework.md) | 持续迭代 | Phase 0-3 | 🚧 进行中 |
| Desktop 模块（显示后端 + 窗口管理） | [`desktop/`](desktop/) | 持续迭代 | Phase 0-6 | 🚧 进行中 |

## 状态图例

- ⬜ **待开始** (Todo) - 尚未开始的任务
- 🚧 **进行中** (In Progress) - 正在开发的任务
- ✅ **已完成** (Done) - 已完成的任务
- ⚠️ **已废弃** (Deprecated) - 不再需要的任务
- 🔄 **阻塞中** (Blocked) - 被依赖阻塞的任务

## 项目状态报告

> 项目进度以 [`../status/current.md`](../status/current.md) 为唯一事实来源。本看板仅维护各模块的待办清单，不再重复声明整体完成度。

已完成阶段的归档见 [`done/SUMMARY.md`](done/SUMMARY.md)。

---

*看板最后更新: 2026-06-15*
