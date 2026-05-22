---
title: installhooks.sh / installhooks.ps1
description: "文档编写日期: 2026-03-20，自动安装Git钩子到.git/hooks/目录。"
---

# install_hooks.sh / install_hooks.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```bash
# Linux/macOS
bash scripts/release/hooks/install_hooks.sh

# Windows PowerShell
.\scripts\release\hooks\install_hooks.ps1
```bash

## Scripts详解

### 脚本用途
自动安装Git钩子到.git/hooks/目录。

### 安装的钩子
- **pre-commit**: 代码格式检查（所有分支）
- **pre-push**: Docker构建验证（仅 main 和 release 分支）

### 特性
- **自动备份现有钩子**: 对于非CFDesktop钩子，自动备份为`.backup.YYYYMMDDHHMMSS`格式
- **智能覆盖**: 检测已安装的CFDesktop钩子，直接覆盖无需备份
- **交互式确认**: 安装前需用户确认
- **彩色输出**: 使用ANSI颜色码提供清晰的状态反馈

### 颜色输出说明
| 颜色 | 用途 |
|------|------|
| 蓝色 (>>> ) | 信息提示 |
| 绿色 (✓) | 成功操作 |
| 黄色 (⚠) | 警告信息 |
| 红色 (✗) | 错误信息 |

### 路径解析
脚本自动从当前位置解析：
- 脚本目录: `scripts/release/hooks/`
- 项目根目录: 向上3级目录
- 钩子目标: `.git/hooks/`

### 卸载方法
```bash
# Linux/macOS
rm .git/hooks/pre-commit .git/hooks/pre-push

# Windows PowerShell
Remove-Item .git\hooks\pre-commit, .git\hooks\pre-push
```text

### 验证安装
```bash
# Linux/macOS
ls -la .git/hooks/pre-commit .git/hooks/pre-push

# Windows PowerShell
dir .git\hooks\pre-*
```text

### 相关文件
- `/home/charliechen/CFDesktop/scripts/release/hooks/install_hooks.sh`
- `/home/charliechen/CFDesktop/scripts/release/hooks/install_hooks.ps1`
- `/home/charliechen/CFDesktop/scripts/release/hooks/pre-commit.sample`
- `/home/charliechen/CFDesktop/scripts/release/hooks/pre-push.sample`
