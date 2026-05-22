---
title: linuxfastdevelopbuild.sh
description: "文档编写日期: 2026-03-20，是快速开发构建脚本，执行配置 + 构建流程，不清理构建目录，支"
---

# linux_fast_develop_build.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/linux_fast_develop_build.sh [develop|deploy|ci] [-c|--config <config_file>]
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

`linux_fast_develop_build.sh` 是快速开发构建脚本，执行**配置 + 构建**流程，不清理构建目录，支持增量编译。

### 功能特点

1. **增量编译** - 不清理构建目录，利用CMake的增量编译能力
2. **两步构建** - 配置和构建
3. **并行构建** - 支持多核并行编译
4. **快速迭代** - 适合开发过程中的频繁编译

### 执行流程

#### 第一步：CMake配置

```bash
Step 1: Configuring with CMake
```text

调用 `linux_configure.sh` 执行CMake配置。如果构建目录已存在且有有效配置，此步骤会很快完成。

#### 第二步：构建项目

```bash
Step 2: Building project
```text

使用CMake构建项目。如果配置了并行任务数，会使用 `--parallel` 参数加速编译。

### 使用示例

```bash
# 使用默认开发配置
./scripts/build_helpers/linux_fast_develop_build.sh develop

# 使用部署配置
./scripts/build_helpers/linux_fast_develop_build.sh deploy

# 使用CI配置
./scripts/build_helpers/linux_fast_develop_build.sh ci

# 使用自定义配置文件
./scripts/build_helpers/linux_fast_develop_build.sh develop -c my_config.ini
```text

### 输出示例

```bash
========================================
Starting Linux FAST Build Process
========================================
Project root: /home/charliechen/CFDesktop
========================================
Step 1: Configuring with CMake
========================================
Executing: linux_configure.sh develop
...
Configuration completed successfully!
========================================
Step 2: Building project
========================================
Command: cmake --build build_develop --parallel 4
...
```text

### 配置参数

脚本从配置文件的 `[options]` 部分读取并行任务数：

```ini
[options]
jobs=4
```bash

如果未设置，则不使用并行参数。

### 与完整构建的对比

| 特性 | linux_fast_develop_build.sh | linux_develop_build.sh |
|------|----------------------------|------------------------|
| 清理构建目录 | 否 | 是 |
| 增量编译 | 是 | 否 |
| 构建速度 | 快 | 慢 |
| 运行测试 | 否 | 是 |
| 适用场景 | 快速迭代 | 首次构建、清理构建 |

### 使用场景

1. **快速迭代** - 开发过程中的频繁编译
2. **单文件修改** - 只修改了少数几个文件
3. **调试编译** - 快速验证代码修改
4. **时间紧张** - 需要快速获得可执行文件

### 依赖脚本

该脚本依赖以下脚本和库：

1. `lib_common.sh` - 通用日志函数
2. `lib_config.sh` - 配置文件处理
3. `lib_paths.sh` - 路径解析
4. `linux_configure.sh` - CMake配置脚本

### 注意事项

1. 不清理构建目录，可能导致旧文件残留
2. 不运行测试，需要单独执行 `linux_run_tests.sh`
3. 如果CMake配置有重大变化，可能需要先清理构建目录
4. 增量编译速度取决于修改的文件数量

### 推荐工作流程

```bash
# 首次构建或需要清理时
./scripts/build_helpers/linux_develop_build.sh

# 日常开发迭代
./scripts/build_helpers/linux_fast_develop_build.sh

# 需要测试时
./scripts/build_helpers/linux_run_tests.sh
```text
