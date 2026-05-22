---
title: "docker-compose.yml"
description: "文档编写日期: 2026-03-20，所有服务挂载项目根目录到容器的"
---

# docker-compose.yml

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```bash
docker-compose -f scripts/docker/docker-compose.yml [COMMAND]
```text

### 常用命令
```bash
# 构建镜像
docker-compose -f scripts/docker/docker-compose.yml build

# 运行AMD64容器
docker-compose -f scripts/docker/docker-compose.yml run build-amd64

# 运行ARM64容器
docker-compose -f scripts/docker/docker-compose.yml run build-arm64

# 运行验证服务
docker-compose -f scripts/docker/docker-compose.yml run verify
```bash

## Scripts详解

### 服务说明

| 服务 | 平台 | 说明 |
|------|------|------|
| build-amd64 | linux/amd64 | AMD64构建环境 |
| build-arm64 | linux/arm64 | ARM64构建环境（需要支持ARM的Docker） |
| verify | linux/amd64 | 快速验证服务（默认AMD64） |

### 卷挂载
所有服务挂载项目根目录到容器的 `/project`

### 工作目录
`/project`

### 镜像命名

| 服务 | 镜像名 |
|------|--------|
| build-amd64 | cfdesktop-build:amd64 |
| build-arm64 | cfdesktop-build:arm64 |
| verify | cfdesktop-build:latest |

### 构建配置
- **上下文**: 项目根目录（`scripts/docker/`的父目录）
- **Dockerfile**: `scripts/docker/Dockerfile.build`
- **默认命令**: `/bin/bash`

### 注意事项
verify服务目前使用`/bin/bash`作为默认命令。在完成Phase 3后，可取消注释以下命令以使用`ci_build_entry.sh`：
```bash
command: bash scripts/build_helpers/ci_build_entry.sh ci
```text
