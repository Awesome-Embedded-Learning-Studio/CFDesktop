#!/bin/bash
# This script configures and builds the project (FAST version - no cleaning)
# It calls the configure script first, then builds
# Usage: ./linux_fast_develop_build.sh [-c|--config <config_file>]
set -e

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source library functions
source "$SCRIPT_DIR/../lib/bash/lib_common.sh"
source "$SCRIPT_DIR/../lib/bash/lib_config.sh"
source "$SCRIPT_DIR/../lib/bash/lib_paths.sh"

# Default values
CONFIG_MODE="develop"
CONFIG_FILE=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        develop|deploy|ci)
            CONFIG_MODE="$1"
            shift
            ;;
        -h|--help)
            echo "Usage: $(basename "$0") [develop|deploy|ci] [-c|--config <config_file>]"
            echo ""
            echo "Arguments:"
            echo "  develop    Use development configuration (default)"
            echo "  deploy     Use deployment configuration"
            echo "  ci         Use CI configuration"
            echo "  -c, --config <file>  Use custom configuration file"
            echo "  -h, --help           Show this help message"
            exit 0
            ;;
        *)
            log_error "Unknown argument '$1'"
            log_error "Usage: $(basename "$0") [develop|deploy|ci] [-c|--config <config_file>]"
            exit 1
            ;;
    esac
done

log_separator "="
log_info "Starting Linux FAST Build Process"
log_separator "="

log_info "Project root: $PROJECT_ROOT"
cd "$PROJECT_ROOT"

# Step 1: Call the configure script
log_separator "="
log_info "Step 1: Configuring with CMake"
log_separator "="

CONFIGURE_SCRIPT="$SCRIPT_DIR/linux_configure.sh"
CONFIGURE_ARGS=("$CONFIG_MODE")
if [[ -n "$CONFIG_FILE" ]]; then
    CONFIGURE_ARGS+=("-c" "$CONFIG_FILE")
fi
log_info "Executing: $CONFIGURE_SCRIPT ${CONFIGURE_ARGS[*]}"

if bash "$CONFIGURE_SCRIPT" "${CONFIGURE_ARGS[@]}"; then
    log_success "Configuration completed successfully!"
else
    exit_code=$?
    log_error "Configure script failed with exit code: $exit_code"
    exit $exit_code
fi

# Step 2: Load config for build
# Use get_default_config_file from lib_config.sh
if [[ -z "$CONFIG_FILE" ]]; then
    CONFIG_FILE="$(get_default_config_file "$CONFIG_MODE")"
fi

# Resolve relative path
if [[ "$CONFIG_FILE" != /* ]] && [[ "$CONFIG_FILE" != ~* ]]; then
    CONFIG_FILE="$SCRIPT_DIR/$CONFIG_FILE"
fi

# Safety check: config file must exist
if [[ ! -f "$CONFIG_FILE" ]]; then
    log_error "Configuration file not found: $CONFIG_FILE"
    log_error "Please create the configuration file from the .template file"
    log_error "Example: cp \"$CONFIG_FILE.template\" \"$CONFIG_FILE\""
    exit 1
fi

eval "$(get_ini_config "$CONFIG_FILE")"
BUILD_DIR="$config_paths_build_dir"
JOBS="${config_options_jobs:-}"

# Safety check: BUILD_DIR must not be empty
if [[ -z "$BUILD_DIR" ]]; then
    log_error "Configuration error: build_dir is not set in config file"
    log_error "Please check the [paths] section in: $CONFIG_FILE"
    exit 1
fi

# Step 3: Build with CMake
log_separator "="
log_info "Step 2: Building project"

BUILD_CMD=(cmake --build "$BUILD_DIR")
if [[ -n "$JOBS" ]]; then
    BUILD_CMD+=(--parallel "$JOBS")
    log_info "Command: cmake --build $BUILD_DIR --parallel $JOBS"
else
    log_info "Command: cmake --build $BUILD_DIR"
fi
log_separator "="

# Run build and stream output in real-time
"${BUILD_CMD[@]}"
