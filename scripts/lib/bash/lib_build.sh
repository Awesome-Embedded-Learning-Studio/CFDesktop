#!/bin/bash
# =============================================================================
# CFDesktop Scripts Library - Build Module
# =============================================================================
#
# 提供构建相关的辅助函数
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib_build.sh"
#   clean_build_dir "$BUILD_DIR"
#   run_cmake_configure "$GENERATOR" "$BUILD_TYPE"
#
# 注意: 此模块依赖 lib_common.sh 和 lib_config.sh
#
# =============================================================================

# 确保依赖已加载
if ! type log >/dev/null 2>&1; then
    # 如果 lib_common.sh 未加载，从相对路径加载
    _lib_build_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    source "$_lib_build_dir/lib_common.sh"
fi

# =============================================================================
# 构建目录操作
# =============================================================================

# 清理构建目录
# 参数:
#   $1 - 构建目录路径
clean_build_dir() {
    local build_path="$1"

    # 安全检查：路径不能为空
    if [[ -z "$build_path" ]]; then
        log "ERROR: Build path is empty! Refusing to clean to prevent accidental deletion." "ERROR"
        return 1
    fi

    # 安全检查：路径不能是根目录
    if [[ "$build_path" == "/" ]]; then
        log "ERROR: Refusing to clean root directory!" "ERROR"
        return 1
    fi

    # 安全检查：路径不能是用户主目录
    if [[ "$build_path" == "$HOME" ]] || [[ "$build_path" == "$HOME/" ]]; then
        log "ERROR: Refusing to clean home directory!" "ERROR"
        return 1
    fi

    # 安全检查：路径不能是项目根目录（如果已设置）
    if [[ -n "${PROJECT_ROOT:-}" ]] && [[ "$build_path" == "$PROJECT_ROOT" ]] || [[ "$build_path" == "$PROJECT_ROOT/" ]]; then
        log "ERROR: Refusing to clean project root directory!" "ERROR"
        return 1
    fi

    # 安全检查：路径必须是包含 build、out、build_ 等关键词的子目录
    local basename
    basename=$(basename "$build_path")
    if [[ ! "$basename" =~ ^(build|out|build_.+|cmake-.+)$ ]]; then
        log "WARNING: Build directory name '$basename' does not match expected pattern. Aborting for safety." "WARNING"
        log "Expected patterns: build, out, build_*, cmake-*" "WARNING"
        return 1
    fi

    if [[ -d "$build_path" ]]; then
        log "Removing build directory: $build_path" "INFO"
        # 使用 find 进行更可靠的删除（特别是在 Docker 挂载卷上）
        if find "$build_path" -mindepth 1 -delete 2>/dev/null && \
           rmdir "$build_path" 2>/dev/null; then
            log "Build directory cleaned successfully" "SUCCESS"
            return 0
        elif find "$build_path" -mindepth 1 -exec rm -rf {} + 2>/dev/null && \
             rmdir "$build_path" 2>/dev/null; then
            log "Build directory cleaned successfully" "SUCCESS"
            return 0
        elif rm -rf "$build_path" 2>/dev/null; then
            log "Build directory cleaned successfully" "SUCCESS"
            return 0
        else
            log "Failed to clean build directory" "ERROR"
            return 1
        fi
    else
        log "Build directory does not exist, skipping clean" "INFO"
        return 0
    fi
}

# 确保构建目录存在
# 参数:
#   $1 - 构建目录路径
ensure_build_dir() {
    local build_path="$1"
    if [[ ! -d "$build_path" ]]; then
        log "Creating build directory: $build_path" "INFO"
        mkdir -p "$build_path"
    fi
}

# =============================================================================
# CMake 操作
# =============================================================================

# 运行 CMake 配置
# 参数:
#   $1 - Generator (如 "Ninja", "Unix Makefiles")
#   $2 - Build Type (Debug, Release, RelWithDebInfo)
#   $3 - Source 目录
#   $4 - Build 目录
#   $5 - 额外的 CMake 参数（可选）
run_cmake_configure() {
    local generator="$1"
    local build_type="$2"
    local source_dir="$3"
    local build_dir="$4"
    shift 4
    local extra_args=("$@")

    log "Configuring with CMake..." "INFO"
    log "  Generator: $generator" "INFO"
    log "  Build Type: $build_type" "INFO"
    log "  Source: $source_dir" "INFO"
    log "  Build: $build_dir" "INFO"

    local cmake_args=(
        "-G" "$generator"
        "-DCMAKE_BUILD_TYPE=$build_type"
        "-S" "$source_dir"
        "-B" "$build_dir"
        "${extra_args[@]}"
    )

    if cmake "${cmake_args[@]}"; then
        log "CMake configuration completed successfully" "SUCCESS"
        return 0
    else
        local exit_code=$?
        log "CMake configuration failed with exit code: $exit_code" "ERROR"
        return $exit_code
    fi
}

# 运行 CMake 构建
# 参数:
#   $1 - Build 目录
#   $2 - 目标名称（可选，默认构建所有）
#   $3 - 并行作业数（可选，默认自动检测）
run_cmake_build() {
    local build_dir="$1"
    local target="${2:---all}"
    local jobs="${3:-}"

    log "Building with CMake..." "INFO"
    log "  Build directory: $build_dir" "INFO"
    [[ -n "$jobs" ]] && log "  Parallel jobs: $jobs" "INFO"

    local build_args=("--build" "$build_dir" "--target" "$target")
    if [[ -n "$jobs" ]]; then
        build_args+=("--parallel" "$jobs")
    fi

    if cmake "${build_args[@]}"; then
        log "Build completed successfully" "SUCCESS"
        return 0
    else
        local exit_code=$?
        log "Build failed with exit code: $exit_code" "ERROR"
        return $exit_code
    fi
}

# =============================================================================
# 构建状态检查
# =============================================================================

# 检查 CMake 缓存是否存在
# 参数:
#   $1 - Build 目录
has_cmake_cache() {
    local build_dir="$1"
    [[ -f "$build_dir/CMakeCache.txt" ]]
}

# 获取 CMake 缓存中的变量值
# 参数:
#   $1 - Build 目录
#   $2 - 变量名
get_cmake_cache_var() {
    local build_dir="$1"
    local var_name="$2"
    local cache_file="$build_dir/CMakeCache.txt"

    if [[ -f "$cache_file" ]]; then
        grep "^${var_name}:" "$cache_file" | cut -d= -f2-
    fi
}

# =============================================================================
# 并行构建
# =============================================================================

# 获取推荐的并行作业数
# 返回:
#   CPU 核心数（或 1 如果无法检测）
get_parallel_job_count() {
    if command -v nproc >/dev/null 2>&1; then
        nproc
    elif command -v sysctl >/dev/null 2>&1; then
        sysctl -n hw.ncpu 2>/dev/null || echo 1
    else
        echo 1
    fi
}

# =============================================================================
# 构建时间统计
# =============================================================================

# 开始构建计时
build_timer_start() {
    _BUILD_START_TIME=$(date +%s)
}

# 结束构建计时并显示耗时
build_timer_end() {
    if [[ -n "${_BUILD_START_TIME:-}" ]]; then
        local end_time
        end_time=$(date +%s)
        local duration=$((end_time - _BUILD_START_TIME))
        local minutes=$((duration / 60))
        local seconds=$((duration % 60))
        log "Build time: ${minutes}m ${seconds}s" "INFO"
        unset _BUILD_START_TIME
    fi
}
