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

# ------ Build Settings ------
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

# Include development helpers generation
include(cmake/generate_develop_helpers.cmake)

log_info("DevHelpers" "Will Generate VSCode clangd configuration")
generate_vscode_clangd()

# Print build configuration
log_config_summary()
log_qt_dir()
# Log base module start
log_module_start("base")
add_subdirectory(base)
# Log base module end
log_module_end("base")


log_module_start("example")
add_subdirectory("example")
log_module_end("example")

# Log test module start
log_module_start("test")
add_subdirectory(test)
# Log test module end
log_module_end("test")

log_info("Final" "Configuration Ends! You can try to build now!")