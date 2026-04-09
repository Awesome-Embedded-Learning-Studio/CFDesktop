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
    VERSION 0.16.0
    DESCRIPTION "CFDesktop: Given Your Device Portable Desktop Anywhere"
    HOMEPAGE_URL "https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop"
    LANGUAGES CXX C
)

# ============================================================
# Pre-configure Checks and Setup
# ============================================================
include(cmake/check_pre_configure.cmake)

# ============================================================
# Generate Meta Information Headers
# ============================================================
include(cmake/generate_meta_info.cmake)


# Generate meta info headers after Qt6 is found
cf_generate_meta_info()

# Log base module start
log_module_start("base")
add_subdirectory(base)
# Log base module end
log_module_end("base")

# Log base module start
log_module_start("ui")
add_subdirectory(ui)
# Log base module end
log_module_end("ui")

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