---
title: windowsdevelopbuild.ps1
description: "文档编写日期: 2026-03-20，本脚本执行完整的开发构建流程，包含以下步骤："
---

# windows_develop_build.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\build_helpers\windows_develop_build.ps1
```text

### 使用示例
```powershell
# 执行完整的开发构建 (清理 + 配置 + 编译 + 测试)
.\scripts\build_helpers\windows_develop_build.ps1
```bash

## Scripts详解

### 脚本用途
本脚本执行**完整的开发构建流程**，包含以下步骤：
1. 清理构建目录
2. 调用快速构建脚本进行配置和编译
3. 自动运行测试

适用于需要确保完全重新编译的场景，如：
- 切换分支后需要重新构建
- 怀疑有缓存导致的编译问题
- 首次构建项目

### 依赖模块
- **LibCommon.psm1** - 提供日志输出、分隔线等通用功能
- **LibConfig.psm1** - 提供INI配置文件读取功能
- **LibPaths.psm1** - 提供路径解析和项目根目录定位功能
- **LibBuild.psm1** - 提供构建目录清理功能

### 配置文件
使用 `build_develop_config.ini` 配置文件。

### 执行流程

#### Step 1: 清理构建目录
调用`Clean-BuildDir`函数删除构建目录中的所有内容，确保干净的编译环境。

#### Step 2: 调用快速构建脚本
执行 `windows_fast_develop_build.ps1`，该脚本负责：
- 运行CMake配置
- 执行实际编译

#### Step 3: 运行测试
调用 `windows_run_tests.ps1 -Config develop` 执行测试套件。

### 安全检查
- 配置文件存在性检查
- `build_dir`非空检查
- `build_dir`不能是项目根目录检查

### 退出代码
- `0` - 构建和测试都成功
- 非`0` - 构建失败 (测试失败不会导致退出，仅显示警告)

### 与快速构建的区别
| 特性 | windows_develop_build.ps1 | windows_fast_develop_build.ps1 |
|------|---------------------------|-------------------------------|
| 清理构建目录 | 是 | 否 |
| 运行测试 | 是 | 否 |
| 执行速度 | 较慢 | 较快 |
| 适用场景 | 完全重新构建 | 增量编译 |

### 注意事项
- 测试失败不会导致脚本退出，仅显示警告信息
- 如需仅构建不测试，请使用 `windows_fast_develop_build.ps1`
- 如需仅运行测试，请直接使用 `windows_run_tests.ps1`
