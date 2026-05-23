# Git Hooks

CFDesktop 项目的 Git hooks 配置。

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
| `pre-commit.sample` | 代码格式检查钩子 |
| `pre-push.sample` | Docker 构建验证钩子（自动检测验证级别） |
| `version_utils.sh` | 版本号解析辅助函数 |
| `install_hooks.sh` | Linux/macOS 安装脚本 |
| `install_hooks.ps1` | Windows 安装脚本 |

## 验证级别

- **main 分支**: X64 FastBuild + Tests
- **release 分支**: 根据 Major/Minor/Patch 自动检测
- **feat 分支**: 跳过 pre-push 验证

## 详细文档

完整使用指南请参考: [document/release_rule/git_hooks_guide.md](../../../document/release_rule/git_hooks_guide.md)

> 完整脚本文档: [document/scripts/release/hooks/](../../../document/scripts/release/hooks/)
