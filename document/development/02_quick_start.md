---
title: 快速开始指南
description: 30 分钟内搭建 CFDesktop 开发环境
---

# 快速开始指南

> 30 分钟内搭建 CFDesktop 开发环境

## 目录

- [前置条件](#前置条件)
- [五步快速开始](#五步快速开始)
- [Windows 用户](#windows-用户)
- [验证](#验证)
- [后续步骤](#后续步骤)
- [常见问题排查](#常见问题排查)

---

## 前置条件

在开始之前，请确保已安装以下工具：

| 需求 | 最低版本 | 推荐版本 | 检查命令 |
|:-----|---------:|:---------|:---------|
| **Git** | 2.30+ | 最新版 | `git --version` |
| **CMake** | 3.16 | 3.20+ | `cmake --version` |
| **Qt6** | 6.8.3 | 6.8+ | 检查 Qt 安装目录 |
| **编译器** | LLVM/Clang 或 GCC | 最新版 | `clang --version` 或 `gcc --version` |
| **Docker**（可选） | 20.10+ | 最新版 | `docker --version` |

### 平台特定要求

**Windows：**
- 需要使用 Git Bash 或 WSL2 来运行构建脚本
- 需要安装 Qt6，并配置 MinGW 或 LLVM-MinGW 工具链
- Windows 专用脚本需要 PowerShell 5.1+

**Linux：**
- GCC 12+ 或 Clang 16+
- Qt6 开发包
- Ninja 构建系统（可选，但推荐安装）

---

## 五步快速开始

### 第 1 步：克隆仓库

```bash
git clone https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop.git
cd CFDesktop
```bash

### 第 2 步：安装 VSCode 扩展

安装以下 VSCode 扩展以获得最佳开发体验：

| 扩展 | 用途 | 是否必需 |
|:-----|:-----|:--------:|
| [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) | C++ 语言服务器 | 是 |
| [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake) | CMake 语法高亮 | 推荐 |
| [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) | CMake 集成 | 推荐 |
| [Qt for Python](https://marketplace.visualstudio.com/items?itemName=seanwu.vscode-qt-for-python) | Qt 支持 | 可选 |

**快速安装（命令面板）：**

1. 按 `Ctrl+Shift+P`（Windows/Linux）或 `Cmd+Shift+P`（macOS）
2. 输入 "Extensions: Install Extensions"
3. 搜索上方的每个扩展名称

### 第 3 步：Docker 首次构建（推荐）

最快的上手方式是使用 Docker，它提供了预配置的构建环境：

```bash
# Fast build (reuses existing image if available)
bash scripts/build_helpers/docker_start.sh --fast-build --build-project-fast
```bash

**此命令会执行以下操作：**
1. 构建（或复用）包含所有依赖的 Docker 镜像
2. 使用 CMake 配置项目
3. 构建所有模块（base、ui、examples）
4. 将输出放置在 `out/build_develop/` 目录中

**Docker 选项：**

| 选项 | 说明 |
|:-----|:-----|
| `--fast-build` | 跳过镜像清理，复用已有镜像 |
| `--build-project` | 完整的干净构建 |
| `--build-project-fast` | 快速增量构建 |
| `--run-project-test` | 构建并运行测试 |
| `--arch arm64` | 为 ARM64 架构构建 |
| `--verify` | 运行 CI 风格的验证构建 |

### 第 4 步：运行示例程序

构建成功后，运行示例程序：

**Windows：**
```powershell
# Material Design Gallery
.\out\build_develop\examples\gui\material_gallery.exe

# Button Widget Example
.\out\build_develop\examples\ui\button.exe

# CPU Information Demo
.\out\build_develop\examples\base\cpu_info.exe
```text

**Linux：**
```bash
# Material Design Gallery
./out/build_develop/examples/gui/material_gallery

# Button Widget Example
./out/build_develop/examples/ui/button

# CPU Information Demo
./out/build_develop/examples/base/cpu_info
```bash

**可用示例：**

| 分类 | 示例 | 说明 |
|:-----|:-----|:-----|
| **base/** | cpu_info, memory_info | 系统信息演示 |
| **ui/** | button, label, textfield, checkbox, radiobutton, textarea, groupbox | Material Design 控件 |
| **gui/** | material_gallery, theme | 完整 UI 演示 |

### 第 5 步：运行测试

通过运行测试套件验证构建：

**Windows：**
```powershell
.\scripts\build_helpers\windows_run_tests.ps1
```text

**Linux：**
```bash
bash scripts/build_helpers/linux_run_tests.sh
```text

**Docker：**
```bash
bash scripts/build_helpers/docker_start.sh --run-project-test
```bash

---

## Windows 用户

### 使用 Git Bash

CFDesktop 的构建脚本为 Unix 风格的 shell 设计。在 Windows 上，请使用 **Git Bash**：

1. 安装 [Git for Windows](https://git-scm.com/download/win)
2. 从开始菜单打开 Git Bash
3. 导航到项目目录：
   ```bash
   cd /d/ProjectHome/CFDesktop
   ```

### 使用 WSL2（替代方案）

为了获得更好的性能，可以使用 Windows Subsystem for Linux：

1. 安装 WSL2：
   ```powershell
   wsl --install
   ```

2. 从 WSL 中访问 Windows 文件：
   ```bash
   cd /mnt/d/ProjectHome/CFDesktop
   ```

### 路径格式转换

在 Windows 上运行脚本时，请注意路径格式的差异：

| 类型 | 格式 | 示例 |
|:-----|:-----|:-----|
| **Windows** | `D:\Path\To\File` | `D:/ProjectHome/CFDesktop` |
| **Git Bash** | `/d/Path/To/File` | `/d/ProjectHome/CFDesktop` |
| **Docker 挂载** | `/d/Path/To/File` | `/d/ProjectHome/CFDesktop` |
| **PowerShell** | `D:\Path\To\File` | `D:\ProjectHome\CFDesktop` |

构建脚本会自动处理 Docker 挂载的路径转换。

### Windows 构建脚本

对于原生 Windows 构建（不使用 Docker），请使用 PowerShell 脚本：

```powershell
# Configure and build (fast)
.\scripts\build_helpers\windows_fast_develop_build.ps1

# Configure and build (full clean)
.\scripts\build_helpers\windows_develop_build.ps1

# Configure only
.\scripts\build_helpers\windows_configure.ps1

# Run tests
.\scripts\build_helpers\windows_run_tests.ps1
```yaml

---

## 验证

### 检查 Docker 安装

```bash
# Verify Docker is installed
docker --version

# Verify Docker daemon is running
docker info
```text

预期输出：
```text
Docker version 20.10.x
...
Server Version: 20.10.x
```text

### 检查构建是否成功

构建成功后会创建以下目录结构：

```text
out/build_develop/
├── bin/
│   ├── cfbase.dll          # Base library (Windows)
│   ├── cfui.dll            # UI library (Windows)
│   └── ...
├── lib/
│   ├── libcfbase.a         # Static libraries (Linux)
│   └── ...
├── examples/
│   ├── base/               # Base examples
│   ├── ui/                 # UI widget examples
│   └── gui/                # GUI examples
├── runtimes/               # Qt runtime DLLs (Windows)
└── test/                   # Test executables
```text

### 检查构建日志

使用 Docker 时，构建日志保存在 `scripts/docker/logger/` 中：

```bash
# List recent build logs
ls -lt scripts/docker/logger/

# View the latest log
cat scripts/docker/logger/ci_build_*.log | tail -50
```bash

---

## 后续步骤

完成快速开始后：

1. **阅读构建系统文档**：[`03_build_system.md`](03_build_system.md)
2. **探索示例程序**：浏览 `example/` 目录中的示例代码
3. **了解项目结构**：查看[项目骨架设计](../design_stage/00_phase0_project_skeleton.md)
4. **配置开发环境**：参考 [VSCode 配置](../design_stage/00_phase0_project_skeleton.md#五开发环境配置)

---

## 常见问题排查

### Docker 构建失败

| 问题 | 解决方案 |
|:-----|:---------|
| Docker 守护进程未运行 | 启动 Docker Desktop |
| 权限不足 | 在 Linux 上使用 `sudo`，或在 Windows 上以管理员身份运行终端 |
| 端口冲突 | 确保没有其他容器占用所需端口 |
| 内存不足 | 在设置中增加 Docker 内存限制 |

### 构建错误

| 问题 | 解决方案 |
|:-----|---------|
| 未找到 Qt | 设置 `Qt6_DIR` 环境变量，或使用 Docker 构建 |
| 未找到编译器 | 安装 LLVM/Clang 或 GCC，或使用 Docker 构建 |
| CMake 版本过低 | 将 CMake 升级到 3.16+ |
| 缺少依赖 | 使用 Docker 构建，其中包含所有依赖 |

### Windows 特定问题

| 问题 | 解决方案 |
|:-----|---------|
| 路径过长 | 在 Windows 中启用长路径支持，或将项目移到更靠近驱动器根目录的位置 |
| PowerShell 执行策略限制 | 运行 `Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser` |
| Git Bash 路径问题 | 使用 `/d/` 格式表示 D: 盘 |

### 获取帮助

如果遇到本文未涵盖的问题：

1. 查看[设计文档](../../design_stage/)获取详细信息
2. 参考[构建系统文档](03_build_system.md)
3. 在 [GitHub](https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop/issues) 上提交 issue

---

## 快速参考

### 常用命令

```bash
# Clone and setup
git clone https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop.git
cd CFDesktop

# Docker build (fastest)
bash scripts/build_helpers/docker_start.sh --fast-build --build-project-fast

# Run tests
bash scripts/build_helpers/docker_start.sh --run-project-test

# Interactive shell in Docker
bash scripts/build_helpers/docker_start.sh

# Build specific architecture
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify
```bash

### 配置文件

| 文件 | 用途 |
|:-----|:-----|
| `build_develop_config.ini` | 开发构建配置 |
| `build_deploy_config.ini` | 部署构建配置 |
| `build_ci_config.ini` | CI 构建配置 |

### 输出目录

| 构建类型 | 输出目录 |
|:---------|:---------|
| 开发 | `out/build_develop/` |
| 部署 | `out/build_deploy/` |
| CI | `out/build_ci/` |

---

**Last Updated**: 2026-03-07
