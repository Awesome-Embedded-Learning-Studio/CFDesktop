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

> **校准日期**：2026-06-26　|　**版本**：0.19.0
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
2. **MS3 任务栏**（底部居中图标条 + hover 动画）— ✅ 最小切片完成（`CenteredTaskbar` 注册 Bottom 面板 + `TaskbarIcon` 居中图标/hover 放大/自绘 ripple/运行指示器；`appClicked` 已接 `AppLaunchService::launch` 真启动并记 PID，运行指示器经 `WindowManager` 窗口追踪联动——见 `desktop/ui/CFDesktopEntity.cpp:133-180`）
3. **MS4 应用启动器**（应用网格 + QProcess 启动）— 🚧 进行中（最小启动闭环跑通：`AppLaunchService` 用 `QProcess::startDetached` 启动 taskbar 图标对应应用，点 Files/Browser 可真打开；开始菜单弹窗网格待做）
4. **MS5 窗口管理**（窗口装饰 + 任务栏联动）— 🚧 进行中（追踪+联动切片跑通：`WindowManager` 追踪外部窗口 + `IWindow::pid()` + Taskbar 运行指示器联动；靠 PID 匹配，直接启动(xterm)可靠、间接启动(xdg-open)受限；**窗口装饰已定策→overlay 渲染**〔策略 A：CFDesktop 自绘 overlay 层呈现标题栏+控制按钮作纯视觉指示，因 WSL X11 客户端模式下外部窗口由 XWayland 管理无法直接装饰；X11 WM〔B〕/ Wayland Compositor〔C〕延后到 EGLFS/Wayland 后端，详见 [milestone_05](../todo/desktop/milestone_05_window_management.md):157-160〕）

闭环达成后按需推进：CrashHandler、IPC、EGLFS 嵌入式后端、输入抽象层、P2/P3 控件。

> **HWTier 优先级决策（2026-06-26）**：检测 + 评分 + 策略覆写**已完成**（Phase 1，`base/system/hardware_tier/`：`IHardwareCollector / Scorer / Assessor / Policy` 全套就绪）。故 [`summary.md`](../todo/desktop/summary.md) 与 [`06_infrastructure.md`](../todo/desktop/06_infrastructure.md) 里「HWTier 0% 🔴、上线前必做」的表述**已过时**——那针对的是 embedded/production 上线。当前 demo / 可见桌面路线：**CapabilityPolicy 策略引擎**（把档位转成动效/渲染/内存降级策略并接入 Shell）**延后**，开发期一律按 High Tier；待 embedded pivot 再接。

## 最近里程碑（git 可证）

- **2026-06**：`refactor: refactor the ui subsystem`；`hwtier system enabled`；文档清理
- **2026-06（壁纸资源包发现）**：壁纸层支持运行时动态发现第三方资源包——新增 `Wallpapers` PathType（`<desktop_active_root>/Wallpapers/`）+ `filter_target_recursive` 递归扫描；`make_layer()` 改为「首个有图的源胜出」（config → Pictures 平铺，空则回退 Wallpapers 递归）。CF-Gallery 等包安装到 `Wallpapers/<pack>/` 即被发现，**CFDesktop 零编译期耦合**（无 submodule/无 `#ifdef`/无 CMake option）。详见 [aels_cross_repo_deps.md](../todo/desktop/aels_cross_repo_deps.md)。动画轮播引擎（Gradient 交叉淡入 / Movement 平移 + QTimer 定时 + Sequential/Random 选择器）**已落地**（2026-06-30,`feat/wallpaper-animation-engine`）——新增 `WallPaperEngine` + `TransitionComposer`,strategy 过渡状态机 + 逐帧 QImage 合成,后端无关;6 个 `switch_*` 配置 key + 顺手接 `scaling`/`background_color`;16 例单测全过。详见 [wallpaper_animation_engine.md](../todo/desktop/wallpaper_animation_engine.md) 末尾「实施记录」。
- **2026-06（首个独立 App:Calculator）**：移植 CCIMXDesktop `Caculator` 为**独立可执行**(`apps/calculator/`)—— parser(递归下降 AST)保留 QString 建 `cfdesktop_calculator_parser` lib(53 例单测);UI cfui MD3(`Button` 网格 + `Label`)重写;CFDesktop 经 `AppLaunchService::launch`(QProcess)启动(隔离进程),`AppLaunchService` 加 `applicationDirPath()` 解析(自家 app 同 `bin/` 优先)。确立「工具型 App → 独立可执行」范式(展示型 about 仍 builtin)。
- **2026-06（App 发现机制）**：`AppDiscoverer` 扫描 `<bin>/../apps/<id>/app.json` 自动发现注册 app(manifest:app_id/display_name/icon/exec);`loadAppsConfig` fallback 链 **discover → apps.json → defaultApps**;calculator 改首个 manifest app(包自包含 `apps/calculator/{calculator, app.json}`)。App 即插即用,无需 recompile。
- **已达成**：Milestone 1「桌面骨架可见」；Phase 0 / 1 / 2 / A(CI) / 6 / G(Widget) / H(显示后端)（详见 [SUMMARY.md](../todo/done/SUMMARY.md)）

## 新人入门

1. [`README.md`](../../README.md) — 项目定位
2. [`AGENT.md`](../../AGENT.md) — 构建命令（configure / fast build / run tests）
3. 本文件「下一步路线」— 取首个任务 MS2 上手
