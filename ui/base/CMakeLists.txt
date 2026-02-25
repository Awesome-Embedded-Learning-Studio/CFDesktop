log_info("UI_Base" "Start Configure the UI Base")

# STATIC library for linking into cfui.dll
add_library(cf_ui_base STATIC
    math_helper.cpp
    color_helper.cpp
    color.cpp
    device_pixel.cpp
    geometry_helper.cpp
    easing.cpp
)

# Define CFUI_EXPORTS so symbols are marked as dllexport when compiling for the DLL
target_compile_definitions(cf_ui_base PUBLIC CFUI_EXPORTS)

# Add include directories so headers can be found with ui/base/ prefix
# We need to add the ui parent directory so that ui/base/math_helper.h works
target_include_directories(cf_ui_base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link Qt for compile dependencies (headers, etc.)
target_link_libraries(cf_ui_base PUBLIC Qt6::Core Qt6::Gui)

log_info("UI_Base" "the UI Base Configuration Done")
