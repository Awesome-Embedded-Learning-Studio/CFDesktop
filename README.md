<div align="center">

  # CFDesktop

  ### 为嵌入式设备打造的现代化 Material Design 3 桌面框架

  [![License: MIT][license-badge]] [![Version: 0.19.0][version-badge]]
  [![C++23][cpp-badge]] [![Qt 6.8][qt-badge]] [![CMake][cmake-badge]]
  [![Documentation][docs-badge]]

  [项目简介](#项目简介) • [项目进度](#项目进度) • [架构概览](#架构概览) • [快速开始](#快速开始)

</div>

---

## 项目简介

**CFDesktop** 是一个基于 Qt6 的嵌入式桌面框架项目，旨在解决以下问题：

### 解决的问题

- **嵌入式设备 UI 现代化** - 提供符合 Material Design 3 规范的现代 UI
- **性能自适应** - 根据设备硬件能力动态调整 UI 复杂度，从低端 ARM 到高性能 RK3588 都能流畅运行
- **开发效率低** - 提供完整的 SDK 和模拟器，简化嵌入式 GUI 应用开发
- **跨平台移植难** - 模块化设计，易于移植到不同架构 (x86_64/ARM64/ARMhf)

### 目标用户

- 想尝试部署所有端统一的桌面模拟器的嵌入式Linux爱好者
- Qt 应用开发爱好者（嘿！想想一个All Same的桌面，可以运行在imx6ull，rk系列甚至是上位机的桌面！）
- 嵌入式 Linux 系统集成商（幻想开始.png）

---

## AI Agent 支持

本项目内置 AI agent 协作支持（**Claude Code** 与 **Codex CLI**），项目约定集中在 [`AGENT.md`](AGENT.md)（单一来源，`CLAUDE.md` / `AGENTS.md` 为各工具入口）。使用 AI 时据此快速理解架构与规范；**不使用 AI 可安全忽略** `AGENT.md` / `CLAUDE.md` / `AGENTS.md` 与 `.claude/` 目录，它们不影响构建。

---

## 项目进度

> 项目进度以 [`document/status/current.md`](document/status/current.md) 为唯一事实来源。以下为对外概览，可能滞后，精确状态请查阅该文件。

**当前焦点**：跑通「看到桌面 → 点图标 → 开应用」最小闭环。

- ✅ **base** 硬件探针层（含 HWTier）完成
- 🚧 **ui / desktop** 核心就绪，最小闭环控件与后端开发中
- ⬜ 输入抽象、模拟器、Wayland / 嵌入式后端待开始

下一步路线与各 Phase 细节见 [`document/status/current.md`](document/status/current.md)，待办清单见 [`document/todo/`](document/todo/)。

---

## 架构概览

### 分层设计

```
┌─────────────────────────────────────────────────────────────────┐
│                    Material Widget Layer                         │
│  (P0: 7控件 ✅ | P1: 12控件 ✅ | P2: 27控件 ⬜ | P3: 25控件 ⬜)    │
├─────────────────────────────────────────────────────────────────┤
│                    Material Behavior Layer (100%)                │
│  StateMachine | RippleHelper | ElevationController | FocusRing   │
├─────────────────────────────────────────────────────────────────┤
│                     Animation Engine Layer (90%)                 │
│  AnimationFactory | Fade/Slide/Scale ✅                         │
├─────────────────────────────────────────────────────────────────┤
│                      Theme Engine Layer (100%)                   │
│  ThemeManager | ColorScheme | Typography | Motion | Radius      │
├─────────────────────────────────────────────────────────────────┤
│                    Math & Utility Layer (100%)                   │
│  MathHelper | Color | Easing | Geometry | DevicePixel           │
├─────────────────────────────────────────────────────────────────┤
│                      Desktop Base Layer (85%)                    │
│  ConfigStore | Logger | ASCII Art | File Operations | System    │
└─────────────────────────────────────────────────────────────────┘
```

### 模块说明

| 模块 | 路径 | 功能 |
|:---|:---|:---|
| 配置中心 | [desktop/base/config_manager/](desktop/base/config_manager/) | 四层存储 (Temp/App/User/System)、变更监听、JSON 持久化 |
| 日志系统 | [desktop/base/logger/](desktop/base/logger/) | 多等级日志、异步处理、多 Sink 支持 |
| 文件操作 | [desktop/base/file_operations/](desktop/base/file_operations/) | 跨平台文件操作、权限管理、监控 |
| ASCII 艺术 | [desktop/base/ascii_art/](desktop/base/ascii_art/) | 启动 Logo 渲染、ASCII 图形 |
| 系统检测 | [base/system/](base/system/) | CPU/Memory 信息检测、跨平台支持 |
| 设备抽象 | [base/device/](base/device/) | 控制台设备抽象层、策略链 |
| 基础工具 | [base/include/base/](base/include/base/) | Hash、Optional、ScopeGuard、Singleton 等 |
| UI 核心 | [ui/core/](ui/core/) | Material Design 主题引擎、Token 系统 |
| UI 组件 | [ui/components/](ui/components/) | 动画工厂、动画组、策略模式 |
| UI 控件 | [ui/widget/material/](ui/widget/material/) | Material Design 控件实现 |

### Display Backend

CFDesktop 通过 `IDisplayServerBackend` 接口抽象了三种显示模式，使 Shell、PanelManager 和 WindowManager 可以在不同平台上一致工作：

- **Client 模式**: 作为现有桌面环境内的应用运行（Windows、Linux Gnome/KDE）
- **Compositor 模式**: CFDesktop 自身作为显示服务器/合成器管理外部应用窗口（X11/Wayland）
- **DirectRender 模式**: 直接渲染到 framebuffer，无需窗口系统（嵌入式 EGLFS/linuxfb）

| 后端 | 状态 | 说明 |
|:---|:---:|:---|
| Windows 后端 | 100% | Win32 DWM 集成、SetWinEventHook 窗口管理 |
| WSL X11 后端 | 100% | XCB + XWayland、QSocketNotifier 驱动 |
| Wayland 合成器 | 0% | 待实现 |
| EGLFS / LinuxFB | 0% | 嵌入式直驱，待实现 |

后端通过 `DisplayServerBackendFactory` 工厂在启动时自动选择平台实现。

### 技术栈

- **语言**: C++23
- **框架**: Qt 6.8.3
- **构建**: CMake 3.16+, Ninja
- **测试**: Google Test
- **文档**: VitePress + Doxygen

---

## 快速开始

### 前置要求

- C++23 编译器 (LLVM/Clang 或 GCC)
- CMake 3.16+
- Qt 6.8+

### 构建项目

```bash
# 克隆仓库
git clone https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop
cd CFDesktop

# Windows 快速构建
.\scripts\build_helpers\windows_fast_develop_build.ps1

# Linux 快速构建
./scripts/build_helpers/linux_fast_develop_build.sh
```

### 运行示例

```bash
# 运行 Material Gallery (查看所有 UI 组件)
.\out\build_deploy\bin\material_gallery.exe

# 运行主题定制示例
.\out\build_deploy\bin\material_theme_customizer.exe
```

### 开发环境配置

项目使用 VSCode + Clangd 作为推荐开发环境，首次构建后会自动生成配置文件。

详细配置说明: [document/development/README.md](document/development/README.md)

---

## 完整文档

📚 **项目文档站**: [https://awesome-embedded-learning-studio.github.io/CFDesktop/](https://awesome-embedded-learning-studio.github.io/CFDesktop/)

本地预览文档:

```bash
pnpm install
pnpm dev
pnpm build
```

### 开发文档

| 文档 | 说明 | 链接 |
|:---|:---|:---|
| 开发环境设置 | 前置要求、快速开始、构建系统 | [development/](https://awesome-embedded-learning-studio.github.io/CFDesktop/development/) |
| 当前状态 | 开发重启事实源 | [status/current](https://awesome-embedded-learning-studio.github.io/CFDesktop/status/current) |
| UI 框架文档 | Material Design 实现架构 | [HandBook/ui/](https://awesome-embedded-learning-studio.github.io/CFDesktop/HandBook/ui/) |
| AI 辅助开发 | 通用 AI 协作指南 | [development/ai-assistant-guide](https://awesome-embedded-learning-studio.github.io/CFDesktop/development/ai-assistant-guide) |

### 设计文档

| 文档 | 说明 | 链接 |
|:---|:---|:---|
| 工程骨架设计 | 项目基础设施与环境配置 | [design_stage/00_phase0_project_skeleton.md](document/design_stage/00_phase0_project_skeleton.md) |
| Base 库设计 | 基础库设计与实现 | [design_stage/02_phase2_base_library.md](document/design_stage/02_phase2_base_library.md) |
| UI 框架设计 | Material Design 分层架构 | [HandBook/ui/architecture/](document/HandBook/ui/architecture/) |

### TODO 跟踪

- [**待办清单**](document/todo/) - 按优先级分类的待办事项
- [任务看板](document/todo/README.md) - 当前开发任务列表
- [当前状态](document/status/current.md) - 项目重启事实源

---

## 贡献

欢迎贡献代码、报告问题或提出建议！

请查看 [document/todo/](document/todo/) 了解当前开发任务和优先级。

---

## 许可证

本项目采用 [MIT](LICENSE) 开源许可证。

---

<div align="center">

  **很早就像做一个酷酷的统一桌面，现在，终于可以尝试开始了！**

</div>

<!-- Badge Links -->
[license-badge]: https://img.shields.io/badge/License-MIT-yellow.svg
[version-badge]: https://img.shields.io/badge/version-0.18.0-blue.svg
[cpp-badge]: https://img.shields.io/badge/C++-23-00599C.svg
[qt-badge]: https://img.shields.io/badge/Qt-6.8-41CD52.svg
[cmake-badge]: https://img.shields.io/badge/CMake-3.16+-064F8C.svg
[docs-badge]: https://img.shields.io/badge/docs-latest-brightgreen.svg
