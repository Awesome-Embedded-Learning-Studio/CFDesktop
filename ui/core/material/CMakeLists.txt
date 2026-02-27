log_info("UI_Core_Material" "Start Configure the UI Core Material")

# STATIC library for linking into cfui.dll
add_library(cf_ui_core_material STATIC
    cfmaterial_scheme.cpp
    cfmaterial_fonttype.cpp
    cfmaterial_radius_scale.cpp
    cfmaterial_motion.cpp
    cfmaterial_theme.cpp
    material_factory.cpp
)

# Define CFUI_EXPORTS so symbols are marked as dllexport when compiling for the DLL
target_compile_definitions(cf_ui_core_material PUBLIC CFUI_EXPORTS)

# Add include directories
target_include_directories(cf_ui_core_material PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link Qt for compile dependencies
target_link_libraries(cf_ui_core_material PUBLIC
    Qt6::Core
    Qt6::Gui
    CFDesktop::base_headers
)

log_info("UI_Core_Material" "the UI Core Material Configuration Done")
