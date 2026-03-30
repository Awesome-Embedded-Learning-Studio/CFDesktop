<div align="center">

  # CFDesktop

  ### 为嵌入式设备打造的现代化 Material Design 3 桌面框架

  [![License: MIT][license-badge]] [![Version: 0.13.1][version-badge]]
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

## 项目进度

### 已完成 ✅

| 阶段 | 模块 | 完成度 | 说明 |
|:---|:---|:---:|:---|
| Phase 0 | 工程骨架 | 100% | CMake 构建系统、代码规范、CI/CD、Docker 多架构构建 |
| Phase 1 | 硬件探针 | 90% | CPU/Memory/GPU/网络检测完成，缺HWTier/Policy |
| Phase 2 | Base 库核心 | 80% | ConfigStore、Logger、DPI基础转换(ui/base)、ASCII Art、File Operations |
| Phase 5 | 测试体系 | 55% | Google Test 集成，base/logger/ui基础有覆盖 |
| Phase 6 | UI 框架核心 | 75% | Material Design 3 分层架构 (Layer 1-4)，缺布局/手势 |
| Phase 6 | P0 核心控件 | 100% | Button, TextField, TextArea, Label, CheckBox, RadioButton, GroupBox |
| Phase 6 | P1 控件 | 100% | Slider, ProgressBar, Switch, ToggleButton, etc. (12个) |
| Desktop | 桌面基础 | 80% | 配置中心、日志系统、启动初始化、文件操作 |

### 进行中 🚧

| 阶段 | 模块 | 完成度 | 说明 |
|:---|:---|:---:|:---|
| Phase 2 | 配置日志增强 | 80% | DPI基础转换已有(ui/base)，缺自动检测/版本控制/迁移/验证/网络日志 |
| Phase 5 | UI控件测试 | 0% | 19个P0/P1控件无单元测试 |
| Desktop | 桌面的基础组件 | 10% | Console欢迎完成，PanelManager/ShellLayer空缺 |

### 待开始 ⬜

| 阶段 | 模块 | 说明 |
|:---|:---|:---|
| Phase 1 | 硬件探针完善 | HWTier 档位、CapabilityPolicy |
| Phase 6 | 布局系统 | Grid、Stack、ConstraintLayout |
| Phase 6 | 手势识别 | 触摸/手势统一接口 |
| Phase 3 | 输入抽象层 | 触摸/按键/旋钮/手势统一接口 |
| Phase 4 | 多平台模拟器 | 开发调试用模拟器、设备配置、DPI 注入 |
| Phase 6 | P2 控件 | 27个高级控件 (DatePicker, MenuBar, Dialog, etc.) |
| Phase 6 | P3 控件 | 25个专业控件 (SplitView, ChartView, etc.) |
| Phase 5 | 测试完善 | desktop 模块、性能基准、UI 自动化 |
| 文档 | API/示例补充 | 约40%文档缺失，50%示例缺失 |

### 快速统计 (2026-03-27 更新)

| 类别 | 完成度 | 说明 |
|:---|:---:|:---|
| UI 控件 (P0+P1) | 100% (19个) | 完整实现 |
| UI 控件 (P2+P3) | 0% (52个) | 待开发 |
| UI 控件测试 | 0% | **关键缺口** |
| 文档覆盖 | 60% | ~268个文档 |
| 示例覆盖 | 50% | ~80个示例 |
| 测试覆盖 | 55% | Base/Logger良好，UI控件缺失 |

📋 **完整待办清单**: [TODO.md](TODO.md)
📊 **详细状态报告**: [document/todo/done/PROJECT_STATUS_REPORT.md](document/todo/done/PROJECT_STATUS_REPORT.md)

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

### 技术栈

- **语言**: C++23
- **框架**: Qt 6.8.3
- **构建**: CMake 3.16+, Ninja
- **测试**: Google Test
- **文档**: MkDocs + Doxygen

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

### 开发文档

| 文档 | 说明 | 链接 |
|:---|:---|:---|
| 开发环境设置 | 前置要求、快速开始、构建系统 | [development/](https://awesome-embedded-learning-studio.github.io/CFDesktop/development/) |
| API 参考手册 | 基础库 API 文档 | [HandBook/api/](https://awesome-embedded-learning-studio.github.io/CFDesktop/HandBook/api/) |
| UI 框架文档 | Material Design 实现架构 | [HandBook/ui/](https://awesome-embedded-learning-studio.github.io/CFDesktop/HandBook/ui/) |
| 代码示例 | 各模块使用示例 | [example/](https://awesome-embedded-learning-studio.github.io/CFDesktop/examples/) |

### 设计文档

| 文档 | 说明 | 链接 |
|:---|:---|:---|
| 工程骨架设计 | 项目基础设施与环境配置 | [design_stage/00_phase0_project_skeleton.md](document/design_stage/00_phase0_project_skeleton.md) |
| Base 库设计 | 基础库设计与实现 | [design_stage/02_phase2_base_library.md](document/design_stage/02_phase2_base_library.md) |
| UI 框架设计 | Material Design 分层架构 | [HandBook/ui/architecture/](document/HandBook/ui/architecture/) |

### TODO 跟踪

- [**待办清单**](TODO.md) - 按优先级分类的待办事项 (P0-P3)
- [任务看板](document/todo/README.md) - 当前开发任务列表
- [状态报告](document/todo/done/PROJECT_STATUS_REPORT.md) - 项目整体进度报告

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
[version-badge]: https://img.shields.io/badge/version-0.13.1-blue.svg
[cpp-badge]: https://img.shields.io/badge/C++-23-00599C.svg
[qt-badge]: https://img.shields.io/badge/Qt-6.8-41CD52.svg
[cmake-badge]: https://img.shields.io/badge/CMake-3.16+-064F8C.svg
[docs-badge]: https://img.shields.io/badge/docs-latest-brightgreen.svg
