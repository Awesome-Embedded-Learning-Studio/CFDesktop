#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Common Module
# =============================================================================
#
# 提供统一的日志输出和颜色定义
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib_common.sh"
#   log_info "This is an info message"
#   log_error "This is an error message"
#
# =============================================================================

# =============================================================================
# 颜色定义
# =============================================================================

readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[0;33m'
readonly CYAN='\033[0;36m'
readonly GRAY='\033[0;90m'
readonly NC='\033[0m' # No Color

# =============================================================================
# 日志函数
# =============================================================================

# 统一日志函数
# 参数:
#   $1 - 消息内容
#   $2 - 日志级别 (INFO, SUCCESS, WARNING, ERROR)，默认为 INFO
log() {
    local message="$1"
    local level="${2:-INFO}"
    local timestamp
    timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    local color=""

    case "$level" in
        INFO)
            color="\033[0;36m"
            ;;
        SUCCESS)
            color="\033[0;32m"
            ;;
        WARNING)
            color="\033[0;33m"
            ;;
        ERROR)
            color="\033[0;31m"
            ;;
        *)
            color="\033[0m"
            ;;
    esac

    echo -e "${color}[$timestamp] [$level] $message\033[0m"
}

# 便捷日志函数
log_info()    { log "$1" "INFO"; }
log_success() { log "$1" "SUCCESS"; }
log_warn()    { log "$1" "WARNING"; }
log_error()   { log "$1" "ERROR"; }
log_cyan()    { printf "${CYAN}%s${NC}\n" "$*"; }

# =============================================================================
# 分隔线函数
# =============================================================================

# 打印分隔线
log_separator() {
    local char="${1:-=}"
    local width="${2:-40}"
    local line=""
    for ((i=0; i<width; i++)); do
        line+="$char"
    done
    log_info "$line"
}

# =============================================================================
# 调试函数
# =============================================================================

# 调试输出（仅在 DEBUG 模式下显示）
log_debug() {
    if [[ "${DEBUG:-false}" == "true" ]]; then
        log "$1" "DEBUG"
    fi
}

# =============================================================================
# 进度显示
# =============================================================================

# 显示进度信息
log_progress() {
    local current="$1"
    local total="$2"
    local message="${3:-Processing}"
    local percent=$((current * 100 / total))
    log_info "[$current/$total] ($percent%) $message"
}
