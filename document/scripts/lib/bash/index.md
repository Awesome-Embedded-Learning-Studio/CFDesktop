---
title: Bash 脚本库
description: 本目录包含 Bash 共享工具函数，为 Linux 平台的构建、测试与发布脚本提供通用功能支持，包括彩色日志输出、路径工具、环境变量检测与 CMake 封装调用等。
---

# Bash 脚本库

本目录包含 Bash 共享工具函数，为 Linux 平台的构建、测试与发布脚本提供通用功能支持，包括彩色日志输出、路径工具、环境变量检测与 CMake 封装调用等。

## 库文件列表

| 文件 | 说明 |
|------|------|
| `lib_common.sh` | 日志输出、颜色定义、通用工具函数 |
| `lib_config.sh` | INI 配置文件解析 |
| `lib_build.sh` | 构建相关工具函数 |

## 使用方式

所有库文件可以独立 source 使用，或被其他脚本引用。

### 基本加载方式

```bash
#!/bin/bash

# 获取库目录
SCRIPT_LIB="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 加载需要的模块
source "$SCRIPT_LIB/lib_common.sh"
source "$SCRIPT_LIB/lib_config.sh"
source "$SCRIPT_LIB/lib_build.sh"
```

## 模块详细文档

- [lib_common.sh](lib_common.sh.md) - 日志、颜色、通用工具
- [lib_build.sh](lib_build.sh.md) - 构建相关函数
- [lib_config.sh](lib_config.sh.md) - 配置文件处理

## 快速参考

### 日志输出 (lib_common.sh)

```bash
log_info "信息消息"
log_success "成功消息"
log_warn "警告消息"
log_error "错误消息"
log_separator
log_progress 5 10 "处理中"
```

### 配置解析 (lib_config.sh)

```bash
eval "$(get_ini_config config.ini)"
echo "$config_cmake_generator"

value=$(get_ini_value config.ini "cmake" "generator")
has_ini_value config.ini "cmake" "generator" && echo "存在"
```

### 构建操作 (lib_build.sh)

```bash
clean_build_dir "$BUILD_DIR"
run_cmake_configure "Ninja" "Release" "$SOURCE_DIR" "$BUILD_DIR"
run_cmake_build "$BUILD_DIR" "--all" $(get_parallel_job_count)
```
