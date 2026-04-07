# Docker 构建环境设置指南

**状态**: ✅ 已完成
**完成日期**: 2026-03-07

## 概述

本部分创建了多架构 Docker 构建环境，支持 AMD64 和 ARM64 平台的原生编译和测试。

## 设计目标

1. **多架构支持**: 通过 `--platform` 参数支持 AMD64 和 ARM64
2. **环境一致性**: 本地 Docker 与 CI 环境完全一致
3. **独立配置**: 不依赖本地开发环境
4. **易于使用**: 提供便捷的启动脚本

## 文件结构

```
scripts/
├── docker/
│   ├── Dockerfile.build          # 多架构 Docker 镜像定义
│   ├── .dockerignore             # Docker 忽略规则
│   └── docker-compose.yml        # Docker Compose 配置
└── build_helpers/
    ├── docker_start.sh           # Linux 启动脚本
    └── docker_start.ps1          # Windows 启动脚本
```

## Docker 镜像

### Dockerfile.build

**文件**: `scripts/docker/Dockerfile.build`

**基础镜像**: Ubuntu 24.04

**主要特性**:
- 自动调用 `install_build_dependencies.sh` 安装依赖
- 使用 aqtinstall 安装 Qt 6.8.1（支持国内镜像加速）
- 支持 `INSTALL_DEPS=0` 跳过依赖安装（用于调试）
- 自动配置 Python 虚拟环境

**构建参数**:
```dockerfile
ARG BASE_IMAGE=ubuntu:24.04     # 基础镜像
ARG QT_VERSION=6.8.1            # Qt 版本
ARG QT_ARCH=linux_gcc_64        # Qt 架构 (gcc_64/gcc_arm64)
ARG QT_MIRROR=                  # Qt 镜像源
ARG INSTALL_DEPS=1              # 是否安装依赖
```

**构建镜像**:
```bash
# AMD64
docker build --platform linux/amd64 \
  -f scripts/docker/Dockerfile.build \
  -t cfdesktop-build .

# ARM64
docker build --platform linux/arm64 \
  -f scripts/docker/Dockerfile.build \
  --build-arg QT_ARCH=linux_gcc_arm64 \
  -t cfdesktop-build:arm64 .
```

## Docker Compose 配置

**文件**: `scripts/docker/docker-compose.yml`

**服务定义**:
```yaml
services:
  build-amd64:   # AMD64 构建环境
  build-arm64:   # ARM64 构建环境
  verify:        # 快速验证服务
```

**使用方式**:
```bash
# 构建所有服务
docker-compose -f scripts/docker/docker-compose.yml build

# 运行 AMD64 构建
docker-compose -f scripts/docker/docker-compose.yml run build-amd64

# 运行 ARM64 构建
docker-compose -f scripts/docker/docker-compose.yml run build-arm64

# 运行验证
docker-compose -f scripts/docker/docker-compose.yml run verify
```

## 启动脚本

### Linux: docker_start.sh

**文件**: `scripts/build_helpers/docker_start.sh`

**功能**:
- 检查 Docker 是否安装和运行
- 支持多架构构建（amd64/arm64）
- 快速构建模式（复用已有镜像）
- CI 验证模式（运行完整构建+测试）
- 彩色日志输出
- 日志文件记录

**命令行参数**:
```bash
--arch amd64|arm64    # 目标架构（默认: amd64）
--fast-build          # 跳过镜像清理，复用已有镜像
--verify              # 运行 CI 构建验证
--no-log              # 禁用文件日志
--help                # 显示帮助信息
```

**使用示例**:
```bash
# 交互式 shell（清理构建）
bash scripts/build_helpers/docker_start.sh

# 快速启动（复用镜像）
bash scripts/build_helpers/docker_start.sh --fast-build

# 运行 CI 验证
bash scripts/build_helpers/docker_start.sh --verify

# ARM64 构建
bash scripts/build_helpers/docker_start.sh --arch arm64
```

### Windows: docker_start.ps1

**文件**: `scripts/build_helpers/docker_start.ps1`

**功能**: 与 Linux 版本相同，适配 PowerShell 环境。

## 技术细节

### Qt 安装流程

Dockerfile 通过调用 `install_build_dependencies.sh` 完成 Qt 安装：

1. 安装 Python3 和 pip
2. 安装 aqtinstall
3. 使用 aqtinstall 下载 Qt 6.8.1 预编译包
4. 配置环境变量 `QT6_DIR=/opt/Qt/6.8.1/<arch>`
5. 将 Qt 添加到 CMAKE_PREFIX_PATH

### Qt 架构映射

| 容器平台 | QT_ARCH 参数 | Qt 安装路径 | 工具链文件 |
|----------|-------------|-------------|-----------|
| linux/amd64 | linux_gcc_64 | /opt/Qt/6.8.1/gcc_64 | ci-x86_64-toolchain.cmake |
| linux/arm64 | linux_gcc_arm64 | /opt/Qt/6.8.1/gcc_arm64 | ci-aarch64-toolchain.cmake |

### 依赖安装

通过 `scripts/dependency/install_build_dependencies.sh` 安装：

- **编译工具**: gcc, g++, cmake, ninja-build, ccache
- **Qt 依赖**: libgl1-mesa-dev, libxkbcommon-x11-dev, libfontconfig1-dev
- **其他**: git, wget, python3-venv

### 路径挂载处理

脚本自动处理不同操作系统的路径格式：

```bash
# Windows (Git Bash / MSYS)
MOUNT_PATH="$(pwd | sed 's|^\([A-Za-z]\):|/\1|' | sed 's|\\|/|g')"

# Linux / macOS
MOUNT_PATH="$PROJECT_ROOT"
```

## 验证方法

### 1. 构建镜像

```bash
# AMD64
docker build --platform linux/amd64 \
  -f scripts/docker/Dockerfile.build \
  -t cfdesktop-build .

# ARM64
docker build --platform linux/arm64 \
  -f scripts/docker/Dockerfile.build \
  --build-arg QT_ARCH=linux_gcc_arm64 \
  -t cfdesktop-build:arm64 .
```

### 2. 验证环境

```bash
# 启动交互式 shell
docker run --rm --platform linux/amd64 \
  -v $(pwd):/project \
  cfdesktop-build

# 容器内验证
which cmake      # /usr/bin/cmake
which qmake6     # /opt/Qt/6.8.1/gcc_64/bin/qmake6
gcc --version    # Ubuntu GCC
```

### 3. 运行构建

```bash
# 使用启动脚本
bash scripts/build_helpers/docker_start.sh --verify

# 手动运行
docker run --rm --platform linux/amd64 \
  -v $(pwd):/project \
  cfdesktop-build \
  bash scripts/build_helpers/ci_build_entry.sh ci
```

## 日志系统

启动脚本提供完整的日志记录：

- **日志目录**: `scripts/docker/logger/`
- **文件命名**: `ci_build_YYYYMMDD_HHMMSS.log`
- **日志内容**: 构建参数、时间戳、完整输出

## 常见问题

### Docker 未安装或未运行

脚本会自动检测并提示：
```
错误: Docker 未安装或未运行
请安装 Docker Desktop: https://www.docker.com/products/docker-desktop
```

### ARM64 在 x86_64 主机上无法运行

需要启用 QEMU 模拟：
```bash
# 安装 QEMU
docker run --privileged --rm tonistiigi/binfmt --install all
```

### Windows 路径问题

Windows 用户应使用 PowerShell 或 CMD，Git Bash 路径格式可能有问题。

## 已知限制

1. **镜像大小**: 完整镜像约 2-3 GB（包含 Qt 和依赖）
2. **ARM64 速度**: 在 x86_64 主机上通过 QEMU 模拟运行较慢
3. **网络**: 首次构建需要下载 Qt 包（支持国内镜像加速）

## 后续步骤

Phase 2 完成后，继续实施：

- [Phase 3: CI 构建入口](ci-build-entry.md)

---

*文档版本: v2.0 | 最后更新: 2026-03-07*
