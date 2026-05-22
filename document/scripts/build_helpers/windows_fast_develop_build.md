---
title: windowsfastdevelopbuild.ps1
description: "文档编写日期: 2026-03-20，本脚本执行快速开发构建，包含："
---

# windows_fast_develop_build.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\build_helpers\windows_fast_develop_build.ps1
```text

### 使用示例
```powershell
# 执行快速开发构建 (配置 + 编译)
.\scripts\build_helpers\windows_fast_develop_build.ps1
```text

## Scripts详解

### 脚本用途
本脚本执行**快速开发构建**，包含：
1. 调用CMake配置脚本
2. 执行项目编译

**不执行清理操作**，适用于增量编译场景，如：
- 日常开发中的频繁编译
- 仅修改少量源文件后的重新编译
- 需要快速验证代码更改

### 依赖模块
- **LibCommon.psm1** - 提供日志输出、分隔线等通用功能
- **LibConfig.psm1** - 提供INI配置文件读取功能
- **LibPaths.psm1** - 提供路径解析和项目根目录定位功能
- **LibBuild.psm1** - 提供构建计时器功能

### 配置文件
使用 `build_develop_config.ini` 配置文件。

### 执行流程

#### Step 1: 调用配置脚本
执行 `windows_configure.ps1 -Config develop` 进行CMake配置。

#### Step 2: 加载配置并编译
从配置文件读取构建目录和并行任务数，然后执行编译：
```bash
cmake --build <build_dir> [--parallel <jobs>]
```bash

### 构建计时
脚本使用`Start-BuildTimer`和`Stop-BuildTimer`记录编译耗时。

### 安全检查
- 配置文件存在性检查
- `build_dir`非空检查

### 退出代码
- `0` - 编译成功
- 非`0` - 配置或编译失败

### 与完整构建的区别
| 特性 | windows_develop_build.ps1 | windows_fast_develop_build.ps1 |
|------|---------------------------|-------------------------------|
| 清理构建目录 | 是 | 否 |
| 运行测试 | 是 | 否 |
| 执行速度 | 较慢 | 较快 |
| 适用场景 | 完全重新构建 | 增量编译 |

### 配置文件选项
在 `build_develop_config.ini` 的 `[options]` 节可设置：
```ini
[options]
jobs = 8  # 并行编译任务数，留空则使用CMake默认值
```text

### 注意事项
- 本脚本不运行测试，如需测试请使用 `windows_run_tests.ps1`
- 如果遇到奇怪的编译问题，建议使用 `windows_develop_build.ps1` 进行完整清理重建
