---
title: 构建配置文件说明
description: "文档编写日期: 2026-03-20，配置文件位于  目录，用于控制 CMake 构建参数。根据不同"
---

# 构建配置文件说明

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

配置文件位于 `scripts/build_helpers/` 目录，用于控制 CMake 构建参数。根据不同的构建场景选择相应的配置文件。

## 配置文件类型

| 配置文件 | 用途 | 构建类型 | 工具链 |
|----------|------|----------|--------|
| `build_develop_config.ini` | 开发构建配置 | Debug | `linux/gcc` |
| `build_deploy_config.ini` | 部署构建配置 | Release | `linux/gcc` |
| `build_ci_config.ini` | CI 构建配置 (AMD64/x86_64) | Release | `linux/ci-x86_64` |
| `build_ci_aarch64_config.ini` | CI 构建配置 (ARM64 交叉编译) | Release | `linux/ci-aarch64` |
| `build_ci_armhf_config.ini` | CI 构建配置 (ARM32 HF 交叉编译) | Release | `linux/ci-armhf` |
| `build_develop_config.ini.template` | 开发配置模板 | - | - |
| `build_deploy_config.ini.template` | 部署配置模板 | - | - |

## 配置项说明

### [cmake] - CMake 基础配置

| 配置项 | 说明 | 可选值 |
|--------|------|--------|
| `generator` | CMake 生成器类型 | `Unix Makefiles`、`Ninja`、`Visual Studio 17 2022` 等 |
| `toolchain` | 编译工具链路径 | `linux/gcc`、`linux/ci-x86_64`、`linux/ci-aarch64`、`linux/ci-armhf` |
| `build_type` | 构建类型 | `Debug`、`Release`、`RelWithDebInfo` |

#### build_type 详细说明

| 构建类型 | 优化级别 | 调试信息 | 适用场景 |
|----------|----------|----------|----------|
| `Debug` | `-O0` (无优化) | 完整调试信息 (`-g`) | 开发调试阶段 |
| `Release` | `-O3` (最高优化) | 最小调试信息 | 生产环境部署 |
| `RelWithDebInfo` | `-O2` (优化) | 完整调试信息 (`-g`) | 需要调试的发布版本 |

### [paths] - 路径配置

| 配置项 | 说明 | 示例值 |
|--------|------|--------|
| `source` | 源代码目录 (相对于项目根目录) | `.` 表示项目根目录 |
| `build_dir` | 构建输出目录 (相对于项目根目录) | `out/build_develop`、`out/build_deploy`、`out/build_ci` 等 |

#### 构建目录对照表

| 构建场景 | build_dir |
|----------|-----------|
| 开发构建 | `out/build_develop` |
| 部署构建 | `out/build_deploy` |
| CI 构建 (x86_64) | `out/build_ci` |
| CI 构建 (ARM64) | `out/build_ci_aarch64` |
| CI 构建 (ARM32 HF) | `out/build_ci_armhf` |

### [options] - 额外选项

| 配置项 | 说明 |
|--------|------|
| `jobs` | 并行编译任务数 (使用 Makefile 时生效) |

## 工具链说明

### 本地开发工具链

| 工具链 | 路径 | 平台 | 说明 |
|--------|------|------|------|
| `linux/gcc` | `cmake/toolchain/linux/gcc.cmake` | 本地 Linux | 使用系统默认 GCC 编译器 |

### CI 构建工具链

| 工具链 | 路径 | 平台 | 说明 |
|--------|------|------|------|
| `linux/ci-x86_64` | `cmake/toolchain/linux/ci-x86_64.cmake` | AMD64 | Docker CI 环境标准构建 |
| `linux/ci-aarch64` | `cmake/toolchain/linux/ci-aarch64.cmake` | ARM64 | 交叉编译，使用 `aarch64-linux-gnu-gcc` |
| `linux/ci-armhf` | `cmake/toolchain/linux/ci-armhf.cmake` | ARM32 HF | 交叉编译，使用 `arm-linux-gnueabihf-gcc`，针对 IMX6ULL 等 Cortex-A7 平台 |

## 模板文件

`.template` 文件是配置模板，用于创建新的配置文件：

1. 复制模板文件：`cp build_develop_config.ini.template build_custom_config.ini`
2. 根据需要修改配置项
3. 使用新配置文件进行构建

## 各配置文件详细参数

### build_develop_config.ini - 开发构建

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/gcc
build_type=Debug

[paths]
source=.
build_dir=out/build_develop

[options]
jobs=12
```text

**特点**：使用 Debug 模式，适合日常开发调试。

### build_deploy_config.ini - 部署构建

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/gcc
build_type=Release

[paths]
source=.
build_dir=out/build_deploy

[options]
jobs=16
```text

**特点**：使用 Release 模式，最高优化级别，适合生产部署。

### build_ci_config.ini - CI 构建 (AMD64)

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
```text

**特点**：用于 Docker CI 环境，x86_64 平台标准化构建。

### build_ci_aarch64_config.ini - CI 构建 (ARM64)

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
```text

**特点**：在 x86_64 主机上交叉编译 ARM64 程序，使用 `aarch64-linux-gnu-gcc`。

### build_ci_armhf_config.ini - CI 构建 (ARM32 HF)

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
```text

**特点**：在 x86_64 主机上交叉编译 ARM32 HF 程序，使用 `arm-linux-gnueabihf-gcc`，目标平台包括 IMX6ULL (i.MX 6UltraLite) 等 ARM Cortex-A7 设备。
