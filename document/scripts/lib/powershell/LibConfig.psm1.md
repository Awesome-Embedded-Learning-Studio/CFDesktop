---
title: LibConfig.psm1
description: "文档编写日期: 2026-03-20，描述: 获取特定的配置值"
---

# LibConfig.psm1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式

```powershell
Import-Module scripts/lib/powershell/LibConfig.psm1
```text

或者：

```powershell
. "$PSScriptRoot\LibConfig.psm1"
```bash

## Scripts详解

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| Get-IniConfig | 读取 INI 配置文件并返回嵌套哈希表 |
| Get-IniValue | 获取特定的配置值 |
| Test-IniValue | 检查配置项是否存在 |
| Get-DefaultConfigFile | 获取默认配置文件路径 |

---

### Get-IniConfig

**描述**: 读取 INI 配置文件并返回嵌套哈希表结构

**参数**:
- `FilePath` (必需): INI 文件路径

**返回值**: 嵌套哈希表，结构为 @{ section = @{ key = value } }

**INI 文件格式支持**:
- 支持注释行（以 # 或 ; 开头）
- 支持 Section 头部格式 `[section_name]`
- 支持 Key=Value 对
- 自动去除值两端的引号（单引号或双引号）

**异常**: 如果文件不存在，抛出异常

**示例**:
```powershell
# 假设 config.ini 内容为：
# [cmake]
# generator = Ninja
# build_type = Release

$config = Get-IniConfig -FilePath "build_config.ini"
$buildType = $config["cmake"]["build_type"]
Write-LogInfo "Build type: $buildType"
```yaml

---

### Get-IniValue

**描述**: 获取特定的配置值

**参数**:
- `FilePath` (必需): INI 文件路径
- `Section` (必需): Section 名称
- `Key` (必需): Key 名称

**返回值**: 配置值（字符串），如果未找到则返回空字符串

**示例**:
```powershell
$generator = Get-IniValue -FilePath "config.ini" -Section "cmake" -Key "generator"
if (-not [string]::IsNullOrEmpty($generator)) {
    Write-LogInfo "Generator: $generator"
}
```yaml

---

### Test-IniValue

**描述**: 检查配置项是否存在且非空

**参数**:
- `FilePath` (必需): INI 文件路径
- `Section` (必需): Section 名称
- `Key` (必需): Key 名称

**返回值**: Boolean，配置项存在且非空返回 $true，否则返回 $false

**示例**:
```powershell
if (Test-IniValue -FilePath "config.ini" -Section "cmake" -Key "generator") {
    Write-LogInfo "Generator is defined"
} else {
    Write-LogWarning "Generator is not defined"
}
```bash

---

### Get-DefaultConfigFile

**描述**: 根据配置模式获取默认配置文件路径

**参数**:
- `Mode` (可选): 配置模式，可选值为 develop、deploy、ci，默认为 develop
- `ScriptDir` (可选): 脚本目录，默认自动检测

**返回值**: 配置文件的绝对路径

**配置文件映射**:
| Mode | 返回文件 |
|------|----------|
| develop | build_develop_config.ini |
| deploy | build_deploy_config.ini |
| ci | build_ci_config.ini |

**示例**:
```powershell
# 开发模式配置
$configFile = Get-DefaultConfigFile -Mode "develop"

# 部署模式配置
$configFile = Get-DefaultConfigFile -Mode "deploy"

# CI 模式配置，指定脚本目录
$configFile = Get-DefaultConfigFile -Mode "ci" -ScriptDir "C:\Scripts"
```yaml

---

## 完整使用示例

```powershell
# 加载模块
. "$PSScriptRoot\LibConfig.psm1"

# 获取默认配置文件路径
$configFile = Get-DefaultConfigFile -Mode "develop"

# 读取完整配置
$config = Get-IniConfig -FilePath $configFile

# 方式1：直接访问嵌套哈希表
$generator = $config["cmake"]["generator"]
$buildType = $config["cmake"]["build_type"]

# 方式2：使用 Get-IniValue 函数
$installDir = Get-IniValue -FilePath $configFile -Section "paths" -Key "install_dir"

# 方式3：检查配置是否存在
if (Test-IniValue -FilePath $configFile -Section "cmake" -Key "generator") {
    Write-LogSuccess "CMake generator is configured"
}
```text
