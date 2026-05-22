---
title: linuxdeploybuild.sh
description: "文档编写日期: 2026-03-20，是完整的部署构建脚本，执行清理 + 配置 + 构建 + 测试的"
---

# linux_deploy_build.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/linux_deploy_build.sh [develop|deploy|ci] [-c|--config <config_file>]
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

`linux_deploy_build.sh` 是完整的部署构建脚本，执行**清理 + 配置 + 构建 + 测试**的全流程，专用于生产环境部署。

### 功能特点

1. **完整清理构建** - 清除旧构建目录后重新构建
2. **三步构建流程** - 清理、构建、测试
3. **部署优化** - 使用部署配置（Release模式）
4. **自动测试** - 构建完成后自动运行测试

### 与 linux_develop_build.sh 的区别

| 特性 | linux_deploy_build.sh | linux_develop_build.sh |
|------|------------------------|------------------------|
| 默认配置模式 | deploy | develop |
| 构建类型 | Release | Debug |
| 默认配置文件 | build_deploy_config.ini | build_develop_config.ini |
| 优化级别 | 高（生产部署） | 低（开发调试） |
| 调试信息 | 无/最少 | 完整 |

### 执行流程

#### 第一步：清理构建目录

```bash
Step 1: Cleaning build directory
```text

使用 `lib_build.sh` 中的 `clean_build_dir` 函数清理构建目录。

#### 第二步：调用快速部署构建脚本

```bash
Step 2: Calling fast build script
```text

调用 `linux_fast_deploy_build.sh` 执行实际的配置和构建。

#### 第三步：运行测试

```bash
Step 3: Running tests
```text

调用 `linux_run_tests.sh` 运行所有测试。测试失败不会导致脚本退出（仅警告）。

### 使用示例

```bash
# 使用默认部署配置
./scripts/build_helpers/linux_deploy_build.sh deploy

# 使用开发配置
./scripts/build_helpers/linux_deploy_build.sh develop

# 使用CI配置
./scripts/build_helpers/linux_deploy_build.sh ci

# 使用自定义配置文件
./scripts/build_helpers/linux_deploy_build.sh deploy -c my_deploy_config.ini
```text

### 输出示例

```text
========================================
Starting Linux Build Process (Full Clean + Build)
========================================
Project root: /home/charliechen/CFDesktop
Changing to project directory
Loading configuration from: build_deploy_config.ini
Configuration loaded successfully!
Source directory: . (resolved: /home/charliechen/CFDesktop)
Build directory: build_deploy
========================================
Step 1: Cleaning build directory
========================================
...
========================================
Step 2: Calling fast build script
========================================
Executing: linux_fast_deploy_build.sh deploy
...
========================================
Build process completed successfully!
========================================
========================================
Step 3: Running tests
========================================
Executing: linux_run_tests.sh deploy
...
========================================
All tests passed successfully!
========================================
```text

### 部署配置示例

`build_deploy_config.ini` 通常包含以下配置：

```ini
[cmake]
generator = Ninja
toolchain = toolchain/linux-gcc.cmake
build_type = Release

[paths]
source = .
build_dir = build_deploy

[options]
jobs = 4
```bash

### 使用场景

1. **生产发布** - 构建用于生产环境的版本
2. **性能测试** - 需要测试优化后的代码性能
3. **交付客户** - 构建最终交付版本
4. **CI/CD** - 自动化部署流水线

### 依赖脚本

该脚本依赖以下脚本和库：

1. `lib_common.sh` - 通用日志函数
2. `lib_config.sh` - 配置文件处理
3. `lib_build.sh` - 构建相关函数（clean_build_dir）
4. `lib_paths.sh` - 路径解析
5. `linux_fast_deploy_build.sh` - 快速部署构建脚本
6. `linux_run_tests.sh` - 测试脚本

### 错误处理

| 错误 | 原因 | 解决方案 |
|------|------|----------|
| 配置文件不存在 | 部署配置文件未创建 | 从模板复制配置文件 |
| build_dir为空 | 配置文件中未设置 | 在配置文件中设置 build_dir |
| 构建失败 | 编译错误 | 检查编译错误日志 |
| 测试失败 | 测试用例失败 | 检查测试日志 |

### 注意事项

1. 部署构建使用 Release 模式，调试信息有限
2. 构建目录会被完全清除
3. 测试失败不会中断构建流程，但需要关注测试结果
4. 默认使用 `deploy` 配置模式
