# Phase 0: 工程骨架搭建 - 状态文档

> **模块ID**: Phase 0
> **状态**: ✅ 已完成
> **总体进度**: 100%
> **最后更新**: 2026-03-11

---

## 一、模块概述

工程骨架模块是 CFDesktop 项目的基础设施层，负责提供完整的构建系统、开发环境配置和 CI/CD 流水线。该模块为所有后续开发工作提供稳固的工程化基础。

### 核心职责

1. **CMake 构建系统** - 跨平台编译、依赖管理、输出配置
2. **代码规范配置** - 格式化、静态分析、编码标准
3. **开发工具集成** - IDE 配置、调试支持、辅助脚本
4. **CI/CD 流水线** - 自动构建、文档部署、质量检查

---

## 二、完成状态总览

| 项目 | 完成度 | 状态 |
|------|--------|------|
| CMake 构建系统 | 90% | ✅ |
| 代码规范配置 | 80% | ✅ |
| 开发工具配置 | 70% | ✅ |
| CI/CD 流水线 | 100% | ✅ |
| 交叉编译支持 | 0% | ⚠️ 已取消 (Docker替代) |

---

## 三、已完成工作

### 3.1 CMake 构建系统 (90%)

**主配置文件**: `CMakeLists.txt`

**已完成功能**:
- CMake 3.16+ 版本要求
- C++17 标准设置
- Qt6 集成 (Core, Gui, Widgets)
- 自动化 MOC/RCC/UIC 处理
- 构建日志辅助系统
- 编译命令导出 (compile_commands.json)
- 分类输出目录配置 (bin/, lib/, examples/)
- VSCode clangd 配置自动生成
- VSCode 调试配置自动生成

**关键配置摘要**:
```cmake
cmake_minimum_required(VERSION 3.16)
project(CFDesktop VERSION 0.0.1 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 依赖
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 子模块
add_subdirectory(base)   # 基础库
add_subdirectory(ui)     # UI 框架
add_subdirectory(example) # 示例程序
add_subdirectory(test)    # 测试代码
```

**依赖文件**:
- `cmake/build_log_helper.cmake` - 构建日志辅助
- `cmake/check_toolchain.cmake` - 工具链检查
- `cmake/custom_target_helper.cmake` - 自定义目标辅助
- `cmake/OutputDirectoryConfig.cmake` - 输出目录配置
- `cmake/generate_develop_helpers.cmake` - 开发辅助生成
- `cmake/ExampleLauncher.cmake` - Windows 启动脚本生成
- `cmake/QtDeployUtils.cmake` - Qt 部署工具

### 3.2 代码规范配置 (80%)

**配置文件**: `.clang-format`

**已完成功能**:
- 基于 LLVM 风格
- 4 空格缩进
- 100 字符列宽
- 左对齐指针/引用 (int* a)
- 大括号附加风格 (Attach)
- 包含块排序
- C++17 标准

**配置摘要**:
```yaml
BasedOnStyle: LLVM
IndentWidth: 4
UseTab: Never
ColumnLimit: 100
PointerAlignment: Left
BreakBeforeBraces: Attach
Standard: c++17
SortIncludes: true
```

### 3.3 开发工具集成 (70%)

**VSCode 配置**: `.vscode/settings.json`

**已完成功能**:
- clangd 路径配置
- 编译命令目录指定
- 查询驱动器配置
- 后台索引启用
- clang-tidy 集成
- 头文件插入优化

**其他 VSCode 配置**:
- `.vscode/extensions.json` - 推荐扩展
- `.vscode/launch.json` - 调试配置 (自动生成)

### 3.4 CI/CD 流水线 (100%)

**策略**: Git Hooks 本地验证，无需远程 CI 构建流水线

#### 3.4.1 Git Pre-Push Hook

**文件**: `scripts/release/hooks/pre-push.sample`

**已完成功能**:
- 版本号检查 (阻止未更新版本的推送)
- Docker 构建验证 (本地执行)
- main 分支: X64 FastBuild + Tests
- release 分支: 根据 Major/Minor/Patch 自动检测验证级别
  - Major: X64 + ARM64 完整构建
  - Minor: X64 完整构建
  - Patch: X64 FastBuild + Tests

#### 3.4.2 Git Pre-Commit Hook

**文件**: `scripts/release/hooks/pre-commit.sample`

**已完成功能**:
- 空白字符检查 (trailing whitespace)
- C++ 代码自动格式化 (clang-format)
- 支持跨平台 (Windows PowerShell / Linux bash)

#### 3.4.3 Docker 构建系统

**文件**: `scripts/docker/Dockerfile.build`

**已完成功能**:
- 多架构支持 (amd64/arm64/armhf)
- Qt 6.8.1 通过 aqtinstall 自动安装
- 依赖自动化安装 (`scripts/dependency/install_build_dependencies.sh`)

#### 3.4.4 Docker 构建脚本

**文件**: `scripts/build_helpers/docker_start.sh`

**已完成功能**:
- 多架构构建 (--arch amd64/arm64)
- CI 验证模式 (--verify)
- 快速构建 (--fast-build)
- 项目构建 (--build-project)
- 测试运行 (--run-project-test)
- 美化日志输出

#### 3.4.5 CI 构建入口

**文件**: `scripts/build_helpers/ci_build_entry.sh`

**已完成功能**:
- 自动架构检测 (x86_64/aarch64/armv7l)
- 自动选择对应配置文件

#### 3.4.6 版本工具

**文件**: `scripts/release/hooks/version_utils.sh`

**已完成功能**:
- 版本号解析 (Major/Minor/Patch)
- 验证级别自动检测
- 本地/远程版本比较

#### 3.4.7 Hooks 安装脚本

**文件**: `scripts/release/hooks/install_hooks.sh`

**已完成功能**:
- 自动安装 pre-commit 和 pre-push hooks
- 支持交互式确认
- 备份现有 hooks

#### 3.4.8 文档部署流水线

**文件**: `.github/workflows/deploy.yml`

**已完成功能**:
- MkDocs 文档自动构建
- Doxygen API 文档生成
- doxybook2 Markdown 转换
- GitHub Pages 自动部署
- Python 3.11 环境配置

**触发条件**:
- Push 到 main 分支
- 手动触发 (workflow_dispatch)

### 3.5 目录结构

**已建立的目录结构**:
```
CFDesktop/
├── base/           # 基础库模块
│   ├── system/     # 系统检测 (CPU, 内存)
│   └── include/    # 公共头文件
├── ui/             # UI 框架
│   ├── core/       # 核心组件
│   ├── material/   # Material Design
│   └── base/       # 基础工具
├── example/        # 示例程序
├── test/           # 测试代码
├── cmake/          # CMake 模块
├── .github/        # GitHub 配置
└── .vscode/        # VSCode 配置
```

---

## 四、实施时间线

### Week 1 任务

#### Day 1-2: 目录结构创建
- [x] 创建完整目录树
  - [x] `base/` - 基础库源码 (system/, utilities/)
  - [x] `ui/` - UI 框架源码 (core/, components/, widget/)
  - [x] `example/` - 示例程序
  - [x] `test/` - 测试代码
  - [x] `cmake/` - CMake 模块
  - [x] `scripts/` - 构建和辅助脚本
- [x] 编写主 `CMakeLists.txt`
  - [x] 设置 C++17 标准
  - [x] 配置 Qt6 依赖
  - [x] 添加子目录
  - [x] 自动化 MOC/RCC/UIC
- [x] 创建各子模块的 CMakeLists.txt 框架
- [x] 配置 VSCode 开发环境
  - [x] `.vscode/settings.json` - clangd 配置
  - [x] `.vscode/extensions.json` - 推荐扩展
  - [x] `.vscode/launch.json` - 调试配置

#### Day 3-4: ~~交叉编译配置~~ (已取消，使用 Docker 多架构替代)
- ~~编写 ARMv7 工具链文件~~
- ~~编写 ARM64 工具链文件~~
- [x] Docker 多架构构建替代方案
  - [x] `scripts/docker/Dockerfile.build` - 支持 amd64/arm64/armhf

#### Day 5: 代码规范配置
- [x] 配置 `.clang-format`
  - [x] 基于 LLVM 风格
  - [x] 设置缩进为 4 空格
  - [x] 配置列宽 100
  - [x] 设置指针对齐方式
- [x] 配置 Git pre-commit hook
  - [x] 创建 `scripts/release/hooks/pre-commit.sample` 脚本
  - [x] 自动格式化 (clang-format)
  - [x] 添加 trailing whitespace 检查
  - [x] 配置可跳过选项 (git commit --no-verify)
  - [x] 安装脚本配置 (`scripts/release/hooks/install_hooks.sh`)

### Week 2 任务

#### Day 1-3: CI/CD 搭建
- [x] 创建 Docker 构建镜像
  - [x] `scripts/docker/Dockerfile.build` - 多架构支持 (amd64/arm64/armhf)
  - [x] 配置 Qt6 环境 (aqtinstall 6.8.1)
- [x] 编写 Git Hooks 工作流
  - [x] `scripts/release/hooks/pre-push.sample` - Push 前验证
  - [x] `scripts/release/hooks/pre-commit.sample` - Commit 前检查
  - [x] `scripts/release/hooks/version_utils.sh` - 版本工具
  - [x] `scripts/release/hooks/install_hooks.sh` - Hooks 安装脚本
- [x] Docker 构建脚本
  - [x] `scripts/build_helpers/docker_start.sh` - 本地 Docker 构建
  - [x] `scripts/build_helpers/ci_build_entry.sh` - CI 入口 (自动架构检测)
- [x] 配置部署流程 `.github/workflows/deploy.yml` - MkDocs 文档自动部署
- [x] 测试完整构建流程

#### Day 4-5: 开发工具完善
- [x] 创建 VSCode 工作区配置 `.vscode/settings.json`
  - [x] CMake 配置参数
  - [x] Clangd 配置
- [x] 配置推荐扩展 `.vscode/extensions.json`
  - [x] CMake Tools
  - [x] C/C++
  - [x] clang-format
- [x] 编写 Hello World 测试程序
  - [x] `example/base/system/` - 系统信息示例
  - [x] `example/ui/widget/material/` - 控件示例
- [x] 编写开发环境设置文档
  - [x] `document/development/01_prerequisites.md` - 前置要求
  - [x] `document/development/02_quick_start.md` - 快速开始
  - [x] `document/development/03_build_system.md` - 构建系统
  - [x] `document/development/04_development_tools.md` - 开发工具
  - [x] `document/development/05_docker_build.md` - Docker 构建
  - [x] `document/development/06_git_hooks.md` - Git Hooks
  - [x] `document/development/07_troubleshooting.md` - 常见问题

---

## 五、重要架构决策

### 5.1 CMakePresets.json - 已取消

**原因**:
- 不方便维护
- 配置与命令行参数重复
- 团队更习惯使用 -DCMAKE_* 参数

**替代方案**:
- 使用 build_*.config.ini 配置文件
- 通过命令行参数传递构建配置

### 5.2 src/base/sdk/shell 三层结构 - 已调整

**原计划**:
```
src/
├── base/    # 基础库
├── sdk/     # SDK 层
└── shell/   # Shell UI
```

**实际采用**:
```
base/        # 基础库
ui/          # UI 框架
example/     # 示例程序
```

**原因**: 更简洁的模块划分

### 5.3 ARM 交叉编译工具链 - 已推迟

**原因**: 优先实现桌面平台功能，嵌入式平台后续支持

---

## 六、验收标准

### CI/CD
- [x] Push 代码前自动触发本地构建验证 (pre-push hook)
- [x] Commit 前自动代码格式检查 (pre-commit hook)
- [x] 版本号检查 (阻止未更新版本的推送)
- [x] 多架构构建支持 (amd64/arm64/armhf)
- [x] 代码格式化一键执行 (pre-commit hook 自动格式化)
- [x] MkDocs 文档自动部署到 gh-pages

### 开发环境
- [x] VSCode 能够正确索引所有符号 (clangd 配置)
- [x] 代码格式化一键执行 (pre-commit hook)
- [x] 调试配置可用 (`.vscode/launch.json` 自动生成)
- [x] 新团队成员能在 30 分钟内完成环境搭建 (`document/development/`)

---

## 七、关键文件路径

### 已实现文件

```
CFDesktop/
├── CMakeLists.txt                    # 主 CMake 配置
├── .clang-format                     # 代码格式化配置
├── .vscode/
│   ├── settings.json                 # VSCode 设置
│   ├── extensions.json               # 推荐扩展
│   └── launch.json                   # 调试配置 (自动生成)
├── .github/
│   └── workflows/
│       └── deploy.yml                # 文档部署流水线
├── scripts/
│   ├── docker/
│   │   └── Dockerfile.build          # 多架构 Docker 镜像
│   ├── build_helpers/
│   │   ├── docker_start.sh           # Docker 构建脚本
│   │   ├── ci_build_entry.sh         # CI 构建入口
│   │   └── build_ci_*.ini            # 多架构配置
│   ├── release/
│   │   └── hooks/
│   │       ├── pre-commit.sample     # Pre-commit hook
│   │       ├── pre-push.sample       # Pre-push hook
│   │       ├── version_utils.sh      # 版本工具
│   │       └── install_hooks.sh      # Hooks 安装脚本
│   └── dependency/
│       └── install_build_dependencies.sh  # 依赖安装
└── cmake/
    ├── build_log_helper.cmake        # 构建日志
    ├── check_toolchain.cmake         # 工具链检查
    ├── custom_target_helper.cmake    # 自定义目标
    ├── OutputDirectoryConfig.cmake   # 输出配置
    ├── generate_develop_helpers.cmake # 开发辅助生成
    ├── ExampleLauncher.cmake         # 启动脚本生成
    └── QtDeployUtils.cmake           # Qt 部署工具
```

### 待创建文件

```
CFDesktop/
├── .clang-tidy                       # 静态分析配置 (不考虑)
├── .github/
│   └── workflows/
│       └── build.yml                 # 构建流水线 (不需要，Git Hooks 替代)
└── cmake/
    └── toolchains/
        ├── arm-linux-gnueabihf.cmake # ARMv7 工具链 (推迟)
        └── aarch64-linux-gnu.cmake   # ARM64 工具链 (推迟)
```

---

## 八、风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| Docker 多架构构建时间较长 | 开发效率 | 使用 --fast-build 缓存镜像 |
| ARM64 QEMU 仿真速度慢 | CI 耗时 | 仅在 release/Major 版本验证 |
| 工具链版本兼容性 | 编译失败 | 固定 Docker 镜像版本 |

---

## 九、相关文档

- 原始TODO: ~~已归档~~（原 `00_project_skeleton.md` 已删除）
- 设计文档: `../../design_stage/00_phase0_project_skeleton.md`
- 开发环境设置: `../../development/`

---

*文档版本: v1.0*
*生成时间: 2026-03-11*
