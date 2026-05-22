---
title: windowsdeploybuild.ps1
description: "文档编写日期: 2026-03-20，本脚本执行完整的部署构建流程，包含以下步骤："
---

# windows_deploy_build.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\build_helpers\windows_deploy_build.ps1
```text

### 使用示例
```powershell
# 执行完整的部署构建 (清理 + 配置 + 编译 + 测试)
.\scripts\build_helpers\windows_deploy_build.ps1
```bash

## Scripts详解

### 脚本用途
本脚本执行**完整的部署构建流程**，包含以下步骤：
1. 清理构建目录
2. 调用快速部署构建脚本进行配置和编译
3. 自动运行测试

适用于准备发布版本的场景，如：
- 生成发布版本
- 进行完整的集成测试
- 确保干净的构建环境

### 依赖模块
- **LibCommon.psm1** - 提供日志输出、分隔线等通用功能
- **LibConfig.psm1** - 提供INI配置文件读取功能
- **LibPaths.psm1** - 提供路径解析和项目根目录定位功能
- **LibBuild.psm1** - 提供构建目录清理功能

### 配置文件
使用 `build_deploy_config.ini` 配置文件。

### 执行流程

#### Step 1: 清理构建目录
调用`Clean-BuildDir`函数删除构建目录中的所有内容，确保干净的编译环境。

#### Step 2: 调用快速部署构建脚本
执行 `windows_fast_deploy_build.ps1`，该脚本负责：
- 运行CMake配置 (使用deploy配置)
- 执行实际编译

#### Step 3: 运行测试
调用 `windows_run_tests.ps1 -Config deploy` 执行测试套件。

### 安全检查
- 配置文件存在性检查
- `build_dir`非空检查
- `build_dir`不能是项目根目录检查

### 退出代码
- `0` - 构建成功 (测试失败不会导致退出)
- 非`0` - 构建失败

### 与开发构建的区别
| 特性 | windows_develop_build.ps1 | windows_deploy_build.ps1 |
|------|---------------------------|--------------------------|
| 配置文件 | build_develop_config.ini | build_deploy_config.ini |
| 构建类型 | 通常为 Debug | 通常为 Release |
| 适用场景 | 日常开发 | 发布部署 |

### 与快速部署构建的区别
| 特性 | windows_deploy_build.ps1 | windows_fast_deploy_build.ps1 |
|------|---------------------------|-------------------------------|
| 清理构建目录 | 是 | 否 |
| 运行测试 | 是 | 否 |
| 执行速度 | 较慢 | 较快 |
| 适用场景 | 完全重新构建 | 增量编译 |

### 注意事项
- 测试失败不会导致脚本退出，仅显示警告信息
- 如需仅构建不测试，请使用 `windows_fast_deploy_build.ps1`
- 部署构建通常使用Release配置，优化性能和减小体积
