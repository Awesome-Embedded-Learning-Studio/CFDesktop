---
title: windowsruntests.ps1
description: "文档编写日期: 2026-03-20，本脚本使用CTest运行项目的测试套件。它会："
---

# windows_run_tests.ps1

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```powershell
.\scripts\build_helpers\windows_run_tests.ps1 [-Config <develop|deploy>]
```bash

### 参数说明
| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `-Config` | String | `develop` | 配置类型，可选值: `develop` 或 `deploy` |

### 使用示例
```powershell
# 使用默认开发配置运行测试
.\scripts\build_helpers\windows_run_tests.ps1

# 使用部署配置运行测试
.\scripts\build_helpers\windows_run_tests.ps1 -Config deploy
```text

## Scripts详解

### 脚本用途
本脚本使用CTest运行项目的测试套件。它会：
1. 根据指定配置读取构建目录
2. 定位测试子目录
3. 执行CTest并显示测试结果

适用于：
- 验证代码正确性
- 回归测试
- CI/CD流程中的测试阶段

### 依赖模块
- **LibCommon.psm1** - 提供日志输出、分隔线等通用功能
- **LibConfig.psm1** - 提供INI配置文件读取功能
- **LibPaths.psm1** - 提供路径解析和项目根目录定位功能

### 配置文件
根据`-Config`参数选择配置文件：
- **develop模式**: `build_develop_config.ini`
- **deploy模式**: `build_deploy_config.ini`

### 执行流程

#### 1. 加载配置
从配置文件读取`build_dir`设置。

#### 2. 构建测试目录
测试目录路径为：`<项目根目录>/<build_dir>/test`

#### 3. 验证测试目录存在
如果测试目录不存在，脚本会报错并提示先运行构建脚本。

#### 4. 执行测试
```bash
ctest --test-dir <test_dir> --output-on-failure
```text

### CTest参数说明
- `--test-dir`: 指定测试目录
- `--output-on-failure`: 测试失败时显示详细输出

### 安全检查
- 配置文件存在性检查
- `build_dir`非空检查
- 测试目录存在性检查

### 退出代码
- `0` - 所有测试通过
- 非`0` - 有测试失败

### 测试失败处理
脚本会：
- 显示警告信息
- 返回失败的退出代码
- 不会中断执行流程

### 使用建议
1. **构建后立即测试**: 在构建脚本中自动调用本脚本
2. **独立运行测试**: 修改代码后可单独运行测试验证
3. **CI集成**: 在CI流程中使用 `-Config deploy` 确保发布版本质量

### 常见问题

**Q: 测试目录不存在怎么办？**
A: 请先运行相应的构建脚本，如 `windows_develop_build.ps1` 或 `windows_deploy_build.ps1`

**Q: 如何查看详细测试输出？**
A: 脚本使用 `--output-on-failure` 参数，失败的测试会自动显示详细输出

**Q: 如何只运行特定测试？**
A: 本脚本运行全部测试，如需运行特定测试，请直接使用 CTest 命令并添加筛选参数
