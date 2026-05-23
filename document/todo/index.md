---
title: CFDesktop 项目 TODO 看板
description: 本目录包含 CFDesktop 项目各模块的详细 TODO 清单，基于设计文档和架构规范整理而成。
---

# CFDesktop 项目 TODO 看板

本目录包含 CFDesktop 项目各模块的详细 TODO 清单，基于 `design_stage` 设计文档和 `MaterialRules.md` 架构规范整理而成。

## 模块索引

| TODO 文件 | 模块 | 预计周期 | 依赖 | 状态 |
|----------|------|---------|------|------|
| [done/00_project_skeleton_status.md](done/00_project_skeleton_status.md) | 工程骨架搭建 | 1~2 周 | - | ✅ 100% |
| [done/01_hardware_probe_status.md](done/01_hardware_probe_status.md) | 硬件探针与能力分级 | 2~3 周 | Phase 0 | 🚧 90% |
| ~~[done/02_base_library_status.md](done/02_base_library_status.md)~~ | ~~Base 库核心~~ | ~~3~4 周~~ | Phase 0, 1 | ✅ 100% |
| [02_input_layer.md](base/02_input_layer.md) | 输入抽象层 | 1~2 周 | Phase 0, 1 | ⬜ 0% |
| [03_simulator.md](base/03_simulator.md) | 多平台模拟器 | 2~3 周 | Phase 0, 2 | ⬜ 0% |
| [04_testing.md](base/04_testing.md) | 测试体系 | 贯穿全程 | 所有阶段 | 🚧 55% |
| [99_ui_material_framework.md](base/99_ui_material_framework.md) | UI Material Framework | 持续迭代 | Phase 0-3 | 🚧 95% |
| desktop/ | Desktop 模块 (显示后端+窗口管理) | 持续迭代 | Phase 0-6 | 🚧 90% |

## 状态图例

- ⬜ **待开始** (Todo) - 尚未开始的任务
- 🚧 **进行中** (In Progress) - 正在开发的任务
- ✅ **已完成** (Done) - 已完成的任务
- ⚠️ **已废弃** (Deprecated) - 不再需要的任务
- 🔄 **阻塞中** (Blocked) - 被依赖阻塞的任务

## 项目状态报告

- **综合报告**: [PROJECT_STATUS_REPORT.md](done/PROJECT_STATUS_REPORT.md)
- **整体完成度**: 约 75%
- **显示后端详情**: [done/14_display_backend_status.md](done/14_display_backend_status.md) — Windows + WSL X11 后端已完成

---

*最后更新: 2026-03-30*
