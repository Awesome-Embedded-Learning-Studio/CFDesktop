#!/bin/bash
# This script cleans the build directory then calls the fast version to build
# Usage: ./linux_develop_build.sh [-c|--config <config_file>]
set -e

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source library functions
source "$SCRIPT_DIR/../lib/bash/lib_common.sh"
source "$SCRIPT_DIR/../lib/bash/lib_config.sh"
source "$SCRIPT_DIR/../lib/bash/lib_build.sh"
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
log_info "Starting Linux Build Process (Full Clean + Build)"
log_separator "="

log_info "Project root: $PROJECT_ROOT"
log_info "Changing to project directory"

cd "$PROJECT_ROOT"

# Use get_default_config_file from lib_config.sh
if [[ -z "$CONFIG_FILE" ]]; then
    CONFIG_FILE="$(get_default_config_file "$CONFIG_MODE")"
fi

# Resolve relative path
if [[ "$CONFIG_FILE" != /* ]] && [[ "$CONFIG_FILE" != ~* ]]; then
    CONFIG_FILE="$SCRIPT_DIR/$CONFIG_FILE"
fi

log_info "Loading configuration from: $CONFIG_FILE"

# Safety check: config file must exist
if [[ ! -f "$CONFIG_FILE" ]]; then
    log_error "Configuration file not found: $CONFIG_FILE"
    log_error "Please create the configuration file from the .template file"
    log_error "Example: cp \"$CONFIG_FILE.template\" \"$CONFIG_FILE\""
    exit 1
fi

# Use get_ini_config from lib_config.sh
eval "$(get_ini_config "$CONFIG_FILE")"
log_success "Configuration loaded successfully!"

# Extract configuration values
SOURCE_DIR="$config_paths_source"
BUILD_DIR="$config_paths_build_dir"

# Safety check: BUILD_DIR must not be empty
if [[ -z "$BUILD_DIR" ]]; then
    log_error "Configuration error: build_dir is not set in config file"
    log_error "Please check the [paths] section in: $CONFIG_FILE"
    exit 1
fi

# Safety check: BUILD_DIR must not be project root
if [[ "$BUILD_DIR" == "." ]] || [[ "$BUILD_DIR" == "/" ]]; then
    log_error "Configuration error: build_dir cannot be project root or root directory"
    log_error "Current value: $BUILD_DIR"
    exit 1
fi

# Resolve source directory: if relative, make it relative to project root
if [[ "$SOURCE_DIR" = /* ]]; then
    # Already an absolute path
    RESOLVED_SOURCE_DIR="$SOURCE_DIR"
else
    # Relative path, resolve against project root
    RESOLVED_SOURCE_DIR="$(cd "$PROJECT_ROOT/$SOURCE_DIR" 2>/dev/null && pwd)" || "$PROJECT_ROOT/$SOURCE_DIR"
fi

log_info "Source directory: $SOURCE_DIR (resolved: $RESOLVED_SOURCE_DIR)"
log_info "Build directory: $BUILD_DIR"

# Step 1: Clean build directory
log_separator "="
log_info "Step 1: Cleaning build directory"
log_separator "="

FULL_BUILD_PATH="$PROJECT_ROOT/$BUILD_DIR"

# Use clean_build_dir from lib_build.sh
if ! clean_build_dir "$FULL_BUILD_PATH"; then
    log_error "Failed to clean build directory"
    exit 1
fi

# Step 2: Call the fast build script
log_separator "="
log_info "Step 2: Calling fast build script"
log_separator "="

FAST_BUILD_SCRIPT="$SCRIPT_DIR/linux_fast_develop_build.sh"
FAST_BUILD_ARGS=("$CONFIG_MODE")
if [[ -n "$CONFIG_FILE" ]]; then
    # Resolve to relative path for passing to subprocess
    REL_CONFIG_FILE="${CONFIG_FILE#$SCRIPT_DIR/}"
    FAST_BUILD_ARGS+=("-c" "$REL_CONFIG_FILE")
fi
log_info "Executing: $FAST_BUILD_SCRIPT ${FAST_BUILD_ARGS[*]}"

if bash "$FAST_BUILD_SCRIPT" "${FAST_BUILD_ARGS[@]}"; then
    log_separator "="
    log_success "Build process completed successfully!"
    log_separator "="
else
    exit_code=$?
    log_error "Fast build script failed with exit code: $exit_code"
    exit $exit_code
fi

# Step 3: Run tests
log_separator "="
log_info "Step 3: Running tests"
log_separator "="

TEST_SCRIPT="$SCRIPT_DIR/linux_run_tests.sh"
TEST_ARGS=("$CONFIG_MODE")
if [[ -n "$CONFIG_FILE" ]]; then
    REL_CONFIG_FILE="${CONFIG_FILE#$SCRIPT_DIR/}"
    TEST_ARGS+=("-c" "$REL_CONFIG_FILE")
fi
log_info "Executing: $TEST_SCRIPT ${TEST_ARGS[*]}"

if bash "$TEST_SCRIPT" "${TEST_ARGS[@]}"; then
    log_separator "="
    log_success "All tests passed successfully!"
    log_separator "="
else
    exit_code=$?
    log_separator "="
    log_warn "Some tests failed with exit code: $exit_code"
    log_separator "="
    # Don't exit on test failure, just warn
fi
