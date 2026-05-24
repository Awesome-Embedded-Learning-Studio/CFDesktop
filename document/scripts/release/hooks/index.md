---
title: Git Hooks
description: 本目录包含发布流程中使用的 Git Hooks 脚本，用于在 `git push` 等关键操作前后自动执行代码检查、文档生成和版本一致性验证，防止不符合规范的代码进入远程仓库。
---

# Git Hooks

本目录包含发布流程中使用的 Git Hooks 脚本，用于在 `git push` 等关键操作前后自动执行代码检查、文档生成和版本一致性验证，防止不符合规范的代码进入远程仓库。

## 快速安装

```bash
# Linux/macOS
bash scripts/release/hooks/install_hooks.sh

# Windows PowerShell
.\scripts\release\hooks\install_hooks.ps1
```

## 文件说明

| 文件 | 说明 |
|------|------|
| pre-commit.sample | 代码格式检查钩子 |
| pre-push.sample | Docker构建验证钩子 |
| version_utils.sh | 版本号解析辅助函数 |
| install_hooks.sh | Linux/macOS安装脚本 |
| install_hooks.ps1 | Windows安装脚本 |

## 验证级别

- **main分支**: X64 FastBuild + Tests
- **release分支**: 根据Major/Minor/Patch自动检测
- **feat分支**: 跳过pre-push验证

## 详细文档

完整使用指南请参考: document/release_rule/git_hooks_guide.md
