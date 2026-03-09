#!/bin/bash
# =============================================================================
# CFDesktop Docker Build Environment
# =============================================================================
#
# A CI-style Docker build wrapper with pretty logging
#
# Usage:
#   bash scripts/build_helpers/docker_start.sh [options]
#
# Options:
#   --arch amd64|arm64    Target architecture (default: amd64)
#   --fast-build         Skip image cleanup, reuse existing if available
#   --verify             Run CI build verification instead of interactive shell
#   --build-project      Build image + run full clean build
#   --build-project-fast Build image + run fast build (no clean)
#   --run-project-test   Build image + run tests
#   --stay-on-error      Stay in container on CI failure for debugging
#   --no-log             Disable file logging (default: log to file)
#   --no-deps            Skip dependency installation in image
#   --help               Show this help message
#
# Examples:
#   bash scripts/build_helpers/docker_start.sh                      # Interactive shell
#   bash scripts/build_helpers/docker_start.sh --verify             # Run CI build
#   bash scripts/build_helpers/docker_start.sh --build-project      # Build image + full clean build
#   bash scripts/build_helpers/docker_start.sh --build-project-fast # Build image + fast build
#   bash scripts/build_helpers/docker_start.sh --run-project-test   # Build image + run tests
#   bash scripts/build_helpers/docker_start.sh --fast-build         # Reuse existing image
#   bash scripts/build_helpers/docker_start.sh --arch arm64         # ARM64 build
# =============================================================================

set -e

# =============================================================================
# Configuration
# =============================================================================
readonly IMAGE_NAME="cfdesktop-build"
readonly PROJECT_NAME="CFDesktop"

# Color codes
readonly COLOR_RESET="\033[0m"
readonly COLOR_CYAN="\033[0;36m"
readonly COLOR_GREEN="\033[0;32m"
readonly COLOR_YELLOW="\033[0;33m"
readonly COLOR_RED="\033[0;31m"
readonly COLOR_WHITE="\033[0;37m"
readonly COLOR_DIM="\033[0;90m"
readonly COLOR_BLUE="\033[0;34m"

# Unicode symbols
readonly SYMBOL_CHECK="✓"
readonly SYMBOL_CROSS="✗"
readonly SYMBOL_BULLET="●"
readonly SYMBOL_ARROW="→"

# =============================================================================
# Default values
# =============================================================================
ARCH="amd64"
FAST_BUILD=false
RUN_VERIFY=false
RUN_BUILD_PROJECT=false
RUN_BUILD_PROJECT_FAST=false
RUN_PROJECT_TEST=false
STAY_ON_ERROR=false
ENABLE_LOG=true
NO_DEPS=false

# =============================================================================
# Parse arguments
# =============================================================================
while [[ $# -gt 0 ]]; do
    case $1 in
        --arch)
            ARCH="$2"
            shift 2
            ;;
        --fast-build)
            FAST_BUILD=true
            shift
            ;;
        --verify)
            RUN_VERIFY=true
            shift
            ;;
        --build-project)
            RUN_BUILD_PROJECT=true
            shift
            ;;
        --build-project-fast)
            RUN_BUILD_PROJECT_FAST=true
            shift
            ;;
        --run-project-test)
            RUN_PROJECT_TEST=true
            shift
            ;;
        --stay-on-error)
            STAY_ON_ERROR=true
            shift
            ;;
        --no-log)
            ENABLE_LOG=false
            shift
            ;;
        --no-deps)
            NO_DEPS=true
            shift
            ;;
        --help)
            echo "Usage: bash scripts/build_helpers/docker_start.sh [options]"
            echo ""
            echo "Options:"
            echo "  --arch amd64|arm64    Target architecture (default: amd64)"
            echo "  --fast-build         Skip image cleanup, reuse existing if available"
            echo "  --verify             Run CI build verification instead of interactive shell"
            echo "  --build-project      Build image + run full clean build"
            echo "  --build-project-fast Build image + run fast build (no clean)"
            echo "  --run-project-test   Build image + run tests"
            echo "  --stay-on-error      Stay in container on CI failure for debugging"
            echo "  --no-log             Disable file logging (default: log to file)"
            echo "  --no-deps            Skip dependency installation in image (for manual setup)"
            echo "  --help               Show this help message"
            echo ""
            echo "Architecture Details:"
            echo "  amd64    - Native x86_64 build"
            echo "  arm64    - ARM64 native container (QEMU emulation on x86)"
            echo ""
            echo "Examples:"
            echo "  bash scripts/build_helpers/docker_start.sh                      # Interactive shell"
            echo "  bash scripts/build_helpers/docker_start.sh --verify             # Run CI build"
            echo "  bash scripts/build_helpers/docker_start.sh --build-project      # Build image + full clean build"
            echo "  bash scripts/build_helpers/docker_start.sh --build-project-fast # Build image + fast build"
            echo "  bash scripts/build_helpers/docker_start.sh --run-project-test   # Build image + run tests"
            echo "  bash scripts/build_helpers/docker_start.sh --fast-build         # Reuse existing image"
            echo "  bash scripts/build_helpers/docker_start.sh --arch arm64         # ARM64 build"
            exit 0
            ;;
        *)
            echo -e "${COLOR_RED}Unknown option: $1${COLOR_RESET}"
            echo "Use --help to see available options"
            exit 1
            ;;
    esac
done

# =============================================================================
# Validate architecture
# =============================================================================
if [[ "$ARCH" != "amd64" ]] && [[ "$ARCH" != "arm64" ]]; then
    echo -e "${COLOR_RED}Invalid architecture: $ARCH (must be amd64 or arm64)${COLOR_RESET}"
    exit 1
fi

# Docker platform mapping
# - amd64: native, use linux/amd64
# - arm64: native ARM64 container, use linux/arm64
DOCKER_PLATFORM="linux/$ARCH"

# =============================================================================
# Logging setup
# =============================================================================
LOG_DIR=""
LOG_FILE=""

setup_logging() {
    if [[ "$ENABLE_LOG" == true ]]; then
        # Set LOG_DIR now that PROJECT_ROOT is defined
        LOG_DIR="$PROJECT_ROOT/scripts/docker/logger"
        # Create logger directory if it doesn't exist
        mkdir -p "$LOG_DIR"

        # Generate timestamp: YYYYMMDD_HHMMSS
        local TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
        LOG_FILE="$LOG_DIR/ci_build_${TIMESTAMP}.log"

        # Initialize log file with header
        {
            echo "==============================================================================="
            echo "CFDesktop Docker Build Log"
            echo "==============================================================================="
            echo "Start Time: $(date '+%Y-%m-%d %H:%M:%S %Z')"
            echo "Architecture: $ARCH"
            echo "Platform: $DOCKER_PLATFORM"
            echo "Fast Build: $FAST_BUILD"
            echo "Verify Mode: $RUN_VERIFY"
            echo "==============================================================================="
            echo ""
        } > "$LOG_FILE"

        # Redirect stdout and stderr to both console and log file
        exec > >(tee -a "$LOG_FILE")
        exec 2>&1
    fi
}

# =============================================================================
# Helper functions
# =============================================================================
show_header() {
    local title="$1"
    local line="────────────────────────────────────────────────────────────"
    echo ""
    echo -e "${COLOR_CYAN}${line}${COLOR_RESET}"
    echo -e "${COLOR_CYAN}${title}${COLOR_RESET}"
    echo -e "${COLOR_CYAN}${line}${COLOR_RESET}"
    echo ""
}

show_stage() {
    local name="$1"
    local description="${2:-}"
    echo ""
    echo -e "${COLOR_CYAN}[${SYMBOL_BULLET}] ${name}${COLOR_DIM} - ${description}${COLOR_RESET}"
}

show_success() { echo -e "  ${COLOR_GREEN}${SYMBOL_CHECK} ${1}${COLOR_RESET}"; }
show_error() { echo -e "  ${COLOR_RED}${SYMBOL_CROSS} ${1}${COLOR_RESET}"; }
show_info() { echo -e "  ${COLOR_WHITE}${SYMBOL_ARROW} ${1}${COLOR_RESET}"; }

# =============================================================================
# Docker build output formatter
# =============================================================================
format_docker_line() {
    local line="$1"
    [[ -z "$line" ]] && return

    if [[ "$line" =~ ^#([0-9]+)[[:space:]]+\[(.+?)\][[:space:]]+(.+)$ ]]; then
        echo -e "  ${COLOR_BLUE}▸${COLOR_WHITE} ${BASH_REMATCH[3]}${COLOR_RESET}"
    elif [[ "$line" =~ ^(Get|Hit|Ign|Fetch|Reading|Download|Selecting|Preparing|Unpacking|Setting[[:space:]]up) ]]; then
        echo -e "    ${COLOR_DIM}${line}${COLOR_RESET}"
    elif [[ "$line" =~ [Ee]rror|[Ff]ailed|ERROR|FAILED ]]; then
        echo -e "  ${COLOR_RED}${SYMBOL_CROSS} ${line}${COLOR_RESET}"
    elif [[ "$line" =~ [Ww]arning|WARNING ]]; then
        echo -e "  ${COLOR_YELLOW}[!] ${line}${COLOR_RESET}"
    elif [[ "$line" =~ successfully|tagged|built ]]; then
        echo -e "  ${COLOR_GREEN}${SYMBOL_CHECK} ${line}${COLOR_RESET}"
    else
        echo -e "    ${COLOR_DIM}${line}${COLOR_RESET}"
    fi
}

# =============================================================================
# Main flow
# =============================================================================

# Get paths FIRST (needed for logging)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
DOCKERFILE_PATH="$PROJECT_ROOT/scripts/docker/Dockerfile.build"

# Setup logging before any output (now that PROJECT_ROOT is defined)
setup_logging

show_header "${PROJECT_NAME} Docker Build Environment"

show_stage "CONFIG" "Build parameters"
show_info "Architecture: $ARCH"
show_info "Platform: $DOCKER_PLATFORM"
show_info "Image: $IMAGE_NAME"
show_info "Fast build: $FAST_BUILD"

show_info "Project root: $PROJECT_ROOT"

# =============================================================================
# Check Docker
# =============================================================================
show_stage "CHECK" "Verifying Docker"

if ! command -v docker &> /dev/null; then
    show_error "Docker is not installed!"
    exit 1
fi
show_success "Docker: $(docker --version 2>&1 | awk '{print $3}' | sed 's/,//')"

if ! docker info &> /dev/null; then
    show_error "Docker daemon is not running!"
    exit 1
fi
show_success "Docker daemon is running"

# =============================================================================
# Clean and build image (default behavior)
# =============================================================================

# Function to get build args
get_build_args() {
    # Set QT_ARCH based on target architecture
    case "$ARCH" in
        amd64) QT_ARCH_VALUE="linux_gcc_64" ;;
        arm64)  QT_ARCH_VALUE="linux_gcc_arm64" ;;
        armhf)  QT_ARCH_VALUE="linux_gcc_arm64" ;;
        *)      QT_ARCH_VALUE="linux_gcc_64" ;;
    esac
    BUILD_ARGS="--build-arg QT_ARCH=$QT_ARCH_VALUE"
    # Add INSTALL_DEPS if --no-deps was specified
    if [[ "$NO_DEPS" == true ]]; then
        BUILD_ARGS="$BUILD_ARGS --build-arg INSTALL_DEPS=0"
    fi
    echo "$BUILD_ARGS"
}

# Function to build image with cache (for --build-project modes)
build_image_with_cache() {
    show_stage "BUILD" "Building image with cache: $IMAGE_NAME"
    BUILD_ARGS=$(get_build_args)
    docker build --platform "$DOCKER_PLATFORM" \
        $BUILD_ARGS \
        -f "$DOCKERFILE_PATH" \
        -t "$IMAGE_NAME" \
        "$PROJECT_ROOT" 2>&1 | while IFS= read -r line; do
            format_docker_line "$line"
        done

    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
        show_success "Image built successfully"
    else
        show_error "Build failed!"
        exit 1
    fi
}

if [[ "$FAST_BUILD" == true ]]; then
    # Fast mode: only build if image doesn't exist
    if ! docker image inspect "$IMAGE_NAME" &> /dev/null; then
        show_stage "BUILD" "Image not found, building..."
        BUILD_ARGS=$(get_build_args)
        docker build --platform "$DOCKER_PLATFORM" \
            $BUILD_ARGS \
            -f "$DOCKERFILE_PATH" \
            -t "$IMAGE_NAME" \
            "$PROJECT_ROOT" 2>&1 | while IFS= read -r line; do
                format_docker_line "$line"
            done
    else
        show_success "Using existing image (fast build mode)"
    fi
elif [[ "$RUN_BUILD_PROJECT" == true ]] || [[ "$RUN_BUILD_PROJECT_FAST" == true ]] || [[ "$RUN_PROJECT_TEST" == true ]]; then
    # BuildProject/Test modes: always build with cache
    build_image_with_cache
else
    # Default mode: ALWAYS clean and rebuild
    show_stage "CLEAN" "Removing old image: $IMAGE_NAME"
    if docker image inspect "$IMAGE_NAME" &> /dev/null; then
        docker rmi "$IMAGE_NAME" 2>/dev/null || true
        show_success "Old image removed"
    else
        show_info "No existing image to remove"
    fi

    show_stage "BUILD" "Building fresh image: $IMAGE_NAME"
    BUILD_ARGS=$(get_build_args)
    docker build --no-cache --platform "$DOCKER_PLATFORM" \
        $BUILD_ARGS \
        -f "$DOCKERFILE_PATH" \
        -t "$IMAGE_NAME" \
        "$PROJECT_ROOT" 2>&1 | while IFS= read -r line; do
            format_docker_line "$line"
        done

    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
        show_success "Image built successfully"
    else
        show_error "Build failed!"
        exit 1
    fi
fi

# =============================================================================
# Detect timezone
# =============================================================================
detect_timezone() {
    command -v timedatectl &> /dev/null && timedatectl 2>/dev/null | grep 'Time zone' | awk '{print $3}' && return
    [[ -f /etc/timezone ]] && cat /etc/timezone 2>/dev/null && return
    date +%Z 2>/dev/null || echo "UTC"
}
HOST_TIMEZONE=$(detect_timezone)

# =============================================================================
# Run container
# =============================================================================
# Get mount path
# Convert Windows path to Docker-compatible path
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    MOUNT_PATH="$(echo "$PROJECT_ROOT" | sed 's|^\([A-Za-z]\):|/\1|' | sed 's|\\|/|g')"
else
    MOUNT_PATH="$PROJECT_ROOT"
fi

if [[ "$RUN_VERIFY" == true ]]; then
    show_stage "VERIFY" "Running CI build"

    if [[ "$STAY_ON_ERROR" == true ]]; then
        show_info "Stay-on-error enabled: container will remain open on failure"

        MSYS_NO_PATHCONV=1 docker run --rm -it --platform "$DOCKER_PLATFORM" \
            -v "$MOUNT_PATH:/project" \
            -w /project \
            "$IMAGE_NAME" \
            bash -c "bash scripts/build_helpers/ci_build_entry.sh ci; exit_code=\$?; if [ \$exit_code -ne 0 ]; then echo ''; echo '=== Build failed, staying in container for debugging ==='; echo 'Type \"exit\" to leave the container'; echo ''; /bin/bash; fi; exit \$exit_code"

        exit_code=$?
        if [[ $exit_code -eq 0 ]]; then
            show_success "CI build completed successfully"
        else
            show_error "CI build failed with exit code: ${exit_code}"
            show_info "Container session closed"
            exit $exit_code
        fi
    else
        MSYS_NO_PATHCONV=1 docker run --rm --platform "$DOCKER_PLATFORM" \
            -v "$MOUNT_PATH:/project" \
            -w /project \
            "$IMAGE_NAME" \
            bash scripts/build_helpers/ci_build_entry.sh ci

        exit_code=$?
        if [[ $exit_code -eq 0 ]]; then
            show_success "CI build completed successfully"
        else
            show_error "CI build failed with exit code: ${exit_code}"
            exit $exit_code
        fi
    fi
elif [[ "$RUN_BUILD_PROJECT" == true ]] || [[ "$RUN_BUILD_PROJECT_FAST" == true ]]; then
    # Determine which script to run
    if [[ "$RUN_BUILD_PROJECT" == true ]]; then
        BUILD_SCRIPT="linux_develop_build.sh"
        BUILD_DESC="Full Clean Build"
    else
        BUILD_SCRIPT="linux_fast_develop_build.sh"
        BUILD_DESC="Fast Build (incremental)"
    fi

    # Determine config file based on architecture
    case "$ARCH" in
        amd64)  CONFIG_FILE="build_ci_config.ini" ;;
        arm64)  CONFIG_FILE="build_ci_aarch64_config.ini" ;;
        armhf)  CONFIG_FILE="build_ci_armhf_config.ini" ;;
        *)      CONFIG_FILE="build_ci_config.ini" ;;
    esac

    show_stage "BUILD" "Running $BUILD_DESC in container"
    MSYS_NO_PATHCONV=1 docker run --rm -it --platform "$DOCKER_PLATFORM" \
        -v "$MOUNT_PATH:/project" \
        -w /project \
        "$IMAGE_NAME" \
        bash scripts/build_helpers/$BUILD_SCRIPT ci -c $CONFIG_FILE

    exit_code=$?
    if [[ $exit_code -eq 0 ]]; then
        show_success "Build completed successfully"
    else
        show_error "Build failed with exit code: ${exit_code}"
        exit $exit_code
    fi
elif [[ "$RUN_PROJECT_TEST" == true ]]; then
    # Determine config file based on architecture
    case "$ARCH" in
        amd64)  CONFIG_FILE="build_ci_config.ini" ;;
        arm64)  CONFIG_FILE="build_ci_aarch64_config.ini" ;;
        armhf)  CONFIG_FILE="build_ci_armhf_config.ini" ;;
        *)      CONFIG_FILE="build_ci_config.ini" ;;
    esac

    show_stage "TEST" "Running tests in container"
    MSYS_NO_PATHCONV=1 docker run --rm -it --platform "$DOCKER_PLATFORM" \
        -v "$MOUNT_PATH:/project" \
        -w /project \
        "$IMAGE_NAME" \
        bash scripts/build_helpers/linux_run_tests.sh ci -c $CONFIG_FILE

    exit_code=$?
    if [[ $exit_code -eq 0 ]]; then
        show_success "All tests passed"
    else
        show_error "Tests failed with exit code: ${exit_code}"
        exit $exit_code
    fi
else
    show_stage "RUN" "Starting interactive shell"
    show_info "Type 'exit' to leave the container"
    echo ""

    MSYS_NO_PATHCONV=1 docker run --rm -it --platform "$DOCKER_PLATFORM" \
        -v "$MOUNT_PATH:/project" \
        -w /project \
        "$IMAGE_NAME" \
        /bin/bash
fi

show_stage "DONE" "Session ended"

# Log completion
if [[ "$ENABLE_LOG" == true ]] && [[ -n "$LOG_FILE" ]]; then
    {
        echo ""
        echo "==============================================================================="
        echo "End Time: $(date '+%Y-%m-%d %H:%M:%S %Z')"
        echo "Log saved to: $LOG_FILE"
        echo "==============================================================================="
    } >> "$LOG_FILE"
fi
