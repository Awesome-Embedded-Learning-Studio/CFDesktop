# Git Hooks 使用说明

本文档介绍 CFDesktop 项目中 Git Hooks 的安装、配置和使用方法。

## 目录

- [概述](#概述)
- [Pre-Commit Hook](#pre-commit-hook)
- [Pre-Push Hook](#pre-push-hook)
- [版本工具](#版本工具)
- [安装与卸载](#安装与卸载)
- [常见问题](#常见问题)

## 概述

CFDesktop 使用 Git Hooks 来确保代码质量和构建验证。项目包含两个主要 Hook：

| Hook | 触发时机 | 用途 |
|------|---------|------|
| pre-commit | `git commit` 前 | 代码质量检查和自动格式化 |
| pre-push | `git push` 前 | Docker 构建验证和版本检查 |

## Pre-Commit Hook

### 功能

Pre-commit Hook 在每次提交前自动执行以下检查：

1. **空白字符检查**
   - 检测并阻止带有尾随空格（trailing whitespace）的提交
   - 支持空格和 Tab 字符检测

2. **C++ 代码自动格式化**
   - 使用 `clang-format` 自动格式化暂存的 C/C++ 文件
   - 格式化后自动更新暂存区

### 跨平台支持

Hook 会自动检测运行平台并使用相应的脚本：

| 平台 | 使用的脚本 |
|------|-----------|
| Windows (Git Bash/MSYS) | `scripts/develop/remove_trailing_space.ps1` |
| Linux/macOS | `scripts/develop/remove_trailing_space.sh` |

### 手动修复空白字符问题

如果 pre-commit 检查失败，可以手动运行修复脚本：

```bash
# Linux/macOS
bash scripts/develop/remove_trailing_space.sh --staged

# Windows (PowerShell)
pwsh scripts/develop/remove_trailing_space.ps1 -Staged
```

### 绕过 Pre-Commit 检查

紧急情况下可以使用 `--no-verify` 跳过检查（不推荐）：

```bash
git commit --no-verify -m "紧急修复"
```

### 检查模式

remove_trailing_space 脚本支持多种模式：

```bash
# 检查所有文件（不修改）
bash scripts/develop/remove_trailing_space.sh --check

# 仅检查暂存文件
bash scripts/develop/remove_trailing_space.sh --staged-check

# 修复暂存文件
bash scripts/develop/remove_trailing_space.sh --staged

# 预览将要修改的内容
bash scripts/develop/remove_trailing_space.sh --dry-run
```

## Pre-Push Hook

### 功能

Pre-push Hook 在推送前执行 Docker 构建验证，确保代码可以成功构建。

### 分支验证策略

不同的分支有不同的验证策略：

#### main 分支

- **验证级别**: X64 FastBuild + Tests
- **说明**: 快速验证，适用于日常开发推送

#### release 分支

根据版本号自动检测验证级别：

| 版本变化 | 验证级别 | 说明 |
|---------|---------|------|
| Major (x.0.0 -> y.0.0) | X64 + ARM64 完整构建 + 测试 | 完整多架构验证 |
| Minor (x.y.0 -> x.z.0) | X64 完整构建 + 测试 | 标准验证 |
| Patch (x.y.z -> x.y.w) | X64 FastBuild + 测试 | 快速验证 |

#### 其他分支

- 跳过验证，直接允许推送

### 版本号检查

推送到 main 或 release 分支时，会检查 `CMakeLists.txt` 中的版本号是否已更新：

```cmake
project(CFDesktop VERSION X.Y.Z LANGUAGES CXX)
```

如果版本号未变更，推送将被阻止，提示信息如下：

```
========================================
版本号未变更，推送被阻止
========================================

当前版本: 0.13.1
远程版本: 0.13.1

请按以下步骤更新版本号：

  1. 更新 CMakeLists.txt 版本号:
     project(CFDesktop VERSION X.Y.Z LANGUAGES CXX)

  2. 更新 README.md 中的版本徽章 (如果存在)

版本号规则:
  - Patch: 0.13.1 -> 0.13.2 (bug 修复、小改动)
  - Minor: 0.13.1 -> 0.14.0 (新功能)
  - Major: 0.13.1 -> 1.0.0 (破坏性变更)
```

### 首次推送

如果是首次推送（远程没有版本标签），版本号检查会被跳过。

### 绕过 Pre-Push 检查

紧急情况下可以使用 `--no-verify` 跳过检查（不推荐）：

```bash
git push --no-verify
```

### Docker 环境要求

Pre-push Hook 需要以下环境：

- Docker 已安装并运行
- Docker daemon 可用

如果 Docker 不可用，Hook 会显示安装提示并退出。

## 版本工具

`scripts/release/hooks/version_utils.sh` 提供版本号解析和验证级别检测功能。

### 可用函数

| 函数 | 说明 |
|------|------|
| `get_major_version <version>` | 获取主版本号 (X.y.z) |
| `get_minor_version <version>` | 获取次版本号 (x.Y.z) |
| `get_patch_version <version>` | 获取补丁版本号 (x.y.Z) |
| `determine_verify_level <local> <remote>` | 根据版本号确定验证级别 |
| `get_verify_level_description <level>` | 获取验证级别的描述 |
| `get_local_version` | 获取当前分支的版本标签 |
| `get_remote_version` | 获取远程 main 分支的版本标签 |
| `get_cmake_version <project_root>` | 获取 CMakeLists.txt 中定义的版本号 |
| `print_version_info <local> <remote> <level>` | 打印版本信息（调试用） |

### 使用示例

```bash
# 加载版本工具
source scripts/release/hooks/version_utils.sh

# 获取版本号组件
VERSION="1.2.3"
MAJOR=$(get_major_version "$VERSION")  # 输出: 1
MINOR=$(get_minor_version "$VERSION")  # 输出: 2
PATCH=$(get_patch_version "$VERSION")  # 输出: 3

# 确定验证级别
LEVEL=$(determine_verify_level "1.2.3" "1.1.0")  # 输出: minor
```

## 安装与卸载

### 安装 Hooks

使用提供的安装脚本：

```bash
bash scripts/release/hooks/install_hooks.sh
```

安装过程：

1. 检查是否在 Git 仓库中
2. 显示安装信息（源目录、目标目录）
3. 备份现有的自定义 Hook（如果有）
4. 复制 Hook 文件到 `.git/hooks/` 目录
5. 设置执行权限

### 验证安装

安装完成后，可以验证 Hook 是否正确安装：

```bash
ls -la .git/hooks/pre-commit .git/hooks/pre-push
```

### 卸载 Hooks

直接删除 Hook 文件：

```bash
rm .git/hooks/pre-commit .git/hooks/pre-push
```

### 备份机制

安装脚本会自动备份现有的自定义 Hook：

- 如果现有 Hook 包含 "CFDesktop Git Hooks" 标记，直接覆盖
- 如果是用户自定义的 Hook，备份为 `.backup.YYYYMMDDHHMMSS`

## 常见问题

### Pre-Commit 失败

**问题**: 提交时提示空白字符检查失败

**解决方案**:

```bash
# 自动修复暂存文件
bash scripts/develop/remove_trailing_space.sh --staged

# 或强制提交（不推荐）
git commit --no-verify -m "message"
```

### Pre-Push 验证失败

**问题**: Docker 构建验证失败

**解决方案**:

1. 查看构建日志，找到失败原因
2. 本地修复问题后重新提交
3. 或使用 `--no-verify` 强制推送（不推荐）

### 版本号检查失败

**问题**: 推送时提示版本号未变更

**解决方案**:

1. 更新 `CMakeLists.txt` 中的版本号
2. 提交版本号变更
3. 重新推送

```bash
# 更新版本号示例
vim CMakeLists.txt  # 修改 VERSION x.y.z
git add CMakeLists.txt
git commit -m "chore: bump version to x.y.z"
git push
```

### Docker 未运行

**问题**: Pre-push Hook 提示 Docker daemon 未运行

**解决方案**:

1. 启动 Docker Desktop (Windows/macOS)
2. 或启动 Docker 服务 (Linux)

```bash
# Linux
sudo systemctl start docker
```

### clang-format 未安装

**问题**: C++ 代码无法自动格式化

**解决方案**:

```bash
# Ubuntu/Debian
sudo apt install clang-format

# macOS
brew install clang-format

# Windows (使用 Chocolatey)
choco install clang-format
```

### ARM64 构建很慢

**问题**: release 分支 Major 版本验证时 ARM64 构建非常慢

**原因**: ARM64 使用 QEMU 仿真，速度较慢

**解决方案**:

1. 耐心等待（首次构建可能需要 30+ 分钟）
2. 使用 `--fast-build` 缓存镜像
3. 在原生 ARM64 环境中构建

## 相关文档

- [环境准备](./01_prerequisites.md)
- [构建系统](./03_build_system.md)
- [常见问题排查](./07_troubleshooting.md)
- [Docker 构建](./05_docker_build.md)
