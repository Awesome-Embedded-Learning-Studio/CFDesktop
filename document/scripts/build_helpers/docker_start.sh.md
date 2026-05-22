---
title: dockerstart.sh
description: "文档编写日期: 2026-03-20，是CFDesktop项目的Docker构建环境包装脚本，提供C"
---

# docker_start.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
bash scripts/build_helpers/docker_start.sh [options]
```bash

### 参数说明

| 参数 | 说明 |
|------|------|
| `--arch amd64\|arm64` | 目标架构（默认：amd64） |
| `--fast-build` | 跳过镜像清理，复用现有镜像 |
| `--verify` | 运行CI构建验证而非交互式shell |
| `--build-project` | 构建镜像 + 运行完整清理构建 |
| `--build-project-fast` | 构建镜像 + 运行快速构建 |
| `--run-project-test` | 构建镜像 + 运行测试 |
| `--stay-on-error` | CI失败时保持在容器中以便调试 |
| `--no-log` | 禁用文件日志 |
| `--no-deps` | 跳过镜像中的依赖安装 |
| `--help` | 显示帮助信息 |

## Scripts详解

`docker_start.sh` 是CFDesktop项目的Docker构建环境包装脚本，提供CI风格的构建体验。

### 功能特点

1. **多架构支持** - 支持amd64和arm64架构
2. **多种运行模式** - 交互式、CI验证、构建、测试
3. **美观的日志输出** - 彩色日志和Unicode符号
4. **自动日志记录** - 可选的文件日志记录
5. **QEMU支持** - 在x86主机上构建ARM64镜像

### 架构支持

| 架构 | Docker平台 | 说明 |
|------|-------------|------|
| amd64 | linux/amd64 | 原生x86_64构建 |
| arm64 | linux/arm64 | ARM64原生容器（x86上使用QEMU仿真） |

### 运行模式

#### 交互式模式（默认）

```bash
bash scripts/build_helpers/docker_start.sh
```text

启动一个交互式bash shell，可以手动执行命令。

#### CI验证模式

```bash
bash scripts/build_helpers/docker_start.sh --verify
```text

在容器中运行完整的CI构建流程。

#### 构建项目模式

```bash
# 完整清理构建
bash scripts/build_helpers/docker_start.sh --build-project

# 快速构建
bash scripts/build_helpers/docker_start.sh --build-project-fast
```text

#### 测试模式

```bash
bash scripts/build_helpers/docker_start.sh --run-project-test
```text

### 使用示例

```bash
# 基本交互式shell
bash scripts/build_helpers/docker_start.sh

# ARM64构建
bash scripts/build_helpers/docker_start.sh --arch arm64

# 快速构建（复用镜像）
bash scripts/build_helpers/docker_start.sh --fast-build

# CI构建验证
bash scripts/build_helpers/docker_start.sh --verify

# CI构建 + 失败时调试
bash scripts/build_helpers/docker_start.sh --verify --stay-on-error

# 禁用日志
bash scripts/build_helpers/docker_start.sh --no-log

# 跳过依赖安装（手动设置）
bash scripts/build_helpers/docker_start.sh --no-deps
```text

### 输出示例

```text

══════════════════════════════════════════════════════════════════════════
CFDesktop Docker Build Environment
══════════════════════════════════════════════════════════════════════════

[●] CONFIG - Build parameters
  → Architecture: amd64
  → Platform: linux/amd64
  → Image: cfdesktop-build
  → Fast build: false
  → Project root: /home/charliechen/CFDesktop

[●] CHECK - Verifying Docker
  ✓ Docker: 24.0.7
  ✓ Docker daemon is running

[●] CLEAN - Removing old image: cfdesktop-build
  ✓ Old image removed

[●] BUILD - Building fresh image: cfdesktop-build
  ▸ [1/10] FROM docker.io/library/ubuntu:22.04
  ...
  ✓ Successfully built cfdesktop-build

[●] RUN - Starting interactive shell
  → Type 'exit' to leave the container
```bash

### Docker镜像

脚本使用 `scripts/docker/Dockerfile.build` 构建镜像：

- **镜像名称**: `cfdesktop-build`
- **基础镜像**: Ubuntu 22.04
- **工作目录**: `/project`

### 构建参数

脚本会根据目标架构设置QT_ARCH构建参数：

| 架构 | QT_ARCH值 |
|------|-----------|
| amd64 | linux_gcc_64 |
| arm64 | linux_gcc_arm64 |
| armhf | linux_gcc_arm64 |

### 日志功能

日志默认保存在 `scripts/docker/logger/` 目录：

```text
scripts/docker/logger/ci_build_YYYYMMDD_HHMMSS.log
```text

日志包含：
- 开始时间
- 架构信息
- 完整构建输出
- 结束时间

### 调试功能

使用 `--stay-on-error` 选项可以在CI失败时保持容器打开：

```bash
bash scripts/build_helpers/docker_start.sh --verify --stay-on-error
```text

失败后会显示：

```text
=== Build failed, staying in container for debugging ===
Type "exit" to leave the container
```bash

### 路径处理

脚本会自动处理不同操作系统的路径转换：

- **Linux/Mac**: 直接使用原路径
- **Windows (Git Bash/MSYS)**: 自动转换为Docker兼容路径

### 退出码

| 退出码 | 说明 |
|--------|------|
| 0 | 成功 |
| 1 | Docker未安装或未运行 |
| 1 | 构建失败 |
| 测试退出码 | 测试失败时的原始退出码 |

### 使用场景

1. **跨平台构建** - 在统一环境中构建不同架构
2. **CI/CD** - 自动化构建流水线
3. **隔离构建** - 避免污染主机环境
4. **多架构测试** - 测试不同架构的兼容性

### 注意事项

1. 需要预先安装Docker
2. ARM64构建在x86主机上需要QEMU（Docker自动处理）
3. 默认模式会清理旧镜像，使用 `--fast-build` 复用
4. Windows用户需要注意路径转换问题
5. 首次构建镜像可能需要较长时间

### 与CI集成

脚本设计用于与CI系统集成：

```yaml
# .github/workflows/ci.yml 示例
- name: Build in Docker
  run: bash scripts/build_helpers/docker_start.sh --verify
```text

或调用测试入口：

```yaml
- name: Run Tests
  run: bash scripts/build_helpers/docker_start.sh --run-project-test
```text
