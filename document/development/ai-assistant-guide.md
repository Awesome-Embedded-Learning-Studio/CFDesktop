---
title: AI 辅助开发指南
description: 本页是通用 AI 协作入口，适用于 Claude Code、Codex 或其他代码助手。私人配置文件
---

# AI 辅助开发指南

本页是通用 AI 协作入口，适用于 Claude Code、Codex 或其他代码助手。私人配置文件如 `CLAUDE.md`、`MEMORY.md`、`.claude/`、`.codex/` 不属于仓库公共基建。

## 优先读取

1. `README.md`
2. `document/status/current.md`
3. `document/development/index.md`
4. `document/design_stage/system_architecture_overview.md`
5. `document/todo/desktop/milestone_00_overview.md`

## 检索习惯

- 优先使用 `rg` 和 `rg --files`。
- 先读最近的 `CMakeLists.txt`，再判断 target 和依赖方向。
- 先确认文件是否已有测试，再新增或修改测试。
- 遇到历史状态报告时，先和 `document/status/current.md` 对照。

## 项目约束

- `base -> ui -> desktop` 是严格单向依赖。
- `base/` 不应包含 UI 或 desktop 概念。
- `ui/` 不应包含 desktop 概念。
- 新公共 API 应补 Doxygen 注释。
- 文档站使用 VitePress，不再使用 MkDocs。

## 安全边界

- 不提交私人 AI 配置。
- 不修改 `out/`、`node_modules/`、VitePress dist/cache 等生成物。
- 不把历史 TODO 当成当前事实源。
- 大规模删除或重排文档前，先确认它是否仍被导航引用。
