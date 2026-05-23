---
title: 构建辅助脚本
description: 本目录包含平台相关的构建辅助脚本，提供 Linux（Bash）和 Windows（PowerShell）双平台的 CMake 配置、快速开发构建、完整构建以及测试运行等标准化构建流程。同时提供 Docker 环境下的构建脚本以确保跨平台一致性。
---

# 构建辅助脚本

本目录包含 CFDesktop 项目的构建辅助脚本，提供 Linux（Bash）和 Windows（PowerShell）双平台的 CMake 配置、快速开发构建、完整构建以及测试运行等标准化构建流程。同时提供 Docker 环境下的构建脚本以确保跨平台一致性。

## Linux脚本

| 脚本 | 说明 |
|------|------|
| ci_build_entry.sh | CI构建入口 |
| linux_configure.sh | CMake配置 |
| linux_develop_build.sh | 完整开发构建 |
| linux_fast_develop_build.sh | 快速开发构建 |
| linux_deploy_build.sh | 完整部署构建 |
| linux_fast_deploy_build.sh | 快速部署构建 |
| linux_run_tests.sh | 运行测试 |
| docker_start.sh | Docker启动脚本 |

## 构建类型

| 类型 | 说明 | 配置文件 |
|------|------|----------|
| develop | 开发构建，包含调试符号 | build_develop_config.ini |
| deploy | 部署构建，优化体积 | build_deploy_config.ini |
| fast_develop | 快速开发构建，增量编译 | build_develop_config.ini |
| fast_deploy | 快速部署构建，增量编译 | build_deploy_config.ini |

## 配置文件

| 文件 | 说明 |
|------|------|
| build_develop_config.ini | 开发构建配置 |
| build_deploy_config.ini | 部署构建配置 |
| build_ci_config.ini | CI构建配置 |
| build_ci_aarch64_config.ini | ARM64 CI配置 |
| build_ci_armhf_config.ini | ARM HF CI配置 |

## 快速开始

### 开发构建

```bash
# 完整开发构建（清理后构建）
./scripts/build_helpers/linux_develop_build.sh

# 快速开发构建（增量编译）
./scripts/build_helpers/linux_fast_develop_build.sh
```

### 部署构建

```bash
# 完整部署构建（清理后构建）
./scripts/build_helpers/linux_deploy_build.sh

# 快速部署构建（增量编译）
./scripts/build_helpers/linux_fast_deploy_build.sh
```

### 仅配置

```bash
./scripts/build_helpers/linux_configure.sh [develop|deploy|ci]
```

### 运行测试

```bash
./scripts/build_helpers/linux_run_tests.sh [develop|deploy|ci]
```

### Docker构建

```bash
# 交互式shell
./scripts/build_helpers/docker_start.sh

# CI构建验证
./scripts/build_helpers/docker_start.sh --verify

# 构建项目（完整清理）
./scripts/build_helpers/docker_start.sh --build-project

# 构建项目（快速）
./scripts/build_helpers/docker_start.sh --build-project-fast

# 运行测试
./scripts/build_helpers/docker_start.sh --run-project-test
```

## 架构支持

脚本支持多架构构建：

| 架构 | 平台 | 说明 |
|------|------|------|
| x86_64 / amd64 | linux/amd64 | 标准PC架构 |
| aarch64 | linux/arm64 | ARM64架构 |
| armv7l / armhf | linux/armhf | ARM32架构 (IMX6ULL) |

CI构建脚本会自动检测容器架构并选择相应的配置文件。
