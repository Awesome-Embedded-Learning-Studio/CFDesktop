---
title: "里程碑总览: 从空白桌面到可见可用"
description: "创建日期: 2026-03-31，目标: 将当前空白桌面推进到用户可见、可交互的状态"
---

# 里程碑总览: 从空白桌面到可见可用

> **创建日期**: 2026-03-31
> **目标**: 将当前空白桌面推进到用户可见、可交互的状态

---

## 里程碑一览

| # | 里程碑 | 状态 | 核心交付 | 预计工作量 |
|---|--------|------|----------|-----------|
| MS1 | [桌面骨架可见](../done/SUMMARY.md) | ✅ | 壁纸/背景 + 正确的面板布局计算 | 1-2 天 |
| MS2 | [状态栏](milestone_02_status_bar.md) | ✅ 功能落地 | 顶部时间+系统图标面板 | 3-5 天 |
| MS3 | [任务栏/导航栏](milestone_03_taskbar.md) | ✅ 最小切片完成 | 底部任务栏 (居中图标) | 5-7 天 |
| MS4 | [应用启动器](milestone_04_app_launcher.md) | 🚧 闭环跑通·网格待做 | 应用网格弹窗 + 外部进程启动 | 5-7 天 |
| MS5 | [窗口管理可见](milestone_05_window_management.md) | 🚧 追踪联动跑通·装饰待做 | 窗口装饰 + 最小化/最大化/关闭 + 任务栏联动 | 7-10 天 |
| MS6 | [小组件+控制中心](milestone_06_widget_control_center.md) | ⬜ 待开始 | 桌面时钟 + 下拉控制面板 + 主题切换 | 7-10 天 |

---

## 依赖关系

```text
MS1: 桌面骨架 (壁纸+布局) ✅ 已完成
 │
 ├──→ MS2: 状态栏 (时间+图标) ✅ 功能落地
 │      │
 │      ├──→ MS3: 任务栏/导航栏 ✅ 最小切片完成
 │      │      │
 │      │      ├──→ MS4: 应用启动器 🚧 闭环跑通(网格待做)
 │      │      │
 │      │      └──→ MS5: 窗口管理可见 🚧 追踪联动跑通(装饰待做)
 │      │
 │      └──→ MS6: 小组件+控制中心 (可与 MS3/4/5 并行) ⬜
```bash

**关键路径 (最快通路)**: MS1 → MS2 → MS3 → MS4 → MS5

---

## 可跳过 / 延后的模块

以下对「可见可用」不构成阻塞：

- ⚠️ Phase 3 输入抽象层 — 可见桌面路线不阻塞（Qt 原生事件够用）；但 **tslib 校准 + 虚拟键盘是 6ULL 真机触摸/中文输入的刚需**，闭环后须做（见 [02_input_layer.md](../base/02_input_layer.md)）
- ❌ Phase 4 多平台模拟器 — 开发环境不需要
- ⚠️ Render Backend 具体实现 — 可见桌面用 Qt QPainter 已够用；但 **6ULL 嵌入式落地硬阻塞**，EGLFS/LinuxFB 后端必须先做（见 [07_render_backend.md](07_render_backend.md)）
- ✅ HWTier 检测/评分/档位覆写**已完成**（仅 CapabilityPolicy 策略引擎延后，demo 路线按 High Tier）
- ❌ CrashHandler / IPC — 不影响可见桌面；但 **IPC 是后续通知/媒体/设置/崩溃的共同硬阻塞**（见 [06_infrastructure.md](06_infrastructure.md)）
- ❌ Wayland Compositor 后端 — WSL X11 已可开发调试
- ❌ P2/P3 高级控件 — 用不到

---

## 已有可复用资产

| 已有模块 | 里程碑 | 用途 |
|----------|--------|------|
| `PanelManager` (100%) | MS1, MS2, MS3 | 注册面板、计算可用区域 |
| `ShellLayer` + `IShellLayer` (接口 100%) | MS1 | 承载桌面内容 |
| `IShellLayerStrategy` (接口 100%) | MS1 | 策略模式控制 Shell 行为 |
| `IStatusBar` (接口已定义) | MS2 | 状态栏接口 |
| `IPanel` (接口 100%) | MS2, MS3 | 面板接口 |
| `WindowManager` (基础 30%) | MS5 | 扩展窗口策略 |
| `IWindow` / `IWindowBackend` (100%) | MS5 | 窗口抽象 |
| Material Button (5 变体) | MS3, MS4, MS5 | 按钮 |
| Material ListView | MS4, MS5 | 列表 |
| Material Slider | MS6 | 滑块 |
| Material Switch | MS6 | 开关 |
| Material ProgressBar | MS1, MS2 | 进度条 |
| ThemeManager + ColorScheme | 全部 | 主题系统 |
| Animation Engine | MS3, MS4 | 动画 |
| SimpleBootWidget | MS1 | 启动画面 |
| WSL X11 / Windows Backend | MS5 | 窗口追踪 |

---

*最后更新: 2026-06-26*
