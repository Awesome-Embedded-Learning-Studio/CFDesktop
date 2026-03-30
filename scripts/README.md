# Scripts

为了让编译更加的容易，这里是编译助手脚本。
默认情况下，脚本会将本次编译产物全部丢到 `out/build` 下作为测试产物。

## 目录结构总览

```
scripts/
├── build_helpers/     # 构建与编译脚本（Windows + Linux）
├── dependency/        # 依赖安装脚本
├── develop/           # 开发辅助工具（格式化、清理）
├── docker/            # Docker 构建环境
├── doxygen/           # Doxygen 文档工具
├── lib/               # 公共脚本库（Bash + PowerShell）
├── release/           # 发布流程与 Git hooks
└── run_helpers/       # 运行与测试辅助
```

## 各目录说明

### build_helpers/ -- 构建脚本

核心构建脚本目录，支持 Linux (Bash) 和 Windows (PowerShell) 两个平台。

**配置文件：**

| 文件 | 用途 |
|------|------|
| `build_develop_config.ini` | 开发构建配置 |
| `build_deploy_config.ini` | 发布构建配置 |
| `build_ci_config.ini` | CI 构建配置（x64） |
| `build_ci_aarch64_config.ini` | CI 构建配置（ARM64） |
| `build_ci_armhf_config.ini` | CI 构建配置（ARMHF） |

**Linux 脚本：**

| 脚本 | 用途 |
|------|------|
| `linux_configure.sh` | CMake 配置（首次或配置变更时运行） |
| `linux_develop_build.sh` | 开发构建（全量编译） |
| `linux_fast_develop_build.sh` | 快速开发构建（增量编译） |
| `linux_deploy_build.sh` | 发布构建（全量编译） |
| `linux_fast_deploy_build.sh` | 快速发布构建（增量编译） |
| `linux_run_tests.sh` | 运行测试 |
| `ci_build_entry.sh` | CI 入口脚本 |
| `docker_start.sh` | 启动 Docker 构建环境 |

**Windows 脚本：**

| 脚本 | 用途 |
|------|------|
| `windows_configure.ps1` | CMake 配置 |
| `windows_develop_build.ps1` | 开发构建（全量编译） |
| `windows_fast_develop_build.ps1` | 快速开发构建（增量编译） |
| `windows_deploy_build.ps1` | 发布构建（全量编译） |
| `windows_fast_deploy_build.ps1` | 快速发布构建（增量编译） |
| `windows_run_tests.ps1` | 运行测试 |
| `docker_start.ps1` | 启动 Docker 构建环境 |

### dependency/ -- 依赖安装

| 脚本 | 用途 |
|------|------|
| `install_build_dependencies.sh` | 安装项目编译所需的系统依赖 |

### develop/ -- 开发辅助

| 脚本 | 用途 |
|------|------|
| `format_cpp.sh` / `format_cpp.ps1` | C++ 代码格式化（clang-format） |
| `remove_trailing_space.sh` / `remove_trailing_space.ps1` | 移除文件末尾空白字符 |

### docker/ -- Docker 构建环境

| 文件 | 用途 |
|------|------|
| `Dockerfile.build` | 构建环境 Docker 镜像定义 |
| `docker-compose.yml` | Docker Compose 编排配置 |
| `logger/` | Docker 构建日志归档 |

### doxygen/ -- 文档工具

| 脚本 | 用途 |
|------|------|
| `lint.py` | Doxygen 注释风格检查 |

### lib/ -- 公共脚本库

提供可复用的 Bash 和 PowerShell 函数模块，避免各脚本中重复代码。详细说明请参考 [lib/README.md](lib/README.md)。

| 模块 | Bash | PowerShell |
|------|------|------------|
| 通用工具 | `lib_common.sh` | `LibCommon.psm1` |
| 路径解析 | `lib_paths.sh` | `LibPaths.psm1` |
| INI 配置解析 | `lib_config.sh` | `LibConfig.psm1` |
| 参数解析 | `lib_args.sh` | `LibArgs.psm1` |
| 构建工具 | `lib_build.sh` | `LibBuild.psm1` |
| Git 工具 | `lib_git.sh` | `LibGit.psm1` |

### release/ -- 发布流程

发布版本管理和 Git hooks 配置。详见 [release/README.md](release/README.md)。

### run_helpers/ -- 运行辅助

| 脚本 | 用途 |
|------|------|
| `windows_run_llvm_boottest.ps1` | Windows 下运行 LLVM 引导测试 |

## 使用方法

### 首次配置

```bash
# Linux
bash scripts/dependency/install_build_dependencies.sh
bash scripts/build_helpers/linux_configure.sh
```

```powershell
# Windows PowerShell
.\scripts\build_helpers\windows_configure.ps1
```

### 日常开发构建

```bash
# Linux 全量构建
bash scripts/build_helpers/linux_develop_build.sh

# Linux 快速增量构建
bash scripts/build_helpers/linux_fast_develop_build.sh
```

```powershell
# Windows 全量构建
.\scripts\build_helpers\windows_develop_build.ps1

# Windows 快速增量构建
.\scripts\build_helpers\windows_fast_develop_build.ps1
```

### CI 构建

```bash
# 入口脚本（自动检测环境）
bash scripts/build_helpers/ci_build_entry.sh
```

### 代码格式化

```bash
# Linux
bash scripts/develop/format_cpp.sh
bash scripts/develop/remove_trailing_space.sh
```

```powershell
# Windows
.\scripts\develop\format_cpp.ps1
.\scripts\develop\remove_trailing_space.ps1
```


