---
title: removetrailingspace.ps1
description: "文档编写日期: 2026-03-20，Windows PowerShell版本的行尾空白删除工具，与"
---

# remove_trailing_space.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\develop\remove_trailing_space.ps1 [OPTIONS]
```bash

### 参数说明
| 参数 | 说明 |
|------|------|
| -n, --dry-run, -DryRun | 显示将要更改的内容，不修改文件 |
| -c, --check, -Check | 检查模式，发现空格返回错误码1 |
| -s, --staged, -Staged | 仅处理已暂存的文件 |
| -S, --staged-check, -StagedCheck | 检查已暂存文件（用于pre-commit钩子） |
| -h, --help, -Help | 显示帮助信息 |

### 使用示例
```powershell
# 删除所有文件的行尾空格
.\scripts\develop\remove_trailing_space.ps1

# 仅处理暂存文件
.\scripts\develop\remove_trailing_space.ps1 -Staged

# 预览哪些文件有空格
.\scripts\develop\remove_trailing_space.ps1 -DryRun

# 检查暂存文件（pre-commit钩子）
.\scripts\develop\remove_trailing_space.ps1 -StagedCheck
```powershell

## Scripts详解 (Detailed Explanation)

### 脚本用途
Windows PowerShell版本的行尾空白删除工具，与bash版本功能对等。

### 排除的文件类型
- **图片**: .png, .jpg, .jpeg, .gif, .ico, .bmp, .svg
- **归档**: .pdf, .zip, .tar, .gz, .7z, .rar
- **字体**: .ttf, .otf, .woff, .woff2, .eot

### 工作模式

| 模式 | 说明 |
|------|------|
| 正常模式 | 处理git追踪的所有文件，自动删除行尾空格 |
| Staged模式 | 仅处理git已暂存的文件 |
| Dry Run | 预览模式，不修改文件，显示行号和内容 |
| Check | 检查模式，有问题返回错误码1 |
| StagedCheck | 检查暂存文件但不修改（pre-commit钩子） |

### Git集成
- 使用 `git ls-files` 获取追踪文件（尊重.gitignore）
- 使用 `git diff --cached --name-only --diff-filter=ACM` 获取暂存文件
- 过滤器 `ACM` 仅包含 Added, Copied, Modified 的文件

### 处理细节
- 使用 `Get-Content -Encoding UTF8` 读取文件
- 使用正则 `\s+$` 匹配行尾空白
- 使用 `$_ -replace '\s+$', ''` 删除行尾空白
- 使用 `Out-File -Encoding UTF8 -NoNewline` 写入文件
- try-catch 自动跳过二进制文件

### 输出格式
```yaml
=== Remove Trailing Whitespace ===
Project: C:\path\to\project
Mode: Staged files only

src/main.cpp:
  12:int main()    ← trailing space
  45:    return 0; ← trailing space

=== Summary ===
Processed: 150 files
Fixed: 2 files
```bash

### 退出码
| 退出码 | 说明 |
|--------|------|
| 0 | 成功，无行尾空白或已修复 |
| 1 | 检查模式下发现行尾空白 |

### 依赖模块
- LibCommon.psm1 - 提供 `Write-LogInfo`, `Write-LogSuccess`, `Write-LogWarning`, `Write-LogError` 等日志函数
- LibPaths.psm1 - 提供 `Get-ScriptDir`, `Get-ProjectRoot` 路径函数

### 依赖项
- git (必需)
- PowerShell 5.1+
- PowerShell模块: LibCommon.psm1, LibPaths.psm1

### 注意事项
- UTF-8编码读写文件
- 自动跳过二进制文件（通过扩展名过滤和try-catch）
- 支持Windows路径和Unix路径混合显示
- 使用 `$ErrorActionPreference = "Stop"` 确保错误传播
