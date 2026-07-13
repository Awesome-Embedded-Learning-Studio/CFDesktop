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
    VERSION 1.0.1
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

# Standalone desktop apps now live in the separate `CFDeskit` repo
# (https://github.com/Awesome-Embedded-Learning-Studio/CFDeskit). The main
# repo no longer compiles apps — they are deployed as prebuilt artifacts into
# <active_root>/apps/<id>/ at install/runtime, where AppDiscoverer scans them.
# See third_party/apps/README.md for the deployment contract.

# ============================================================
# UI layer (Material Design 3 components, platform backends; was desktop/ui/)
# ============================================================
log_module_start("ui")
add_subdirectory(ui)
log_module_end("ui")

# ============================================================
# Main layer (init chain + entry; was desktop/main/)
# ============================================================
log_module_start("main")
add_subdirectory(main)
log_module_end("main")

# ============================================================
# Unified CFDesktop Shared Library — assembles static libs from base/ui/main.
# Moved here from the deleted desktop/CMakeLists.txt.
# ============================================================
log_info("Desktop" "Creating unified CFDesktop shared library")

add_library(CFDesktop_shared SHARED)

if(WIN32)
    target_compile_definitions(CFDesktop_shared PRIVATE CFDESKTOP_EXPORTS)
endif()
if(UNIX AND NOT APPLE)
    target_compile_options(CFDesktop_shared PRIVATE -fvisibility=hidden)
endif()

target_include_directories(CFDesktop_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/main>
)

# Static libs folded into the .so via --whole-archive.
set(CFDESKTOP_STATIC_LIBS
    cffilesystem
    cfconfig
    cfasciiart
    CFDesktopMain
    CFDesktopUi
    cf_desktop_render
    cfipc
    cfcrash
)

# Sources compiled directly into the DLL (with CFDESKTOP_EXPORTS defined).
target_sources(CFDesktop_shared PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main/desktop_run_session.cpp
)

if(MSVC)
    target_link_libraries(CFDesktop_shared PRIVATE ${CFDESKTOP_STATIC_LIBS})
    foreach(_cfdesktop_module IN LISTS CFDESKTOP_STATIC_LIBS)
        target_link_options(CFDesktop_shared PRIVATE "/WHOLEARCHIVE:$<TARGET_FILE:${_cfdesktop_module}>")
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE)
    target_link_libraries(CFDesktop_shared PRIVATE
        "-Wl,--whole-archive"
        ${CFDESKTOP_STATIC_LIBS}
        "-Wl,--no-whole-archive"
    )
else()
    target_link_libraries(CFDesktop_shared PRIVATE ${CFDESKTOP_STATIC_LIBS})
endif()

target_link_libraries(CFDesktop_shared PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
# QuarkWidgets::quarkwidgets is PUBLIC so the thin EXE can include the
# QApplication subclass (MaterialApplication) it instantiates in main.cpp.
target_link_libraries(CFDesktop_shared
PRIVATE
    cfbase
    cflogger
PUBLIC
    QuarkWidgets::quarkwidgets
)

if(WIN32)
    target_link_libraries(CFDesktop_shared PUBLIC
        wbemuuid PowrProf pdh psapi
    )
endif()

add_library(CFDesktop::lib ALIAS CFDesktop_shared)

# ============================================================
# Main executable — thin shell that calls into the DLL
# ============================================================
add_executable(CFDesktop main.cpp)
target_link_libraries(CFDesktop PRIVATE CFDesktop_shared cfipc)

log_info("Desktop Buildings" "Configuring Desktop Buildings Done")

log_module_start("example")
add_subdirectory("example")
log_module_end("example")

# Enable CTest at the top level so CTestTestfile.cmake is generated for the
# build root (enable_testing() inside test/CMakeLists.txt only covers that scope).
enable_testing()

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