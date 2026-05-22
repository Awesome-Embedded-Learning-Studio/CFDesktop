---
title: CFDesktop CI/CD 架构文档
description: CFDesktop 采用多架构 Docker 容器构建方案，确保代码在不同平台上都能正确编译和运行。
---

# CFDesktop CI/CD 架构文档

## 概述

CFDesktop 采用多架构 Docker 容器构建方案，确保代码在不同平台上都能正确编译和运行。

### 核心特性

- **利用现有机制**: 通过 `check_toolchain.cmake` 实现工具链选择
- **多架构支持**: 使用 Docker `--platform` 参数支持 AMD64 和 ARM64 原生编译
- **本地验证**: 通过 Docker 在本地进行多架构构建测试
- **独立配置**: CI 构建与本地开发构建使用独立的配置和目录

## 系统架构

```text
开发者本地                           Docker 容器构建
    │                                      │
    │  修改代码                            │
    │      │                               │
    │  本地测试                            │
    │      │                               │
    ├──────┴───────────────────────────────>│
    │           │                          │
    │           │                     ┌────┴────┐
    │           │                     │ 选择架构 │
    │           │                     └────┬────┘
    │           │                          │
    │           │                    ┌─────┴─────┐
    │           │                    │  启动容器 │
    │           │                    └─────┬─────┘
    │           │                          │
    │           │              ┌───────────┼───────────┐
    │           │              │           │           │
    │           │         ┌────▼───┐  ┌───▼────┐  ┌──▼─────┐
    │           │         │ AMD64  │  │ ARM64  │  │ ARM32  │
    │           │         │ Docker │  │ Docker │  │ Docker │
    │           │         │ + Test │  │ + Test │  │ + Test │
    │           │         └────────┘  └────────┘  └────────┘
    │           │              │           │           │
    │           │              └───────────┼───────────┘
    │           │                          │
    │           │                     构建成功?
    │           │                          │
    │           │                    ┌─────▼─────┐
    │           │                    │ 验证产物  │
    │           │                    └───────────┘
    │           │
    │      继续开发 ✓
    │
    └──> 代码就绪
```text

## 设计理念

### 1. 工具链选择机制

项目使用 [`check_toolchain.cmake`](../../cmake/check_toolchain.cmake) 实现工具链自动选择：

```text
配置: toolchain=linux/ci-x86_64
    ↓
CMake: -DUSE_TOOLCHAIN=linux/ci-x86_64
    ↓
自动解析: cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake
    ↓
设置: CMAKE_TOOLCHAIN_FILE
```bash

### 2. 分离的架构特定工具链

| 架构 | 工具链文件 | Qt 路径 | 配置文件 |
|------|-----------|---------|---------|
| AMD64/x86_64 | ci-x86_64-toolchain.cmake | /opt/Qt/6.8.1/gcc_64 | build_ci_config.ini |
| ARM64/aarch64 | ci-aarch64-toolchain.cmake | /opt/Qt/6.8.1/gcc_arm64 | build_ci_aarch64_config.ini |
| ARM32/armhf | ci-armhf-toolchain.cmake | /opt/Qt/6.8.1/gcc_armhf | build_ci_armhf_config.ini |

### 3. 多架构 Docker 原生编译

| 对比项 | 交叉编译方案 | 多架构容器方案（本方案） |
|--------|---------------|------------------------|
| ARM 测试 | ❌ 无法在 x86_64 主机运行 | ✅ 在 ARM64 容器中运行 |
| 工具链复杂度 | ❌ 需要交叉编译工具链 | ✅ 使用原生工具链 |
| 测试真实性 | ⚠️ 无法验证 ARM 产物 | ✅ 真实 ARM 环境测试 |
| 配置复杂度 | ⚠️ 需要配置 sysroot 等 | ✅ 使用相同的工具链文件 |

## 实施阶段

CI/CD 流水线分为 5 个实施阶段，当前已完成前 3 个阶段：

| 阶段 | 名称 | 说明 | 状态 |
|------|------|------|------|
| Phase 1 | [CI 工具链设置](toolchain-setup.md) | 创建 CI 专用工具链文件 | ✅ 已完成 |
| Phase 2 | [Docker 构建环境](docker-environment.md) | 创建多架构 Dockerfile | ✅ 已完成 |
| Phase 3 | [CI 构建入口](ci-build-entry.md) | 创建统一构建脚本 | ✅ 已完成 |
| Phase 4 | GitHub Actions | 配置自动化工作流 | ⏭️ 跳过（不在远端构建） |
| Phase 5 | 异步合并机制 | 实现 pre-push hook | ⏳ 待实施 |

## 文档导航

### Phase 1: CI 工具链设置
- **[CI 工具链设置指南](toolchain-setup.md)** - 工具链文件设计和使用说明

### Phase 2: Docker 构建环境
- **[Docker 构建环境设置指南](docker-environment.md)** - Docker 镜像和使用说明

### Phase 3: CI 构建入口
- **[CI 构建入口设置指南](ci-build-entry.md)** - 构建脚本和配置说明

## 相关文件

### 工具链文件

| 文件路径 | 说明 |
|----------|------|
| [cmake/check_toolchain.cmake](../../cmake/check_toolchain.cmake) | 工具链选择机制 |
| [cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake](../../cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake) | AMD64 CI 工具链 |
| [cmake/cmake_toolchain/linux/ci-aarch64-toolchain.cmake](../../cmake/cmake_toolchain/linux/ci-aarch64-toolchain.cmake) | ARM64 CI 工具链 |
| [cmake/cmake_toolchain/linux/ci-armhf-toolchain.cmake](../../cmake/cmake_toolchain/linux/ci-armhf-toolchain.cmake) | ARM32 CI 工具链 (IMX6ULL) |

### 配置文件

| 文件路径 | 说明 |
|----------|------|
| [scripts/build_helpers/build_ci_config.ini](../../scripts/build_helpers/build_ci_config.ini) | AMD64 CI 配置 |
| [scripts/build_helpers/build_ci_aarch64_config.ini](../../scripts/build_helpers/build_ci_aarch64_config.ini) | ARM64 CI 配置 |
| [scripts/build_helpers/build_ci_armhf_config.ini](../../scripts/build_helpers/build_ci_armhf_config.ini) | ARM32 CI 配置 |

### 脚本文件

| 文件路径 | 说明 |
|----------|------|
| [scripts/build_helpers/ci_build_entry.sh](../../scripts/build_helpers/ci_build_entry.sh) | CI 构建入口脚本 |
| [scripts/build_helpers/docker_start.sh](../../scripts/build_helpers/docker_start.sh) | Linux Docker 启动脚本 |
| [scripts/build_helpers/docker_start.ps1](../../scripts/build_helpers/docker_start.ps1) | Windows Docker 启动脚本 |
| [scripts/dependency/install_build_dependencies.sh](../../scripts/dependency/install_build_dependencies.sh) | 依赖安装脚本 |

### Docker 文件

| 文件路径 | 说明 |
|----------|------|
| [scripts/docker/Dockerfile.build](../../scripts/docker/Dockerfile.build) | 多架构 Docker 镜像定义 |
| [scripts/docker/docker-compose.yml](../../scripts/docker/docker-compose.yml) | Docker Compose 配置 |
| [scripts/docker/.dockerignore](../../scripts/docker/.dockerignore) | Docker 忽略规则 |

## 快速开始

### Docker 快速验证

```bash
# AMD64 构建
bash scripts/build_helpers/docker_start.sh --verify

# ARM64 构建
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify
```text

### 直接运行构建

```bash
# 在 Linux 环境中直接运行
bash scripts/build_helpers/ci_build_entry.sh ci
```text

### 查看完整文档

```bash
# 查看完整的流水线设计文档
cat PIPELINE.md
```yaml

---

*文档版本: v2.0 | 最后更新: 2026-03-07*
