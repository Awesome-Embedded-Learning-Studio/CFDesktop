cmake_minimum_required(VERSION 3.16)
# Include build log helper
message("Including Requested CMake Dependencies...")
include(cmake/build_log_helper.cmake)
message("Logger OK")
message("Including Requested CMake Dependencies OK")

# Toolchain configuration (must be before project() call)
# Supports both:
#   cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake -S . -B out/build
#   cmake -DUSE_TOOLCHAIN=windows/llvm -S . -B out/build
include(cmake/check_toolchain.cmake)

project(CFDesktop
    VERSION 0.19.0
    DESCRIPTION "CFDesktop: Given Your Device Portable Desktop Anywhere"
    HOMEPAGE_URL "https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop"
    LANGUAGES CXX C
)

# ============================================================
# Pre-configure Checks and Setup
# ============================================================
include(cmake/check_pre_configure.cmake)

# Configure git hooks (core.hooksPath) so pre-commit/pre-push work after configure
include(cmake/install_hooks.cmake)

# ============================================================
# Generate Meta Information Headers
# ============================================================
include(cmake/generate_meta_info.cmake)


# Generate meta info headers after Qt6 is found
cf_generate_meta_info()

# Generate desktop_settings.h
include(cmake/generate_desktop_settings.cmake)
cf_generate_desktop_settings()

# Apply desktop root overlay (needs CFDESKTOP_DEFAULT_ROOT from above)
cf_apply_desktop_root_overlay()

# ============================================================
# Third-party: aex (foundation utils, header-only). MUST precede base —
# the slimmed cfbase links aex::aex for expected/scope_guard/macro.
# ============================================================
include(cmake/third_party_helper.cmake)
setup_third_party(
    NAME           aex
    GIT_REPOSITORY https://github.com/Awesome-Embedded-Learning-Studio/aex.git
    GIT_TAG        v0.1.0)

# Log base module start
log_module_start("base")
add_subdirectory(base)
# Log base module end
log_module_end("base")

# NOTE: ui/ was extracted to the QuarkWidgets submodule (Phase 2). It is consumed
# below via setup_third_party() before desktop, which links QuarkWidgets::quarkwidgets.
setup_third_party(
    NAME           QuarkWidgets
    GIT_REPOSITORY https://github.com/Awesome-Embedded-Learning-Studio/QuarkWidgets.git
    GIT_TAG        v0.1.0)

log_module_start("desktop")
add_subdirectory(desktop)
# Log base module end
log_module_end("desktop")

log_module_start("example")
add_subdirectory("example")
log_module_end("example")

# Log test module start
log_module_start("test")
add_subdirectory(test)
# Log test module end
log_module_end("test")

# ============================================================
# Post-Build Setup (必须在所有 add_subdirectory 之后)
# ============================================================

# 生成 Windows 启动脚本 (如果使用 ExampleLauncher)
if(WIN32)
    log_info("Launcher" "Generating Windows launcher scripts")
    cf_generate_all_launchers()
endif()

# 设置共享 Qt 部署 (Windows)
if(WIN32)
    log_info("QtDeploy" "Setting up shared Qt deployment")
    cf_setup_shared_qt_deployment()
endif()

# ============================================================
# Final Configuration Summary
# ============================================================
log_build_summary_table()

log_info("Final" "Configuration Ends! You can try to build now!")