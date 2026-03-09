#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Git Module
# =============================================================================
#
# 提供 Git 相关的辅助函数，包括版本号解析和验证
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib_git.sh"
#   local_version=$(get_local_version)
#   verify_level=$(determine_verify_level "$local_version" "$remote_version")
#
# =============================================================================

# =============================================================================
# 版本号解析函数
# =============================================================================

# 解析版本号，返回主版本号
# 参数:
#   $1 - 版本字符串（如 "1.2.3"）
# 输出:
#   主版本号（如 "1"）
get_major_version() {
    local version="$1"
    echo "$version" | cut -d. -f1
}

# 解析版本号，返回次版本号
# 参数:
#   $1 - 版本字符串（如 "1.2.3"）
# 输出:
#   次版本号（如 "2"）
get_minor_version() {
    local version="$1"
    echo "$version" | cut -d. -f2 -s
}

# 解析版本号，返回补丁版本号
# 参数:
#   $1 - 版本字符串（如 "1.2.3"）
# 输出:
#   补丁版本号（如 "3"），如果不存在则返回空
get_patch_version() {
    local version="$1"
    echo "$version" | cut -d. -f3 -s
}

# 比较两个版本号
# 参数:
#   $1 - 版本号 1
#   $2 - 版本号 2
# 返回:
#   0 - 版本号相等
#   1 - 版本号 1 大于版本号 2
#   2 - 版本号 1 小于版本号 2
compare_versions() {
    local v1="$1"
    local v2="$2"

    if [[ "$v1" == "$v2" ]]; then
        return 0
    fi

    local IFS=.
    local i ver1=($v1) ver2=($v2)

    # 填充空位
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++)); do
        ver1[i]=0
    done
    for ((i=${#ver2[@]}; i<${#ver1[@]}; i++)); do
        ver2[i]=0
    done

    for ((i=0; i<${#ver1[@]}; i++)); do
        if [[ -z ${ver1[i]} ]]; then ver1[i]=0; fi
        if [[ -z ${ver2[i]} ]]; then ver2[i]=0; fi
        if ((10#${ver1[i]} > 10#${ver2[i]})); then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]})); then
            return 2
        fi
    done

    return 0
}

# =============================================================================
# Git 版本标签获取
# =============================================================================

# 获取当前分支的版本标签
# 返回:
#   最近的版本标签，如果没有则返回 "0.0.0"
get_local_version() {
    git describe --tags --abbrev=0 2>/dev/null || echo "0.0.0"
}

# 获取远程 main 分支的版本标签
# 返回:
#   远程 main 分支最近的版本标签，如果没有则返回 "0.0.0"
get_remote_version() {
    # 确保有最新的远程信息
    git fetch origin main:refs/remotes/origin/main --quiet 2>/dev/null || true
    git describe --tags --abbrev=0 origin/main 2>/dev/null || echo "0.0.0"
}

# 获取所有的版本标签列表
# 输出:
#   按版本排序的标签列表（每行一个）
get_all_version_tags() {
    git tag -l --sort=-version:refname 2>/dev/null || echo ""
}

# 获取最新的版本标签
# 输出:
#   最新的版本标签
get_latest_version_tag() {
    get_all_version_tags | head -1
}

# =============================================================================
# CMake 版本号获取
# =============================================================================

# 获取 CMakeLists.txt 中定义的版本号
# 参数:
#   $1 - 项目根目录路径
# 输出:
#   CMakeLists.txt 中的版本号，如果未找到则返回空字符串
get_cmake_version() {
    local project_root="$1"
    local cmake_file="$project_root/CMakeLists.txt"

    if [[ ! -f "$cmake_file" ]]; then
        echo ""
        return
    fi

    # 提取 VERSION x.y.z 格式的版本号
    grep -oP 'VERSION\s+\K[\d.]+' "$cmake_file" 2>/dev/null || echo ""
}

# =============================================================================
# 验证级别检测
# =============================================================================

# 根据本地版本和远程版本确定验证级别
# 参数:
#   $1 - 本地版本号
#   $2 - 远程版本号
# 输出:
#   major, minor, 或 patch
determine_verify_level() {
    local local_version="$1"
    local remote_version="$2"

    # 处理空版本号情况
    if [[ -z "$local_version" ]]; then
        local_version="0.0.0"
    fi
    if [[ -z "$remote_version" ]]; then
        remote_version="0.0.0"
    fi

    # 提取版本号部分
    local local_major
    local local_minor
    local_major=$(get_major_version "$local_version")
    local_minor=$(get_minor_version "$local_version")

    local remote_major
    local remote_minor
    remote_major=$(get_major_version "$remote_version")
    remote_minor=$(get_minor_version "$remote_version")

    # 比较版本号
    # Major 版本变化
    if [[ "$local_major" != "$remote_major" ]]; then
        echo "major"
    # Minor 版本变化
    elif [[ "$local_minor" != "$remote_minor" ]]; then
        echo "minor"
    # Patch 版本变化或无变化
    else
        echo "patch"
    fi
}

# 获取验证级别的描述信息
# 参数:
#   $1 - 验证级别 (major, minor, patch)
# 输出:
#   验证级别的描述文本
get_verify_level_description() {
    local level="$1"

    case "$level" in
        major)
            echo "Major 版本变更: X64 + ARM64 完整构建 + 测试"
            ;;
        minor)
            echo "Minor 版本变更: X64 完整构建 + 测试"
            ;;
        patch)
            echo "Patch 版本变更: X64 快速构建 + 测试"
            ;;
        *)
            echo "未知验证级别: $level"
            ;;
    esac
}

# =============================================================================
# Git 状态检查
# =============================================================================

# 检查是否有未提交的更改
# 返回:
#   0 - 有未提交的更改
#   1 - 没有未提交的更改
has_uncommitted_changes() {
    ! git diff-index --quiet HEAD -- 2>/dev/null
}

# 获取当前分支名称
# 输出:
#   当前分支名称
get_current_branch() {
    git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown"
}

# 检查是否在 main 分支上
# 返回:
#   0 - 在 main 分支
#   1 - 不在 main 分支
is_on_main_branch() {
    [[ "$(get_current_branch)" == "main" ]]
}

# =============================================================================
# 调试辅助函数
# =============================================================================

# 打印版本信息（用于调试）
# 参数:
#   $1 - 本地版本
#   $2 - 远程版本
#   $3 - 验证级别（可选）
print_version_info() {
    local local_version="$1"
    local remote_version="$2"
    local verify_level="${3:-}"

    echo "版本信息:"
    echo "  本地版本: $local_version"
    echo "  远程版本: $remote_version"

    if [[ -n "$verify_level" ]]; then
        echo "  验证级别: $verify_level"
        echo "  $(get_verify_level_description "$verify_level")"
    fi
}
