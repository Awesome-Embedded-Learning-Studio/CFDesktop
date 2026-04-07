# CFDesktop 项目 TODO 看板

## 概述

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

## 里程碑时间线

| 里程碑 | 时间 | 交付物 |
|--------|------|--------|
| M0 | Week 2 | ✅ 工程骨架 + Git Hooks CI/CD |
| M1 | Week 5 | 硬件探针 + 三档能力分级 |
| M2 | Week 9 | Base 库 + 主题引擎 + 输入抽象 |
| M3 | Week 15 | Shell UI 主体可用 |
| M4 | Week 18 | SDK 导出 + 示例应用 |
| M5 | Week 21 | 模拟器可用 |
| M6 | Week 23 | 应用商店基础 + 完整 CI/CD |

## 快速链接

### 按角色查找

- **新手入门**: 从 [done/00_project_skeleton_status.md](done/00_project_skeleton_status.md) 开始
- **基础开发**: [done/01_hardware_probe_status.md](done/01_hardware_probe_status.md)
- **UI 开发**: [99_ui_material_framework.md](base/99_ui_material_framework.md) + [02_input_layer.md](base/02_input_layer.md)
- **调试工具**: [03_simulator.md](base/03_simulator.md)
- **测试工程师**: [04_testing.md](base/04_testing.md)

### 按任务类型查找

- **架构设计**: 各模块文档中的"架构设计"章节
- **API 接口**: 各模块文档中的"类接口设计"章节
- **单元测试**: [04_testing.md](base/04_testing.md) + 各模块文档中的"单元测试"章节
- **性能优化**: 各模块文档中的"性能要求"章节

## 文档同步

本 TODO 目录与以下文档保持同步:
- `../design_stage/` - 详细设计文档
- `../../ui/MaterialRules.md` - UI Material 架构规范
- `../../BLUEPRINT.md` - 项目整体规划

## 更新记录

| 日期 | 变更 | 影响模块 |
|------|------|----------|
| 2026-03-30 | 更新 v0.13.1 进度: WSL X11 Backend Ready, Windows Desktop Backend, 显示后端架构完成 | desktop/, done/ |
| 2026-03-18 | Base库完成，删除02_base_library.md，重新编号 | base/ |
| 2026-03-07 | CI/CD 完成 (Git Hooks 策略) | 工程骨架 |
| 2026-03-05 | 创建 TODO 看板 | 全部 |

---

*最后更新: 2026-03-30*
