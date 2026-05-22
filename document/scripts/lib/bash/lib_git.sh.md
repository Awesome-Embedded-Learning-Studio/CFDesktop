---
title: libgit.sh
description: "文档编写日期: 2026-03-20，提供 Git 相关的辅助函数，主要包括："
---

# lib_git.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
source scripts/lib/bash/lib_git.sh
```bash

## Scripts详解

### 库用途
提供 Git 相关的辅助函数，主要包括：
- 版本号解析（主版本、次版本、补丁版本）
- 版本号比较
- Git 标签获取（本地和远程）
- CMake 版本号提取
- 验证级别检测
- Git 状态检查

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| `get_major_version` | 解析版本号，返回主版本号（X.y.z 中的 X） |
| `get_minor_version` | 解析版本号，返回次版本号（x.Y.z 中的 Y） |
| `get_patch_version` | 解析版本号，返回补丁版本号（x.y.Z 中的 Z） |
| `compare_versions` | 比较两个版本号大小（返回 0=相等, 1=v1>v2, 2=v1<v2） |
| `get_local_version` | 获取当前分支的最近版本标签 |
| `get_remote_version` | 获取远程 main 分支的最近版本标签 |
| `get_all_version_tags` | 获取所有版本标签列表（按版本排序） |
| `get_latest_version_tag` | 获取最新的版本标签 |
| `get_cmake_version` | 从 CMakeLists.txt 中提取项目版本号 |
| `determine_verify_level` | 根据本地和远程版本差异确定验证级别 |
| `get_verify_level_description` | 获取验证级别的描述信息 |
| `has_uncommitted_changes` | 检查是否有未提交的更改 |
| `get_current_branch` | 获取当前 Git 分支名称 |
| `is_on_main_branch` | 检查是否在 main 分支上 |
| `print_version_info` | 打印版本信息（用于调试） |

### 核心函数详解

#### compare_versions
比较两个语义化版本号的大小。
```bash
compare_versions "1.2.3" "1.2.4"
echo $?  # 输出: 2 (第一个版本小于第二个版本)

compare_versions "2.0.0" "1.9.9"
echo $?  # 输出: 1 (第一个版本大于第二个版本)

compare_versions "1.0.0" "1.0.0"
echo $?  # 输出: 0 (版本相等)
```text

#### determine_verify_level
根据本地版本和远程版本的差异，确定需要进行的验证级别。
```bash
determine_verify_level "1.2.3" "1.3.0"  # 输出: minor
determine_verify_level "1.2.3" "2.0.0"  # 输出: major
determine_verify_level "1.2.3" "1.2.4"  # 输出: patch
```text

验证级别含义：
- **major**: X64 + ARM64 完整构建 + 测试
- **minor**: X64 完整构建 + 测试
- **patch**: X64 快速构建 + 测试

#### get_cmake_version
从项目根目录的 CMakeLists.txt 中提取版本号。
```bash
get_cmake_version "/path/to/project"  # 输出: 1.2.3
```text

### 依赖关系
- Git 命令行工具（git）
- 标准 Unix 工具（cut, grep, head）

### 注意事项
1. 所有 Git 相关函数都需要在 Git 仓库中执行
2. `get_remote_version` 会自动执行 `git fetch` 获取最新远程信息
3. 版本号比较遵循语义化版本规范（Semantic Versioning）
4. 当没有找到标签时，版本相关函数会返回 "0.0.0"
