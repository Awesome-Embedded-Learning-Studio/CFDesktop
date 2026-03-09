#!/usr/bin/env bash
# =============================================================================
# Unit Tests for lib_config.sh
# =============================================================================
#
# Usage:
#   bash-unit test_lib_config.sh
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
source "$LIB_DIR/lib_config.sh"

# =============================================================================
# 测试辅助函数
# =============================================================================

# 创建临时 INI 文件
create_test_ini() {
    local filepath="$1"
    cat > "$filepath" << 'EOF'
# This is a comment
[cmake]
generator = Ninja
build_type = Release

[paths]
source = .
build_dir = build

[options]
use_ccache = true
parallel_jobs = 4
EOF
}

# =============================================================================
# 测试用例
# =============================================================================

test_get_ini_config_returns_config_variables() {
    local temp_file
    temp_file=$(mktemp)
    create_test_ini "$temp_file"

    local output
    output=$(get_ini_config "$temp_file")

    assert_contains "config_cmake_generator" "$output"
    assert_contains "config_paths_build_dir" "$output"

    rm -f "$temp_file"
}

test_get_ini_config_parses_values_correctly() {
    local temp_file
    temp_file=$(mktemp)
    create_test_ini "$temp_file"

    eval "$(get_ini_config "$temp_file")"

    assert_equals "Ninja" "$config_cmake_generator"
    assert_equals "Release" "$config_cmake_build_type"
    assert_equals "build" "$config_paths_build_dir"
    assert_equals "true" "$config_options_use_ccache"
    assert_equals "4" "$config_options_parallel_jobs"

    rm -f "$temp_file"
}

test_get_ini_config_handles_quoted_values() {
    local temp_file
    temp_file=$(mktemp)
    cat > "$temp_file" << 'EOF'
[section]
key1 = "quoted value"
key2 = 'single quoted'
key3 = unquoted
EOF

    eval "$(get_ini_config "$temp_file")"

    assert_equals "quoted value" "$config_section_key1"
    assert_equals "single quoted" "$config_section_key2"
    assert_equals "unquoted" "$config_section_key3"

    rm -f "$temp_file"
}

test_get_ini_config_returns_error_for_missing_file() {
    assert_fails "get_ini_config '/nonexistent/file.ini' 2>&1"
}

test_get_ini_value_returns_correct_value() {
    local temp_file
    temp_file=$(mktemp)
    create_test_ini "$temp_file"

    local value
    value=$(get_ini_value "$temp_file" "cmake" "generator")

    assert_equals "Ninja" "$value"

    rm -f "$temp_file"
}

test_get_default_config_file_returns_develop_config() {
    local config
    config=$(get_default_config_file "develop" "$(mktemp -d)")
    assert_contains "build_develop_config.ini" "$config"
}

test_get_default_config_file_returns_deploy_config() {
    local config
    config=$(get_default_config_file "deploy" "$(mktemp -d)")
    assert_contains "build_deploy_config.ini" "$config"
}

test_get_default_config_file_returns_ci_config() {
    local config
    config=$(get_default_config_file "ci" "$(mktemp -d)")
    assert_contains "build_ci_config.ini" "$config"
}

# 如果直接运行此脚本，执行所有测试
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    bash-unit "$0"
fi
