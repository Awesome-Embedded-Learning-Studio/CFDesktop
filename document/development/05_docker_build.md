# Docker 构建指南

本文档介绍如何使用 Docker 构建多架构的 CFDesktop 项目，包括镜像说明、命令速查、多架构构建和调试配置。

## 目录

- [Docker 镜像说明](#docker-镜像说明)
- [docker_start.sh 命令速查](#docker_startsh-命令速查)
- [多架构构建](#多架构构建)
- [构建配置文件](#构建配置文件)
- [日志和调试](#日志和调试)
- [常见问题](#常见问题)

---

## Docker 镜像说明

### 基础镜像

| 属性 | 值 |
|------|-----|
| 基础镜像 | Ubuntu 24.04 |
| 镜像名称 | `cfdesktop-build` |
| Dockerfile | `scripts/docker/Dockerfile.build` |

### Qt 版本

| 属性 | 值 |
|------|-----|
| Qt 版本 | 6.8.1 |
| 安装方式 | aqtinstall (预编译二进制) |
| 镜像源 | https://mirrors.aliyun.com/qt (国内) |

### 支持架构

| 架构 | 平台标识 | 目标平台 | 应用场景 |
|------|----------|----------|----------|
| amd64 | linux/amd64 | x86_64 | 本地开发、CI 构建 |
| arm64 | linux/arm64 | ARM64 | RK3568、RK3588 |
| armhf | linux/arm/v7 | ARM32 | IMX6ULL (交叉编译) |

---

## docker_start.sh 命令速查

### 命令选项表

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `--arch amd64\|arm64` | 目标架构 | amd64 |
| `--fast-build` | 跳过镜像清理，复用现有镜像 | false |
| `--verify` | 运行 CI 构建验证 | false |
| `--build-project` | 构建镜像 + 完整清理构建 | false |
| `--build-project-fast` | 构建镜像 + 快速构建（增量） | false |
| `--run-project-test` | 构建镜像 + 运行测试 | false |
| `--stay-on-error` | CI 失败时保持容器开启用于调试 | false |
| `--no-log` | 禁用文件日志记录 | false |
| `--no-deps` | 跳过镜像中的依赖安装 | false |
| `--help` | 显示帮助信息 | - |

### 命令示例

#### 交互式 Shell

```bash
# 默认进入交互式 shell（amd64 架构）
bash scripts/build_helpers/docker_start.sh

# 指定架构
bash scripts/build_helpers/docker_start.sh --arch arm64
```

#### CI 验证模式

```bash
# 运行 CI 构建验证
bash scripts/build_helpers/docker_start.sh --verify

# 失败时保持容器开启（用于调试）
bash scripts/build_helpers/docker_start.sh --verify --stay-on-error
```

#### 完整构建

```bash
# 构建镜像 + 完整清理构建
bash scripts/build_helpers/docker_start.sh --build-project

# 指定架构
bash scripts/build_helpers/docker_start.sh --build-project --arch arm64
```

#### 快速构建

```bash
# 构建镜像 + 快速构建（增量，不清理）
bash scripts/build_helpers/docker_start.sh --build-project-fast

# 快速构建 ARM64
bash scripts/build_helpers/docker_start.sh --build-project-fast --arch arm64
```

#### 运行测试

```bash
# 构建镜像 + 运行测试
bash scripts/build_helpers/docker_start.sh --run-project-test

# 指定架构
bash scripts/build_helpers/docker_start.sh --run-project-test --arch arm64
```

#### 快速模式

```bash
# 复用现有镜像（跳过镜像重建）
bash scripts/build_helpers/docker_start.sh --fast-build
```

#### 组合选项

```bash
# CI 验证 + 失败保持 + 禁用日志
bash scripts/build_helpers/docker_start.sh --verify --stay-on-error --no-log

# 快速构建 + ARM64
bash scripts/build_helpers/docker_start.sh --build-project-fast --arch arm64 --fast-build
```

---

## 多架构构建

### 架构对比

| 架构 | 构建模式 | 目标设备 | 构建配置文件 | 输出目录 |
|------|----------|----------|--------------|----------|
| amd64 | 原生 | x86_64 PC | build_ci_config.ini | out/build_ci |
| arm64 | 原生/QEMU | RK3568/RK3588 | build_ci_aarch64_config.ini | out/build_ci_aarch64 |
| armhf | 交叉编译 | IMX6ULL | build_ci_armhf_config.ini | out/build_ci_armhf |

### AMD64 构建（本地开发）

```bash
# 默认架构，无需指定
bash scripts/build_helpers/docker_start.sh --verify
```

### ARM64 构建

```bash
# ARM64 原生容器（需要 QEMU 支持）
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify

# 完整构建
bash scripts/build_helpers/docker_start.sh --arch arm64 --build-project
```

**ARM64 应用场景：**
- Rockchip RK3568 (四核 ARM Cortex-A55)
- Rockchip RK3588 (八核 ARM Cortex-A76/A55)

### ARMHF 构建（交叉编译）

```bash
# ARM32 交叉编译（在 amd64 容器中）
bash scripts/build_helpers/docker_start.sh --arch armhf --verify
```

**ARMHF 应用场景：**
- NXP i.MX 6ULL (ARM Cortex-A7)

### QEMU 支持

ARM64 构建依赖 QEMU 模拟器，确保系统已安装：

```bash
# 检查 QEMU 是否安装
docker run --rm --platform linux/arm64 ubuntu:24.04 uname -m
# 应输出: aarch64

# 如果失败，安装 binfmt 支持
docker run --privileged --rm tonistiigi/binfmt --install all
```

---

## 构建配置文件

### build_ci_config.ini (AMD64)

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/ci-x86_64
build_type=Release

[paths]
source=.
build_dir=out/build_ci

[options]
jobs=16
```

### build_ci_aarch64_config.ini (ARM64)

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/ci-aarch64
build_type=Release

[paths]
source=.
build_dir=out/build_ci_aarch64

[options]
jobs=16
```

### build_ci_armhf_config.ini (ARMHF)

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/ci-armhf
build_type=Release

[paths]
source=.
build_dir=out/build_ci_armhf

[options]
jobs=16
```

### 配置文件说明

| 节 | 选项 | 说明 |
|-----|------|------|
| [cmake] | generator | CMake 生成器 |
| [cmake] | toolchain | 工具链文件路径 |
| [cmake] | build_type | 构建类型 |
| [paths] | source | 源码目录（相对项目根目录） |
| [paths] | build_dir | 构建输出目录 |
| [options] | jobs | 并行编译任务数 |

---

## 日志和调试

### 日志目录

构建日志保存在 `scripts/docker/logger/` 目录：

```bash
# 日志文件命名格式
ci_build_YYYYMMDD_HHMMSS.log
```

### 日志内容

日志文件包含以下信息：

```
==============================================================================
CFDesktop Docker Build Log
==============================================================================
Start Time: 2026-03-07 14:30:00 CST
Architecture: amd64
Platform: linux/amd64
Fast Build: false
Verify Mode: true
==============================================================================
```

### --stay-on-error 调试

当 CI 构建失败时，使用 `--stay-on-error` 保持容器开启：

```bash
bash scripts/build_helpers/docker_start.sh --verify --stay-on-error
```

**效果：**
- 构建失败后不会退出容器
- 可以手动检查构建输出
- 可以手动运行调试命令

**使用场景：**
```bash
# 失败后可以在容器中执行
cd /project
ls -la out/build_ci/
cat build.log
```

### 禁用日志

```bash
# 不生成日志文件（输出到终端）
bash scripts/build_helpers/docker_start.sh --verify --no-log
```

---

## 常见问题

### Q: Docker 镜像构建失败？

**A:** 检查以下几点：

1. 确认 Docker 服务运行中：
   ```bash
   docker info
   ```

2. 检查网络连接（镜像拉取）：
   ```bash
   docker pull ubuntu:24.04
   ```

3. 清理旧镜像重试：
   ```bash
   docker rmi cfdesktop-build
   ```

### Q: ARM64 构建很慢？

**A:** ARM64 在 x86_64 主机上通过 QEMU 模拟，速度较慢是正常的。优化建议：

1. 使用 `--fast-build` 复用镜像
2. 使用 `--build-project-fast` 增量构建
3. 减少并行任务数

### Q: 如何在容器中调试？

**A:** 使用交互式模式：

```bash
# 进入容器
bash scripts/build_helpers/docker_start.sh

# 手动构建
cd /project
cmake -B out/build_ci -DCMAKE_BUILD_TYPE=Release
cmake --build out/build_ci

# 运行程序
./out/build_ci/bin/your_app
```

### Q: 如何修改 Qt 版本？

**A:** 编辑 `scripts/docker/Dockerfile.build`：

```dockerfile
ARG QT_VERSION=6.8.1  # 修改为需要的版本
```

然后重新构建镜像。

### Q: 容器中如何访问外网？

**A:** Docker 容器默认使用宿主机网络配置，可以直接访问外网。如果遇到问题：

```bash
# 检查容器网络
docker run --rm ubuntu:24.04 ping -c 3 8.8.8.8

# 检查 DNS
docker run --rm ubuntu:24.4 cat /etc/resolv.conf
```

---

## 相关文档

- [开发工具配置](./04_development_tools.md)
- [构建系统](./03_build_system.md)
- [快速开始](./02_quick_start.md)
- [项目索引](../index.md)
