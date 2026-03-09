#!/bin/bash
# =============================================================================
# CFDesktop Build Dependencies Installation Script (Optimized)
# =============================================================================
set -euo pipefail

# =============================================================================
# Configuration
# =============================================================================
QT_VERSION="${QT_VERSION:-6.8.1}"
QT_MIRROR="${QT_MIRROR:-https://mirrors.aliyun.com/qt}"
QT_BASE_DIR="/opt/Qt"
# 根据架构设置 QT6_DIR 为实际安装路径（不使用软链接）
_QT_ARCH_SUBDIR="gcc_64"
if [[ "$(uname -m)" == "aarch64" ]]; then
    _QT_ARCH_SUBDIR="linux_gcc_arm64"
fi
QT6_DIR="${QT6_DIR:-$QT_BASE_DIR/$QT_VERSION/$_QT_ARCH_SUBDIR}"

# 【优化1】移除 VENV_DIR，Docker 中直接用系统 pip
DEBIAN_FRONTEND=noninteractive

# Color output
RED='\033[0;31m'; GREEN='\033[0;32m'
YELLOW='\033[1;33m'; BLUE='\033[0;34m'; NC='\033[0m'

log_info()    { echo -e "${BLUE}[INFO]${NC}    $(date '+%H:%M:%S') $*"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $(date '+%H:%M:%S') $*"; }
log_warn()    { echo -e "${YELLOW}[WARN]${NC}    $(date '+%H:%M:%S') $*"; }
log_error()   { echo -e "${RED}[ERROR]${NC}   $(date '+%H:%M:%S') $*" >&2; }
log_step()    { echo -e "\n${GREEN}==>${NC} $*\n$(printf '=%.0s' {1..79})"; }

trap 'log_error "Script failed at line $LINENO (exit code: $?)"; exit 1' ERR

# =============================================================================
# Auto-detect host architecture for Qt installation
# =============================================================================
# Detect the current architecture to install the correct Qt version
# - x86_64 -> linux_gcc_64
# - aarch64 -> linux_gcc_arm64
_detect_qt_arch() {
    local arch=$(uname -m)
    case "$arch" in
        x86_64|amd64)
            echo "linux_gcc_64"
            ;;
        aarch64|arm64)
            echo "linux_gcc_arm64"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# Valid Qt architectures
_valid_qt_archs=("linux_gcc_64" "linux_gcc_arm64" "gcc_64" "android-clang" "ios")

_is_valid_qt_arch() {
    local arch="$1"
    for valid in "${_valid_qt_archs[@]}"; do
        [[ "$arch" == "$valid" ]] && return 0
    done
    return 1
}

# If QT_ARCH is not explicitly set OR it doesn't match current architecture, auto-detect
# This allows Dockerfile defaults to be overridden by actual container architecture
if [[ -z "${QT_ARCH:-}" ]] || ! _is_valid_qt_arch "$QT_ARCH"; then
    QT_ARCH="$(_detect_qt_arch)"
elif [[ "$QT_ARCH" == "linux_gcc_64" ]] && [[ "$(uname -m)" == "aarch64" ]]; then
    # Special case: Dockerfile default linux_gcc_64 on ARM64 container -> auto-fix
    log_warn "Detected ARM64 container but QT_ARCH=linux_gcc_64, auto-correcting to linux_gcc_arm64"
    QT_ARCH="linux_gcc_arm64"
elif [[ "$QT_ARCH" == "linux_gcc_arm64" ]] && [[ "$(uname -m)" == "x86_64" ]]; then
    # Special case: ARM64 Qt on AMD64 container (cross-compile setup) -> keep as-is
    log_info "Cross-compile mode: ARM64 Qt on AMD64 container"
fi

log_info "Detected architecture: $(uname -m) -> Qt arch: $QT_ARCH"

# 根据 QT_ARCH 和当前架构确定 aqtinstall 的 host 参数
# linux_gcc_64 -> linux
# linux_gcc_arm64 -> linux_arm64 (原生) 或 linux (交叉编译时的目标平台)
QT_AQT_HOST="linux"
if [[ "$QT_ARCH" == "linux_gcc_arm64" ]] && [[ "$(uname -m)" == "aarch64" ]]; then
    QT_AQT_HOST="linux_arm64"  # 原生 ARM64
elif [[ "$QT_ARCH" == "linux_gcc_arm64" ]]; then
    QT_AQT_HOST="linux_arm64"  # 交叉编译也用 linux_arm64 host
fi
log_info "aqtinstall host: $QT_AQT_HOST"

# =============================================================================
main() {
    log_info "Qt version: $QT_VERSION | Arch: $QT_ARCH | Mirror: $QT_MIRROR"

    # -------------------------------------------------------------------------
    # 【优化2】配置 APT 镜像源
    # -------------------------------------------------------------------------
    log_step "Step 1: Configure APT mirror"

    # 标记是否已成功配置镜像（避免重复 apt update）
    _mirror_configured=false

    # 检查 ca-certificates 是否已安装
    if ! dpkg -s ca-certificates &>/dev/null; then
        # 未安装 CA 证书，先用 HTTP 镜像绕过
        log_warn "ca-certificates missing, trying HTTP bootstrap..."

        _write_http_mirror() {
            local mirror="$1"
            local sources_file="/etc/apt/sources.list.d/ubuntu.sources"
            [ -f "$sources_file" ] || return 1
            cp "$sources_file" "${sources_file}.bak"
            local ports_mirror="$mirror"
            [[ "$mirror" != *"/ubuntu-ports" ]] && ports_mirror="${mirror%/ubuntu}/ubuntu-ports"
            sed -i \
                -e "s|https\?://archive.ubuntu.com/ubuntu|$mirror|g" \
                -e "s|https\?://security.ubuntu.com/ubuntu|$mirror|g" \
                -e "s|https\?://ports.ubuntu.com/ubuntu-ports|$ports_mirror|g" \
                "$sources_file"
        }

        local arch="$(uname -m)"
        local http_mirror="http://mirrors.aliyun.com/ubuntu"
        [[ "$arch" == "aarch64" ]] && http_mirror="http://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports"
        HTTP_MIRROR="${QT_MIRROR_APT:-$http_mirror}"

        if _write_http_mirror "$HTTP_MIRROR" && timeout 60 apt update 2>/dev/null; then
            log_success "HTTP mirror active: $HTTP_MIRROR"
            rm -f /etc/apt/sources.list.d/ubuntu.sources.bak 2>/dev/null || true
            _mirror_configured=true
        else
            log_warn "HTTP mirror failed, using default sources"
            cp /etc/apt/sources.list.d/ubuntu.sources.bak /etc/apt/sources.list.d/ubuntu.sources 2>/dev/null || true
        fi
    fi

    # 如果还没配置成功镜像，配置 HTTPS 镜像
    if [[ "$_mirror_configured" == "false" ]]; then
        local arch="$(uname -m)"
        local mirrors=()
        if [[ "$arch" == "aarch64" ]]; then
            mirrors=(
                "https://mirrors.tuna.tsinghua.edu.cn/ubuntu-ports"
                "https://mirrors.ustc.edu.cn/ubuntu-ports"
            )
        else
            mirrors=(
                "https://mirrors.aliyun.com/ubuntu"
                "https://mirrors.tuna.tsinghua.edu.cn/ubuntu"
            )
        fi

        local sources_file="/etc/apt/sources.list.d/ubuntu.sources"
        if [ -f "$sources_file" ]; then
            cp "$sources_file" "${sources_file}.bak"
            for mirror in "${mirrors[@]}"; do
                log_info "Trying APT mirror: $mirror"
                local ports_mirror="$mirror"
                [[ "$mirror" != *"/ubuntu-ports" ]] && ports_mirror="${mirror%/ubuntu}/ubuntu-ports"
                sed -i \
                    -e "s|http://archive.ubuntu.com/ubuntu|$mirror|g" \
                    -e "s|https://archive.ubuntu.com/ubuntu|$mirror|g" \
                    -e "s|http://security.ubuntu.com/ubuntu|$mirror|g" \
                    -e "s|http://ports.ubuntu.com/ubuntu-ports|$ports_mirror|g" \
                    -e "s|https://ports.ubuntu.com/ubuntu-ports|$ports_mirror|g" \
                    "$sources_file"

                if timeout 60 apt update 2>/dev/null; then
                    log_success "APT mirror active: $mirror"
                    rm -f "${sources_file}.bak" 2>/dev/null || true
                    _mirror_configured=true
                    break
                fi
                log_warn "Mirror failed: $mirror, trying next..."
                cp "${sources_file}.bak" "$sources_file"
            done
            rm -f "${sources_file}.bak" 2>/dev/null || true
        fi
    fi

    # -------------------------------------------------------------------------
    log_step "Step 2: Install system build dependencies"
    # -------------------------------------------------------------------------
    # 【优化4】精简包列表，移除 Docker 中无意义的包
    # 注意：ca-certificates 包含在此列表，用于 HTTP bootstrap 后的补装
    apt install -y --no-install-recommends \
        `# === 编译工具链 ===` \
        build-essential cmake ninja-build ccache git wget curl \
        `# === Python ===` \
        python3 python3-pip \
        `# === 证书（Bootstrap 时可能未安装）===` \
        ca-certificates \
        `# === OpenGL / EGL ===` \
        libgl1-mesa-dev libglu1-mesa-dev libegl1-mesa-dev libopengl-dev \
        `# === XCB 核心（Qt 平台插件必需）===` \
        libxcb1-dev libx11-dev libx11-xcb-dev \
        libxcb-glx0-dev libxcb-shape0-dev libxcb-shm0-dev \
        libxcb-sync-dev libxcb-xkb-dev libxcb-xfixes0-dev \
        libxcb-randr0-dev libxcb-render-util0-dev \
        libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev \
        libxcb-xinerama0-dev libxcb-xv0-dev \
        `# === XKB / Wayland ===` \
        libxkbcommon-dev libxkbcommon-x11-dev libwayland-dev \
        `# === 字体 / 渲染 ===` \
        libfontconfig1-dev libfreetype-dev libharfbuzz-dev \
        `# === 系统基础库 ===` \
        libglib2.0-dev libdbus-1-dev libssl-dev \
        libicu-dev libsqlite3-dev libpcre2-dev \
        libdouble-conversion-dev libzstd-dev liblzma-dev \
        `# === 图像格式（Qt 自带部分，这里只保留系统级必要）===` \
        libjpeg-dev libpng-dev \
        `# === 其他 Qt 运行时依赖 ===` \
        libxcomposite-dev libinput-dev libudev-dev \
        libcups2-dev

    log_success "System dependencies installed"
    log_info "GCC: $(gcc --version | head -n1)"
    log_info "CMake: $(cmake --version | head -n1)"

    # -------------------------------------------------------------------------
    log_step "Step 3: Install aqtinstall"
    # -------------------------------------------------------------------------
    # 【优化5】Docker 中直接用系统 pip，无需 venv
    pip3 install --no-cache-dir --break-system-packages aqtinstall -i https://pypi.tuna.tsinghua.edu.cn/simple --trusted-host pypi.tuna.tsinghua.edu.cn
    mkdir -p /etc/xdg/aqtinstall
cat > /etc/xdg/aqtinstall/aqtinstall.ini << 'EOF'
[aqt]
logfile=/dev/null
logfile_append=False
EOF
    log_success "aqtinstall $(aqt --version 2>/dev/null) ready"

    # -------------------------------------------------------------------------
    log_step "Step 4: Install Qt $QT_VERSION"
    # -------------------------------------------------------------------------
    _try_qt_install() {
        local mirror="${1:-}"
        local timeout_sec="${2:-300}"
        local arch="${3:-$QT_ARCH}"
        local host="${4:-linux}"  # host 类型: linux 或 linux_arm64
        local extra_args=()
        [ -n "$mirror" ] && extra_args+=(-b "$mirror")

        log_info "Installing Qt $QT_VERSION $arch for host=$host (timeout: ${timeout_sec}s, mirror: ${mirror:-official})"
        timeout "$timeout_sec" aqt install-qt "$host" desktop "$QT_VERSION" "$arch" \
            -O "$QT_BASE_DIR" \
            -m qt5compat qtserialport qtimageformats \
            "${extra_args[@]}"
    }

    if ! _try_qt_install "$QT_MIRROR" 300 "$QT_ARCH" "$QT_AQT_HOST"; then
        log_warn "Primary mirror failed, falling back to official source (may be slow)..."
        _try_qt_install "" 600 "$QT_ARCH" "$QT_AQT_HOST" || {
            log_error "Qt installation failed from all sources"
            exit 1
        }
    fi

    # Qt 安装目录结构（aqtinstall 直接安装到实际目录）
    # linux_gcc_64 -> /opt/Qt/6.8.1/gcc_64
    # linux_gcc_arm64 -> /opt/Qt/6.8.1/linux_gcc_arm64
    log_info "Qt installation base: $QT_BASE_DIR"
    log_info "Qt version: $QT_VERSION"
    log_info "Qt arch subdir: $_QT_ARCH_SUBDIR"
    if [ -d "$QT_BASE_DIR/$QT_VERSION" ]; then
        log_info "Qt version directory contents:"
        ls -la "$QT_BASE_DIR/$QT_VERSION/" || true
    fi

    # 打印 Qt 配置信息（用于 CMake）
    log_info "=== Qt Configuration ==="
    log_info "Qt6_DIR (for CMake): $QT6_DIR/lib/cmake/Qt6"
    log_info "qmake path: $QT6_DIR/bin/qmake"
    log_info "Qt modules: qt5compat qtserialport qtimageformats"

    # -------------------------------------------------------------------------
    log_step "Step 5: Configure ccache"
    # -------------------------------------------------------------------------
    # 【优化6】修复错误的 ccache 配置语法
    ccache --max-size=5G
    ccache --set-config=compiler_check=content
    ccache --set-config=compression=true
    ccache --set-config=compression_level=1
    ccache -z  # 重置统计
    log_success "ccache configured (max-size: 5G)"

    # -------------------------------------------------------------------------
    log_step "Installation Complete!"
    # -------------------------------------------------------------------------
    log_success "All done! Qt $QT_VERSION installed at $QT6_DIR"
    log_info "Add to your build environment:"
    echo "  export Qt6_DIR=$QT6_DIR/lib/cmake/Qt6"
    echo "  export PATH=\$QT6_DIR/bin:\$PATH"
    echo "  export LD_LIBRARY_PATH=\$QT6_DIR/lib:\$LD_LIBRARY_PATH"

    # 将环境变量写入 .bashrc 方便交互模式使用
    cat >> /root/.bashrc << EOF
# Qt6 environment
export Qt6_DIR=$QT6_DIR/lib/cmake/Qt6
export PATH=\$QT6_DIR/bin:\$PATH
export LD_LIBRARY_PATH=\$QT6_DIR/lib:\$LD_LIBRARY_PATH
EOF
}

main "$@"