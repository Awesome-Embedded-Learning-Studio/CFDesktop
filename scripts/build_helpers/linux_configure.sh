#!/bin/bash
# This script ONLY configures the project using CMake
# It does NOT build the project
# Usage: ./linux_configure.sh [develop|deploy]

set -e

# Default to develop if no argument provided
CONFIG="${1:-develop}"

# Validate config argument
if [[ "$CONFIG" != "develop" && "$CONFIG" != "deploy" ]]; then
    echo "ERROR: Invalid configuration '$CONFIG'. Use 'develop' or 'deploy'." >&2
    exit 1
fi

# Function to read INI configuration file
get_ini_config() {
    local filepath="$1"
    local current_section=""
    local config=""

    if [[ ! -f "$filepath" ]]; then
        echo "ERROR: Configuration file not found: $filepath" >&2
        exit 1
    fi

    while IFS= read -r line || [[ -n "$line" ]]; do
        # Trim whitespace
        line=$(echo "$line" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

        # Skip empty lines and comments
        if [[ -z "$line" ]] || [[ "$line" =~ ^[#\;] ]]; then
            continue
        fi

        # Section header
        if [[ "$line" =~ ^\[([^\]]+)\]$ ]]; then
            current_section="${BASH_REMATCH[1]}"
            continue
        fi

        # Key=value pair
        if [[ "$line" =~ ^([^=]+)=(.*)$ ]]; then
            local key=$(echo "${BASH_REMATCH[1]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            local value=$(echo "${BASH_REMATCH[2]}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

            if [[ -n "$current_section" ]]; then
                config+="config_${current_section}_${key}=\"$value\""$'\n'
            fi
        fi
    done < "$filepath"

    echo "$config"
}

# Log function
log() {
    local level="$2"
    local message="$1"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    local color=""

    case "$level" in
        INFO)
            color="\033[0;36m"    # Cyan
            ;;
        SUCCESS)
            color="\033[0;32m"    # Green
            ;;
        WARNING)
            color="\033[0;33m"    # Yellow
            ;;
        ERROR)
            color="\033[0;31m"    # Red
            ;;
        *)
            color="\033[0m"       # White/Default
            ;;
    esac

    echo -e "${color}[$timestamp] [$level] $message\033[0m"
}

log "========================================" "INFO"
log "Starting Linux CMake Configuration" "INFO"
log "Configuration: $CONFIG" "INFO"
log "========================================" "INFO"

# Get the script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

log "Project root: $PROJECT_ROOT" "INFO"
log "Changing to project directory" "INFO"

cd "$PROJECT_ROOT"

# Determine which config file to use
if [[ "$CONFIG" == "deploy" ]]; then
    CONFIG_FILE="$SCRIPT_DIR/build_deploy_config.ini"
else
    CONFIG_FILE="$SCRIPT_DIR/build_develop_config.ini"
fi

log "Loading configuration from: $CONFIG_FILE" "INFO"

# Parse and evaluate configuration
eval "$(get_ini_config "$CONFIG_FILE")"
log "Configuration loaded successfully!" "SUCCESS"

# Extract configuration values
GENERATOR="$config_cmake_generator"
TOOLCHAIN="$config_cmake_toolchain"
SOURCE_DIR="$config_paths_source"
BUILD_DIR="$config_paths_build_dir"

# Resolve source directory: if relative, make it relative to project root
if [[ "$SOURCE_DIR" = /* ]]; then
    RESOLVED_SOURCE_DIR="$SOURCE_DIR"
else
    RESOLVED_SOURCE_DIR="$(cd "$PROJECT_ROOT/$SOURCE_DIR" 2>/dev/null && pwd)" || "$PROJECT_ROOT/$SOURCE_DIR"
fi

log "Generator: $GENERATOR" "INFO"
log "Toolchain: $TOOLCHAIN" "INFO"
log "Source directory: $SOURCE_DIR (resolved: $RESOLVED_SOURCE_DIR)" "INFO"
log "Build directory: $BUILD_DIR" "INFO"

# Configure with CMake
log "========================================" "INFO"
log "Configuring with CMake (NO BUILD)" "INFO"
log "Command: cmake -G $GENERATOR -DUSE_TOOLCHAIN=$TOOLCHAIN -S $RESOLVED_SOURCE_DIR -B $BUILD_DIR" "INFO"
log "========================================" "INFO"

if cmake -G "$GENERATOR" -DUSE_TOOLCHAIN="$TOOLCHAIN" -S "$RESOLVED_SOURCE_DIR" -B "$BUILD_DIR"; then
    log "========================================" "INFO"
    log "CMake configuration completed successfully!" "SUCCESS"
    log "To build the project, run: cmake --build $BUILD_DIR" "INFO"
    log "========================================" "INFO"
else
    exit_code=$?
    log "CMake configuration failed with exit code: $exit_code" "ERROR"
    exit $exit_code
fi
