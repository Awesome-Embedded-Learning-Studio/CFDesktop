---
title: linuxdevelopbuild.sh
description: "文档编写日期: 2026-03-20，是完整的开发构建脚本，执行清理 + 配置 + 构建 + 测试的"
---

# linux_develop_build.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/linux_develop_build.sh [develop|deploy|ci] [-c|--config <config_file>]
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

`linux_develop_build.sh` 是完整的开发构建脚本，执行**清理 + 配置 + 构建 + 测试**的全流程。

### 功能特点

1. **完整清理构建** - 清除旧构建目录后重新构建
2. **三步构建流程** - 清理、构建、测试
3. **安全检查** - 验证配置文件和路径有效性
4. **自动测试** - 构建完成后自动运行测试

### 执行流程

#### 第一步：清理构建目录

```bash
Step 1: Cleaning build directory
```text

使用 `lib_build.sh` 中的 `clean_build_dir` 函数清理构建目录。这确保每次都是干净的构建。

#### 第二步：调用快速构建脚本

```bash
Step 2: Calling fast build script
```text

调用 `linux_fast_develop_build.sh` 执行实际的配置和构建。

#### 第三步：运行测试

```bash
Step 3: Running tests
```text

调用 `linux_run_tests.sh` 运行所有测试。测试失败不会导致脚本退出（仅警告）。

### 使用示例

```bash
# 使用默认开发配置
./scripts/build_helpers/linux_develop_build.sh develop

# 使用部署配置
./scripts/build_helpers/linux_develop_build.sh deploy

# 使用CI配置
./scripts/build_helpers/linux_develop_build.sh ci

# 使用自定义配置文件
./scripts/build_helpers/linux_develop_build.sh develop -c my_config.ini
```text

### 输出示例

```text
========================================
Starting Linux Build Process (Full Clean + Build)
========================================
Project root: /home/charliechen/CFDesktop
Changing to project directory
Loading configuration from: build_develop_config.ini
Configuration loaded successfully!
Source directory: . (resolved: /home/charliechen/CFDesktop)
Build directory: build_develop
========================================
Step 1: Cleaning build directory
========================================
...
========================================
Step 2: Calling fast build script
========================================
Executing: linux_fast_develop_build.sh develop
...
========================================
Build process completed successfully!
========================================
========================================
Step 3: Running tests
========================================
Executing: linux_run_tests.sh develop
...
========================================
All tests passed successfully!
========================================
```bash

### 与快速构建的对比

| 特性 | linux_develop_build.sh | linux_fast_develop_build.sh |
|------|------------------------|----------------------------|
| 清理构建目录 | 是 | 否 |
| 配置项目 | 是 | 是 |
| 构建项目 | 是 | 是 |
| 运行测试 | 是 | 否 |
| 适用场景 | 首次构建、需要清理 | 增量编译、快速迭代 |

### 错误处理

| 错误 | 原因 | 解决方案 |
|------|------|----------|
| 配置文件不存在 | 配置文件未创建 | 从模板复制配置文件 |
| build_dir为空 | 配置文件中未设置 | 在配置文件中设置 build_dir |
| build_dir为项目根 | 安全检查失败 | 修改配置文件中的 build_dir |
| 构建失败 | 编译错误 | 检查编译错误日志 |

### 依赖脚本

该脚本依赖以下脚本和库：

1. `lib_common.sh` - 通用日志函数
2. `lib_config.sh` - 配置文件处理
3. `lib_build.sh` - 构建相关函数（clean_build_dir）
4. `lib_paths.sh` - 路径解析
5. `linux_fast_develop_build.sh` - 快速构建脚本
6. `linux_run_tests.sh` - 测试脚本

### 使用场景

1. **首次构建** - 新获取代码后的首次完整构建
2. **清理构建** - 需要确保干净构建环境
3. **CI/CD** - 自动化流水线中的完整构建
4. **问题排查** - 排除增量编译导致的问题

### 注意事项

1. 构建目录会被完全清除，请确保没有重要文件
2. 测试失败不会中断构建流程
3. 默认使用 `develop` 配置模式
