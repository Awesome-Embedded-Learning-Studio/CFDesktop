#!/bin/bash
# =============================================================================
# Version Utilities for Git Hooks
# =============================================================================
#
# 提供版本号解析和验证级别检测功能
#
# =============================================================================

# =============================================================================
# 版本号解析函数
# =============================================================================

# 解析版本号，返回主版本号
get_major_version() {
    local version="$1"
    echo "$version" | cut -d. -f1
}

# 解析版本号，返回次版本号
get_minor_version() {
    local version="$1"
    echo "$version" | cut -d. -f2
}

# 解析版本号，返回补丁版本号
get_patch_version() {
    local version="$1"
    echo "$version" | cut -d. -f3 -s
}

# =============================================================================
# 验证级别检测函数
# =============================================================================

# 根据本地版本和远程版本确定验证级别
# 参数:
#   $1 - local_version (本地版本号)
#   $2 - remote_version (远程版本号)
# 返回:
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
    local local_major=$(get_major_version "$local_version")
    local local_minor=$(get_minor_version "$local_version")
    # local_patch 不用于比较，仅用于记录

    local remote_major=$(get_major_version "$remote_version")
    local remote_minor=$(get_minor_version "$remote_version")

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
# Git 相关函数
# =============================================================================

# 获取当前分支的版本标签
# 返回: 最近的版本标签，如果没有则返回 "0.0.0"
get_local_version() {
    git describe --tags --abbrev=0 2>/dev/null || echo "0.0.0"
}

# 获取远程 main 分支的版本标签
# 返回: 远程 main 分支最近的版本标签，如果没有则返回 "0.0.0"
get_remote_version() {
    # 确保有最新的远程信息
    git fetch origin main:refs/remotes/origin/main --quiet 2>/dev/null || true
    git describe --tags --abbrev=0 origin/main 2>/dev/null || echo "0.0.0"
}

# 获取 CMakeLists.txt 中定义的版本号
# 参数: $1 - 项目根目录路径
# 返回: CMakeLists.txt 中的版本号，如果未找到则返回空字符串
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
# 调试辅助函数
# =============================================================================

# 打印版本信息（用于调试）
print_version_info() {
    local local_version="$1"
    local remote_version="$2"
    local verify_level="$3"

    echo "版本信息:"
    echo "  本地版本: $local_version"
    echo "  远程版本: $remote_version"
    echo "  验证级别: $verify_level"
    echo "  $(get_verify_level_description "$verify_level")"
}
