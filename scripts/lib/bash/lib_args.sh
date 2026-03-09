#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Arguments Module
# =============================================================================
#
# 提供参数解析辅助函数
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib_args.sh"
#   while [[ $# -gt 0 ]]; do
#       case "$1" in
#           -c|--config)
#               CONFIG_FILE="$(parse_config_file "$1" "$2")"
#               shift 2
#               ;;
#           develop|deploy|ci)
#               CONFIG="$(parse_config_mode "$1")"
#               shift
#               ;;
#       esac
#   done
#
# =============================================================================

# =============================================================================
# 配置模式解析
# =============================================================================

# 解析配置模式参数
# 参数:
#   $1 - 模式字符串
# 输出:
#   模式字符串（如果有效）
# 返回:
#   0 - 有效模式
#   1 - 无效模式
parse_config_mode() {
    case "$1" in
        develop|deploy|ci)
            echo "$1"
            return 0
            ;;
        *)
            return 1
            ;;
    esac
}

# 检查是否为有效的配置模式
is_valid_config_mode() {
    parse_config_mode "$1" >/dev/null 2>&1
}

# =============================================================================
# 参数值解析
# =============================================================================

# 解析 --config 或 -c 参数
# 参数:
#   $1 - 参数名（如 -c 或 --config）
#   $2 - 参数值
# 输出:
#   配置文件路径（如果是有效的 config 参数）
# 返回:
#   0 - 是有效的 config 参数
#   1 - 不是 config 参数
parse_config_file() {
    local key="$1"
    local value="$2"

    if [[ "$key" == "-c" ]] || [[ "$key" == "--config" ]]; then
        echo "$value"
        return 0
    fi
    return 1
}

# =============================================================================
# 帮助信息
# =============================================================================

# 显示标准用法帮助
# 参数:
#   $1 - 脚本名称（可选）
show_standard_usage() {
    local script_name="${1:-$(basename "${BASH_SOURCE[1]}")}"
    echo "Usage: $script_name [develop|deploy|ci] [-c|--config <config_file>]"
}

# 显示详细帮助信息
# 参数:
#   $1 - 脚本名称
#   $2 - 额外说明（可选）
show_detailed_usage() {
    local script_name="$1"
    local extra_description="${2:-}"

    echo ""
    echo "========================================"
    echo "  $script_name"
    echo "========================================"
    echo ""
    if [[ -n "$extra_description" ]]; then
        echo "$extra_description"
        echo ""
    fi
    echo "Arguments:"
    echo "  develop    Use development build configuration"
    echo "  deploy     Use deployment build configuration"
    echo "  ci         Use CI build configuration"
    echo ""
    echo "Options:"
    echo "  -c, --config <file>  Use specified configuration file"
    echo "  -h, --help           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $script_name develop"
    echo "  $script_name deploy -c custom_config.ini"
    echo ""
}

# =============================================================================
# 参数验证
# =============================================================================

# 验证参数数量
# 参数:
#   $1 - 实际参数数量
#   $2 - 期望的最小参数数量
# 返回:
#   0 - 参数数量足够
#   1 - 参数数量不足
validate_arg_count() {
    local actual="$1"
    local expected="$2"
    [[ "$actual" -ge "$expected" ]]
}

# 检查是否需要显示帮助
# 参数:
#   $1 - 参数值
# 返回:
#   0 - 是帮助参数
#   1 - 不是帮助参数
is_help_arg() {
    case "$1" in
        -h|--help|help)
            return 0
            ;;
        *)
            return 1
            ;;
    esac
}

# =============================================================================
# 错误处理
# =============================================================================

# 显示未知参数错误
# 参数:
#   $1 - 未知参数
#   $2 - 脚本名称（可选）
show_unknown_arg_error() {
    local arg="$1"
    local script_name="${2:-$(basename "${BASH_SOURCE[1]}")}"

    echo "ERROR: Unknown argument '$arg'" >&2
    echo "Run '$script_name --help' for usage information." >&2
}

# 显示缺失参数值错误
# 参数:
#   $1 - 参数名
show_missing_value_error() {
    local arg="$1"
    echo "ERROR: Missing value for argument '$arg'" >&2
}
