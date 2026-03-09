#!/bin/bash
# CI Build Entry Point for Docker
# This script is called by docker_start.ps1/docker_start.sh in CI mode
# Automatically detects container architecture and selects appropriate config
# Usage: ./ci_build_entry.sh [ci|test]

set -e

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# =============================================================================
# Auto-detect container architecture using uname
# =============================================================================
DETECTED_ARCH=$(uname -m)
case "$DETECTED_ARCH" in
    x86_64|amd64)
        CI_CONFIG="$SCRIPT_DIR/build_ci_config.ini"
        ARCH_NAME="AMD64 (native)"
        ;;
    aarch64)
        CI_CONFIG="$SCRIPT_DIR/build_ci_aarch64_config.ini"
        ARCH_NAME="ARM64 (native)"
        ;;
    armv7l|armhf)
        CI_CONFIG="$SCRIPT_DIR/build_ci_armhf_config.ini"
        ARCH_NAME="ARM32 (native, IMX6ULL)"
        ;;
    *)
        echo "ERROR: Unknown architecture: $DETECTED_ARCH"
        echo "Supported: x86_64, aarch64, armv7l"
        exit 1
        ;;
esac

MODE="${1:-ci}"

log() {
    local level="$2"
    local message="$1"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    local color=""

    case "$level" in
        INFO)    color="\033[0;36m" ;;
        SUCCESS) color="\033[0;32m" ;;
        WARNING) color="\033[0;33m" ;;
        ERROR)   color="\033[0;31m" ;;
        *)       color="\033[0m" ;;
    esac

    echo -e "${color}[$timestamp] [$level] $message\033[0m"
}

log "========================================" "INFO"
log "CI Build Entry Point" "INFO"
log "Detected Architecture: $DETECTED_ARCH" "INFO"
log "Build Target: $ARCH_NAME" "INFO"
log "Mode: $MODE" "INFO"
log "Config: $CI_CONFIG" "INFO"
log "========================================" "INFO"

case "$MODE" in
    ci)
        # Full CI build: configure + build + test
        log "Starting CI build process..." "INFO"

        # Extract config filename for display
        CONFIG_FILE=$(basename "$CI_CONFIG")

        # Run the develop build script with CI config
        log "Executing: linux_develop_build.sh ci -c $CONFIG_FILE" "INFO"
        if bash "$SCRIPT_DIR/linux_develop_build.sh" ci -c "$CONFIG_FILE"; then
            log "========================================" "INFO"
            log "CI build completed successfully!" "SUCCESS"
            log "========================================" "INFO"
            exit 0
        else
            exit_code=$?
            log "========================================" "INFO"
            log "CI build failed with exit code: $exit_code" "ERROR"
            log "========================================" "INFO"
            exit $exit_code
        fi
        ;;
    test)
        # Run tests only
        log "Running tests only..." "INFO"

        CONFIG_FILE=$(basename "$CI_CONFIG")
        log "Executing: linux_run_tests.sh ci -c $CONFIG_FILE" "INFO"

        if bash "$SCRIPT_DIR/linux_run_tests.sh" ci -c "$CONFIG_FILE"; then
            log "========================================" "INFO"
            log "All tests passed!" "SUCCESS"
            log "========================================" "INFO"
            exit 0
        else
            exit_code=$?
            log "========================================" "INFO"
            log "Tests failed with exit code: $exit_code" "ERROR"
            log "========================================" "INFO"
            exit $exit_code
        fi
        ;;
    *)
        log "Unknown mode: $MODE" "ERROR"
        log "Usage: $0 [ci|test]" "INFO"
        exit 1
        ;;
esac
