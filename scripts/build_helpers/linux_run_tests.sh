#!/bin/bash
# This script runs CMake tests using CTest
# It reads the build directory from the specified config file
# Usage: ./linux_run_tests.sh [develop|deploy|ci] [-c|--config <config_file>]

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
log_info "Running Tests (Config: $CONFIG_MODE)"
log_separator "="

log_info "Project root: $PROJECT_ROOT"
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

# Get build directory from config
BUILD_DIR="$config_paths_build_dir"

# Safety check: BUILD_DIR must not be empty
if [[ -z "$BUILD_DIR" ]]; then
    log_error "Configuration error: build_dir is not set in config file"
    log_error "Please check the [paths] section in: $CONFIG_FILE"
    exit 1
fi

BUILD_DIR="$PROJECT_ROOT/$BUILD_DIR/test"

log_info "Test directory: $BUILD_DIR"
log_info "Command: ctest --test-dir $BUILD_DIR --output-on-failure"

# Check if build directory exists
if [[ ! -d "$BUILD_DIR" ]]; then
    log_error "Build directory does not exist: $BUILD_DIR"
    log_error "Please run the build script first before running tests."
    exit 1
fi

# Run tests
log_separator "="
log_info "Running tests..."
log_separator "="

if ctest --test-dir "$BUILD_DIR" --output-on-failure; then
    log_separator "="
    log_success "All tests passed successfully!"
    log_separator "="
    exit 0
else
    exit_code=$?
    log_separator "="
    log_warn "Some tests failed with exit code: $exit_code"
    log_separator "="
    exit $exit_code
fi
