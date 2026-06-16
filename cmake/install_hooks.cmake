# cmake/install_hooks.cmake
# =============================================================================
# Configure-time: point `git core.hooksPath` at the version-controlled hooks
# directory so pre-commit / pre-push take effect right after `cmake configure`.
# No manual `install_hooks.sh` needed — the hooks live in the repo and git runs
# them directly. Idempotent and non-fatal (skips if not a git repo).
# =============================================================================
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git config core.hooksPath scripts/release/hooks
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _cf_hooks_result
        OUTPUT_QUIET ERROR_QUIET
    )
    if(_cf_hooks_result EQUAL 0)
        message(STATUS "Git hooks: core.hooksPath -> scripts/release/hooks (auto-configured)")
    else()
        message(STATUS "Git hooks: skipped (git config unavailable; non-fatal)")
    endif()
endif()
