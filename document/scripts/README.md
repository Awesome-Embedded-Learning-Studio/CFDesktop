# Scripts文档

> 文档编写日期: 2026-03-20

本目录包含CFDesktop项目所有脚本的完整文档。

## 目录结构

```
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

## 文档规范

每个脚本文档包含：
- **使用办法**: 基本语法、参数、示例
- **Scripts详解**: 用途、依赖、核心功能

## 脚本语言

| 平台 | 脚本类型 |
|------|----------|
| Linux/macOS | Bash (.sh) |
| Windows | PowerShell (.ps1) |

## 相关文档

- [开发指南](../../development/) - 项目开发文档
- [CI/CD文档](../../ci/) - 持续集成文档
