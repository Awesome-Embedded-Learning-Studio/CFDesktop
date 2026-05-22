---
title: 构建系统文档
description: CFDesktop CMake 构建系统完整指南
---

# 构建系统文档

> CFDesktop CMake 构建系统完整指南

## 目录

- [概述](#概述)
- [CMake 架构](#cmake-架构)
- [项目模块](#项目模块)
- [构建类型](#构建类型)
- [工具链配置](#工具链配置)
- [构建脚本](#构建脚本)
- [输出目录](#输出目录)
- [常用构建选项](#常用构建选项)
- [高级用法](#高级用法)

---

## 概述

CFDesktop 使用基于 CMake 的模块化构建系统，专为跨平台开发和嵌入式部署而设计。该构建系统支持：

- **多平台**：Windows（MinGW/LLVM）、Linux（GCC/Clang）
- **多架构**：x86_64、ARM64、ARMhf
- **构建类型**：Debug、Release、RelWithDebInfo
- **容器化构建**：Docker 多架构支持

### 构建系统示意图

```text
CFDesktop Build System
├── Configuration Files (.ini)
│   ├── build_develop_config.ini    (Debug builds)
│   ├── build_deploy_config.ini     (Release builds)
│   └── build_ci_config.ini         (CI builds)
│
├── Build Scripts
│   ├── windows_*.ps1               (PowerShell scripts)
│   ├── linux_*.sh                  (Bash scripts)
│   └── docker_start.sh             (Docker wrapper)
│
├── CMake Modules
│   ├── check_toolchain.cmake       (Toolchain selection)
│   ├── OutputDirectoryConfig.cmake (Output directory management)
│   └── generate_develop_helpers.cmake (IDE config generation)
│
└── Toolchains
    ├── windows/llvm-toolchain.cmake
    ├── windows/gcc-toolchain.cmake
    ├── linux/ci-x86_64-toolchain.cmake
    └── linux/ci-aarch64-toolchain.cmake
```yaml

---

## CMake 架构

### 根 CMakeLists.txt

根 `CMakeLists.txt` 是构建系统的入口点：

```cmake
cmake_minimum_required(VERSION 3.16)
project(CFDesktop VERSION 0.9.0 LANGUAGES CXX)

# Toolchain configuration (supports shorthand: -DUSE_TOOLCHAIN=windows/llvm)
include(cmake/check_toolchain.cmake)

# Build type validation
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

# Compiler flags per build type
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "" FORCE)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Qt6 dependency
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Subdirectories
add_subdirectory(base)
add_subdirectory(ui)
add_subdirectory(example)
add_subdirectory(test)
```text

### CMake 模块结构

```text
cmake/
├── build_log_helper.cmake         # Logging utilities
├── check_toolchain.cmake          # Toolchain selection
├── custom_target_helper.cmake     # Custom target helpers
├── OutputDirectoryConfig.cmake    # Output directory management
├── ExampleLauncher.cmake          # Windows launcher generation
├── QtDeployUtils.cmake            # Qt deployment utilities
└── generate_develop_helpers.cmake # IDE configuration generation
```bash

---

## 项目模块

### 模块概览

| 模块 | 描述 | 输出 | 依赖 |
|:-----|:-----|:-----|:-----|
| **base/** | 基础工具和平台抽象 | `cfbase.dll` / `libcfbase.so` | Qt6::Core |
| **ui/** | UI 框架和组件 | `cfui.dll` / `libcfui.so` | base、Qt6::Core、Qt6::Gui |
| **example/** | 示例程序 | `examples/{category}/` | base、ui |
| **test/** | 单元测试 | `test/` | base、ui、GoogleTest |

### Base 模块

Base 模块提供基础工具：

```text
base/
├── include/                    # Header-only utilities
│   ├── cfbase/
│   │   ├── expected.hpp       # std::expected-like type
│   │   ├── scope_guard.hpp    # RAII resource management
│   │   └── weak_ptr.hpp       # Weak pointer utilities
│   └── CFDesktop/
│       └── Base/
│           └── system/
│               ├── cpu/        # CPU detection
│               └── memory/     # Memory detection
└── system/
    ├── cpu/CMakeLists.txt      # CPU module
    └── memory/CMakeLists.txt   # Memory module
```text

**统一 Base 库：**
所有 base 组件链接为一个单独的共享库（Windows 上为 `cfbase.dll`，Linux 上为 `libcfbase.so`）。

```cmake
# base/CMakeLists.txt
add_library(cfbase SHARED)
target_sources(cfbase PRIVATE
    $<TARGET_OBJECTS:cfbase_cpu>
    $<TARGET_OBJECTS:cfbase_memory>
)
target_link_libraries(cfbase PUBLIC Qt6::Core)
```text

### UI 模块

UI 模块提供 Material Design 框架：

```text
ui/
├── base/                       # Math utilities
│   ├── math_helper.hpp
│   ├── color_helper.hpp
│   ├── geometry_helper.hpp
│   └── easing.hpp
├── core/                       # Theme engine
│   ├── theme/
│   ├── color_scheme/
│   ├── motion_spec/
│   └── typography/
├── components/                 # Animation system
│   ├── animation/
│   ├── timing_animation/
│   └── spring_animation/
└── widget/                     # Widget adapters
    └── material/
        ├── button/
        ├── label/
        ├── textfield/
        └── ...
```text

**统一 UI 库：**
所有 UI 组件链接为一个单独的共享库（Windows 上为 `cfui.dll`，Linux 上为 `libcfui.so`）。

```cmake
# ui/CMakeLists.txt
add_library(cfui SHARED)
target_link_libraries(cfui PUBLIC
    cf_ui_core
    cf_ui_base
    cf_ui_components
    cf_ui_widget
    CFDesktop::base
    Qt6::Core
    Qt6::Gui
)
```bash

---

## 构建类型

### 可用的构建类型

| 构建类型 | 优化级别 | 调试信息 | 使用场景 |
|:---------|:---------|:--------:|:---------|
| **Debug** | `-O0` | 完整（`-g`） | 开发与调试 |
| **Release** | `-O3` | 无 | 生产部署 |
| **RelWithDebInfo** | `-O2` | 完整（`-g`） | 性能分析和测试 |

### 构建类型选择

构建类型通过 `.ini` 文件进行配置：

```ini
[cmake]
build_type=Debug    # or Release, RelWithDebInfo
```bash

**配置文件：**

| 文件 | 构建类型 | 使用场景 |
|:-----|:---------|:---------|
| `build_develop_config.ini` | Debug | 日常开发 |
| `build_deploy_config.ini` | Release | 生产构建 |
| `build_ci_config.ini` | Release | CI/CD 流水线 |

---

## 工具链配置

### 工具链简写

CFDesktop 支持工具链选择的简写表示法：

```bash
cmake -DUSE_TOOLCHAIN=windows/llvm -S . -B build
cmake -DUSE_TOOLCHAIN=windows/gcc -S . -B build
cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B build
```bash

### 可用工具链

| 平台 | 工具链 | 简写 | 编译器 |
|:-----|:-------|:-----|:-------|
| **Windows** | LLVM-MinGW | `windows/llvm` | clang/LLVM |
| **Windows** | MinGW-GCC | `windows/gcc` | gcc/MinGW |
| **Linux** | CI x86_64 | `linux/ci-x86_64` | gcc (Docker) |
| **Linux** | CI ARM64 | `linux/ci-aarch64` | aarch64 gcc (Docker) |

### 工具链文件结构

工具链文件位于 `cmake/cmake_toolchain/{platform}/`：

```text
cmake/cmake_toolchain/
├── windows/
│   ├── llvm-toolchain.cmake
│   └── gcc-toolchain.cmake
└── linux/
    ├── ci-x86_64-toolchain.cmake
    └── ci-aarch64-toolchain.cmake
```text

### Windows LLVM-MinGW 工具链

```cmake
# cmake/cmake_toolchain/windows/llvm-toolchain.cmake
set(CMAKE_PREFIX_PATH "D:/QT/Qt6.6.0/6.8.3/llvm-mingw_64")
set(CMAKE_C_COMPILER "D:/QT/Qt6.6.0/Tools/llvm-mingw1706_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/QT/Qt6.6.0/Tools/llvm-mingw1706_64/bin/g++.exe")
```text

### Linux CI 工具链

```cmake
# cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(QT6_BASE_DIR "/opt/Qt/6.8.1/gcc_64")
set(Qt6_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")
```yaml

---

## 构建脚本

### 脚本概览

构建脚本按平台和用途组织：

```text
scripts/build_helpers/
├── windows_*.ps1              # Windows PowerShell scripts
│   ├── windows_configure.ps1
│   ├── windows_fast_develop_build.ps1
│   ├── windows_develop_build.ps1
│   └── windows_run_tests.ps1
├── linux_*.sh                 # Linux Bash scripts
│   ├── linux_configure.sh
│   ├── linux_fast_develop_build.sh
│   ├── linux_develop_build.sh
│   └── linux_run_tests.sh
└── docker_start.sh            # Docker wrapper script
```text

### Windows 构建脚本

#### 配置脚本

```powershell
# Configure only (no build)
.\scripts\build_helpers\windows_configure.ps1 [-Config <develop|deploy>]
```text

**该脚本执行以下操作：**
1. 从 `.ini` 文件加载配置
2. 验证构建类型
3. 运行 CMake 配置
4. 生成构建文件

#### 快速构建脚本

```powershell
# Fast incremental build
.\scripts\build_helpers\windows_fast_develop_build.ps1
```text

**该脚本执行以下操作：**
1. 调用配置脚本
2. 使用 CMake 构建（不清理）
3. 使用并行任务

#### 完整构建脚本

```powershell
# Full clean build
.\scripts\build_helpers\windows_develop_build.ps1
```text

**该脚本执行以下操作：**
1. 清理构建目录
2. 调用快速构建脚本
3. 运行测试

### Linux 构建脚本

#### 配置脚本

```bash
# Configure only
bash scripts/build_helpers/linux_configure.sh [develop|deploy|ci] [-c <config_file>]
```text

#### 快速构建脚本

```bash
# Fast incremental build
bash scripts/build_helpers/linux_fast_develop_build.sh [develop|deploy|ci]
```text

#### 完整构建脚本

```bash
# Full clean build
bash scripts/build_helpers/linux_develop_build.sh [develop|deploy|ci]
```text

### Docker 构建脚本

```bash
# Interactive shell
bash scripts/build_helpers/docker_start.sh

# Fast build
bash scripts/build_helpers/docker_start.sh --fast-build --build-project-fast

# Full build
bash scripts/build_helpers/docker_start.sh --build-project

# Run tests
bash scripts/build_helpers/docker_start.sh --run-project-test

# CI verification
bash scripts/build_helpers/docker_start.sh --verify

# ARM64 build
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify
```bash

**Docker 选项：**

| 选项 | 描述 |
|:-----|:-----|
| `--arch amd64\|arm64` | 目标架构 |
| `--fast-build` | 复用已有镜像 |
| `--verify` | 运行 CI 验证 |
| `--build-project` | 完整清理构建 |
| `--build-project-fast` | 快速增量构建 |
| `--run-project-test` | 运行测试 |
| `--stay-on-error` | 出错时保留容器 |
| `--no-log` | 禁用文件日志 |
| `--no-deps` | 跳过依赖安装 |

---

## 输出目录

### 输出目录结构

```text
out/build_{config}/
├── bin/                        # Executables and shared libraries
│   ├── cfbase.dll              # Base library (Windows)
│   ├── cfui.dll                # UI library (Windows)
│   └── ...
├── lib/                        # Static libraries
│   ├── libcfbase.a
│   └── ...
├── examples/                   # Example programs
│   ├── base/                   # Base examples
│   │   ├── cpu_info
│   │   └── memory_info
│   ├── ui/                     # UI widget examples
│   │   ├── button
│   │   ├── label
│   │   └── ...
│   └── gui/                    # GUI examples
│       ├── material_gallery
│       └── theme
├── plugins/                    # Qt plugins
├── resources/                  # Resource files
├── runtimes/                   # Qt runtime DLLs (Windows)
└── test/                       # Test executables
    ├── base_test
    ├── ui_test
    └── ...
```text

### 输出目录配置

输出目录在 `cmake/OutputDirectoryConfig.cmake` 中配置：

```cmake
# Global output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Example-specific outputs
function(cf_set_example_output_dir TARGET_NAME CATEGORY)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/${CATEGORY}"
    )
endfunction()
```yaml

---

## 常用构建选项

### 配置文件选项

配置文件使用 INI 格式：

```ini
[cmake]
# CMake generator
generator=MinGW Makefiles        # or "Unix Makefiles", "Ninja"

# Toolchain selection
toolchain=windows/llvm          # or windows/gcc, linux/ci-x86_64

# Build type
build_type=Debug                # or Release, RelWithDebInfo

[paths]
# Source directory (relative to project root)
source=.

# Build output directory (relative to project root)
build_dir=out/build_develop

[options]
# Parallel jobs for compilation
jobs=16
```bash

### CMake 选项

| 选项 | 描述 | 默认值 |
|:-----|:-----|:-------|
| `CMAKE_BUILD_TYPE` | 构建类型（Debug/Release/RelWithDebInfo） | 必填 |
| `CMAKE_PREFIX_PATH` | Qt 安装路径 | 由工具链提供 |
| `USE_TOOLCHAIN` | 工具链简写 | 必填 |
| `CMAKE_EXPORT_COMPILE_COMMANDS` | 生成 compile_commands.json | ON |
| `BUILD_TESTING` | 构建测试 | ON |

---

## 高级用法

### 自定义工具链配置

要使用自定义工具链：

1. 在 `cmake/cmake_toolchain/{platform}/` 中创建工具链文件
2. 使用简写表示法：

```bash
cmake -DUSE_TOOLCHAIN=windows/mytoolchain -S . -B build
```text

或使用完整路径：

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake -S . -B build
```text

### 增量构建

为了加速开发，可使用增量构建：

**Linux：**
```bash
bash scripts/build_helpers/linux_fast_develop_build.sh
```text

**Windows：**
```powershell
.\scripts\build_helpers\windows_fast_develop_build.ps1
```text

### 并行构建

通过配置文件控制并行任务数：

```ini
[options]
jobs=8                          # Use 8 parallel jobs
```text

或通过 CMake：

```bash
cmake --build build --parallel 8
```text

### 构建特定目标

要构建特定目标：

```bash
cmake --build build --target cfbase
cmake --build build --target cfui
cmake --build build --target material_gallery
```text

### 清理构建

要执行完全清理构建：

**Linux：**
```bash
bash scripts/build_helpers/linux_develop_build.sh
```text

**Windows：**
```powershell
.\scripts\build_helpers\windows_develop_build.ps1
```text

或手动执行：

```bash
rm -rf out/build_develop
cmake -DUSE_TOOLCHAIN=windows/llvm -DCMAKE_BUILD_TYPE=Debug -S . -B out/build_develop
cmake --build out/build_develop
```cpp

---

## 跨平台构建矩阵

### 支持的平台

| 平台 | 架构 | 工具链 | 状态 |
|:-----|:----:|:-------|:----:|
| Windows 10+ | x86_64 | LLVM-MinGW | 已支持 |
| Windows 10+ | x86_64 | MinGW-GCC | 已支持 |
| Linux | x86_64 | GCC | 已支持 |
| Linux | x86_64 | Clang | 已支持 |
| Linux (Docker) | x86_64 | GCC | 已支持 |
| Linux (Docker) | ARM64 | aarch64 gcc | 已支持 |

### 构建命令参考

| 平台 | 命令 |
|:-----|:-----|
| **Windows (LLVM)** | `.\scripts\build_helpers\windows_fast_develop_build.ps1` |
| **Windows (GCC)** | 编辑 `build_develop_config.ini`：`toolchain=windows/gcc` |
| **Linux（原生）** | `bash scripts/build_helpers/linux_fast_develop_build.sh` |
| **Linux (Docker)** | `bash scripts/build_helpers/docker_start.sh --build-project-fast` |
| **ARM64 (Docker)** | `bash scripts/build_helpers/docker_start.sh --arch arm64 --build-project-fast` |

---

## IDE 集成

### VSCode 配置

构建系统会自动生成 VSCode 配置文件：

- `.vscode/launch.json` - 调试配置
- `.clangd` - Clangd 语言服务器配置
- `compile_commands.json` - 编译数据库

这些文件由 `cmake/generate_develop_helpers.cmake` 在 CMake 配置阶段生成。

### QtCreator

QtCreator 可以直接打开项目：

1. 文件 -> 打开文件或项目
2. 选择 `CMakeLists.txt`
3. 配置构建目录
4. 选择工具链
5. 点击"运行 CMake"

---

## 常见问题排查

### 常见问题

| 问题 | 解决方案 |
|:-----|:---------|
| 找不到 CMake | 安装 CMake 3.16+ 或将其添加到 PATH |
| 找不到 Qt | 设置 `Qt6_DIR` 或使用正确的工具链 |
| 找不到编译器 | 安装编译器或使用 Docker 构建 |
| 权限被拒绝 | 在 Linux 上使用 `sudo`，或在 Windows 上以管理员身份运行 |
| 构建时内存不足 | 在 `.ini` 文件中减少并行任务数 |

### Debug 模式构建

用于调试时，请确保使用 Debug 构建类型：

```ini
[cmake]
build_type=Debug
```text

这将：
- 禁用优化（`-O0`）
- 包含完整的调试符号（`-g`）
- 启用断言

### Release 构建

用于生产部署：

```ini
[cmake]
build_type=Release
```yaml

这将：
- 启用最大优化（`-O3`）
- 禁用调试信息
- 定义 `NDEBUG`

---

## 相关文档

- [快速入门指南](02_quick_start.md) - 30 分钟上手
- [项目骨架设计](../design_stage/00_phase0_project_skeleton.md) - 详细的项目架构
- [Base 库设计](../design_stage/02_phase2_base_library.md) - Base 模块文档
- [UI 框架设计](../todo/base/99_ui_material_framework.md) - UI 模块文档

---

**Last Updated**: 2026-03-07
