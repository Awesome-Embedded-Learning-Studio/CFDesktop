---
title: cibuildentry.sh
description: "文档编写日期: 2026-03-20，是CI（持续集成）环境下的构建入口脚本，主要用于Docker容"
---

# ci_build_entry.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/ci_build_entry.sh [ci|test]
```bash

## Scripts详解

`ci_build_entry.sh` 是CI（持续集成）环境下的构建入口脚本，主要用于Docker容器内执行。该脚本会自动检测容器架构并选择相应的配置文件。

### 功能特点

1. **自动架构检测** - 使用 `uname -m` 自动检测容器架构
2. **多架构支持** - 支持AMD64、ARM64、ARM32（IMX6ULL）
3. **双模式运行** - 支持完整CI构建和仅测试模式

### 支持的架构

| 架构标识 | 说明 | 配置文件 |
|----------|------|----------|
| x86_64, amd64 | AMD64原生 | build_ci_config.ini |
| aarch64 | ARM64原生 | build_ci_aarch64_config.ini |
| armv7l, armhf | ARM32原生（IMX6ULL） | build_ci_armhf_config.ini |

### 运行模式

#### ci 模式

执行完整的CI构建流程：

```bash
./scripts/build_helpers/ci_build_entry.sh ci
```text

该模式会执行：
1. 调用 `linux_develop_build.sh` 进行配置和构建
2. 运行测试
3. 返回构建结果

#### test 模式

仅运行测试（不重新构建）：

```bash
./scripts/build_helpers/ci_build_entry.sh test
```text

该模式会调用 `linux_run_tests.sh` 运行已有构建的测试。

### 日志输出

脚本使用彩色日志输出：

- `[INFO]` - 青色 - 一般信息
- `[SUCCESS]` - 绿色 - 成功信息
- `[WARNING]` - 黄色 - 警告信息
- `[ERROR]` - 红色 - 错误信息

### 退出码

- `0` - 构建或测试成功
- `非0` - 构建或测试失败，返回具体的错误码

### 使用场景

1. **Docker CI环境** - 由 `docker_start.sh --verify` 调用
2. **多架构CI流水线** - 自动适配不同架构的容器
3. **自动化测试** - 快速验证代码质量

### 示例

```bash
# 在Docker容器中运行完整CI构建
docker run --rm cfdesktop-build bash scripts/build_helpers/ci_build_entry.sh ci

# 仅运行测试
docker run --rm cfdesktop-build bash scripts/build_helpers/ci_build_entry.sh test
```text

### 错误处理

当遇到未知架构时，脚本会输出错误信息并退出：

```text
ERROR: Unknown architecture: <架构名>
Supported: x86_64, aarch64, armv7l
```text

### 注意事项

1. 该脚本设计用于Docker容器环境
2. 必须在项目根目录或脚本所在目录下执行
3. 需要相应的配置文件存在于 `scripts/build_helpers/` 目录
