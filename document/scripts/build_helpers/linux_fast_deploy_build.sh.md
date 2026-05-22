---
title: linuxfastdeploybuild.sh
description: "文档编写日期: 2026-03-20，是快速部署构建脚本，执行配置 + 构建流程，不清理构建目录，支"
---

# linux_fast_deploy_build.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/linux_fast_deploy_build.sh [develop|deploy|ci] [-c|--config <config_file>]
```bash

### 参数说明

| 参数 | 说明 |
|------|------|
| `develop` | 使用开发配置 |
| `deploy` | 使用部署配置（默认） |
| `ci` | 使用CI配置 |
| `-c, --config <file>` | 使用自定义配置文件 |
| `-h, --help` | 显示帮助信息 |

## Scripts详解

`linux_fast_deploy_build.sh` 是快速部署构建脚本，执行**配置 + 构建**流程，不清理构建目录，支持增量编译。

### 功能特点

1. **增量编译** - 不清理构建目录，利用CMake的增量编译能力
2. **两步构建** - 配置和构建
3. **并行构建** - 支持多核并行编译
4. **部署优化** - 使用部署配置（Release模式）

### 与 linux_fast_develop_build.sh 的区别

| 特性 | linux_fast_deploy_build.sh | linux_fast_develop_build.sh |
|------|----------------------------|----------------------------|
| 默认配置模式 | deploy | develop |
| 构建类型 | Release | Debug |
| 默认配置文件 | build_deploy_config.ini | build_develop_config.ini |
| 优化级别 | 高（生产部署） | 低（开发调试） |
| 调试信息 | 无/最少 | 完整 |

### 执行流程

#### 第一步：CMake配置

```bash
Step 1: Configuring with CMake
```text

调用 `linux_configure.sh` 执行CMake配置。

#### 第二步：构建项目

```bash
Step 2: Building project
```text

使用CMake构建项目。如果配置了并行任务数，会使用 `--parallel` 参数加速编译。

### 使用示例

```bash
# 使用默认部署配置
./scripts/build_helpers/linux_fast_deploy_build.sh deploy

# 使用开发配置
./scripts/build_helpers/linux_fast_deploy_build.sh develop

# 使用CI配置
./scripts/build_helpers/linux_fast_deploy_build.sh ci

# 使用自定义配置文件
./scripts/build_helpers/linux_fast_deploy_build.sh deploy -c my_config.ini
```text

### 输出示例

```bash
========================================
Starting Linux FAST Build Process (Deploy)
========================================
Project root: /home/charliechen/CFDesktop
========================================
Step 1: Configuring with CMake
========================================
Executing: linux_configure.sh deploy
...
Configuration completed successfully!
========================================
Step 2: Building project
========================================
Command: cmake --build build_deploy --parallel 4
...
```text

### 配置参数

脚本从配置文件的 `[options]` 部分读取并行任务数：

```ini
[options]
jobs=4
```text

如果未设置，则不使用并行参数。

### 使用场景

1. **部署迭代** - 部署版本的快速迭代编译
2. **性能验证** - 快速验证优化后的代码
3. **增量部署** - 只修改了部分代码的部署构建
4. **时间紧张的部署** - 需要快速获得部署版本

### 依赖脚本

该脚本依赖以下脚本和库：

1. `lib_common.sh` - 通用日志函数
2. `lib_config.sh` - 配置文件处理
3. `lib_paths.sh` - 路径解析
4. `linux_configure.sh` - CMake配置脚本

### 注意事项

1. 不清理构建目录，可能导致旧文件残留
2. 不运行测试，需要单独执行 `linux_run_tests.sh`
3. Release模式下调试信息有限
4. 首次构建或配置有重大变化时，建议使用 `linux_deploy_build.sh`

### 推荐工作流程

```bash
# 首次部署构建或需要清理时
./scripts/build_helpers/linux_deploy_build.sh

# 部署版本快速迭代
./scripts/build_helpers/linux_fast_deploy_build.sh

# 需要测试时
./scripts/build_helpers/linux_run_tests.sh deploy
```text
