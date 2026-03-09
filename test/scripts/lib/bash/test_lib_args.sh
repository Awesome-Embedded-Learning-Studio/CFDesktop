#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_args.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_args.sh
#
# =============================================================================

# 测试框架依赖检查（自动安装）
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_HELPER_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
if ! source "$TEST_HELPER_DIR/lib/bash_unit_helper.sh" || ! ensure_bash_unit; then
    exit 1
fi

# 加载被测试的模块
LIB_DIR="$(cd "$SCRIPT_DIR/../../../../scripts/lib/bash" && pwd)"
source "$LIB_DIR/lib_args.sh"

# =============================================================================
# 测试用例
# =============================================================================

test_parse_config_mode_returns_develop() {
    local result
    result=$(parse_config_mode "develop")
    assert_equals "develop" "$result"
}

test_parse_config_mode_returns_deploy() {
    local result
    result=$(parse_config_mode "deploy")
    assert_equals "deploy" "$result"
}

test_parse_config_mode_returns_ci() {
    local result
    result=$(parse_config_mode "ci")
    assert_equals "ci" "$result"
}

test_parse_config_mode_returns_empty_for_invalid() {
    local result
    result=$(parse_config_mode "invalid") || true
    assert_equals "" "$result"
}

test_is_valid_config_mode_returns_true_for_valid() {
    is_valid_config_mode "develop" && status=0 || status=$?
    assert_equals 0 $status

    is_valid_config_mode "deploy" && status=0 || status=$?
    assert_equals 0 $status

    is_valid_config_mode "ci" && status=0 || status=$?
    assert_equals 0 $status
}

test_is_valid_config_mode_returns_false_for_invalid() {
    is_valid_config_mode "invalid" && status=0 || status=$?
    assert_not_equals 0 $status
}

test_parse_config_file_returns_value_for_short_arg() {
    local result
    result=$(parse_config_file "-c" "config.ini")
    assert_equals "config.ini" "$result"
}

test_parse_config_file_returns_value_for_long_arg() {
    local result
    result=$(parse_config_file "--config" "config.ini")
    assert_equals "config.ini" "$result"
}

test_parse_config_file_returns_empty_for_non_config_arg() {
    local result
    result=$(parse_config_file "--other" "value") || true
    assert_equals "" "$result"
}

test_is_help_arg_returns_true_for_help() {
    is_help_arg "-h" && status=0 || status=$?
    assert_equals 0 $status

    is_help_arg "--help" && status=0 || status=$?
    assert_equals 0 $status

    is_help_arg "help" && status=0 || status=$?
    assert_equals 0 $status
}

test_is_help_arg_returns_false_for_non_help() {
    is_help_arg "--other" && status=0 || status=$?
    assert_not_equals 0 $status

    is_help_arg "build" && status=0 || status=$?
    assert_not_equals 0 $status
}

test_validate_arg_count_returns_true_when_enough() {
    validate_arg_count 5 3 && status=0 || status=$?
    assert_equals 0 $status
}

test_validate_arg_count_returns_false_when_not_enough() {
    validate_arg_count 2 5 && status=0 || status=$?
    assert_not_equals 0 $status
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
