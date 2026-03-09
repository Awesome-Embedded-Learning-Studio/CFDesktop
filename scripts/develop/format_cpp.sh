#!/usr/bin/env bash
##
## @file format_cpp.sh
## @brief Format all C++ files in the project using clang-format
## @date 2026-03-01
##
## Usage:
##   ./scripts/develop/format_cpp.sh [OPTIONS]
##
## Options:
##   -n, --dry-run    Show what would be changed without modifying files
##   -c, --check      Return exit code 1 if any files need formatting
##   -h, --help       Show this help message
##

set -eo pipefail

# Colors for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[0;33m'
readonly CYAN='\033[0;36m'
readonly GRAY='\033[0;90m'
readonly NC='\033[0m' # No Color

# Print colored output
log_info()    { printf "${GRAY}%s${NC}\n" "$*"; }
log_success() { printf "${GREEN}%s${NC}\n" "$*"; }
log_warn()    { printf "${YELLOW}%s${NC}\n" "$*"; }
log_error()   { printf "${RED}%s${NC}\n" "$*"; }
log_cyan()    { printf "${CYAN}%s${NC}\n" "$*"; }

# Check if clang-format exists
check_clang_format() {
    if ! command -v clang-format >/dev/null 2>&1; then
        log_error "ERROR: clang-format not found!"
        log_warn "Please install clang-format:"
        log_info "  - Ubuntu/Debian: sudo apt install clang-format"
        log_info "  - Fedora/RHEL:   sudo dnf install clang-format"
        log_info "  - Arch:          sudo pacman -S clang"
        log_info "  - macOS:         brew install clang-format"
        return 1
    fi

    local version
    version=$(clang-format --version 2>/dev/null)
    log_success "Found: $version"
    return 0
}

# Show help
show_help() {
    grep '^##' "$0" | sed 's/^## \?//g' | sed '/^$/d'
    exit 0
}

# Get script directory
get_script_dir() {
    local source="${BASH_SOURCE[0]}"
    while [ -h "$source" ]; do
        local dir
        dir="$(cd -P "$(dirname "$source")" && pwd)"
        source="$(readlink "$source")"
        [[ $source != /* ]] && source="$dir/$source"
    done
    cd -P "$(dirname "$source")/../.." && pwd
}

# Main
main() {
    local dry_run=false
    local check_mode=false
    local root_dir
    root_dir="$(get_script_dir)"

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -n|--dry-run)
                dry_run=true
                shift
                ;;
            -c|--check)
                check_mode=true
                shift
                ;;
            -h|--help)
                show_help
                ;;
            *)
                log_error "Unknown option: $1"
                echo ""
                show_help
                ;;
        esac
    done

    echo ""
    log_cyan "=== C++ Code Formatter ==="
    log_info "Project: CFDesktop"
    echo ""

    # Check clang-format
    if ! check_clang_format; then
        exit 1
    fi

    # Count and process files
    local processed=0
    local changed=0

    # Use find with -print0 and read for safer file handling
    while IFS= read -r -d '' file; do
        [ -z "$file" ] && continue
        ((processed++))

        # Get relative path
        local rel_path="${file#$root_dir/}"
        rel_path="${rel_path#/}"

        if [[ "$dry_run" == "true" ]]; then
            # Check if file would be changed
            if ! clang-format "$file" >/dev/null 2>&1; then
                log_warn "[$processed] Skipped (binary/error): $rel_path"
                continue
            fi
            if diff -q "$file" <(clang-format "$file") >/dev/null 2>&1; then
                log_info "[$processed] OK: $rel_path"
            else
                log_warn "[$processed] Would reformat: $rel_path"
                ((changed++))
            fi
        elif [[ "$check_mode" == "true" ]]; then
            if ! clang-format "$file" >/dev/null 2>&1; then
                log_warn "[$processed] Skipped (binary/error): $rel_path"
                continue
            fi
            if diff -q "$file" <(clang-format "$file") >/dev/null 2>&1; then
                log_info "[$processed] OK: $rel_path"
            else
                log_error "[$processed] Needs formatting: $rel_path"
                ((changed++))
            fi
        else
            # Format in-place
            log_info "[$processed] Formatting: $rel_path"
            if clang-format -i "$file" 2>/dev/null; then
                ((changed++))
            else
                log_warn "[$processed] Skipped (binary/error): $rel_path"
            fi
        fi
    done < <(find "$root_dir" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.cc" -o -name "*.cxx" \) \
        ! -path "*/third_party/*" \
        ! -path "*/build/*" \
        ! -path "*/out/*" \
        ! -path "*/.git/*" \
        -print0 | sort -z)

    echo ""
    log_cyan "=== Summary ==="
    log_info "Processed: $processed files"

    if [[ "$dry_run" == "true" ]]; then
        log_warn "Would reformat: $changed files"
    elif [[ "$check_mode" == "true" ]]; then
        if [[ $changed -gt 0 ]]; then
            log_error "Files needing formatting: $changed"
            log_warn "Run without --check to format"
            exit 1
        else
            log_success "All files formatted correctly!"
        fi
    else
        log_success "Formatted: $changed files"
    fi

    exit 0
}

main "$@"
