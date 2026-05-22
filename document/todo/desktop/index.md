---
title: desktop
description: 桌面本体 (Desktop Shell) 开发规划
---

# desktop

> 桌面本体 (Desktop Shell) 开发规划

## Overview

本目录包含 CFDesktop 桌面本体（Desktop Shell）的开发规划和 TODO 文档。

## 模块索引

| 文件 | 阶段 | 状态 |
|------|------|------|
| [summary.md](summary.md) | 总体规划文档 | 📋 参考 |
| [06_infrastructure.md](06_infrastructure.md) | Phase A: 基础设施补全 | 🚧 ~50% |
| [07_render_backend.md](07_render_backend.md) | Phase B: 渲染后端抽象 | 🚧 接口完成 |
| [08_p1_controls.md](08_p1_controls.md) | Phase C: P1 控件 | ✅ 100% |
| [09_window_manager.md](09_window_manager.md) | Phase D: 窗口管理器 | 🚧 部分完成 |
| [10_shell_navigation.md](10_shell_navigation.md) | Phase E: Shell 导航 | ⬜ 待开始 |
| [11_notification_control.md](11_notification_control.md) | Phase F: 通知+控制中心 | ⬜ 待开始 |
| [12_theme_settings_lockscreen.md](12_theme_settings_lockscreen.md) | Phase G-I: 主题+设置+锁屏 | ⬜ 待开始 |
| [13_widget_apps.md](13_widget_apps.md) | Phase J-L: Widget+壁纸+文件管理+媒体 | ⬜ 待开始 |

## 可见化里程碑 (从空白到可用)

> 以最小路径推进桌面从空白到可见可用，每步有明确的交付物和验收标准。
> 详见 [milestone_00_overview.md](milestone_00_overview.md) 总览。

| 文件 | 里程碑 | 核心交付 | 状态 |
|------|--------|----------|------|
| [milestone_00_overview.md](milestone_00_overview.md) | 总览 | 依赖关系 + 可复用资产 + 可跳过模块 | 📋 参考 |
| [milestone_01_desktop_skeleton.md](milestone_01_desktop_skeleton.md) | MS1: 桌面骨架 | 壁纸/背景 + 面板布局 | ⬜ 待开始 |
| [milestone_02_status_bar.md](milestone_02_status_bar.md) | MS2: 状态栏 | 顶部时间+系统图标 | ⬜ 待开始 |
| [milestone_03_taskbar.md](milestone_03_taskbar.md) | MS3: 任务栏 | 底部居中图标+hover动画 | ⬜ 待开始 |
| [milestone_04_app_launcher.md](milestone_04_app_launcher.md) | MS4: 应用启动器 | 弹出应用网格+进程启动 | ⬜ 待开始 |
| [milestone_05_window_management.md](milestone_05_window_management.md) | MS5: 窗口管理 | 窗口装饰+状态管理+任务栏联动 | ⬜ 待开始 |
| [milestone_06_widget_control_center.md](milestone_06_widget_control_center.md) | MS6: 小组件+控制中心 | 时钟组件+下拉控制面板+主题切换 | ⬜ 待开始 |

**关键路径**: MS1 → MS2 → MS3 → MS4 → MS5 (MS6 可与 MS3-5 并行)

## 已完成归档

详见 [../done/](../done/) 目录，特别是:
- [../done/14_display_backend_status.md](../done/14_display_backend_status.md) — 显示后端完成状态
- [../done/13_widget_apps_status.md](../done/13_widget_apps_status.md) — Widget + 控件完成状态
- [../done/06_infrastructure_status.md](../done/06_infrastructure_status.md) — 基础设施完成状态

---

*Last updated: 2026-03-31*
