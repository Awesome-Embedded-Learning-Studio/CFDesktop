---
title: "pre-push.sample"
description: "文档编写日期: 2026-03-20，在推送前验证Docker构建，仅对main和release分支"
---

# pre-push.sample

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 绕过方法
```bash
git push --no-verify
```bash

## Scripts详解

### 钩子用途
在推送前验证Docker构建，仅对main和release分支生效。

### 分支行为

| 分支 | 验证级别 |
|------|----------|
| main | X64 FastBuild + Tests |
| release/* | 根据版本号自动检测 |
| feat/* | 跳过验证 |

### 验证级别检测
- **Major**: X64 + ARM64 完整构建
- **Minor**: X64 完整构建
- **Patch**: X64 FastBuild + Tests

### 版本号检查
推送前验证CMakeLists.txt版本号是否已变更。

### 依赖项
- Docker
- scripts/build_helpers/docker_start.sh
