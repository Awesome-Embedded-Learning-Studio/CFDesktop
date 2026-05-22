---
title: formatcpp.ps1
description: "文档编写日期: 2026-03-20，使用  自动格式化项目中的 C++ 源代码文件，确保代码风格一"
---

# format_cpp.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```powershell
.\scripts\develop\format_cpp.ps1 [OPTIONS]
```bash

### 参数说明

| 参数 | 说明 |
|------|------|
| `-DryRun` | 显示将要更改的内容，不修改文件 |
| `-Check` | 检查文件格式，需要格式化的文件返回错误码1 |

### 使用示例

```powershell
# 格式化所有C++文件
.\scripts\develop\format_cpp.ps1

# 检查哪些文件需要格式化
.\scripts\develop\format_cpp.ps1 -Check

# 预览格式化效果
.\scripts\develop\format_cpp.ps1 -DryRun
```bash

## Scripts详解 (Detailed Explanation)

### 脚本用途

使用 `clang-format` 自动格式化项目中的 C++ 源代码文件，确保代码风格一致。这是 Bash 版本 `format_cpp.sh` 的 PowerShell 等效实现。

### 支持的文件类型

| 扩展名 | 说明 |
|--------|------|
| `.cpp` | C++ 源文件 |
| `.cc` | C++ 源文件 |
| `.cxx` | C++ 源文件 |
| `.h` | 头文件 |
| `.hpp` | C++ 头文件 |

### 排除目录

- `third_party` - 第三方依赖
- `build` - 构建输出目录
- `out` - 输出目录
- `.git` - Git 版本控制目录

### 依赖项

| 依赖项 | 说明 |
|--------|------|
| **clang-format** | C++ 代码格式化工具 (必需) |
| **LibCommon.psm1** | PowerShell 通用库模块 |
| **LibPaths.psm1** | PowerShell 路径处理模块 |

### 安装 clang-format

```powershell
# Windows (使用 LLVM 安装器)
# 从 https://releases.llvm.org/ 下载 LLVM 安装包

# 或使用 WSL/Linux 子系统
sudo apt install clang-format
```bash

### 工作模式

| 模式 | 说明 |
|------|------|
| **默认模式** | 直接格式化文件，就地修改 |
| **DryRun** | 显示将要格式化的文件列表，不修改任何文件 |
| **Check** | 检查模式，需要格式化的文件会返回错误码1，适用于 CI/CD |

### 模块导入

脚本从 `scripts/lib/powershell/` 目录导入以下模块：

```powershell
Import-Module LibCommon.psm1  # 提供日志函数 (Write-LogInfo, Write-LogSuccess 等)
Import-Module LibPaths.psm1   # 提供路径函数 (Get-ProjectRoot)
```bash

### 输出日志

使用 `LibCommon.psm1` 模块提供的日志函数：

- `Write-LogSuccess` - 成功消息
- `Write-LogWarning` - 警告消息
- `Write-LogError` - 错误消息
- `Write-LogInfo` - 信息消息

### 配置文件

使用项目根目录的 `.clang-format` 配置文件。

### 返回码

| 返回码 | 说明 |
|--------|------|
| 0 | 所有文件格式正确或格式化成功 |
| 1 | clang-format 未安装或 Check 模式下发现需要格式化的文件 |

### 注意事项

1. 确保已安装 `clang-format` 并在 PATH 中
2. 确保 PowerShell 模块 (`LibCommon.psm1`, `LibPaths.psm1`) 存在于 `scripts/lib/powershell/` 目录
3. 使用 `-Check` 参数可用于 CI/CD 流水线中验证代码格式
4. 脚本自动设置 `$ErrorActionPreference = "Stop"` 确保错误时停止执行
