---
title: libpaths.sh
description: "文档编写日期: 2026-03-20，加载后可使用以下环境变量或调用对应的 getter 函数："
---

# lib_paths.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
# 推荐方式：在脚本中先设置 SCRIPT_DIR，然后加载
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../lib/bash/lib_paths.sh"
```bash

加载后可使用以下环境变量或调用对应的 getter 函数：
- `$PROJECT_ROOT` 或 `get_project_root()` - 项目根目录
- `$SCRIPTS_DIR` 或 `get_scripts_dir()` - scripts 目录
- `$LIB_DIR` 或 `get_lib_dir()` - lib 目录
- `$SCRIPT_DIR` 或 `get_script_dir()` - 当前脚本目录

## Scripts详解

### 库用途
提供项目路径解析和管理功能，自动设置项目相关的环境变量，简化脚本中对项目各目录路径的引用。

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| `get_script_dir` | 获取当前脚本所在目录的绝对路径 |
| `get_project_root` | 获取项目根目录的绝对路径 |
| `get_scripts_dir` | 获取 scripts 目录的绝对路径 |
| `get_lib_dir` | 获取 lib 目录的绝对路径 |
| `path_exists` | 检查路径是否存在 |
| `ensure_dir` | 创建目录（如果不存在） |

### 环境变量

加载该库后会自动设置以下环境变量：

| 环境变量 | 说明 |
|----------|------|
| `SCRIPT_DIR` | 当前脚本所在目录（绝对路径） |
| `PROJECT_ROOT` | 项目根目录（scripts 目录的上级目录） |
| `SCRIPTS_DIR` | scripts 目录路径 |
| `LIB_DIR` | scripts/lib 目录路径 |

### 目录结构假设

```text
PROJECT_ROOT/
  ├── scripts/
  │   ├── lib/
  │   │   └── bash/
  │   │       └── lib_paths.sh
  │   └── build_helpers/
  │       └── your_script.sh
  └── ...
```text

### 核心函数详解

#### path_exists
检查给定的路径是否存在（文件或目录）。
```bash
if path_exists "/some/path"; then
    echo "路径存在"
fi
```text

#### ensure_dir
确保目录存在，如果不存在则创建（包括父目录）。
```bash
ensure_dir "$PROJECT_ROOT/build/output"
```text

### 依赖关系
- Bash 内置命令
- 标准 Unix 工具（pwd, mkdir）

### 注意事项
1. 该库假设脚本位于 `scripts/` 目录的子目录中
2. 如果调用脚本未设置 `SCRIPT_DIR`，库会尝试从 `BASH_SOURCE` 自动推断
3. 所有路径都是绝对路径，便于在任何位置调用脚本
4. 环境变量会被导出（export），子进程也可以使用
