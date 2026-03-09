#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Paths Module
# =============================================================================
#
# 提供路径解析功能
#
# 用法:
#   SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
#   source "$SCRIPT_DIR/../lib/bash/lib_paths.sh"
#   # 之后可使用 $PROJECT_ROOT 等环境变量或调用 get_xxx() 函数
#
# =============================================================================

# =============================================================================
# 内部函数（用于初始化环境变量）
# =============================================================================

# 获取项目根目录（内部函数）
# 假设脚本在 scripts/ 的子目录中
# 返回: 项目根目录的绝对路径
_get_project_root() {
    # Use the already-set SCRIPT_DIR and go up two levels
    # scripts/build_helpers -> scripts -> project_root
    cd "$SCRIPT_DIR/../.." && pwd
}

# 获取 scripts 目录（内部函数）
# 返回: scripts 目录的绝对路径
_get_scripts_dir() {
    local project_root
    project_root="$(_get_project_root)"
    echo "$project_root/scripts"
}

# 获取库目录（内部函数）
# 返回: lib 目录的绝对路径
_get_lib_dir() {
    local scripts_dir
    scripts_dir="$(_get_scripts_dir)"
    echo "$scripts_dir/lib"
}

# =============================================================================
# 公共 API 函数
# =============================================================================

# 获取脚本所在目录
# 返回: 当前脚本所在目录的绝对路径
get_script_dir() {
    echo "$SCRIPT_DIR"
}

# 获取项目根目录
# 返回: 项目根目录的绝对路径
get_project_root() {
    echo "$PROJECT_ROOT"
}

# 获取 scripts 目录
# 返回: scripts 目录的绝对路径
get_scripts_dir() {
    echo "$SCRIPTS_DIR"
}

# 获取库目录
# 返回: lib 目录的绝对路径
get_lib_dir() {
    echo "$LIB_DIR"
}

# =============================================================================
# 工具函数
# =============================================================================

# 检查路径是否存在
# 参数:
#   $1 - 要检查的路径
# 返回:
#   0 - 存在
#   1 - 不存在
path_exists() {
    [[ -e "$1" ]]
}

# 创建目录（如果不存在）
# 参数:
#   $1 - 要创建的目录路径
ensure_dir() {
    local dir="$1"
    if [[ ! -d "$dir" ]]; then
        mkdir -p "$dir"
    fi
}

# =============================================================================
# 环境变量设置（供 source 时使用）
# =============================================================================

# SCRIPT_DIR should be set by the calling script before sourcing this file
# If not set, try to determine it from BASH_SOURCE
if [[ -z "$SCRIPT_DIR" ]]; then
    # Find the first non-lib_paths.sh file in BASH_SOURCE
    for ((i=0; i<${#BASH_SOURCE[@]}; i++)); do
        _src="${BASH_SOURCE[$i]}"
        if [[ "$_src" != *"lib_paths.sh" ]]; then
            SCRIPT_DIR="$(cd "$(dirname "$_src")" && pwd)"
            break
        fi
    done
    unset _src
fi

# 设置脚本目录环境变量
export SCRIPT_DIR="${SCRIPT_DIR:-$(pwd)}"

# 设置项目根目录环境变量
export PROJECT_ROOT="${PROJECT_ROOT:-$(_get_project_root)}"

# 设置 scripts 目录环境变量
export SCRIPTS_DIR="${SCRIPTS_DIR:-$(_get_scripts_dir)}"

# 设置库目录环境变量
export LIB_DIR="${LIB_DIR:-$(_get_lib_dir)}"
