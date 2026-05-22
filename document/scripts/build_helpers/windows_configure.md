---
title: windowsconfigure.ps1
description: "文档编写日期: 2026-03-20，本脚本仅用于配置项目，使用CMake生成构建系统文件，不执行编"
---

# windows_configure.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\build_helpers\windows_configure.ps1 [-Config <develop|deploy>]
```bash

### 参数说明
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `-Config` | String | `develop` | 构建配置类型，可选值: `develop` 或 `deploy` |

### 使用示例
```powershell
# 使用默认开发配置
.\scripts\build_helpers\windows_configure.ps1

# 使用部署配置
.\scripts\build_helpers\windows_configure.ps1 -Config deploy
```text

## Scripts详解

### 脚本用途
本脚本**仅用于配置项目**，使用CMake生成构建系统文件，**不执行编译构建**。适用于以下场景：
- 首次设置项目构建环境
- 修改CMake配置后重新生成构建文件
- 仅需配置而不需要立即编译的情况

### 依赖模块
- **LibCommon.psm1** - 提供日志输出、分隔线等通用功能
- **LibConfig.psm1** - 提供INI配置文件读取功能
- **LibPaths.psm1** - 提供路径解析和项目根目录定位功能

### 配置文件
根据`-Config`参数选择配置文件：
- **develop模式**: `build_develop_config.ini`
- **deploy模式**: `build_deploy_config.ini`

配置文件必须存在于`scripts/build_helpers/`目录下，若不存在会提示从`.template`文件创建。

### 配置文件结构
```ini
[cmake]
generator    = 生成器名称 (如 "Ninja", "Visual Studio 17 2022")
toolchain    = 工具链文件路径
build_type   = 构建类型 (Debug/Release/RelWithDebInfo)

[paths]
source       = 源代码目录 (相对于项目根目录)
build_dir    = 构建输出目录 (相对于项目根目录)
```yaml

### 执行流程
1. 加载配置文件
2. 验证`build_type`有效性 (必须是 Debug/Release/RelWithDebInfo 之一)
3. 安全检查: 确保build_dir不为空且不是项目根目录
4. 执行CMake配置命令:
   ```bash
   cmake -G <generator> -DUSE_TOOLCHAIN=<toolchain> \
         -DCMAKE_BUILD_TYPE=<build_type> \
         -S <source_dir> -B <build_dir>
   ```

### 安全检查
脚本内置多项安全检查:
- 配置文件存在性检查
- `build_dir`非空检查
- `build_dir`不能是项目根目录 (`.`、`/`、`\`)检查
- `build_type`有效性检查

### 退出代码
- `0` - 配置成功
- 非`0` - 配置失败

### 后续操作
配置成功后，可使用以下命令进行编译:
```powershell
cmake --build <build_dir>
```yaml
或使用快速构建脚本:
```powershell
.\scripts\build_helpers\windows_fast_develop_build.ps1
```text
