---
title: LibBuild.psm1
description: "文档编写日期: 2026-03-20，描述: 确保构建目录存在，不存在则创建"
---

# LibBuild.psm1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式

```powershell
# LibBuild 依赖 LibCommon，需先加载
Import-Module scripts/lib/powershell/LibCommon.psm1
Import-Module scripts/lib/powershell/LibBuild.psm1
```text

或者：

```powershell
. "$PSScriptRoot\LibCommon.psm1"
. "$PSScriptRoot\LibBuild.psm1"
```bash

## Scripts详解

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| Clean-BuildDir | 清理构建目录（带安全检查） |
| Ensure-BuildDir | 确保构建目录存在 |
| Invoke-CMakeConfigure | 运行 CMake 配置 |
| Invoke-CMakeBuild | 运行 CMake 构建 |
| Test-CmakeCache | 检查 CMake 缓存是否存在 |
| Get-CmakeCacheVar | 获取 CMake 缓存中的变量值 |
| Get-ParallelJobCount | 获取推荐的并行作业数 |
| Start-BuildTimer | 开始构建计时 |
| Stop-BuildTimer | 结束构建计时并显示耗时 |

---

### Clean-BuildDir

**描述**: 清理构建目录（带多重安全检查）

**参数**:
- `BuildPath` (必需): 构建目录路径

**安全检查**:
- 路径不能为空
- 路径不能是根目录
- 路径不能是用户主目录
- 路径不能是项目根目录（如果已设置 $global:ProjectRoot）
- 目录名称必须匹配预期模式：build、out、cmake-build 或 build-*

**返回值**: Boolean，成功返回 $true，失败返回 $false

**示例**:
```powershell
Clean-BuildDir "C:\Build\Output"
```yaml

---

### Ensure-BuildDir

**描述**: 确保构建目录存在，不存在则创建

**参数**:
- `BuildPath` (必需): 构建目录路径

**返回值**: 无

**示例**:
```powershell
Ensure-BuildDir "C:\Build\Output"
```yaml

---

### Invoke-CMakeConfigure

**描述**: 运行 CMake 配置命令

**参数**:
- `Generator` (必需): 生成器名称，如 "Ninja"、"Visual Studio 17 2022"
- `BuildType` (必需): 构建类型，可选值为 Debug、Release、RelWithDebInfo
- `SourceDir` (必需): 源代码目录
- `BuildDir` (必需): 构建目录
- `ExtraArgs` (可选): 额外的 CMake 参数数组

**返回值**: Boolean，成功返回 $true，失败返回 $false

**示例**:
```powershell
Invoke-CMakeConfigure -Generator "Ninja" -BuildType "Release" -SourceDir "." -BuildDir "build"
```text

带额外参数：
```powershell
Invoke-CMakeConfigure -Generator "Ninja" -BuildType "Release" -SourceDir "." -BuildDir "build" -ExtraArgs @("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
```yaml

---

### Invoke-CMakeBuild

**描述**: 运行 CMake 构建命令

**参数**:
- `BuildDir` (必需): 构建目录
- `Target` (可选): 目标名称，默认为 "--all"（构建所有）
- `Parallel` (可选): 并行作业数，默认为 0（自动）

**返回值**: Boolean，成功返回 $true，失败返回 $false

**示例**:
```powershell
# 构建所有目标，自动并行
Invoke-CMakeBuild -BuildDir "build"

# 构建特定目标，指定并行数
Invoke-CMakeBuild -BuildDir "build" -Target "myapp" -Parallel 4
```yaml

---

### Test-CmakeCache

**描述**: 检查 CMake 缓存文件是否存在

**参数**:
- `BuildDir` (必需): 构建目录

**返回值**: Boolean，缓存存在返回 $true，否则返回 $false

**示例**:
```powershell
if (Test-CmakeCache "build") {
    Write-LogInfo "CMake cache exists"
}
```yaml

---

### Get-CmakeCacheVar

**描述**: 从 CMake 缓存文件中读取变量值

**参数**:
- `BuildDir` (必需): 构建目录
- `VarName` (必需): 变量名

**返回值**: 变量值（字符串），未找到返回空字符串

**示例**:
```powershell
$generator = Get-CmakeCacheVar -BuildDir "build" -VarName "CMAKE_GENERATOR"
Write-LogInfo "Generator: $generator"
```yaml

---

### Get-ParallelJobCount

**描述**: 获取系统推荐的并行作业数

**参数**: 无

**返回值**: CPU 核心数（整数）

**示例**:
```powershell
$jobs = Get-ParallelJobCount
Write-LogInfo "Using $jobs parallel jobs"
```yaml

---

### Start-BuildTimer

**描述**: 开始构建计时，记录开始时间到脚本变量

**参数**: 无

**返回值**: 无

**示例**:
```powershell
Start-BuildTimer
# ... 执行构建 ...
Stop-BuildTimer
```yaml

---

### Stop-BuildTimer

**描述**: 结束构建计时并显示耗时，格式为 "Build time: Xm Ys"

**参数**: 无

**返回值**: 无

**示例**:
```powershell
Start-BuildTimer
# ... 执行构建 ...
Stop-BuildTimer
# 输出示例: [2026-03-20 10:30:45] [INFO] Build time: 2m 15s
```yaml

---

## 依赖关系

LibBuild.psm1 依赖 LibCommon.psm1 提供的日志函数，因此必须先加载 LibCommon.psm1。
