---
title: installhooks.ps1
description: "文档编写日期: 2026-03-20，Windows PowerShell版本的Git钩子安装脚本，"
---

# install_hooks.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
# Windows PowerShell
.\scripts\release\hooks\install_hooks.ps1
```text

## Scripts详解

### 脚本用途
Windows PowerShell版本的Git钩子安装脚本，自动安装Git钩子到.git/hooks/目录。

### 依赖模块
```text
scripts\lib\powershell\LibPaths.psm1
```bash
提供路径解析功能模块。

### 安装的钩子
- **pre-commit**: 代码格式检查（所有分支）
- **pre-push**: Docker构建验证（仅 main 和 release 分支）

### 特性
- **自动备份现有钩子**: 对于非CFDesktop钩子，自动备份为`.backup.YYYYMMDDHHMMSS`格式
- **智能覆盖**: 检测已安装的CFDesktop钩子（通过"CFDesktop Git Hooks"标记），直接覆盖无需备份
- **交互式确认**: 安装前需用户确认
- **彩色输出**: 使用ANSI转义码提供清晰的状态反馈

### 颜色输出说明
| 颜色 | 用途 |
|------|------|
| 蓝色 (>>> ) | 信息提示 |
| 绿色 (✓) | 成功操作 |
| 黄色 (⚠) | 警告信息 |
| 红色 (✗) | 错误信息 |

### 路径解析
脚本通过LibPaths.psm1模块自动解析路径：
- 脚本目录: `scripts/release/hooks/`
- 项目根目录: 向上3级目录
- 钩子目标: `.git/hooks/`

### 函数说明
| 函数 | 说明 |
|------|------|
| Log-Info | 输出蓝色信息 |
| Log-Success | 输出绿色成功信息 |
| Log-Warning | 输出黄色警告信息 |
| Log-Error | 输出红色错误信息 |
| Backup-ExistingHook | 备份现有钩子（仅非CFDesktop钩子） |
| Install-Hook | 安装单个钩子文件 |

### 卸载方法
```powershell
Remove-Item .git\hooks\pre-commit, .git\hooks\pre-push
```text

### 验证安装
```powershell
dir .git\hooks\pre-*
```text

### 相关文件
- `/home/charliechen/CFDesktop/scripts/release/hooks/install_hooks.ps1`
- `/home/charliechen/CFDesktop/scripts/lib/powershell/LibPaths.psm1`
- `/home/charliechen/CFDesktop/scripts/release/hooks/pre-commit.sample`
- `/home/charliechen/CFDesktop/scripts/release/hooks/pre-push.sample`
