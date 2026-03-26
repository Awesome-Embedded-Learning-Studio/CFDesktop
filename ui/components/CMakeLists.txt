project(ui_components LANGUAGES CXX)

log_info("UI_Components" "Start Configure the UI Components")

# ============================================================
# Base Animation Library - STATIC library for cfui.dll
# Contains base animation interfaces and implementations
# ============================================================

add_library(cf_ui_components STATIC
    animation.cpp
    animation_factory_manager.cpp
    timing_animation.cpp
)

# Define CFUI_STATIC_BUILD so symbols are NOT marked as dllexport when compiling for static library
target_compile_definitions(cf_ui_components PUBLIC CFUI_STATIC_BUILD)

# Add include directories
target_include_directories(cf_ui_components PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<INSTALL_INTERFACE:include>
)

# Link Qt libraries
target_link_libraries(cf_ui_components PUBLIC
    Qt6::Core
    Qt6::Gui
    CFDesktop::base_headers
    cf_ui_base
)

# Include subdirectory CMakeLists (STATIC libraries)
# Material depends on cf_ui_components, so it must come after
add_subdirectory(material)

log_info("UI_Components" "the UI Components Configuration Done")
