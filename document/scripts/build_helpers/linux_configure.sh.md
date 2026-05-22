---
title: linuxconfigure.sh
description: "文档编写日期: 2026-03-20，是专门用于执行CMake配置的脚本，不执行构建。该脚本读取配置"
---

# linux_configure.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/linux_configure.sh [develop|deploy|ci] [-c|--config <config_file>]
```bash

### 参数说明

| 参数 | 说明 |
|------|------|
| `develop` | 使用开发配置（默认） |
| `deploy` | 使用部署配置 |
| `ci` | 使用CI配置 |
| `-c, --config <file>` | 使用自定义配置文件 |
| `-h, --help` | 显示帮助信息 |

## Scripts详解

`linux_configure.sh` 是专门用于执行CMake配置的脚本，**不执行构建**。该脚本读取配置文件中的参数，调用CMake生成构建系统。

### 功能特点

1. **仅配置不构建** - 只执行CMake配置阶段
2. **多配置支持** - 支持开发、部署、CI三种配置模式
3. **路径安全检查** - 验证配置文件和路径的有效性
4. **性能诊断** - 显示CMake版本和生成器信息

### 配置参数

脚本从配置文件的 `[cmake]` 和 `[paths]` 部分读取参数：

| 配置项 | 说明 | 示例 |
|--------|------|------|
| `config_cmake_generator` | CMake生成器 | "Ninja" / "Unix Makefiles" |
| `config_cmake_toolchain` | 工具链文件路径 | "toolchain/linux-gcc.cmake" |
| `config_cmake_build_type` | 构建类型 | "Debug" / "Release" / "RelWithDebInfo" |
| `config_paths_source` | 源码目录 | "." |
| `config_paths_build_dir` | 构建目录 | "build_develop" |

### 支持的构建类型

| 构建类型 | 说明 |
|----------|------|
| `Debug` | 调试版本，包含完整调试信息 |
| `Release` | 发布版本，优化性能 |
| `RelWithDebInfo` | 优化版本但保留调试信息 |

### 使用示例

```bash
# 使用默认开发配置
./scripts/build_helpers/linux_configure.sh develop

# 使用部署配置
./scripts/build_helpers/linux_configure.sh deploy

# 使用CI配置
./scripts/build_helpers/linux_configure.sh ci

# 使用自定义配置文件
./scripts/build_helpers/linux_configure.sh deploy -c my_config.ini
```text

### 执行流程

1. **解析参数** - 解析命令行参数，确定配置模式和配置文件
2. **加载配置** - 从INI配置文件读取CMake参数
3. **验证配置** - 检查配置文件存在性和参数有效性
4. **解析路径** - 处理相对路径和绝对路径
5. **执行CMake** - 运行CMake配置命令
6. **显示结果** - 报告配置成功或失败

### 输出示例

```bash
========================================
Starting Linux CMake Configuration
Configuration: develop
========================================
Project root: /home/charliechen/CFDesktop
Changing to project directory
Loading configuration from: build_develop_config.ini
Configuration loaded successfully!
Generator: Ninja
Toolchain: toolchain/linux-gcc.cmake
Build Type: Debug
Source directory: . (resolved: /home/charliechen/CFDesktop)
Build directory: build_develop
========================================
Configuring with CMake (NO BUILD)
Command: cmake -G Ninja -DUSE_TOOLCHAIN=toolchain/linux-gcc.cmake -DCMAKE_BUILD_TYPE=Debug -S /home/charliechen/CFDesktop -B build_develop
========================================
=== Performance Diagnostics ===
CMake Version: cmake version 3.x.x
Generator: Ninja

Running CMake configuration...
...
========================================
CMake configuration completed successfully!
To build the project, run: cmake --build build_develop
========================================
```bash

### 错误处理

| 错误 | 原因 | 解决方案 |
|------|------|----------|
| 配置文件不存在 | 配置文件未创建 | 从模板复制配置文件 |
| build_type无效 | 不支持的构建类型 | 使用 Debug/Release/RelWithDebInfo |
| build_dir为空 | 配置文件中未设置 | 在配置文件中设置 build_dir |

### 后续步骤

配置成功后，可以执行：

```bash
# 构建项目
cmake --build build_develop

# 或使用构建脚本
./scripts/build_helpers/linux_fast_develop_build.sh
```text

### 注意事项

1. 配置文件必须存在于 `scripts/build_helpers/` 目录或提供绝对路径
2. 构建目录不能设置为项目根目录或系统根目录（安全检查）
3. 相对路径会相对于项目根目录解析
