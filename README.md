<div align="center">

  # 🖥️ CFDesktop
  ### **一个可部署到任意支持 Qt 的嵌入式设备上的现代化桌面框架**

  [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
  [![C++](https://img.shields.io/badge/C++-23-blue.svg)](https://en.cppreference.com/w/C++23)
  [![Qt](https://img.shields.io/badge/Qt-6.8.3-41CD52.svg)](https://www.qt.io/)
  [![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg)](https://cmake.org/)
  [![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)]()

  [功能特性](#-功能特性) • [快速开始](#-快速开始) • [开发文档](#-开发文档) • [技术栈](#-技术栈)

</div>

---

## 📖 项目简介

**CFDesktop** 旨在为各种嵌入式设备提供统一、现代化的桌面环境。通过模块化设计和硬件能力分级，CFDesktop 能够根据设备性能动态调整用户体验，从低端 ARM 设备到高性能 RK3588 平台都能流畅运行。

### 💡 设计理念

| 理念 | 描述 |
|:---:|:---|
| 🌍 **跨平台** | 支持 Windows/Linux 嵌入式设备，可移植到多种架构 |
| ⚡ **性能自适应** | 根据设备硬件能力自动调整 UI 特效和功能 |
| 🛠️ **开发友好** | 提供完整的 SDK 和模拟器，简化应用开发 |
| 🧩 **模块化** | 各功能模块解耦，便于裁剪和定制 |

---

## 🚀 当前进度

### ✅ 已完成

```diff
+1. Boot Test 环境检测
     ✓ 编译前自动检测依赖是否齐全，确保可以编译通过
     ✓ 支持控制台和 GUI 两种运行模式
     ✓ 提供详细的 Qt 环境检测报告

+2. VSCode + Clangd 开发环境
     ✓ 自动生成 clangd 配置
     ✓ 保证与 QtCreator 几乎一致的丝滑开发体验
     ✓ 支持 C++23 语法高亮和代码补全

+3. 自动化构建脚本
     ✓ Windows PowerShell 构建脚本
     ✓ Linux Bash 构建脚本
     ✓ 支持多种工具链配置（LLVM/GCC）
     ✓ 部署（deploy）与开发（develop）双模式构建
     ✓ 为后续开发打下坚实的基础

+4. CMake 基础设施
     ✓ 模块化 CMake 架构
     ✓ 自定义日志输出系统
     ✓ 工具链检测与配置管理
     ✓ 编译命令生成（compile_commands.json）
```

### 🚧 开发中

| 模块 | 状态 | 阶段 |
|:---|:---:|:---:|
| 🔍 硬件探针模块 | ⏳ 进行中 | Phase 1 |
| 📦 Base 基础库 | 📋 计划中 | Phase 2 |
| ⌨️ 输入抽象层 | 📋 计划中 | Phase 3 |

---

## 🎯 功能特性

<details>
<summary><b>🔍 点击展开核心功能</b></summary>

#### 🖥️ 硬件能力检测
- 自动检测 CPU、GPU、内存性能
- 动态调整 UI 复杂度
- 智能资源管理

#### 🎨 现代化 UI
- 流畅的动画效果
- 响应式布局
- 主题定制支持

#### 🧩 模块化架构
- 插件式扩展
- 松耦合设计
- 易于维护和升级

#### 🛠️ 开发工具链
- 完整的 SDK
- 模拟器支持
- 调试工具

</details>

---

## 🏁 快速开始

### 📋 前置要求

| 依赖 | 最低版本 | 推荐版本 |
|:---|:---:|:---:|
| **编译器** | LLVM/Clang 或 GCC | 最新版 |
| **CMake** | 3.16 | 3.20+ |
| **Qt** | 6.8.3 | 6.8+ |

### 💻 Windows 构建

```powershell
# 部署模式构建
.\scripts\build_helpers\windows_deploy.ps1

# 开发模式构建
.\scripts\build_helpers\windows_develop_build.ps1
```

### 🐧 Linux 构建

```bash
# 部署模式构建
./scripts/build_helpers/linux_deploy_build.sh

# 开发模式构建
./scripts/build_helpers/linux_develop_build.sh
```

### ⚙️ 手动构建

```bash
# 1. 配置 CMake（Windows 示例）
cmake -S . -B out/build -G "Ninja" \
      -DUSE_TOOLCHAIN=windows/gcc

# 1. 配置 CMake（Linux 示例）
cmake -S . -B out/build -G "Unix Makefiles" \
      -DUSE_TOOLCHAIN=linux/gcc

# 2. 编译
cmake --build out/build

# 3. 运行 boot test
./out/build/test/boot_test/boot_test_core
```

### 🔧 构建配置

构建脚本通过 INI 配置文件管理构建参数：

| 配置文件 | 用途 | 输出目录 |
|:---|:---|:---|
| `build_deploy_config.ini` | 部署构建 | `out/build_deploy` |
| `build_develop_config.ini` | 开发构建 | `out/build_develop` |

可在配置文件中调整 CMake 生成器、工具链等参数。

---

## 🛠️ 开发环境

### 🤝 VSCode + Clangd

项目已配置自动生成 clangd 配置。首次运行 CMake 配置后，会在项目根目录生成 `.clangd` 配置文件，提供：

- ✨ 精准的代码补全
- ✔️ 实时语法检查
- 🔍 跳转到定义
- 🔧 重构支持

### 🎨 QtCreator

也可以直接使用 QtCreator 打开 `CMakeLists.txt` 进行开发。

---

## 📚 开发文档

详细的设计文档请查看 [document/design_stage/](document/design_stage/)：

| 文档 | 内容 | 阶段 |
|:---|:---|:---:|
| [工程骨架搭建](document/design_stage/00_phase0_project_skeleton.md) | 项目基础设施与环境配置 | Phase 0 |
| [硬件探针与能力分级](document/design_stage/01_phase1_hardware_probe.md) | 硬件检测与性能评估 | Phase 1 |
| [Base 库核心功能](document/design_stage/02_phase2_base_library.md) | 基础库设计与实现 | Phase 2 |
| [输入抽象层](document/design_stage/03_phase3_input_layer.md) | 输入设备统一接口 | Phase 3 |
| [多平台模拟器](document/design_stage/04_phase6_simulator.md) | 开发调试模拟器 | Phase 6 |
| [测试体系](document/design_stage/05_phase8_testing.md) | 单元测试与集成测试 | Phase 8 |

---

## 🔧 技术栈

<div align="center">

```
┌─────────────────────────────────────────────────────────┐
│                    CFDesktop 架构                         │
├─────────────────────────────────────────────────────────┤
│  C++23  │  Qt 6.8.3  │  CMake  │  LLVM/Clang  │  Ninja  │
└─────────────────────────────────────────────────────────┘
```

| 技术 | 版本 | 用途 |
|:---|:---:|:---|
| **C++** | 23 | 核心开发语言 |
| **CMake** | 3.16+ | 构建系统 |
| **Qt** | 6.8.3+ | UI 框架 |
| **LLVM/Clang** | 最新 | 编译器（首选）|
| **GCC** | 最新 | Linux 编译器 |
| **Ninja** | - | 构建工具 |

</div>

---

## 🗺️ 路线图

<details>
<summary><b>📅 查看完整开发计划</b></summary>

```mermaid
graph LR
    A[Phase 0<br/>工程骨架] --> B[Phase 1<br/>硬件探针]
    B --> C[Phase 2<br/>Base 库]
    C --> D[Phase 3<br/>输入抽象层]
    D --> E[Phase 4<br/>UI 组件库]
    E --> F[Phase 5<br/>窗口管理器]
    F --> G[Phase 6<br/>模拟器]
    G --> H[Phase 7<br/>应用框架]
    H --> I[Phase 8<br/>测试体系]
    I --> J[Phase 9<br/>文档完善]
    J --> K[Phase 10<br/>发布准备]

    style A fill:#4CAF50
    style B fill:#2196F3
    style C fill:#9E9E9E
    style D fill:#9E9E9E
    style E fill:#9E9E9E
    style F fill:#9E9E9E
    style G fill:#9E9E9E
    style H fill:#9E9E9E
    style I fill:#9E9E9E
    style J fill:#9E9E9E
    style K fill:#9E9E9E
```

</details>

---

## 🤝 贡献

欢迎贡献代码、报告问题或提出建议！

---

## 📄 许可证

本项目采用 [MIT](LICENSE) 开源许可证。

---

<div align="center">

  **Made with ❤️ for embedded devices**

  *最后更新: 2026-02-21*

  [⬆ 返回顶部](#-cfdesktop)

</div>
