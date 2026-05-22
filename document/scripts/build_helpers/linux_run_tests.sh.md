---
title: linuxruntests.sh
description: "文档编写日期: 2026-03-20，是测试运行脚本，使用CTest执行CMake测试套件。"
---

# linux_run_tests.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/build_helpers/linux_run_tests.sh [develop|deploy|ci] [-c|--config <config_file>]
```bash

### 参数说明

| 参数 | 说明 |
|------|------|
| `develop` | 使用开发配置（默认） |
| `deploy` | 使用部署配置 |
| `ci` | 使用CI配置 |
| `-c, --config <file>` | 使用自定义配置文件 |
| `-h, --help` | 显示帮助信息 |

## Scripts详解

`linux_run_tests.sh` 是测试运行脚本，使用CTest执行CMake测试套件。

### 功能特点

1. **CTest集成** - 使用CMake/CTest标准测试框架
2. **失败详情** - 使用 `--output-on-failure` 显示测试失败详情
3. **多配置支持** - 支持开发、部署、CI三种配置模式
4. **自动路径解析** - 从配置文件读取测试目录

### 执行流程

1. **解析参数** - 确定配置模式和配置文件
2. **加载配置** - 从INI配置文件读取构建目录
3. **验证构建** - 检查测试目录是否存在
4. **运行测试** - 执行CTest测试套件
5. **报告结果** - 显示测试通过或失败信息

### 测试目录

脚本会在配置的构建目录下的 `test` 子目录中查找测试：

```text
<build_dir>/test/
```text

例如，如果 `build_dir = build_develop`，则测试目录为 `build_develop/test/`。

### 使用示例

```bash
# 使用默认开发配置运行测试
./scripts/build_helpers/linux_run_tests.sh develop

# 使用部署配置运行测试
./scripts/build_helpers/linux_run_tests.sh deploy

# 使用CI配置运行测试
./scripts/build_helpers/linux_run_tests.sh ci

# 使用自定义配置文件
./scripts/build_helpers/linux_run_tests.sh develop -c my_config.ini
```text

### 输出示例

#### 测试成功

```text
========================================
Running Tests (Config: develop)
========================================
Project root: /home/charliechen/CFDesktop
Loading configuration from: build_develop_config.ini
Configuration loaded successfully!
Test directory: /home/charliechen/CFDesktop/build_develop/test
Command: ctest --test-dir build_develop/test --output-on-failure
========================================
Running tests...
========================================
Test project /home/charliechen/CFDesktop/build_develop/test
    Start 1: test_base
1/3 Test #1: test_base ....................   Passed    0.05 sec
    Start 2: test_ui
2/3 Test #2: test_ui ......................   Passed    0.12 sec
    Start 3: test_desktop
3/3 Test #3: test_desktop ..................   Passed    0.08 sec

100% tests passed, 0 tests failed out of 3
Total Test time (real) =   0.25 sec
========================================
All tests passed successfully!
========================================
```text

#### 测试失败

```text
========================================
Running Tests (Config: develop)
========================================
...
1/3 Test #1: test_base ....................   Passed    0.05 sec
2/3 Test #2: test_ui ......................***Failed    0.12 sec
...
========================================
Some tests failed with exit code: 8
========================================
```bash

### 错误处理

| 错误 | 原因 | 解决方案 |
|------|------|----------|
| 配置文件不存在 | 配置文件未创建 | 从模板复制配置文件 |
| build_dir为空 | 配置文件中未设置 | 在配置文件中设置 build_dir |
| 测试目录不存在 | 项目未构建 | 先运行构建脚本 |
| 测试失败 | 测试用例失败 | 查看测试输出修复问题 |

### CTest参数

脚本使用以下CTest参数：

| 参数 | 说明 |
|------|------|
| `--test-dir <dir>` | 指定测试目录 |
| `--output-on-failure` | 测试失败时显示详细输出 |

### 使用场景

1. **构建后验证** - 确保构建后的代码功能正常
2. **回归测试** - 代码修改后验证现有功能
3. **CI/CD** - 自动化测试流水线
4. **问题诊断** - 使用 `--output-on-failure` 诊断失败原因

### 依赖脚本

该脚本依赖以下脚本和库：

1. `lib_common.sh` - 通用日志函数
2. `lib_config.sh` - 配置文件处理
3. `lib_paths.sh` - 路径解析

### 注意事项

1. 测试目录必须存在（需要先构建项目）
2. 测试失败会返回非零退出码
3. `--output-on-failure` 确保能看到失败的详细信息
4. 测试目录固定为 `<build_dir>/test/`

### 集成示例

在CI环境中使用：

```bash
# 完整CI流程
./scripts/build_helpers/linux_develop_build.sh ci

# 或仅运行测试（假设已构建）
./scripts/build_helpers/linux_run_tests.sh ci
```text
