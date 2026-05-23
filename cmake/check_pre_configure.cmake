# ============================================================
# Pre-configure Main Entry Point
# ============================================================
# This file orchestrates all pre-configuration checks and setup
# that should happen after project() declaration but before
# adding any subdirectories.

# ------ OS Detection ------
include(cmake/env_check/detail_os_checker.cmake)
cf_detect_wsl()
if(IS_WSL)
    add_compile_definitions(CFDESKTOP_OS_WSL)
    message(STATUS "Running in WSL environment")
endif()

# ------ Build Type Configuration ------
include(cmake/build_type_config.cmake)

# ------ Build Settings ------
include(cmake/build_settings.cmake)

# ------ Qt Setup ------
include(cmake/qt_setup.cmake)

# ============================================================
# Output Directory Configuration
# ============================================================
# 配置分类输出目录：bin/, examples/, plugins/, runtimes/
log_info("OutputConfig" "Configuring output directories")
include(cmake/OutputDirectoryConfig.cmake)

# ============================================================
# Desktop Overlay
# ============================================================
# 将 cmake/desktop_overlay/ 中的预配置文件拷贝到构建输出目录
log_info("DesktopOverlay" "Checking for overlay files")
include(cmake/desktop_overlay.cmake)
cf_apply_desktop_overlay()

# ============================================================
# Example Launcher Generator (Windows)
# ============================================================
# 为每个 example 生成 Windows 启动脚本
if(WIN32)
    log_info("Launcher" "Including Windows launcher generator")
    include(cmake/ExampleLauncher.cmake)
endif()

# ============================================================
# Qt Shared Deployment (Windows)
# ============================================================
# 共享 Qt DLLs 部署到 runtimes/ 目录
if(WIN32)
    log_info("QtDeploy" "Including shared Qt deployment utilities")
    include(cmake/QtDeployUtils.cmake)
endif()

# ============================================================
# Development Helpers
# ============================================================
# Include development helpers generation
include(cmake/generate_develop_helpers.cmake)

# Skip VSCode config generation in CI environment (docker builds)
if(NOT USE_TOOLCHAIN MATCHES "^linux/ci")
    log_info("DevHelpers" "Will Generate VSCode clangd configuration")
    generate_vscode_clangd()

    log_info("DevHelpers" "Will Generate VSCode debug configuration")
    generate_vscode_debug_config()
else()
    log_info("DevHelpers" "Skipping VSCode config generation in CI mode")
endif()
