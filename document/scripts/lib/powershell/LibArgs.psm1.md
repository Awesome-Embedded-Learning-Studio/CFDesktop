---
title: LibArgs.psm1
description: "文档编写日期: 2026-03-20，LibArgs.psm1 提供命令行参数解析和用户帮助信息显示"
---

# LibArgs.psm1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```powershell
Import-Module scripts/lib/powershell/LibArgs.psm1
```bash

## Scripts详解

### 库用途
LibArgs.psm1 提供命令行参数解析和用户帮助信息显示功能。该模块定义了一套标准的命令行接口规范，支持三种配置模式（develop/deploy/ci）和自定义配置文件选项，统一了项目中构建相关脚本的用户体验。

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| Parse-ConfigMode | 解析配置模式参数，验证有效性 |
| Test-ValidConfigMode | 测试配置模式是否有效（返回布尔值） |
| Show-StandardUsage | 显示单行标准用法信息 |
| Show-DetailedUsage | 显示详细的帮助信息（含示例） |
| Test-HelpArg | 检查参数是否为帮助请求 |
| Show-UnknownArgError | 显示未知参数错误信息 |
| Show-MissingValueError | 显示缺失参数值错误信息 |

### 核心函数详解

#### Parse-ConfigMode
```powershell
Parse-ConfigMode [-Mode] <string>
```text
解析并验证配置模式参数。

**有效模式：**
- `develop`: 开发构建配置
- `deploy`: 部署构建配置
- `ci`: CI 构建配置

**返回值：**
- 有效时返回模式字符串本身
- 无效时返回 `$null`

```powershell
$mode = Parse-ConfigMode "develop"  # 返回 "develop"
$mode = Parse-ConfigMode "invalid"  # 返回 $null
```text

#### Show-DetailedUsage
```powershell
Show-DetailedUsage [[-ScriptName] <string>] [[-Description] <string>]
```text
显示格式化的详细帮助信息，包括：
- 脚本名称（带边框）
- 脚本描述（如果提供）
- 参数说明（develop/deploy/ci）
- 选项说明（-c/--config, -h/--help）
- 使用示例

**参数：**
- `ScriptName`: 脚本名称（默认从 `$MyInvocation.ScriptName` 获取）
- `Description`: 脚本描述（可选）

**输出示例：**
```yaml
========================================
  build.ps1
========================================

Build the project

Arguments:
  develop    Use development build configuration
  deploy     Use deployment build configuration
  ci         Use CI build configuration

Options:
  -c, --config <file>  Use specified configuration file
  -h, --help           Show this help message

Examples:
  .\build.ps1 develop
  .\build.ps1 deploy -c custom_config.ini
```text

#### Test-HelpArg
```powershell
Test-HelpArg [-Arg] <string>
```text
检查参数是否为帮助请求。

**识别的帮助参数：**
- `-h`
- `--help`
- `help`

```powershell
if (Test-HelpArg $args[0]) {
    Show-DetailedUsage
    exit
}
```text

### 标准命令行接口

此模块定义的命令行接口规范：

```text
script.ps1 [develop|deploy|ci] [-c|--config <config_file>] [-h|--help]
```cmake

**参数说明：**
| 位置参数 | 说明 |
|---------|------|
| develop | 使用开发构建配置 |
| deploy | 使用部署构建配置 |
| ci | 使用 CI 构建配置 |

**选项：**
| 选项 | 说明 |
|------|------|
| `-c, --config <file>` | 使用指定的配置文件 |
| `-h, --help` | 显示帮助信息 |

### 导出的变量
无导出变量。

### 依赖关系
- 无外部依赖
- 使用标准 PowerShell 输出命令（`Write-Host`）

### 注意事项
1. 所有输出使用 `Write-Host`，适合直接向控制台输出
2. 错误信息使用颜色编码（红色用于错误，黄色用于提示）
3. 模块不执行实际的参数解析，只提供验证和显示功能
4. 建议在脚本中按以下模式使用：
   ```powershell
   Import-Module scripts/lib/powershell/LibArgs.psm1

   if (Test-HelpArg $args[0]) {
       Show-DetailedUsage -Description "My script description"
       exit
   }

   $mode = Parse-ConfigMode $args[0]
   if (-not $mode) {
       Show-UnknownArgError -Arg $args[0]
       exit 1
   }
   ```
