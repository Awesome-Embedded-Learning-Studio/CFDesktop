---
title: PowerShell 脚本库
description: 本目录包含 PowerShell 共享工具函数，为 Windows 平台的构建、测试与发布脚本提供通用功能支持，包括日志输出、路径处理、环境检测与 CMake 封装调用等，与 Bash 脚本库保持功能对齐。
---

# PowerShell 脚本库

本目录包含 PowerShell 共享工具函数，为 Windows 平台的构建、测试与发布脚本提供通用功能支持，包括日志输出、路径处理、环境检测与 CMake 封装调用等，与 Bash 脚本库保持功能对齐。

## 模块列表

| 文件 | 说明 |
|------|------|
| LibCommon.psm1 | 日志、通用工具函数 |
| LibBuild.psm1 | 构建相关函数（CMake、目录管理） |
| LibConfig.psm1 | INI 配置文件解析 |
| LibGit.psm1 | Git 相关操作函数 |
| LibPaths.psm1 | 路径处理函数 |
| LibArgs.psm1 | 参数解析函数 |

## 使用方式

### 单个模块加载

```powershell
Import-Module scripts/lib/powershell/LibCommon.psm1
Import-Module scripts/lib/powershell/LibConfig.psm1
```

### 脚本内加载（点号加载）

```powershell
. "$PSScriptRoot\LibCommon.psm1"
. "$PSScriptRoot\LibConfig.psm1"
. "$PSScriptRoot\LibBuild.psm1"
```

## 模块详细文档

- [LibCommon.psm1](LibCommon.psm1.md) - 日志输出功能
- [LibBuild.psm1](LibBuild.psm1.md) - 构建工具函数
- [LibConfig.psm1](LibConfig.psm1.md) - 配置文件解析

## 快速参考

### 日志输出 (LibCommon)

```powershell
Write-LogInfo "Information message"
Write-LogSuccess "Operation completed"
Write-LogWarning "Warning message"
Write-LogError "Error message"
Write-LogSeparator
Write-LogProgress -Current 5 -Total 10 -Message "Processing"
```

### 构建操作 (LibBuild)

```powershell
# 清理构建目录
Clean-BuildDir "C:\Build\Output"

# 确保目录存在
Ensure-BuildDir "C:\Build\Output"

# CMake 配置
Invoke-CMakeConfigure -Generator "Ninja" -BuildType "Release" -SourceDir "." -BuildDir "build"

# CMake 构建
Invoke-CMakeBuild -BuildDir "build" -Parallel (Get-ParallelJobCount)
```

### 配置读取 (LibConfig)

```powershell
# 读取完整配置
$config = Get-IniConfig -FilePath "config.ini"
$value = $config["section"]["key"]

# 读取单个值
$value = Get-IniValue -FilePath "config.ini" -Section "section" -Key "key"

# 检查配置存在
if (Test-IniValue -FilePath "config.ini" -Section "section" -Key "key") {
    # 配置存在
}
```
