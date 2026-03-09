#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_paths.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_paths.sh
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
source "$LIB_DIR/lib_paths.sh"

# =============================================================================
# 测试用例
# =============================================================================

test_get_script_dir_returns_absolute_path() {
    local result
    result=$(get_script_dir)
    assert_not_empty "$result"
    # 应该是绝对路径
    assert_equals "/" "${result:0:1}"
}

test_get_project_root_returns_absolute_path() {
    local result
    result=$(get_project_root)
    assert_not_empty "$result"
    # 应该是绝对路径
    assert_equals "/" "${result:0:1}"
}

test_get_scripts_dir_returns_absolute_path() {
    local result
    result=$(get_scripts_dir)
    assert_not_empty "$result"
    assert_equals "/" "${result:0:1}"
}

test_get_lib_dir_returns_absolute_path() {
    local result
    result=$(get_lib_dir)
    assert_not_empty "$result"
    assert_equals "/" "${result:0:1}"
}

test_script_dir_environment_variable_is_set() {
    assert_not_empty "$SCRIPT_DIR"
}

test_project_root_environment_variable_is_set() {
    assert_not_empty "$PROJECT_ROOT"
}

test_scripts_dir_environment_variable_is_set() {
    assert_not_empty "$SCRIPTS_DIR"
}

test_lib_dir_environment_variable_is_set() {
    assert_not_empty "$LIB_DIR"
}

test_path_exists_returns_true_for_existing_directory() {
    path_exists "/tmp" && status=0 || status=$?
    assert_equals 0 $status
}

test_path_exists_returns_false_for_nonexistent_path() {
    path_exists "/nonexistent/path/that/does/not/exist" && status=0 || status=$?
    assert_not_equals 0 $status
}

test_ensure_dir_creates_directory() {
    local temp_dir
    temp_dir=$(mktemp -d)
    local new_dir="$temp_dir/test_subdir"

    ensure_dir "$new_dir"

    assert_equals 0 $?
    [[ -d "$new_dir" ]]
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

test_ensure_dir_does_not_fail_if_exists() {
    local temp_dir
    temp_dir=$(mktemp -d)

    ensure_dir "$temp_dir"
    assert_equals 0 $?

    rm -rf "$temp_dir"
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
