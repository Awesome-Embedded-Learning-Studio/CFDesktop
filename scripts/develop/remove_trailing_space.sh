#!/usr/bin/env bash
##
## @file remove_trailing_space.sh
## @brief Remove trailing whitespace from all text files in the project
## @date 2026-03-07
##
## Usage:
##   ./scripts/develop/remove_trailing_space.sh [OPTIONS]
##
## Options:
##   -n, --dry-run      Show what would be changed without modifying files
##   -c, --check        Return exit code 1 if any files have trailing whitespace
##   -s, --staged       Only check staged files (auto-fix them)
##   -S, --staged-check Check staged files without modifying (for pre-commit hook)
##   -h, --help         Show this help message
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

# Show help
show_help() {
    grep '^##' "$0" | sed 's/^## \?//g' | sed '/^$/d'
    exit 0
}

# Get script directory / project root
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
    local staged_only=false
    local staged_check=false
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
            -s|--staged)
                staged_only=true
                shift
                ;;
            -S|--staged-check)
                staged_check=true
                staged_only=true
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
    log_cyan "=== Remove Trailing Whitespace ==="
    log_info "Project: $root_dir"
    if [[ "$staged_only" == "true" ]]; then
        if [[ "$staged_check" == "true" ]]; then
            log_info "Mode: Check staged files (pre-commit)"
        else
            log_info "Mode: Staged files only"
        fi
    fi
    echo ""

    local processed=0
    local changed=0
    local tab_char=$'\t'

    # Build file list based on mode
    if [[ "$staged_only" == "true" ]]; then
        # Get staged files from git - store in array first
        local staged_files=()
        while IFS= read -r rel_file; do
            [ -z "$rel_file" ] && continue
            staged_files+=("$rel_file")
        done < <(git diff --cached --name-only --diff-filter=ACM 2>/dev/null || true)

        # Process each staged file
        for rel_file in "${staged_files[@]}"; do
            local file="${root_dir}/${rel_file}"
            [ ! -f "$file" ] && continue

            ((processed++))

            # Get relative path
            local rel_path="${file#$root_dir/}"
            rel_path="${rel_path#/}"

            # Check if file has trailing whitespace (space or tab at end of line)
            if grep -q "[ ${tab_char}]\$" "$file" 2>/dev/null; then
                if [[ "$dry_run" == "true" ]] || [[ "$check_mode" == "true" ]]; then
                    log_warn "$rel_path:"
                    grep -n "[ ${tab_char}]\$" "$file" 2>/dev/null | head -5 | while IFS= read -r line; do
                        echo "  $line"
                    done
                    ((changed++))
                else
                    # Remove trailing whitespace in-place
                    sed -i "s/[ ${tab_char}]*\$//" "$file"
                    log_info "Fixed: $rel_path"
                    ((changed++))
                fi
            fi
        done
    else
        # Use git ls-files to get tracked files (respects .gitignore)
        git ls-files | while IFS= read -r rel_path; do
            [ -z "$rel_path" ] && continue
            local file="${root_dir}/${rel_path}"

            ((processed++))

            # Check if file has trailing whitespace (space or tab at end of line)
            if grep -q "[ ${tab_char}]\$" "$file" 2>/dev/null; then
                if [[ "$dry_run" == "true" ]]; then
                    log_warn "$rel_path:"
                    grep -n "[ ${tab_char}]\$" "$file" | head -5 | while IFS= read -r line; do
                        echo "  $line"
                    done
                    ((changed++))
                elif [[ "$check_mode" == "true" ]]; then
                    log_error "$rel_path"
                    ((changed++))
                else
                    # Remove trailing whitespace in-place
                    sed -i "s/[ ${tab_char}]*\$//" "$file"
                    log_info "Fixed: $rel_path"
                    ((changed++))
                fi
            fi
        done || true
    fi

    echo ""
    log_cyan "=== Summary ==="
    log_info "Processed: $processed files"

    if [[ "$dry_run" == "true" ]] || [[ "$check_mode" == "true" ]]; then
        if [[ $changed -gt 0 ]]; then
            log_warn "Files with trailing whitespace: $changed"
            exit 1
        else
            log_success "No trailing whitespace found!"
        fi
    else
        log_success "Fixed: $changed files"
    fi

    echo ""
    exit 0
}

main "$@"
