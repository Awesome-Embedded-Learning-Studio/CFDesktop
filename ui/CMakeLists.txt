log_info("UI" "Start UI Components Configurations")

# Base Supports the UI Components Needs
add_subdirectory(base)

# Core Provides the UI Components Supports
add_subdirectory(core)

# Widget Supports the Middle Level Widgets Abstractions
add_subdirectory(widget)

# Models will provide SDK Like Models for programmers
# Quick Start Apps
add_subdirectory(models)

# ============================================================
# Unified UI Library - Single DLL: cfui.dll
# ============================================================

add_library(cfui SHARED)

# CMake automatically defines CFUI_EXPORTS when building this SHARED library.
# This is used by export.h to control __declspec(dllexport/dllimport) on Windows.
# No manual compile_definitions needed - this is CMake's standard behavior.

# Link against STATIC libraries from sub-modules
target_link_libraries(cfui PUBLIC
    cf_ui_base
    cf_ui_core
)

# Set include directories
target_include_directories(cfui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link Qt libraries and base library (SHARED cfbase.dll)
target_link_libraries(cfui PUBLIC
    Qt6::Core
    Qt6::Gui
    CFDesktop::base
)

# Add alias for consistent naming
add_library(UI::cfui ALIAS cfui)
# For backward compatibility
add_library(UI::cf_ui ALIAS cfui)
add_library(UI::base ALIAS cfui)

log_info("UI" "Start UI Components Configurations Done")
