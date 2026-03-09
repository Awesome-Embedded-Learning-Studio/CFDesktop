#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_build.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_build.sh
#
# =============================================================================

# 测试框架依赖检查（自动安装）
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_HELPER_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
if ! source "$TEST_HELPER_DIR/lib/bash_unit_helper.sh" || ! ensure_bash_unit; then
    exit 1
fi

# 加载额外的断言函数
source "$SCRIPT_DIR/assertions.sh"

# 加载被测试的模块
LIB_DIR="$(cd "$SCRIPT_DIR/../../../../scripts/lib/bash" && pwd)"
source "$LIB_DIR/lib_build.sh"

# =============================================================================
# 测试用例
# =============================================================================

test_ensure_build_dir_creates_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local build_dir="$temp_dir/build"

    ensure_build_dir "$build_dir"

    assert_status_code 0 "[[ -d '$build_dir' ]]"

    rm -rf "$temp_dir"
}

test_ensure_build_dir_does_not_fail_if_exists() {
    local temp_dir
    temp_dir=$(mktemp -d)

    assert_status_code 0 "ensure_build_dir '$temp_dir'"

    rm -rf "$temp_dir"
}

test_get_parallel_job_count_returns_positive_number() {
    local result
    result=$(get_parallel_job_count)
    assert_not_empty "$result"

    # 应该是一个正整数
    assert_status_code 0 "[[ '$result' =~ ^[0-9]+$ ]]"

    assert_status_code 0 "[[ '$result' -gt 0 ]]"
}

test_has_cmake_cache_returns_false_for_nonexistent_dir() {
    assert_fails "has_cmake_cache '/nonexistent/build/dir'"
}

test_has_cmake_cache_returns_false_for_dir_without_cache() {
    local temp_dir
    temp_dir=$(mktemp -d)

    assert_fails "has_cmake_cache '$temp_dir'"

    rm -rf "$temp_dir"
}

test_has_cmake_cache_returns_true_for_dir_with_cache() {
    local temp_dir
    temp_dir=$(mktemp -d)
    touch "$temp_dir/CMakeCache.txt"

    assert_status_code 0 "has_cmake_cache '$temp_dir'"

    rm -rf "$temp_dir"
}

test_build_timer_functions() {
    # 测试计时器函数不报错
    assert_status_code 0 "build_timer_start"

    # 等待一小段时间
    sleep 0.1

    # 注意：build_timer_end 会输出，但我们只检查是否不报错
    assert_status_code 0 "build_timer_end 2>&1 >/dev/null"
}

test_clean_build_dir_removes_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local build_dir="$temp_dir/build"
    mkdir -p "$build_dir"
    touch "$build_dir/test_file.txt"

    assert_status_code 0 "clean_build_dir '$build_dir' 2>&1 >/dev/null"

    # 目录应该被删除
    assert_status_code 0 "[[ ! -d '$build_dir' ]]"

    rm -rf "$temp_dir"
}

test_clean_build_dir_skips_nonexistent_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local build_dir="$temp_dir/build"

    assert_status_code 0 "clean_build_dir '$build_dir' 2>&1 >/dev/null"

    rm -rf "$temp_dir"
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
