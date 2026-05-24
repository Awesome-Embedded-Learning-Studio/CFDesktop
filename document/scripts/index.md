---
title: 脚本工具
description: 本目录包含 CFDesktop 项目全部脚本工具的文档，涵盖构建辅助脚本（Linux Bash / Windows PowerShell）、Docker 构建与部署脚本、第三方依赖管理脚本、开发工作流辅助工具、Doxygen 文档生成脚本以及正式发布流程脚本。
---

# 脚本工具

本目录包含 CFDesktop 项目所有脚本的完整文档，涵盖构建辅助脚本（Linux Bash / Windows PowerShell）、Docker 构建与部署脚本、第三方依赖管理脚本、开发工作流辅助工具、Doxygen 文档生成脚本以及正式发布流程脚本。

## 目录结构

```text
scripts/
├── build_helpers/    # 构建辅助脚本 (Linux/Windows)
├── dependency/       # 依赖安装
├── develop/          # 开发工具 (代码格式化、清理)
├── docker/           # Docker配置
├── doxygen/          # Doxygen工具
├── lib/              # 库文件
│   ├── bash/        # Bash库
│   └── powershell/  # PowerShell库
├── release/          # 发布相关
│   └── hooks/       # Git钩子
└── run_helpers/      # 运行辅助
```

## 快速导航

### 构建相关
- [构建辅助脚本](build_helpers/) - Linux/Windows构建脚本

### 开发工具
- [依赖安装](dependency/install_build_dependencies.sh.md) - 环境配置
- [代码格式化](develop/format_cpp.sh.md) - C++代码格式化
- [空格清理](develop/remove_trailing_space.sh.md) - 删除行尾空格

### 容器化
- [Docker配置](docker/Dockerfile.build.md) - 构建环境镜像

### 库文件
- [Bash库](lib/bash/) - Bash函数库
- [PowerShell库](lib/powershell/) - PowerShell模块

### 版本控制
- [Git钩子](release/hooks/) - Git hooks配置

## 脚本语言

| 平台 | 脚本类型 |
|------|----------|
| Linux/macOS | Bash (.sh) |
| Windows | PowerShell (.ps1) |

## 相关文档

- [开发指南](../../development/) - 项目开发文档
- [CI/CD文档](../../ci/) - 持续集成文档
