---
title: LibPaths.psm1
description: "文档编写日期: 2026-03-20，LibPaths.psm1 提供路径解析和目录管理功能。该模块"
---

# LibPaths.psm1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```powershell
Import-Module scripts/lib/powershell/LibPaths.psm1
```bash

## Scripts详解

### 库用途
LibPaths.psm1 提供路径解析和目录管理功能。该模块专门设计用于在脚本执行时准确定位项目目录结构，无论脚本从何处被调用。它解决了 PowerShell 模块中被调用脚本路径检测的复杂问题。

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| Get-ScriptDir | 获取调用脚本所在目录的绝对路径 |
| Get-ProjectRoot | 获取项目根目录（假设脚本在 scripts/ 子目录中） |
| Get-ScriptsDir | 获取 scripts 目录的绝对路径 |
| Get-LibDir | 获取 lib 目录的绝对路径 |
| Test-PathExistence | 检查指定路径是否存在 |
| New-Directory | 确保目录存在，不存在则创建 |
| ConvertTo-AbsolutePath | 将相对路径解析为绝对路径 |

### 核心函数详解

#### Get-ScriptDir
```powershell
Get-ScriptDir
```text
获取调用此模块的脚本所在目录的绝对路径。这是模块的核心函数，使用多种回退机制确保在各种执行场景下都能正确获取路径：

**检测机制（按优先级）：**
1. 检查调用者设置的全局变量（`$global:CallerPSScriptRoot` 或 `$global:CallerMyInvocationPath`）
2. 解析命令行参数中的 `-File` 参数
3. 遍历调用堆栈，查找非 `.psm1` 文件的脚本路径
4. 从调用堆栈的 `Command` 属性推断路径（支持相对路径）
5. 尝试从调用者作用域获取 `MyInvocation` 或 `PSScriptRoot`
6. 最后回退到当前工作目录

**支持的调用场景：**
- `powershell -File script.ps1`
- `.\script.ps1`（相对路径）
- 在 VSCode / IDE 中调试
- 从其他脚本中 dot-sourcing

#### Get-ProjectRoot
```powershell
Get-ProjectRoot
```text
获取项目根目录。假设脚本位于 `scripts/` 的子目录中，函数会向上追溯两级目录。

**目录结构假设：**
```text
ProjectRoot/
  scripts/
    lib/
      LibPaths.psm1
    some-script.ps1  <-- 从这里调用
```text

#### ConvertTo-AbsolutePath
```powershell
ConvertTo-AbsolutePath [-Path] <string> [[-BasePath] <string>]
```text
将相对路径转换为绝对路径。

**参数：**
- `Path`: 要解析的路径（必需）
- `BasePath`: 基础路径（可选，默认为当前目录）

**行为：**
- 如果输入已经是绝对路径，直接返回
- 相对路径会基于 `BasePath` 解析
- 解析失败时返回拼接后的路径

#### New-Directory
```powershell
New-Directory [-Path] <string>
```text
确保指定目录存在。如果目录不存在则创建，已存在则不做任何操作。

### 导出的变量
无导出变量。

### 依赖关系
- 使用 `Get-CimInstance` 检测 Win32_Process（Windows 平台）
- 使用 `Get-PSCallStack` 遍历调用堆栈

### 注意事项
1. `Get-ScriptDir` 的实现较为复杂，这是为了兼容各种 PowerShell 执行场景
2. 建议在调用脚本中设置 `$global:CallerPSScriptRoot` 以获得最可靠的结果
3. 搜索脚本路径时会递归搜索最多 3 层子目录和向上 6 级父目录
4. 在非 Windows 平台上，`Get-CimInstance` 相关的检测会静默失败并跳过
5. 路径检测的回退机制可能在某些极端情况下返回当前工作目录而非实际脚本目录
