---
title: LibGit.psm1
description: "文档编写日期: 2026-03-20，LibGit.psm1 是一个 Git 辅助函数库，提供版本号"
---

# LibGit.psm1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```powershell
Import-Module scripts/lib/powershell/LibGit.psm1
```bash

## Scripts详解

### 库用途
LibGit.psm1 是一个 Git 辅助函数库，提供版本号解析、Git 仓库状态检查和版本验证级别判断等功能。主要用于构建脚本中获取项目版本信息、比较本地与远程版本差异，以及根据版本变更级别决定构建策略。

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| Get-MajorVersion | 解析版本号，返回主版本号（X.0.0 中的 X） |
| Get-MinorVersion | 解析版本号，返回次版本号（1.X.0 中的 X） |
| Get-PatchVersion | 解析版本号，返回补丁版本号（1.2.X 中的 X） |
| Compare-Versions | 比较两个版本号的大小关系 |
| Get-LocalVersion | 获取当前分支最近的 Git 标签版本 |
| Get-RemoteVersion | 获取远程 main 分支最近的 Git 标签版本 |
| Get-AllVersionTags | 获取所有版本标签列表（按版本排序） |
| Get-LatestVersionTag | 获取最新的版本标签 |
| Get-CmakeVersion | 从 CMakeLists.txt 中提取版本号 |
| Determine-VerifyLevel | 根据本地/远程版本差异确定验证级别 |
| Get-VerifyLevelDescription | 获取验证级别的描述信息 |
| Test-UncommittedChanges | 检查工作区是否有未提交的更改 |
| Get-CurrentBranch | 获取当前 Git 分支名称 |
| Test-MainBranch | 检查是否在 main 分支上 |
| Show-VersionInfo | 打印版本信息（用于调试输出） |

### 核心函数详解

#### Determine-VerifyLevel
```powershell
Determine-VerifyLevel [-LocalVersion] <string> [-RemoteVersion] <string>
```text
根据本地版本和远程版本的差异，确定需要执行的验证级别：

- **major**: 主版本号不同（1.x.x vs 2.x.x），需要 X64 + ARM64 完整构建 + 测试
- **minor**: 次版本号不同（1.2.x vs 1.3.x），需要 X64 完整构建 + 测试
- **patch**: 补丁版本号不同（1.2.3 vs 1.2.4），需要 X64 快速构建 + 测试

#### Compare-Versions
```powershell
Compare-Versions -Version1 <string> -Version2 <string>
```text
比较两个语义化版本号的大小：

- 返回 `-1`: Version1 < Version2
- 返回 `0`: Version1 = Version2
- 返回 `1`: Version1 > Version2

该函数会自动处理不同长度的版本号（如 `1.0` vs `1.0.0`）。

#### Get-LocalVersion / Get-RemoteVersion
```powershell
Get-LocalVersion
Get-RemoteVersion
```text
- `Get-LocalVersion`: 获取当前分支最近的 Git 标签，若无标签返回 `"0.0.0"`
- `Get-RemoteVersion`: 自动执行 `git fetch` 获取最新远程信息，然后返回远程 main 分支的最新标签

### 导出的变量
无导出变量。

### 依赖关系
- 需要 Git 命令行工具在系统 PATH 中可用
- 无其他 PowerShell 模块依赖

### 注意事项
1. 所有 Git 操作失败时会静默返回默认值，不会抛出异常
2. 版本号解析假设使用语义化版本格式（Major.Minor.Patch）
3. `Get-RemoteVersion` 会自动执行 `git fetch`，可能需要网络连接
4. 验证级别的定义与项目构建流程相关，修改时需同步更新构建脚本
