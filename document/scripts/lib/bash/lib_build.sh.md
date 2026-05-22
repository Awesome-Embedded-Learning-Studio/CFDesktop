---
title: libbuild.sh
description: "文档编写日期: 2026-03-20，注意： 此模块依赖 ，会自动加载。"
---

# lib_build.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
source scripts/lib/bash/lib_build.sh
```bash

**注意：** 此模块依赖 `lib_common.sh`，会自动加载。

## Scripts详解

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| `clean_build_dir` | 清理构建目录（带多重安全检查） |
| `ensure_build_dir` | 确保构建目录存在 |
| `run_cmake_configure` | 运行 CMake 配置 |
| `run_cmake_build` | 运行 CMake 构建 |
| `has_cmake_cache` | 检查 CMake 缓存是否存在 |
| `get_cmake_cache_var` | 获取 CMake 缓存中的变量值 |
| `get_parallel_job_count` | 获取推荐的并行作业数 |
| `build_timer_start` | 开始构建计时 |
| `build_timer_end` | 结束构建计时并显示耗时 |

---

## 函数详细说明

### clean_build_dir(build_path)

清理构建目录，包含多重安全检查以防止意外删除。

**安全检查：**
1. 路径不能为空
2. 路径不能是根目录 `/`
3. 路径不能是用户主目录 `$HOME`
4. 路径不能是项目根目录 `$PROJECT_ROOT`
5. 目录名必须匹配模式：`build`, `out`, `build_*`, `cmake-*`

**参数：**
- `$1` - 构建目录路径

**返回值：**
- `0` - 成功
- `1` - 失败

**示例：**
```bash
clean_build_dir "$BUILD_DIR"
```text

### ensure_build_dir(build_path)

确保构建目录存在，如果不存在则创建。

**参数：**
- `$1` - 构建目录路径

**示例：**
```bash
ensure_build_dir "$BUILD_DIR"
```text

### run_cmake_configure(generator, build_type, source_dir, build_dir, [extra_args])

运行 CMake 配置命令。

**参数：**
- `$1` - Generator（如 "Ninja", "Unix Makefiles"）
- `$2` - Build Type（Debug, Release, RelWithDebInfo）
- `$3` - Source 目录
- `$4` - Build 目录
- `$5+` - 额外的 CMake 参数（可选）

**返回值：**
- `0` - 成功
- 非 `0` - 失败（CMake 退出码）

**示例：**
```bash
run_cmake_configure "Ninja" "Release" "$SOURCE_DIR" "$BUILD_DIR"
run_cmake_configure "Unix Makefiles" "Debug" "$SOURCE_DIR" "$BUILD_DIR" "-DENABLE_TESTS=ON"
```text

### run_cmake_build(build_dir, [target], [jobs])

运行 CMake 构建命令。

**参数：**
- `$1` - Build 目录
- `$2` - 目标名称（可选，默认 `--all`）
- `$3` - 并行作业数（可选，默认自动检测）

**返回值：**
- `0` - 成功
- 非 `0` - 失败（CMake 退出码）

**示例：**
```bash
run_cmake_build "$BUILD_DIR"
run_cmake_build "$BUILD_DIR" "mytarget" 4
```text

### has_cmake_cache(build_dir)

检查 CMake 缓存文件是否存在。

**参数：**
- `$1` - Build 目录

**返回值：**
- `0` - CMake 缓存存在
- `1` - CMake 缓存不存在

**示例：**
```bash
if has_cmake_cache "$BUILD_DIR"; then
    log_info "CMake 已配置，跳过配置步骤"
fi
```text

### get_cmake_cache_var(build_dir, var_name)

获取 CMake 缓存中的变量值。

**参数：**
- `$1` - Build 目录
- `$2` - 变量名

**输出：**
- 变量值，如果未找到则输出空

**示例：**
```bash
generator=$(get_cmake_cache_var "$BUILD_DIR" "CMAKE_GENERATOR")
```text

### get_parallel_job_count()

获取系统推荐的并行作业数。

**返回值：**
- CPU 核心数（或 1 如果无法检测）

**示例：**
```bash
jobs=$(get_parallel_job_count)
run_cmake_build "$BUILD_DIR" "--all" "$jobs"
```text

### build_timer_start()

开始构建计时。

**示例：**
```bash
build_timer_start
```text

### build_timer_end()

结束构建计时并显示耗时。

**示例：**
```bash
build_timer_start
# ... 执行构建 ...
build_timer_end
# 输出: Build time: 2m 15s
```yaml

---

## 使用示例

```bash
#!/bin/bash
source "scripts/lib/bash/lib_build.sh"

BUILD_DIR="build"
SOURCE_DIR="."
GENERATOR="Ninja"
BUILD_TYPE="Release"

# 清理并创建构建目录
clean_build_dir "$BUILD_DIR"
ensure_build_dir "$BUILD_DIR"

# 配置 CMake
run_cmake_configure "$GENERATOR" "$BUILD_TYPE" "$SOURCE_DIR" "$BUILD_DIR"

# 构建计时开始
build_timer_start

# 执行构建
jobs=$(get_parallel_job_count)
run_cmake_build "$BUILD_DIR" "--all" "$jobs"

# 构建计时结束
build_timer_end
```text
