---
title: dockerstart.ps1
description: "文档编写日期: 2026-03-20，本脚本用于在Docker容器中构建CFDesktop项目，提供"
---

# docker_start.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\build_helpers\docker_start.ps1 [OPTIONS]
```bash

### 参数说明
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `-Arch` | String | `amd64` | 目标架构，可选值: `amd64` 或 `arm64` |
| `-FastBuild` | Switch | - | 跳过镜像清理，复用已有镜像 |
| `-Interactive` | Switch | - | 进入交互式Shell (默认行为) |
| `-Verify` | Switch | - | 运行CI构建验证 |
| `-BuildProject` | Switch | - | 构建镜像 + 执行完整清理构建 |
| `-BuildProjectFast` | Switch | - | 构建镜像 + 执行快速构建 |
| `-RunProjectTest` | Switch | - | 构建镜像 + 运行测试 |
| `-StayOnError` | Switch | - | CI失败时保留容器用于调试 |
| `-NoLog` | Switch | - | 禁用文件日志 (默认记录日志) |
| `-Help` | Switch | - | 显示帮助信息 |

### 使用示例
```powershell
# 默认: 进入交互式Shell
.\scripts\build_helpers\docker_start.ps1

# 运行CI构建验证
.\scripts\build_helpers\docker_start.ps1 -Verify

# CI失败时保留容器调试
.\scripts\build_helpers\docker_start.ps1 -Verify -StayOnError

# 构建镜像 + 完整清理构建
.\scripts\build_helpers\docker_start.ps1 -BuildProject

# 构建镜像 + 快速构建
.\scripts\build_helpers\docker_start.ps1 -BuildProjectFast

# 构建镜像 + 运行测试
.\scripts\build_helpers\docker_start.ps1 -RunProjectTest

# ARM64架构构建
.\scripts\build_helpers\docker_start.ps1 -Arch arm64

# 复用已有镜像 (快速模式)
.\scripts\build_helpers\docker_start.ps1 -FastBuild

# 禁用日志记录
.\scripts\build_helpers\docker_start.ps1 -NoLog
```text

## Scripts详解

### 脚本用途
本脚本用于在Docker容器中构建CFDesktop项目，提供：
- 跨平台构建环境 (amd64/arm64)
- 隔离的编译环境
- CI构建验证
- 交互式开发环境

### 依赖模块
- **LibPaths.psm1** - 提供路径解析和项目根目录定位功能

### 配置文件
使用架构特定的CI配置文件：
- **amd64**: `build_ci_config.ini`
- **arm64**: `build_ci_aarch64_config.ini`

### 执行模式

#### 1. 交互模式 (默认)
启动容器并进入交互式bash shell：
```powershell
.\docker_start.ps1
```text
项目根目录挂载到容器内的 `/project`。

#### 2. CI验证模式
运行完整的CI构建：
```powershell
.\docker_start.ps1 -Verify
```text
执行 `scripts/build_helpers/ci_build_entry.sh ci`

#### 3. 构建项目模式
- **完整构建**: 清理 + 配置 + 编译
  ```powershell
  .\docker_start.ps1 -BuildProject
  ```
- **快速构建**: 配置 + 编译 (无清理)
  ```powershell
  .\docker_start.ps1 -BuildProjectFast
  ```

#### 4. 测试模式
运行项目测试：
```powershell
.\docker_start.ps1 -RunProjectTest
```bash

### 架构支持

#### amd64
- 原生x86_64构建
- 使用 `linux_gcc_64` Qt版本
- 平台: `linux/amd64`

#### arm64
- ARM64原生容器
- 使用 `linux_gcc_arm64` Qt版本
- 平台: `linux/arm64`
- 在x86_64主机上使用QEMU模拟 (较慢但产生原生ARM64二进制)

### Docker镜像
- **镜像名称**: `cfdesktop-build`
- **Dockerfile**: `scripts/docker/Dockerfile.build`
- **构建上下文**: 项目根目录

### 构建参数
| 参数 | 用途 |
|------|------|
| `QT_ARCH` | Qt架构 (linux_gcc_64 / linux_gcc_arm64) |
| `INSTALL_DEPS` | 是否安装依赖 (交互模式为0) |

### 日志功能
默认启用日志记录：
- **日志目录**: `scripts/docker/logger/`
- **文件格式**: `ci_build_YYYYMMDD_HHMMSS.log`
- **内容**: 构建过程的完整输出

使用 `-NoLog` 参数禁用日志。

### 安全检查
- Docker存在性检查
- Docker守护进程运行状态检查

### 退出代码
- `0` - 操作成功
- 非`0` - 操作失败

### 调试功能
使用 `-StayOnError` 参数，CI构建失败时容器不会退出，可进入交互模式调试：
```powershell
.\docker_start.ps1 -Verify -StayOnError
```text

### 快速构建模式
使用 `-FastBuild` 参数复用已有镜像：
- 镜像存在时跳过构建
- 镜像不存在时才会构建

### UI输出格式
脚本使用彩色输出：
- 青色 - 标题/阶段名称
- 绿色 - 成功信息
- 红色 - 错误信息
- 黄色 - 警告信息
- 蓝色 - 强调信息
- 暗灰色 - 次要信息

### 注意事项
1. 首次运行需要构建Docker镜像，可能需要较长时间
2. ARM64构建在x86_64主机上使用QEMU模拟，速度较慢
3. 项目目录以卷挂载方式映射到容器，修改会立即生效
4. 容器内工作目录为 `/project`
