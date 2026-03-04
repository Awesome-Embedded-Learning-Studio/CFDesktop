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
project(CFDesktop VERSION 0.0.1 LANGUAGES CXX)

# ------ Build Type Configuration ------
# Validate and set build type
if(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE is not set. Please specify build_type in build_*.config.ini")
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

# Set compiler flags for each build type (use CACHE FORCE to override CMake defaults)
# Debug: No optimization (-O0), full debug info (-g)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Flags used by the C++ compiler during Debug builds" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE STRING "Linker flags used during Debug builds" FORCE)

# Release: Maximum optimization (-O3), no debug info
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags used by the C++ compiler during Release builds" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s" CACHE STRING "Linker flags used during Release builds" FORCE)

# RelWithDebInfo: Optimization (-O2) + full debug info
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the C++ compiler during RelWithDebInfo builds" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "Linker flags used during RelWithDebInfo builds" FORCE)

string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type_upper)

# Log build type and compiler flags (use variable indirection for nested expansion)
set(_cxx_flags_var   "CMAKE_CXX_FLAGS_${_build_type_upper}")
set(_link_flags_var  "CMAKE_EXE_LINKER_FLAGS_${_build_type_upper}")

log_info("BuildSettings" "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
log_info("BuildSettings" "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
log_info("BuildSettings" "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}: ${${_cxx_flags_var}}")
log_info("BuildSettings" "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
log_info("BuildSettings" "CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE}: ${${_link_flags_var}}")

# ------ Build Settings ------
# Set output directories for all targets to ensure DLLs and executables are together
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Disable response files in compile_commands.json for better clangd compatibility
set(CMAKE_CXX_RESPONSE_FILE_FLAG "@")
set(CMAKE_CXX_RESPONSE_FILE_LINK_FLAG "@")
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 0)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 0)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 0)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ------ Build Settings ------

log_info("BuildSettings" "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
log_info("BuildSettings" "CMAKE_CXX_STANDARD_REQUIRED: ${CMAKE_CXX_STANDARD_REQUIRED}")
log_info("BuildSettings" "CMAKE_AUTOMOC: ${CMAKE_AUTOMOC}")
log_info("BuildSettings" "CMAKE_AUTORCC: ${CMAKE_AUTORCC}")
log_info("BuildSettings" "CMAKE_AUTOUIC: ${CMAKE_AUTOUIC}")
log_info("BuildSettings" "CMAKE_EXPORT_COMPILE_COMMANDS: ${CMAKE_EXPORT_COMPILE_COMMANDS}")

# find Qt6, As Requested
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
log_info("Post Qt Setups" "Run Post Qt Dependencies check")
include(cmake/custom_target_helper.cmake)
log_info("Post Qt Setups" "Run Post Qt Dependencies OK")

# ============================================================
# Output Directory Configuration
# ============================================================
# 配置分类输出目录：bin/, examples/, plugins/, runtimes/
log_info("OutputConfig" "Configuring output directories")
include(cmake/OutputDirectoryConfig.cmake)

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

# Include development helpers generation
include(cmake/generate_develop_helpers.cmake)

log_info("DevHelpers" "Will Generate VSCode clangd configuration")
generate_vscode_clangd()

log_info("DevHelpers" "Will Generate VSCode debug configuration")
generate_vscode_debug_config()

# Print build configuration
log_config_summary()
log_qt_dir()

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

log_info("Final" "Configuration Ends! You can try to build now!")