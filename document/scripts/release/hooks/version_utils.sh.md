---
title: versionutils.sh
description: "文档编写日期: 2026-03-20，为Git钩子提供版本号解析、验证级别检测和Git版本信息获取功"
---

# version_utils.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
source scripts/release/hooks/version_utils.sh
```cpp

## Scripts详解

### 脚本用途
为Git钩子提供版本号解析、验证级别检测和Git版本信息获取功能。

### 提供的函数

#### 版本解析
| 函数 | 说明 | 示例 |
|------|------|------|
| get_major_version() | 获取主版本号 | `1.2.3` -> `1` |
| get_minor_version() | 获取次版本号 | `1.2.3` -> `2` |
| get_patch_version() | 获取补丁版本号 | `1.2.3` -> `3` |

#### 验证级别
| 函数 | 说明 |
|------|------|
| determine_verify_level() | 根据本地和远程版本号确定验证级别 |
| get_verify_level_description() | 获取验证级别的描述信息 |

#### Git集成
| 函数 | 说明 |
|------|------|
| get_local_version() | 获取当前分支最近的版本标签 |
| get_remote_version() | 获取远程main分支最近的版本标签 |
| get_cmake_version() | 从CMakeLists.txt获取版本号 |
| get_remote_cmake_version() | 从远程分支的CMakeLists.txt获取版本号 |

#### 调试辅助
| 函数 | 说明 |
|------|------|
| print_version_info() | 打印版本信息（用于调试） |

### 验证级别规则

`determine_verify_level()` 函数比较本地版本和远程版本，返回以下级别：

| 级别 | 条件 | 说明 |
|------|------|------|
| **major** | 主版本号不同 | X64 + ARM64 完整构建 + 测试 |
| **minor** | 主版本相同，次版本不同 | X64 完整构建 + 测试 |
| **patch** | 主、次版本相同 | X64 快速构建 + 测试 |

### 函数参数说明

#### determine_verify_level
```bash
determine_verify_level <local_version> <remote_version>
```text
- **参数1**: 本地版本号（如 `1.2.3`）
- **参数2**: 远程版本号（如 `1.1.0`）
- **返回**: `major`, `minor`, 或 `patch`
- **默认值**: 空版本号会被处理为 `0.0.0`

#### get_cmake_version
```bash
get_cmake_version <project_root>
```text
- **参数1**: 项目根目录路径
- **返回**: CMakeLists.txt中的版本号（提取 `VERSION x.y.z` 格式）
- **失败**: 返回空字符串

#### get_remote_cmake_version
```bash
get_remote_cmake_version [remote_branch]
```text
- **参数1**: 远程分支名（默认: `origin/main`）
- **返回**: 远程CMakeLists.txt中的版本号
- **失败**: 返回空字符串

### Git版本获取说明

- **get_local_version()**: 使用 `git describe --tags --abbrev=0`
- **get_remote_version()**: 先fetch远程信息，然后获取远程main分支的标签

### 使用示例
```bash
# 加载工具库
source scripts/release/hooks/version_utils.sh

# 解析版本
VERSION="1.2.3"
echo $(get_major_version "$VERSION")  # 输出: 1
echo $(get_minor_version "$VERSION")  # 输出: 2
echo $(get_patch_version "$VERSION")  # 输出: 3

# 确定验证级别
LOCAL="1.2.3"
REMOTE="1.1.0"
LEVEL=$(determine_verify_level "$LOCAL" "$REMOTE")
echo "$LEVEL"  # 输出: minor

# 获取描述
echo "$(get_verify_level_description "$LEVEL")"
# 输出: Minor 版本变更: X64 完整构建 + 测试

# 获取CMake版本
CMAKE_VER=$(get_cmake_version "/path/to/project")
echo "$CMAKE_VER"  # 输出: x.y.z
```text

### 相关文件
- `/home/charliechen/CFDesktop/scripts/release/hooks/version_utils.sh`
- `/home/charliechen/CFDesktop/CMakeLists.txt`
