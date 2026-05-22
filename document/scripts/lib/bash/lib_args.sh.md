---
title: libargs.sh
description: "文档编写日期: 2026-03-20，提供命令行参数解析的辅助函数，包括："
---

# lib_args.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 加载方式
```bash
source scripts/lib/bash/lib_args.sh
```text

### 基本用法示例
```bash
while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--config)
            CONFIG_FILE="$(parse_config_file "$1" "$2")"
            shift 2
            ;;
        develop|deploy|ci)
            CONFIG="$(parse_config_mode "$1")"
            shift
            ;;
        -h|--help)
            show_detailed_usage "$(basename "$0")"
            exit 0
            ;;
        *)
            show_unknown_arg_error "$1" "$(basename "$0")"
            exit 1
            ;;
    esac
done
```bash

## Scripts详解

### 库用途
提供命令行参数解析的辅助函数，包括：
- 配置模式解析（develop/deploy/ci）
- 配置文件参数解析（-c/--config）
- 帮助信息显示
- 参数验证
- 错误信息显示

### 提供的函数

| 函数名 | 说明 |
|--------|------|
| `parse_config_mode` | 解析配置模式参数（develop/deploy/ci） |
| `is_valid_config_mode` | 检查是否为有效的配置模式 |
| `parse_config_file` | 解析 --config 或 -c 参数 |
| `show_standard_usage` | 显示标准用法帮助 |
| `show_detailed_usage` | 显示详细帮助信息 |
| `validate_arg_count` | 验证参数数量是否足够 |
| `is_help_arg` | 检查是否为帮助参数 |
| `show_unknown_arg_error` | 显示未知参数错误 |
| `show_missing_value_error` | 显示缺失参数值错误 |

### 核心函数详解

#### parse_config_mode
验证并返回配置模式。仅支持三种预定义模式。
```bash
parse_config_mode "develop"  # 输出: develop, 返回: 0
parse_config_mode "invalid"  # 无输出, 返回: 1
```text

#### is_valid_config_mode
检查给定字符串是否为有效的配置模式。
```bash
if is_valid_config_mode "$1"; then
    CONFIG="$1"
fi
```text

#### parse_config_file
解析配置文件参数，仅对 `-c` 或 `--config` 有效。
```bash
parse_config_file "-c" "config.ini"    # 输出: config.ini, 返回: 0
parse_config_file "--config" "a.conf"  # 输出: a.conf, 返回: 0
parse_config_file "--verbose" "on"     # 无输出, 返回: 1
```text

#### show_standard_usage
显示一行标准用法信息。
```bash
show_standard_usage
# 输出: Usage: script_name [develop|deploy|ci] [-c|--config <config_file>]

show_standard_usage "my_build.sh"
# 输出: Usage: my_build.sh [develop|deploy|ci] [-c|--config <config_file>]
```text

#### show_detailed_usage
显示格式化的详细帮助信息。
```bash
show_detailed_usage "build.sh" "这是一个构建工具脚本"
```text

输出示例：
```yaml
========================================
  build.sh
========================================

这是一个构建工具脚本

Arguments:
  develop    Use development build configuration
  deploy     Use deployment build configuration
  ci         Use CI build configuration

Options:
  -c, --config <file>  Use specified configuration file
  -h, --help           Show this help message

Examples:
  build.sh develop
  build.sh deploy -c custom_config.ini
```text

#### validate_arg_count
验证参数数量是否满足最小要求。
```bash
if ! validate_arg_count "$#" 2; then
    echo "错误: 参数不足"
    exit 1
fi
```text

#### is_help_arg
检查参数是否为帮助请求。
```bash
if is_help_arg "$1"; then
    show_detailed_usage "$(basename "$0")"
    exit 0
fi
```text

支持的格式：`-h`, `--help`, `help`

#### show_unknown_arg_error
显示未知参数的错误信息。
```bash
show_unknown_arg_error "--invalid" "build.sh"
# 输出到 stderr:
# ERROR: Unknown argument '--invalid'
# Run 'build.sh --help' for usage information.
```text

#### show_missing_value_error
显示参数值缺失的错误信息。
```bash
show_missing_value_error "--config"
# 输出到 stderr:
# ERROR: Missing value for argument '--config'
```text

### 依赖关系
- Bash 内置命令
- 标准 Unix 工具（basename）

### 注意事项
1. 该库专注于参数解析，不执行参数处理逻辑
2. 所有错误信息都输出到 stderr
3. 帮助信息格式统一，便于多脚本使用
4. 配置模式仅支持三种预定义值：develop、deploy、ci
