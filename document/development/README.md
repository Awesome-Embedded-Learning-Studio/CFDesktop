# CFDesktop 开发环境文档

欢迎使用 CFDesktop 开发环境设置指南。本文档系列将帮助您搭建完整的开发环境，从基础工具安装到高级配置，逐步引导您成为 CFDesktop 开发者。

---

## 项目简介

**CFDesktop** 是一个基于 Qt6 的现代化嵌入式桌面框架，旨在为各种嵌入式设备提供统一、现代化的桌面环境。

### 核心特性

- **跨平台支持**: Windows 10/11、Ubuntu 22.04+、Debian 12+
- **多架构支持**: x86_64、ARM64、ARMhf
- **性能自适应**: 根据设备硬件能力自动调整 UI 特效和功能
- **Material Design 3**: 完整实现的现代化 UI 组件库
- **模块化设计**: 松耦合架构，便于裁剪和定制

### 技术栈

| 技术 | 版本 | 用途 |
|:---|:---:|:---|
| **C++** | C++23 | 核心开发语言 |
| **Qt** | 6.8.3+ | UI 框架 |
| **CMake** | 3.16+ | 构建系统 |
| **Docker** | 最新 | 多架构构建验证 |
| **Git** | 最新 | 版本控制 |

---

## 文档导航

| 文档 | 内容 | 预计时间 |
|:---|:---|:---:|
| [01. 前置要求](01_prerequisites.md) | 硬件要求、操作系统支持、必需软件安装 | 15-30 分钟 |
| [02. 快速开始](02_quick_start.md) | 最快速的方式启动项目 | 5-10 分钟 |
| [04. 开发工具](04_development_tools.md) | 代码格式化、静态分析、调试工具 | 10-15 分钟 |
| [05. Docker 构建](05_docker_build.md) | Docker 多架构构建指南 | 15-20 分钟 |
| [06. Git Hooks](06_git_hooks.md) | Pre-commit 和 Pre-push Hook 使用说明 | 10-15 分钟 |
| [07. 常见问题](07_troubleshooting.md) | 问题排查和解决方案 | - |

---

## 环境要求速览

### 硬件要求

| 组件 | 最低配置 | 推荐配置 |
|:---|:---:|:---:|
| **CPU** | 4 核心 | 8 核心以上 |
| **RAM** | 8GB | 16GB 或更多 |
| **硬盘** | 20GB 可用空间 | 50GB+ SSD |

### 操作系统支持

| 平台 | 支持版本 | 工具链 |
|:---|:---|:---|
| **Windows** | Windows 10/11 | MinGW 或 LLVM |
| **Linux** | Ubuntu 22.04+, Debian 12+ | GCC 或 Clang |

### 必需软件

| 软件 | 最低版本 | 推荐版本 |
|:---|:---:|:---:|
| **Docker Desktop** | 最新稳定版 | 最新版 |
| **Git** | 2.30+ | 最新版 |
| **VSCode** | (推荐) 最新版 | 最新版 |
| **Qt6** | 6.8.3 | 6.8.3+ |
| **CMake** | 3.16 | 3.20+ |
| **Python** | 3.8+ | 3.10+ (用于 aqtinstall) |

---

## 推荐开发流程

```mermaid
graph LR
    A[1. 环境准备] --> B[2. 克隆项目]
    B --> C[3. 配置 Qt6]
    C --> D[4. 首次构建]
    D --> E[5. 运行测试]
    E --> F[6. 开始开发]

    style A fill:#4CAF50
    style B fill:#2196F3
    style C fill:#9C27B0
    style D fill:#FF9800
    style E fill:#FF5722
    style F fill:#9E9E9E
```

### 快速开始

```bash
# 1. 克隆项目
git clone https://github.com/your-org/CFDesktop.git
cd CFDesktop

# 2. Windows 快速构建
.\scripts\build_helpers\windows_fast_develop_build.ps1

# 3. Linux 快速构建
./scripts/build_helpers/linux_fast_develop_build.sh
```

---

## 下一步

请按照文档顺序阅读：

1. **[01. 前置要求](01_prerequisites.md)** - 确保您的开发环境满足所有要求
2. **[02. 环境配置](02_environment.md)** - 配置 Qt6 和编译器
3. **[03. 构建系统](03_build_system.md)** - 了解 CMake 构建系统
4. **[04. IDE 配置](04_ide_setup.md)** - 配置您喜欢的开发工具

---

## 获取帮助

### 问题反馈

如果您在环境设置过程中遇到问题：

- **GitHub Issues**: [提交问题](https://github.com/your-org/CFDesktop/issues)
- **讨论区**: [GitHub Discussions](https://github.com/your-org/CFDesktop/discussions)
- **文档**: 查看项目根目录下的 [README.md](../../README.md)

### 常见问题

**Q: 必须使用 Docker 吗？**

A: 不是必须的，但推荐使用 Docker 进行多架构构建验证。本地开发可以直接使用 Qt6 和 CMake。

**Q: 可以使用其他 IDE 吗？**

A: 可以。项目主要配置 VSCode + Clangd，但也支持 QtCreator 和其他支持 CMake 的 IDE。

**Q: Windows 下推荐使用 MinGW 还是 LLVM？**

A: 两者都支持。LLVM/Clang 通常有更好的兼容性和错误信息，MinGW 则更轻量。

---

## 附录

### 相关链接

- [Qt6 官方文档](https://doc.qt.io/qt-6/)
- [CMake 官方文档](https://cmake.org/documentation/)
- [Docker 官方文档](https://docs.docker.com/)
- [aqtinstall 文档](https://aqtinstall.readthedocs.io/)

### 文档更新

- **版本**: 0.13.1
- **最后更新**: 2026-03-30
- **维护者**: CFDesktop 开发团队

---

<div align="center">

  [返回项目首页](../index.md) | [前置要求 &rarr;](01_prerequisites.md)

  **CFDesktop** - 为嵌入式设备打造的现代化桌面框架

</div>
