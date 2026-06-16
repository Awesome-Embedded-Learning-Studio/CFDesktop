<!--
  真相源 (Single Source of Truth)：本文件是 CFDesktop 项目进度的唯一事实来源。
  项目源信息（架构 / 构建 / 规范）不在此写死，而是分发到分工明确的文件 —— 见「项目导航」。
  本文件由人工维护；用定性状态而非百分比（很多工程阶段难以精确量化）。
-->
---
title: CFDesktop 当前项目状态
description: CFDesktop 项目进度的唯一事实来源与全局导航。
---

# CFDesktop 当前项目状态

> **校准日期**：2026-06-15　|　**版本**：0.19.0
> **本文件是项目进度的唯一事实来源（single source of truth）。** 其他位置一律指向此处，勿另行手抄。

## 项目导航（各信息去哪看）

| 想了解 | 去看 |
|:---|:---|
| 项目定位 / 对外介绍 | [`README.md`](../../README.md) |
| 架构规则 / 构建命令 / 编码规范 | [`AGENT.md`](../../AGENT.md) |
| 组件 / API 用法（细节真相源） | [`document/HandBook/`](../HandBook/) |
| 脚本工具说明（细节真相源） | [`document/scripts/`](../scripts/) |
| 各 Phase 设计细节 | [`document/design_stage/`](../design_stage/) |
| 各模块待办清单 | [`document/todo/`](../todo/) |
| 已完成阶段历史归档 | [`document/todo/done/SUMMARY.md`](../todo/done/SUMMARY.md) |

## 当前阶段

**一句话**：跑通「看到桌面 → 点图标 → 开应用」最小闭环。

## 进度状态（定性）

### 三层架构

- **base**（`cfbase`）：✅ 完成。硬件探针(CPU/Memory/GPU/Network) + HWTier 分级 + console + 工具库就绪。
- **ui**（`cfui`）：🚧 进行中。MD3 五层 pipeline + P0/P1 控件(19 个)完成；布局 / 手势 / P2-P3 控件待做。
- **desktop**（`CFDesktop_shared`）：🚧 进行中。DAG 初始化 / 4 层 ConfigStore / 异步 Logger / 窗口骨架就绪；Windows / WSL X11 后端完成；Wayland / 嵌入式待做。

### Phase

- Phase 0 / 1（骨架 / 硬件探针含 HWTier）：✅ 完成
- Phase 2（基础库）：🚧 进行中，接近完成
- Phase 3（输入抽象）/ Phase 4（模拟器）：⬜ 待开始
- Phase 6（UI 框架 + 控件）：P0/P1 ✅ · P2/P3 ⬜
- Phase 8（测试）：🚧 进行中

## 下一步路线（最小闭环先行）

1. **MS2 状态栏**（顶部时间 + 系统图标）— ✅ 功能落地（`StatusBar` 实现 + 注册 PanelManager + 主题跟随 + MD3 美化；offscreen 启动通过，待真机视觉确认）
2. **MS3 任务栏**（底部居中图标条 + hover 动画）— 🚧 进行中（最小切片跑通：`CenteredTaskbar` 注册 Bottom 面板 + `TaskbarIcon` 居中图标/hover 放大/自绘 ripple/运行指示器，构建通过；点击反馈先打 log，待接 MS4 真启动）
3. **MS4 应用启动器**（应用网格 + QProcess 启动）— 🚧 进行中（最小启动闭环跑通：`AppLaunchService` 用 `QProcess::startDetached` 启动 taskbar 图标对应应用，点 Files/Browser 可真打开；开始菜单弹窗网格待做）
4. **MS5 窗口管理**（窗口装饰 + 任务栏联动）— 🚧 进行中（追踪+联动切片跑通：`WindowManager` 追踪外部窗口 + `IWindow::pid()` + Taskbar 运行指示器联动；靠 PID 匹配，直接启动(xterm)可靠、间接启动(xdg-open)受限；窗口装饰/操作因 WSL X11 客户端架构不可行，暂跳过）

闭环达成后按需推进：HWTier 策略引擎、CrashHandler、IPC、EGLFS 嵌入式后端、输入抽象层、P2/P3 控件。

## 最近里程碑（git 可证）

- **2026-06**：`refactor: refactor the ui subsystem`；`hwtier system enabled`；文档清理
- **已达成**：Milestone 1「桌面骨架可见」；Phase 0 / 1 / 2 / A(CI) / 6 / G(Widget) / H(显示后端)（详见 [SUMMARY.md](../todo/done/SUMMARY.md)）

## 新人入门

1. [`README.md`](../../README.md) — 项目定位
2. [`AGENT.md`](../../AGENT.md) — 构建命令（configure / fast build / run tests）
3. 本文件「下一步路线」— 取首个任务 MS2 上手
