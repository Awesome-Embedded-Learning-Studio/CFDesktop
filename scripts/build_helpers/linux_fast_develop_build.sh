#!/bin/bash
# This script configures and builds the project (FAST version - no cleaning)
# It calls the configure script first, then builds
set -e

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
log "Starting Linux FAST Build Process" "INFO"
log "========================================" "INFO"

# Get the script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

log "Project root: $PROJECT_ROOT" "INFO"
cd "$PROJECT_ROOT"

# Step 1: Call the configure script
log "========================================" "INFO"
log "Step 1: Configuring with CMake" "INFO"
log "========================================" "INFO"

CONFIGURE_SCRIPT="$SCRIPT_DIR/linux_configure.sh"
log "Executing: $CONFIGURE_SCRIPT develop" "INFO"

if bash "$CONFIGURE_SCRIPT" develop; then
    log "Configuration completed successfully!" "SUCCESS"
else
    exit_code=$?
    log "Configure script failed with exit code: $exit_code" "ERROR"
    exit $exit_code
fi

# Step 2: Load config for build
CONFIG_FILE="$SCRIPT_DIR/build_develop_config.ini"
eval "$(get_ini_config "$CONFIG_FILE")"
BUILD_DIR="$config_paths_build_dir"
JOBS="${config_options_jobs:-}"

# Step 3: Build with CMake
log "========================================" "INFO"
log "Step 2: Building project" "INFO"

BUILD_CMD="cmake --build \"$BUILD_DIR\""
if [[ -n "$JOBS" ]]; then
    BUILD_CMD="$BUILD_CMD --parallel \"$JOBS\""
    log "Command: cmake --build $BUILD_DIR --parallel $JOBS" "INFO"
else
    log "Command: cmake --build $BUILD_DIR" "INFO"
fi
log "========================================" "INFO"

if eval "$BUILD_CMD"; then
    log "========================================" "INFO"
    log "Build completed successfully!" "SUCCESS"
    log "========================================" "INFO"
else
    exit_code=$?
    log "Build failed with exit code: $exit_code" "ERROR"
    exit $exit_code
fi
