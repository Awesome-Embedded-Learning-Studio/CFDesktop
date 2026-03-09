#!/usr/bin/env bash
# =============================================================================
# CFDesktop Scripts Library - bash-unit Helper
# =============================================================================
#
# 自动检测和安装 bash-unit 测试框架
#
# 用法:
#   source "$(dirname "${BASH_SOURCE[0]}")/lib/bash_unit_helper.sh"
#
# =============================================================================

# 获取项目根目录和第三方工具目录
TEST_HELPER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TEST_HELPER_DIR/../.." && pwd)"
THIRD_PARTY_DIR="$PROJECT_ROOT/third_party"
BASH_UNIT_DIR="$THIRD_PARTY_DIR/bash-unit"

# =============================================================================
# 自动检测和安装 bash-unit
# =============================================================================

ensure_bash_unit() {
    # 首先检查 bash-unit 是否已在 PATH 中（兼容 bash_unit 和 bash-unit 两种命名）
    if command -v bash_unit >/dev/null 2>&1; then
        return 0
    fi

    if command -v bash-unit >/dev/null 2>&1; then
        return 0
    fi

    # 检查是否已在 third_party 中安装
    if [[ -f "$BASH_UNIT_DIR/bash_unit" ]]; then
        export PATH="$PATH:$BASH_UNIT_DIR"
        return 0
    fi

    if [[ -f "$BASH_UNIT_DIR/bash-unit" ]]; then
        export PATH="$PATH:$BASH_UNIT_DIR"
        return 0
    fi

    # 自动安装 bash-unit
    echo "⚠ bash-unit is not installed!"
    echo ""
    echo "Auto-installing bash-unit to: $BASH_UNIT_DIR"
    echo ""

    # 创建 third_party 目录（如果不存在）
    mkdir -p "$THIRD_PARTY_DIR"

    # Clone bash-unit
    if git clone --quiet https://github.com/bash-unit/bash_unit.git "$BASH_UNIT_DIR" 2>/dev/null; then
        # 添加到 PATH
        export PATH="$PATH:$BASH_UNIT_DIR"
        echo "✓ bash-unit installed successfully!"
        echo ""
        return 0
    else
        echo "✗ Failed to clone bash-unit repository!"
        echo ""
        echo "Please install manually:"
        echo "  git clone https://github.com/bash-unit/bash_unit.git $BASH_UNIT_DIR"
        echo "  export PATH=\"\$PATH:$BASH_UNIT_DIR\""
        echo ""
        return 1
    fi
}

# 如果直接运行此脚本，执行检测
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    ensure_bash_unit
fi
