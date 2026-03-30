# Release Helpers

CFDesktop 项目发布流程与版本管理工具。

## Release Process Overview

发布流程通过 Git hooks 和构建脚本自动化管理。当代码推送到 `main` 或 `release` 分支时，Git hooks 会根据版本变更类型自动选择对应的验证级别，确保发布质量。

### 版本号格式

版本号遵循 `Major.Minor.Patch` 三段式格式（语义化版本）。

## Version Changes and Verification Levels

### Major Change（主版本变更）

主版本变更需运行全量构建，包含所有平台的编译验证，确保变更在所有平台上可用（包括未来的交叉编译目标）。

必须验证：
- X64 Build
- ARM64 Build

### Minor Change（次版本变更）

次版本变更需在主平台运行完整构建验证。

必须验证：
- X64 Build

> 如果遇到其他平台的编译问题，以 Patch 方式修复，不重新发布 Minor 版本。

### Patch Version Change / Local Feature Branch Merge（补丁版本变更）

补丁版本变更或本地功能分支合并到主分支时，运行快速构建和测试即可。

必须验证：
- X64 FastBuild（增量编译，不强制全量重编译）
- X64 Tests

## Git Hooks for Release

Git hooks 自动检测版本变更类型并选择验证级别：

| 分支 | 验证级别 |
|------|----------|
| `main` 分支 | X64 FastBuild + Tests |
| `release` 分支 | 根据 Major/Minor/Patch 自动检测 |
| `feat` 分支 | 跳过 pre-push 验证 |

### 安装 Git Hooks

```bash
# Linux/macOS
bash scripts/release/hooks/install_hooks.sh

# Windows PowerShell
.\scripts\release\hooks\install_hooks.ps1
```

### Hook 文件说明

| 文件 | 说明 |
|------|------|
| `pre-commit.sample` | 代码格式检查钩子 |
| `pre-push.sample` | Docker 构建验证钩子（自动检测验证级别） |
| `version_utils.sh` | 版本号解析辅助函数 |

详细使用指南请参考：[document/release_rule/git_hooks_guide.md](../../document/release_rule/git_hooks_guide.md)

## How to Create a Release

1. 确保本地代码已通过所有测试
2. 安装 Git hooks（如尚未安装）
3. 更新版本号（根据变更类型选择 Major / Minor / Patch）
4. 提交并推送到对应分支，Git hooks 将自动执行验证构建
5. 验证通过后，CI 完成构建和部署

## 目录结构

```
scripts/release/
├── README.md               # This file
└── hooks/                  # Git hooks
    ├── install_hooks.sh    # Linux/macOS 安装脚本
    ├── install_hooks.ps1   # Windows 安装脚本
    ├── pre-commit.sample   # pre-commit 钩子示例
    ├── pre-push.sample     # pre-push 钩子示例
    ├── version_utils.sh    # 版本号解析工具
    └── README.md           # Git hooks 详细说明
```
