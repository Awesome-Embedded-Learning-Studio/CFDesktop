---
title: Dockerfile.build
description: "文档编写日期: 2026-03-20，-  - 避免交互式提示"
---

# Dockerfile.build

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 构建镜像
```bash
# 默认构建
docker build -t cfdesktop-build -f scripts/docker/Dockerfile.build .

# 多架构构建
docker buildx build --platform linux/amd64,linux/arm64 \
  -f scripts/docker/Dockerfile.build -t cfdesktop-build .

# ARM64构建
docker build --build-arg QT_ARCH=linux_gcc_arm64 --platform linux/arm64 \
  -f scripts/docker/Dockerfile.build -t cfdesktop-build:arm64 .
```text

### 运行容器
```bash
# AMD64平台
docker run --rm --platform linux/amd64 -v $(pwd):/project cfdesktop-build

# ARM64平台
docker run --rm --platform linux/arm64 -v $(pwd):/project cfdesktop-build
```bash

## Scripts详解

### 基础镜像
`ubuntu:24.04`

### 构建参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| BASE_IMAGE | ubuntu:24.04 | 基础镜像 |
| QT_VERSION | 6.8.1 | Qt版本 |
| QT_ARCH | linux_gcc_64 | Qt架构 |
| QT_MIRROR | 空 | Qt镜像源（可使用清华镜像：https://mirrors.tuna.tsinghua.edu.cn/qt） |
| HOST_TIMEZONE | UTC | 时区设置 |
| INSTALL_DEPS | 1 | 是否安装依赖（1=安装，0=跳过） |

### 环境变量
- `DEBIAN_FRONTEND=noninteractive` - 避免交互式提示
- `QT_ARCH` - Qt架构配置
- `PATH` - 包含Python虚拟环境路径（`/opt/venv/bin`）

### 安装内容
- Qt 6.8.1（通过aqtinstall安装预构建二进制文件）
- 构建工具链（通过`scripts/dependency/install_build_dependencies.sh`）
- Python虚拟环境（位于`/opt/venv`）
- Qt运行时依赖

### 工作目录
`/project`

### 默认命令
`/bin/bash`

### 注意事项
- Ubuntu 24.04 apt源仅提供Qt 6.4.2，因此使用aqtinstall安装Qt 6.8.1
- 设置`INSTALL_DEPS=0`可跳过自动依赖安装，适用于需要手动安装的场景
- Python虚拟环境会自动添加到PATH中
