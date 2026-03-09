#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Config Module
# =============================================================================
#
# 提供 INI 配置文件解析功能
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib_config.sh"
#   eval "$(get_ini_config /path/to/config.ini)"
#   echo "$config_cmake_generator"
#
# =============================================================================

# =============================================================================
# INI 配置文件解析
# =============================================================================

# 读取 INI 配置文件
# 参数:
#   $1 - INI 文件路径
# 输出:
#   可通过 eval 使用的配置变量赋值语句
# 返回:
#   0 - 成功
#   1 - 失败（文件不存在）
get_ini_config() {
    local filepath="$1"
    local current_section=""
    local config=""

    if [[ ! -f "$filepath" ]]; then
        echo "ERROR: Configuration file not found: $filepath" >&2
        return 1
    fi

    while IFS= read -r line || [[ -n "$line" ]]; do
        # 去除首尾空白
        line=$(echo "$line" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

        # 跳过空行和注释
        if [[ -z "$line" ]] || [[ "$line" =~ ^[#\;] ]]; then
            continue
        fi

        # Section 头部 [section_name]
        if [[ "$line" =~ ^\[([^\]]+)\]$ ]]; then
            current_section="${BASH_REMATCH[1]}"
            continue
        fi

        # Key=Value 对
        if [[ "$line" =~ ^([^=]+)=(.*)$ ]]; then
            local key
            local value
            key=$(echo "${BASH_REMATCH[1]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            value=$(echo "${BASH_REMATCH[2]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

            # 去除值两端的引号（如果有）
            if [[ "$value" =~ ^\"(.*)\"$ ]] || [[ "$value" =~ ^\'(.*)\'$ ]]; then
                value="${BASH_REMATCH[1]}"
            fi

            if [[ -n "$current_section" ]]; then
                config+="config_${current_section}_${key}=\"${value}\""$'\n'
            fi
        fi
    done < "$filepath"

    echo "$config"
    return 0
}

# 获取特定配置值
# 参数:
#   $1 - INI 文件路径
#   $2 - Section 名称
#   $3 - Key 名称
# 输出:
#   配置值，如果未找到则输出空字符串
get_ini_value() {
    local filepath="$1"
    local section="$2"
    local key="$3"

    eval "$(get_ini_config "$filepath")"
    local var_name="config_${section}_${key}"
    echo "${!var_name}"
}

# 检查配置项是否存在
# 参数:
#   $1 - INI 文件路径
#   $2 - Section 名称
#   $3 - Key 名称
# 返回:
#   0 - 存在
#   1 - 不存在
has_ini_value() {
    local filepath="$1"
    local section="$2"
    local key="$3"

    local value
    value=$(get_ini_value "$filepath" "$section" "$key")
    [[ -n "$value" ]]
}

# 获取默认配置文件路径
# 参数:
#   $1 - 配置模式 (develop, deploy, ci)
# 输出:
#   配置文件的绝对路径
get_default_config_file() {
    local mode="${1:-develop}"
    local config_dir="${SCRIPT_DIR:-$(dirname "${BASH_SOURCE[1]}")}"

    case "$mode" in
        deploy)
            echo "$config_dir/build_deploy_config.ini"
            ;;
        ci)
            echo "$config_dir/build_ci_config.ini"
            ;;
        develop|*)
            echo "$config_dir/build_develop_config.ini"
            ;;
    esac
}
