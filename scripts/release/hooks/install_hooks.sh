#!/bin/bash
# =============================================================================
# Git Hooks 安装脚本
# =============================================================================
#
# 用法: bash scripts/release/hooks/install_hooks.sh
#
# 此脚本将钩子安装到 .git/hooks/ 目录
# =============================================================================

set -e

# =============================================================================
# 颜色定义
# =============================================================================
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

# =============================================================================
# 路径设置
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
HOOKS_SOURCE_DIR="$SCRIPT_DIR"
HOOKS_TARGET_DIR="$PROJECT_ROOT/.git/hooks"

# =============================================================================
# 日志函数
# =============================================================================
log_info() {
    echo -e "${BLUE}>>>${NC} $1"
}

log_success() {
    echo -e "${GREEN}✓${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

log_error() {
    echo -e "${RED}✗${NC} $1"
}

# =============================================================================
# 检查是否在 Git 仓库中
# =============================================================================
if [[ ! -d "$PROJECT_ROOT/.git" ]]; then
    log_error "未找到 .git 目录"
    echo ""
    echo "请确认你在 Git 仓库中运行此脚本"
    exit 1
fi

# =============================================================================
# 打印标题
# =============================================================================
echo ""
log_info "========================================"
log_info "Git Hooks 安装"
log_info "========================================"
echo ""

# =============================================================================
# 显示安装信息
# =============================================================================
echo "项目根目录: $PROJECT_ROOT"
echo "钩子源目录: $HOOKS_SOURCE_DIR"
echo "钩子目标目录: $HOOKS_TARGET_DIR"
echo ""

# =============================================================================
# 确认安装
# =============================================================================
read -p "是否继续安装? [Y/n] " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]] && [[ ! -z $REPLY ]]; then
    log_warning "安装已取消"
    exit 0
fi

# =============================================================================
# 备份现有钩子
# =============================================================================
backup_existing_hooks() {
    local hook_name="$1"
    local target_hook="$HOOKS_TARGET_DIR/$hook_name"

    if [[ -f "$target_hook" ]]; then
        # 检查是否是我们之前安装的钩子
        if grep -q "CFDesktop Git Hooks" "$target_hook" 2>/dev/null; then
            # 是我们安装的，直接覆盖，不备份
            return
        fi

        # 是用户自定义的钩子，需要备份
        local backup_file="$target_hook.backup.$(date +%Y%m%d%H%M%S)"
        log_warning "发现现有钩子: $hook_name"
        log_warning "备份到: $backup_file"
        cp "$target_hook" "$backup_file"
    fi
}

# =============================================================================
# 安装单个钩子
# =============================================================================
install_hook() {
    local sample_name="$1"
    local hook_name="$2"
    local source_hook="$HOOKS_SOURCE_DIR/$sample_name"
    local target_hook="$HOOKS_TARGET_DIR/$hook_name"

    # 备份现有钩子
    backup_existing_hooks "$hook_name"

    # 复制钩子
    cp "$source_hook" "$target_hook"
    chmod +x "$target_hook"
    log_success "$hook_name 已安装"
}

# =============================================================================
# 安装所有钩子
# =============================================================================
echo ""
log_info "安装 Git Hooks..."
echo ""

# 安装 pre-commit
if [[ -f "$HOOKS_SOURCE_DIR/pre-commit.sample" ]]; then
    install_hook "pre-commit.sample" "pre-commit"
else
    log_warning "pre-commit.sample 不存在，跳过"
fi

# 安装 pre-push
if [[ -f "$HOOKS_SOURCE_DIR/pre-push.sample" ]]; then
    install_hook "pre-push.sample" "pre-push"
else
    log_warning "pre-push.sample 不存在，跳过"
fi

# =============================================================================
# 完成
# =============================================================================
echo ""
log_success "========================================"
log_success "Git Hooks 安装完成！"
log_success "========================================"
echo ""
echo "已安装的钩子:"
echo "  - pre-commit:  代码格式检查（所有分支）"
echo "  - pre-push:    Docker 构建验证（仅 main 和 release 分支）"
echo ""
echo "验证安装:"
echo "  ls -la .git/hooks/pre-commit .git/hooks/pre-push"
echo ""
echo "卸载方法:"
echo "  rm .git/hooks/pre-commit .git/hooks/pre-push"
echo ""
echo "更多信息请参考: scripts/release/hooks/README.md"
echo ""
