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

> **校准日期**：2026-07-05　|　**版本**：0.19.0
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
3. **MS4 应用启动器**（网格 + 搜索 + QProcess + 双态）— 🚧 进行中（双态框架 + 网格 + 搜索框落地:`LaunchKind`/`IBuiltinPanel`/`BuiltinPanelRegistry` 把 `builtin:` hack 正式化、消灭 if 链,calculator 双态,`HardwareTier::prefer_inprocess_apps` 按 Low/High 裁决;`AppLauncher` 网格 + 搜索 QLineEdit 实时过滤;`DesktopEntryIndex` 扫 XDG `.desktop`(firefox 等);Noter 移植成第二个独立 App。入场/退场动画已完成(`enter_fade_`/`enter_slide_` + `exit_fade_`/`exit_slide_` 对称实现)。详见 [milestone_04](../todo/desktop/milestone_04_app_launcher.md)）
4. **MS5 窗口管理**（窗口状态机 + 任务栏联动）— 🚧 进行中（追踪+联动切片跑通：`WindowManager` 追踪外部窗口 + `IWindow::pid()` + Taskbar 运行指示器联动；靠 PID 匹配，直接启动(xterm)可靠、间接启动(xdg-open)受限。**2026-07-05 状态机 + 联动 + FloatingPolicy 落地**：`WindowState` 状态机 (`isValidTransition` + `applyState`)、`IWindow` 加 `minimize/maximize/restore` 非纯虚默认 no-op（Windows 三态全实现 `ShowWindow`；WSL X11 minimize+restore 走 ICCCM `WM_CHANGE_STATE` ClientMessage、maximize 留 no-op 因 XWayland EWMH 不可靠）、WindowManager 查询 (`getWindowInfo`/`getAllWindowInfos`/`findWindowByPid`) + `windowStateChanged`/`windowInfoUpdated` 信号、Taskbar 点击运行图标走 raise/minimize toggle（Normal→minimize、Minimized→restore+raise、其他→仅 raise，不再二次启动）、`FloatingPolicy` 新窗口初始位置（居中+24px 级联+缩进 work area）、WindowInfo 补 `icon_hint`/`z_index`/`is_always_on_top`/`created_at`；FloatingPolicy 13 例 + WindowManager 14 例(FakeWindow+QSignalSpy)单测全绿。**装饰 overlay (策略 A) DEFERRED 到 EGLFS/Wayland-compositor 阶段**——WSL X11/Win32 外部窗口自带原生标题栏、overlay 重影；EGLFS 后端将原生持有装饰权。详见 [milestone_05](../todo/desktop/milestone_05_window_management.md)）

闭环达成后按需推进：CrashHandler（Phase1 ✅ 信号捕获 + async-signal-safe 报告落盘，`base/crash_handler/` cfcrash + `CrashHandlerStage` + 4 单测，详见 [06_infrastructure.md](../todo/desktop/06_infrastructure.md)）、IPC（✅ single-instance raise + IPCMessage/ServiceLocator 框架）、EGLFS 嵌入式后端、输入抽象层、P2/P3 控件。

> **HWTier 优先级决策（2026-06-26）**：检测 + 评分 + 策略覆写**已完成**（Phase 1，`base/system/hardware_tier/`：`IHardwareCollector / Scorer / Assessor / Policy` 全套就绪）。故 [`summary.md`](../todo/desktop/summary.md) 与 [`06_infrastructure.md`](../todo/desktop/06_infrastructure.md) 里「HWTier 0% 🔴、上线前必做」的表述**已过时**——那针对的是 embedded/production 上线。当前 demo / 可见桌面路线：**CapabilityPolicy 策略引擎**（把档位转成动效/渲染/内存降级策略并接入 Shell）**延后**，开发期一律按 High Tier；待 embedded pivot 再接。

## 最近里程碑（git 可证）

- **2026-06**：`refactor: refactor the ui subsystem`；`hwtier system enabled`；文档清理
- **2026-06（壁纸资源包发现）**：壁纸层支持运行时动态发现第三方资源包——新增 `Wallpapers` PathType（`<desktop_active_root>/Wallpapers/`）+ `filter_target_recursive` 递归扫描；`make_layer()` 改为「首个有图的源胜出」（config → Pictures 平铺，空则回退 Wallpapers 递归）。CF-Gallery 等包安装到 `Wallpapers/<pack>/` 即被发现，**CFDesktop 零编译期耦合**（无 submodule/无 `#ifdef`/无 CMake option）。详见 [aels_cross_repo_deps.md](../todo/desktop/aels_cross_repo_deps.md)。动画轮播引擎（Gradient 交叉淡入 / Movement 平移 + QTimer 定时 + Sequential/Random 选择器）**已落地**（2026-06-30,`feat/wallpaper-animation-engine`）——新增 `WallPaperEngine` + `TransitionComposer`,strategy 过渡状态机 + 逐帧 QImage 合成,后端无关;6 个 `switch_*` 配置 key + 顺手接 `scaling`/`background_color`;16 例单测全过。详见 [wallpaper_animation_engine.md](../todo/desktop/wallpaper_animation_engine.md) 末尾「实施记录」。
- **2026-06（首个独立 App:Calculator）**：移植 CCIMXDesktop `Caculator` 为**独立可执行**(`apps/calculator/`)—— parser(递归下降 AST)保留 QString 建 `cfdesktop_calculator_parser` lib(53 例单测);UI cfui MD3(`Button` 网格 + `Label`)重写;CFDesktop 经 `AppLaunchService::launch`(QProcess)启动(隔离进程),`AppLaunchService` 加 `applicationDirPath()` 解析(自家 app 同 `bin/` 优先)。确立「工具型 App → 独立可执行」范式(展示型 about 仍 builtin)。
- **2026-06（App 发现机制）**：`AppDiscoverer` 扫描 `<bin>/../apps/<id>/app.json` 自动发现注册 app(manifest:app_id/display_name/icon/exec);`loadAppsConfig` fallback 链 **discover → apps.json → defaultApps**;calculator 改首个 manifest app(包自包含 `apps/calculator/{calculator, app.json}`)。App 即插即用,无需 recompile。
- **2026-07（双态框架正式化）**：独立进程 + 进程内 builtin 两条腿**正式化**——`LaunchKind{Auto,DetachedProcess,BuiltinPanel}` 替代 `builtin:` 字符串 hack;`IBuiltinPanel` 接口 + `BuiltinPanelRegistry`(map-based 自写,aex 无 named 变体)消灭 `if(id==about)` 链;`loadAppsConfig` 合并 builtin+discovered 两源,`HardwareTierCapabilities::prefer_inprocess_apps`(Low=true)把 manifest `launch_kind:"auto"` 按 tier 裁决(Low→builtin,High→detached,无 builtin 实现则降级 detached+记日志);calculator 双态验证(`CalculatorBuiltinPanel` 组合适配器,同一份 panel 源码两种加载);补 AboutPanel 缺失入口(原 `builtin:about` 点不出)。架构债:desktop 引用 apps/calculator 源文件,待迁中立层([milestone_07](../todo/desktop/milestone_07_app_ecosystem.md))。
- **2026-07（MS4 搜索 + .desktop + Noter 移植）**:`DesktopEntryIndex` 扫 `~/.local/share/applications` + `/usr/share/applications` 的 `.desktop`(freedesktop:Type=Application && !NoDisplay,Exec 清理 `%` 占位符)→ 进网格作 DetachedProcess;`AppLauncher` 加搜索 QLineEdit,`textChanged` 实时按 display_name 模糊过滤;移植 CCIMXNoter → `apps/noter/`(QuarkWidgets MD3 Button 重写工具栏 + QTextEdit 编辑,manifest `launch_kind:auto`),第二个独立 App 验证移植范式可复制。MS4 launcher 闭环(网格+搜索+启动),仅余入场动画。
- **2026-07（并发迁移 3 App）**:SystemState/AlarmyClock/CCCalendar 三 App 用 Agent **并行迁移**(参照 noter 范式,验证批量可复制):**SystemState**(`apps/system_state/`)复用 `cfbase` 的 CPU/memory probe(`getCPUProfileInfo`/`getCPUBonusInfo`/`getSystemMemoryInfo`)+ QTimer 刷新,展示 base 层能力;**AlarmyClock**(`apps/alarm_clock/`)1s 轮询 + QSpinBox 编辑器 + QListWidget 已设列表 + QMessageBox 响铃(音频 TODO);**CCCalendar**(`apps/calendar/`)`QCalendarWidget` + 日期笔记(内存 QMap,持久化 TODO)。集成时修:alarm_clock 的 `Q_DECLARE_METATYPE` 移到全局命名空间(moc 要求)、calendar 的 Qt6 API(`setVerticalGridLineVisible` 不存在、`DefaultLocaleLongDate` Qt6 删除→`Qt::TextDate`)。三 App 全 build 绿 + Doxygen 过。
- **2026-07（App 抽离到 CFDeskit + 发现机制重构）**：5 个独立 app(calculator/noter/alarm_clock/calendar/system_state)抽到独立仓 [CFDeskit](https://github.com/Awesome-Embedded-Learning-Studio/CFDeskit);主仓 `apps/` 源码全删(PR #22),改**运行时部署模式**——app 由 CFDeskit build + install 到 `<active_root>/apps/<id>/`,`AppDiscoverer` 改扫 `active_root/apps`(`Apps` PathType 小写,原 `<bin>/../apps/` 已弃),空目录 → `log_info` 不报错。system_state **vendoring cfbase probe**(STATIC `mini_system_probe` lib,零 `libcfbase.so` 依赖,完全独立演化)。**ABI 部署隔离**:app rpath `$ORIGIN/..` 找 `apps/libquarkwidgets.so`(独立于 desktop `bin/` 那份),QuarkWidgets 加版本导出(PR #2 `quarkwidgets_runtime_abi_version`)+ CFDeskit `abi_check.hpp` 启动自检(mismatch `qFatal`)。源码开发停放 `third_party/apps/`(gitignore,clone 任意 app 仓一视同仁)。calculator parser 单测随源码迁 CFDeskit(15 TEST 全过)。主仓零 app 代码,desktop 本体纯净。注:`LoadKind`/`IBuiltinPanel`/`BuiltinPanelRegistry` 双态机制保留(builtin 仅 about)。
- **2026-07（主仓重组：cfbase + desktop/base 合并 + desktop/ 平铺）**：删 `desktop/` 包装层——`base/`(cfbase) + 原 `desktop/base/`(logger/config/path/filesystem/fundamental/ascii_art) 合并成新顶层 `base/`(cfbase 子目录化,desktop/base 6 模块同居);`desktop/ui` → `ui/`、`desktop/main` → `main/`(升顶层);`desktop/export.h` + `desktop/main.cpp` → 顶层;`CFDesktop_shared` 组装搬到顶层 CMakeLists。源码**零改动**(内部全相对 include,平移后路径不变,实测 `#include "desktop/"` = 0);CMake 重组(base/ 合并 + 顶层 `add_subdirectory(ui/main)` + CFDesktop_shared + exe,补 `enable_testing()` 顶层)。三层架构图 `base → ui → desktop` 改 `base → ui → main`。cfbase 名副其实(真正的 base 层,含探针 + 桌面基础设施)。验证:build 全绿 + ctest 全过 + desktop 启动正常。
- **2026-07（MS5 窗口状态机 + 任务栏 raise/minimize + FloatingPolicy）**：补齐 milestone_05 除装饰外的全部验收项。`WindowState` 状态机(`isValidTransition`+`applyState`,Normal↔Minimized/Maximized、Closed 终态)+ WindowManager 查询/信号(`getWindowInfo`/`getAllWindowInfos`/`findWindowByPid` + `windowStateChanged`/`windowInfoUpdated`);`IWindow` 加 `minimize/maximize/restore` 非纯虚默认 no-op——Windows 三态全实现(`ShowWindow`),WSL X11 实现 minimize+restore(ICCCM 4.1.4 `WM_CHANGE_STATE` ClientMessage → root + SubstructureRedirectMask,即 `XIconifyWindow`/xdotool 路径)、maximize 留默认 no-op(XWayland 下 EWMH `_NET_WM_STATE` 不可靠);`onWindowCame` 同步填 `windows_` map(原来只填 `window_infos_` 致 `find_window` 对外部窗口失效) + destroyed lambda 先发 Closed 再 erase;Taskbar 点击运行图标走 raise/minimize toggle(不再二次启动);`FloatingPolicy` 新窗口初始位置(居中+24px 级联+缩进 work area,镜像 `WindowPlacementPolicy`);WindowInfo 补 `icon_hint`/`z_index`/`is_always_on_top`/`created_at`。单测:FloatingPolicy 13 例 + WindowManager 14 例(FakeWindow+QSignalSpy)。**装饰 overlay (策略 A) DEFERRED 到 EGLFS 阶段**(WSL X11/Win32 外部窗口自带原生标题栏、overlay 重影;EGLFS/Wayland-compositor 后端将原生持有装饰权)。详见 [milestone_05](../todo/desktop/milestone_05_window_management.md)。
- **已达成**：Milestone 1「桌面骨架可见」；Phase 0 / 1 / 2 / A(CI) / 6 / G(Widget) / H(显示后端)（详见 [SUMMARY.md](../todo/done/SUMMARY.md)）

## 新人入门

1. [`README.md`](../../README.md) — 项目定位
2. [`AGENT.md`](../../AGENT.md) — 构建命令（configure / fast build / run tests）
3. 本文件「下一步路线」— 取首个任务 MS2 上手
