project(UI DESCRIPTION "CFDesktop UI Components" LANGUAGES CXX)

log_info("UI" "Start UI Components Configurations")

# All sources under ui/ are compiled into cfui.dll, so define cfui_EXPORTS
# so that CF_UI_EXPORT expands to __declspec(dllexport) instead of dllimport.
add_compile_definitions(cfui_EXPORTS)

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

add_library(cfui SHARED)

# Force all objects into cfui.dll (PRIVATE: only affects cfui's own link step).
# cfui has no own sources, so the linker would otherwise skip static lib objects.
# Note: cf_ui_widget is an INTERFACE library — use its actual static lib deps instead.
target_link_libraries(cfui PRIVATE
    -Wl,--whole-archive
    cf_ui_base
    cf_ui_core
    cf_ui_core_material
    cf_ui_widget_material
    cf_ui_application_support
    cf_ui_components_material
    cf_ui_components
    -Wl,--no-whole-archive
)

# Set include directories
target_include_directories(cfui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/components>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/components/material>
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
