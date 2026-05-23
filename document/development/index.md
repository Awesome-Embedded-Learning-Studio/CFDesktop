---
title: 开发指南
description: 本节面向重新启动开发时的日常入口。
---

# 开发指南

本节面向重新启动开发时的日常入口。

## 推荐阅读顺序

1. [当前项目状态](../status/current.md)
2. [前置环境](01_prerequisites.md)
3. [快速开始](02_quick_start.md)
4. [构建系统](03_build_system.md)
5. [开发工具](04_development_tools.md)
6. [Git Hooks](06_git_hooks.md)
7. [AI 辅助开发指南](ai-assistant-guide.md)

## 常用命令

```bash
# Linux 快速构建
bash scripts/build_helpers/linux_fast_develop_build.sh

# Linux 完整构建 + 测试
bash scripts/build_helpers/linux_develop_build.sh

# 仅运行已有 CTest
QT_QPA_PLATFORM=offscreen ctest --test-dir out/build_develop/test --output-on-failure

# 文档开发
pnpm install
pnpm dev
pnpm build
```

## 环境要求速览

### 硬件要求

| 组件 | 最低配置 | 推荐配置 |
|:---|:---:|:---:|
| **CPU** | 4 核心 | 8 核心以上 |
| **RAM** | 8GB | 16GB 或更多 |
| **硬盘** | 20GB 可用空间 | 50GB+ SSD |

### 操作系统支持

| 平台 | 支持版本 | 工具链 |
|:---|:---|:---|
| **Windows** | Windows 10/11 | MinGW 或 LLVM |
| **Linux** | Ubuntu 22.04+, Debian 12+ | GCC 或 Clang |

### 必需软件

| 软件 | 最低版本 | 推荐版本 |
|:---|:---:|:---:|
| **Docker Desktop** | 最新稳定版 | 最新版 |
| **Git** | 2.30+ | 最新版 |
| **VSCode** | (推荐) 最新版 | 最新版 |
| **Qt6** | 6.8.3 | 6.8.3+ |
| **CMake** | 3.16 | 3.20+ |
| **Python** | 3.8+ | 3.10+ (用于 aqtinstall) |

## 快速克隆与构建

```bash
# 1. 克隆项目
git clone https://github.com/Charliechen114514/CFDesktop.git
cd CFDesktop

# 2. Windows 快速构建
.\scripts\build_helpers\windows_fast_develop_build.ps1

# 3. Linux 快速构建
./scripts/build_helpers/linux_fast_develop_build.sh
```
