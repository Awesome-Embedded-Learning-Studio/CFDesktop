#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_git.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_git.sh
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
source "$LIB_DIR/lib_git.sh"

# =============================================================================
# 测试用例
# =============================================================================

test_get_major_version_extracts_major() {
    local result
    result=$(get_major_version "1.2.3")
    assert_equals "1" "$result"
}

test_get_major_version_handles_single_digit() {
    local result
    result=$(get_major_version "5")
    assert_equals "5" "$result"
}

test_get_minor_version_extracts_minor() {
    local result
    result=$(get_minor_version "1.2.3")
    assert_equals "2" "$result"
}

test_get_minor_version_returns_empty_for_single_digit() {
    local result
    result=$(get_minor_version "1")
    assert_equals "" "$result"
}

test_get_patch_version_extracts_patch() {
    local result
    result=$(get_patch_version "1.2.3")
    assert_equals "3" "$result"
}

test_get_patch_version_returns_empty_for_missing_patch() {
    local result
    result=$(get_patch_version "1.2")
    assert_equals "" "$result"
}

test_compare_versions_equal() {
    assert_status_code 0 "compare_versions '1.2.3' '1.2.3'"
}

test_compare_versions_greater() {
    assert_status_code 1 "compare_versions '1.3.0' '1.2.9'"
}

test_compare_versions_less() {
    assert_status_code 2 "compare_versions '1.2.9' '1.3.0'"
}

test_compare_versions_different_lengths() {
    assert_status_code 0 "compare_versions '1.2' '1.2.0'"
}

test_determine_verify_level_returns_major_for_major_change() {
    local result
    result=$(determine_verify_level "1.2.3" "2.0.0")
    assert_equals "major" "$result"
}

test_determine_verify_level_returns_minor_for_minor_change() {
    local result
    result=$(determine_verify_level "1.2.3" "1.3.0")
    assert_equals "minor" "$result"
}

test_determine_verify_level_returns_patch_for_patch_change() {
    local result
    result=$(determine_verify_level "1.2.3" "1.2.4")
    assert_equals "patch" "$result"
}

test_determine_verify_level_handles_empty_local_version() {
    local result
    result=$(determine_verify_level "" "1.2.3")
    assert_equals "major" "$result"
}

test_determine_verify_level_handles_empty_remote_version() {
    local result
    result=$(determine_verify_level "1.2.3" "")
    assert_equals "major" "$result"
}

test_get_verify_level_description_returns_major_description() {
    local result
    result=$(get_verify_level_description "major")
    assert_contains "Major" "$result"
    assert_contains "X64" "$result"
    assert_contains "ARM64" "$result"
}

test_get_verify_level_description_returns_minor_description() {
    local result
    result=$(get_verify_level_description "minor")
    assert_contains "Minor" "$result"
    assert_contains "X64" "$result"
}

test_get_verify_level_description_returns_patch_description() {
    local result
    result=$(get_verify_level_description "patch")
    assert_contains "Patch" "$result"
    assert_contains "X64" "$result"
}

test_get_verify_level_description_handles_unknown_level() {
    local result
    result=$(get_verify_level_description "unknown")
    assert_contains "未知验证级别" "$result"
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
