#!/bin/bash
# This script ONLY configures the project using CMake
# It does NOT build the project
# Usage: ./linux_configure.sh [develop|deploy|ci] [-c|--config <config_file>]

set -e

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source library functions
source "$SCRIPT_DIR/../lib/bash/lib_common.sh"
source "$SCRIPT_DIR/../lib/bash/lib_config.sh"
source "$SCRIPT_DIR/../lib/bash/lib_args.sh"
source "$SCRIPT_DIR/../lib/bash/lib_paths.sh"

# Default values
CONFIG="develop"
CONFIG_FILE=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        develop|deploy|ci)
            CONFIG="$1"
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

# Use get_default_config_file from lib_config.sh
if [[ -z "$CONFIG_FILE" ]]; then
    CONFIG_FILE="$(get_default_config_file "$CONFIG")"
fi

# Resolve relative path
if [[ "$CONFIG_FILE" != /* ]] && [[ "$CONFIG_FILE" != ~* ]]; then
    CONFIG_FILE="$SCRIPT_DIR/$CONFIG_FILE"
fi

log_separator "="
log_info "Starting Linux CMake Configuration"
log_info "Configuration: $CONFIG"
log_separator "="

# Use PROJECT_ROOT from lib_paths.sh
log_info "Project root: $PROJECT_ROOT"
log_info "Changing to project directory"

cd "$PROJECT_ROOT"

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
GENERATOR="$config_cmake_generator"
TOOLCHAIN="$config_cmake_toolchain"
BUILD_TYPE="$config_cmake_build_type"

if [[ -z "$BUILD_TYPE" ]]; then
    log_error "build_type not specified in config file"
    exit 1
fi

# Validate BuildType value
if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "RelWithDebInfo" ]]; then
    log_error "Invalid build_type '$BUILD_TYPE'. Must be one of: Debug, Release, RelWithDebInfo"
    exit 1
fi

SOURCE_DIR="$config_paths_source"
BUILD_DIR="$config_paths_build_dir"

# Safety check: BUILD_DIR must not be empty
if [[ -z "$BUILD_DIR" ]]; then
    log_error "Configuration error: build_dir is not set in config file"
    log_error "Please check the [paths] section in: $CONFIG_FILE"
    exit 1
fi

# Resolve source directory: if relative, make it relative to project root
if [[ "$SOURCE_DIR" = /* ]]; then
    RESOLVED_SOURCE_DIR="$SOURCE_DIR"
else
    RESOLVED_SOURCE_DIR="$(cd "$PROJECT_ROOT/$SOURCE_DIR" 2>/dev/null && pwd)" || "$PROJECT_ROOT/$SOURCE_DIR"
fi

log_info "Generator: $GENERATOR"
log_info "Toolchain: $TOOLCHAIN"
log_info "Build Type: $BUILD_TYPE"
log_info "Source directory: $SOURCE_DIR (resolved: $RESOLVED_SOURCE_DIR)"
log_info "Build directory: $BUILD_DIR"

# Configure with CMake
log_separator "="
log_info "Configuring with CMake (NO BUILD)"
log_info "Command: cmake -G $GENERATOR -DUSE_TOOLCHAIN=$TOOLCHAIN -DCMAKE_BUILD_TYPE=$BUILD_TYPE -S $RESOLVED_SOURCE_DIR -B $BUILD_DIR"
log_separator "="

# Performance diagnostic: Print system info
log_info "=== Performance Diagnostics ==="
log_info "CMake Version: $(cmake --version | head -1)"
log_info "Generator: $GENERATOR"

# Run CMake
log_info ""
log_info "Running CMake configuration..."

if cmake -G "$GENERATOR" -DUSE_TOOLCHAIN="$TOOLCHAIN" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -S "$RESOLVED_SOURCE_DIR" -B "$BUILD_DIR"; then
    log_separator "="
    log_success "CMake configuration completed successfully!"
    log_info "To build the project, run: cmake --build $BUILD_DIR"
    log_separator "="
else
    exit_code=$?
    log_error "CMake configuration failed with exit code: $exit_code"
    exit $exit_code
fi
