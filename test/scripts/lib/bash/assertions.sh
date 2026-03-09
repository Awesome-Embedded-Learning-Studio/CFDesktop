#!/usr/bin/env bash
# =============================================================================
# CFDesktop Scripts Library - Additional Assertions for bash_unit
# =============================================================================
#
# 提供额外的断言函数，兼容旧的 pgrange/bash-unit
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/assertions.sh"
#
# =============================================================================

# 断言字符串包含子串
# 参数:
#   $1 - 期望包含的子串
#   $2 - 实际字符串
#   $3 - 可选的错误消息
assert_contains() {
    local expected="$1"
    local actual="$2"
    local message="${3:-}"

    if [[ "$actual" != *"$expected"* ]]; then
        if [[ -n "$message" ]]; then
            fail "$message"
        else
            fail "Expected string to contain: [$expected] but was: [$actual]"
        fi
    fi
}

# 断言字符串不包含子串
# 参数:
#   $1 - 不期望包含的子串
#   $2 - 实际字符串
#   $3 - 可选的错误消息
assert_not_contains() {
    local expected="$1"
    local actual="$2"
    local message="${3:-}"

    if [[ "$actual" == *"$expected"* ]]; then
        if [[ -n "$message" ]]; then
            fail "$message"
        else
            fail "Expected string NOT to contain: [$expected] but it did"
        fi
    fi
}

# 断言变量非空
# 参数:
#   $1 - 要检查的变量值
#   $2 - 可选的错误消息
assert_not_empty() {
    local value="$1"
    local message="${2:-}"

    if [[ -z "$value" ]]; then
        if [[ -n "$message" ]]; then
            fail "$message"
        else
            fail "Expected value to be non-empty but it was empty"
        fi
    fi
}

# 断言变量为空
# 参数:
#   $1 - 要检查的变量值
#   $2 - 可选的错误消息
assert_empty() {
    local value="$1"
    local message="${2:-}"

    if [[ -n "$value" ]]; then
        if [[ -n "$message" ]]; then
            fail "$message"
        else
            fail "Expected value to be empty but was: [$value]"
        fi
    fi
}

# 断言数组包含元素
# 参数:
#   $1 - 期望的元素值
#   $2+ - 数组元素
assert_array_contains() {
    local expected="$1"
    shift
    local found=0

    for item in "$@"; do
        if [[ "$item" == "$expected" ]]; then
            found=1
            break
        fi
    done

    if [[ $found -eq 0 ]]; then
        fail "Expected array to contain: [$expected]"
    fi
}
