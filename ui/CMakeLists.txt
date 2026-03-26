project(UI DESCRIPTION "CFDesktop UI Components" LANGUAGES CXX)

log_info("UI" "Start UI Components Configurations")

# Base Supports the UI Components Needs
add_subdirectory(base)

# Core Provides the UI Components Supports
add_subdirectory(core)

# Components Provides animation and UI component implementations
add_subdirectory(components)

# Widget Supports the Middle Level Widgets Abstractions
add_subdirectory(widget)

# Models will provide SDK Like Models for programmers
# Quick Start Apps
add_subdirectory(models)

# ============================================================
# Unified UI Library - Static Library
# ============================================================

add_library(cfui STATIC)

# Define that we are building a static library (disables dllimport/dllexport)
target_compile_definitions(cfui PUBLIC CFUI_STATIC_BUILD)

# Link against STATIC libraries from sub-modules
# Order: dependencies first (left), then dependents (right)
# Static libraries are linked left-to-right, so dependents must come after their dependencies
target_link_libraries(cfui PUBLIC
    cf_ui_base
    cf_ui_core
    cf_ui_widget
    cf_ui_components_material
    cf_ui_components
)

# Set include directories
target_include_directories(cfui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
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
