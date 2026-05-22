---
title: LibCommon.psm1
description: "文档编写日期: 2026-03-20，描述: 写入 INFO 级别日志（青色显示）"
---

# LibCommon.psm1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式

```powershell
Import-Module scripts/lib/powershell/LibCommon.psm1
```text

或者：

```powershell
. "$PSScriptRoot\LibCommon.psm1"
```bash

## Scripts详解

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| Write-Log | 写入带时间戳和颜色编码的日志消息 |
| Write-LogInfo | 写入 INFO 级别日志（青色） |
| Write-LogSuccess | 写入 SUCCESS 级别日志（绿色） |
| Write-LogWarning | 写入 WARNING 级别日志（黄色） |
| Write-LogError | 写入 ERROR 级别日志（红色） |
| Write-LogSeparator | 写入分隔线 |
| Write-LogProgress | 显示进度信息 |

---

### Write-Log

**描述**: 写入带时间戳和颜色编码的日志消息

**参数**:
- `Message` (必需): 要输出的消息内容
- `Level` (可选): 日志级别，可选值为 INFO、SUCCESS、WARNING、ERROR，默认为 INFO

**返回值**: 无

**示例**:
```powershell
Write-Log -Message "Build completed" -Level "SUCCESS"
```yaml

---

### Write-LogInfo

**描述**: 写入 INFO 级别日志（青色显示）

**参数**:
- `Message` (必需): 消息内容，支持多个参数自动拼接

**返回值**: 无

**示例**:
```powershell
Write-LogInfo "Starting build process"
Write-LogInfo "Processing" "file" "1.txt"
```yaml

---

### Write-LogSuccess

**描述**: 写入 SUCCESS 级别日志（绿色显示）

**参数**:
- `Message` (必需): 消息内容，支持多个参数自动拼接

**返回值**: 无

**示例**:
```powershell
Write-LogSuccess "Build completed successfully"
```yaml

---

### Write-LogWarning

**描述**: 写入 WARNING 级别日志（黄色显示）

**参数**:
- `Message` (必需): 消息内容，支持多个参数自动拼接

**返回值**: 无

**示例**:
```powershell
Write-LogWarning "Configuration file not found, using defaults"
```yaml

---

### Write-LogError

**描述**: 写入 ERROR 级别日志（红色显示）

**参数**:
- `Message` (必需): 消息内容，支持多个参数自动拼接

**返回值**: 无

**示例**:
```powershell
Write-LogError "Build failed with exit code: 1"
```yaml

---

### Write-LogSeparator

**描述**: 写入分隔线

**参数**:
- `Char` (可选): 分隔线字符，默认为 "="
- `Width` (可选): 分隔线宽度，默认为 40

**返回值**: 无

**示例**:
```powershell
Write-LogSeparator
Write-LogSeparator -Char "-" -Width 60
```yaml

---

### Write-LogProgress

**描述**: 显示进度信息，格式为 [当前/总数] (百分比%) 描述

**参数**:
- `Current` (必需): 当前进度数
- `Total` (必需): 总数
- `Message` (可选): 进度描述，默认为 "Processing"

**返回值**: 无

**示例**:
```powershell
Write-LogProgress -Current 5 -Total 10 -Message "Processing files"
# 输出: [5/10] (50%) Processing files
```bash

---

## 日志级别颜色映射

| 级别 | 颜色 |
|------|------|
| INFO | Cyan (青色) |
| SUCCESS | Green (绿色) |
| WARNING | Yellow (黄色) |
| ERROR | Red (红色) |
