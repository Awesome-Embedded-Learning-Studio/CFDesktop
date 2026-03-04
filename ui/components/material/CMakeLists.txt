log_info("UI_Components_Material" "Start Configure the UI Components Material")

# STATIC library for linking into cfui.dll
add_library(cf_ui_components_material STATIC
    cfmaterial_fade_animation.cpp
    cfmaterial_slide_animation.cpp
    cfmaterial_scale_animation.cpp
    cfmaterial_property_animation.cpp
    cfmaterial_animation_factory.cpp
    cfmaterial_animation_strategy.cpp
)

# Define CFUI_EXPORTS so symbols are marked as dllexport when compiling for the DLL
target_compile_definitions(cf_ui_components_material PUBLIC CFUI_EXPORTS)

# Add include directories
target_include_directories(cf_ui_components_material PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link Qt for compile dependencies
target_link_libraries(cf_ui_components_material PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    CFDesktop::base_headers
    cf_ui_base
    cf_ui_components
)

log_info("UI_Components_Material" "the UI Components Material Configuration Done")
