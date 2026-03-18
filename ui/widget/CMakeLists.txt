log_info("UI_Widget" "Start Configure the UI Widget")


add_library(cf_ui_application_support STATIC
    application_support/application.cpp
)

target_compile_definitions(cf_ui_application_support PUBLIC CFUI_EXPORTS)

target_include_directories(cf_ui_application_support PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(cf_ui_application_support PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    CFDesktop::base_headers
    cf_ui_base
    cf_ui_components
    cf_ui_components_material
    cf_ui_core
)

add_library(cf_ui_widget_material STATIC
    material/application/material_application.cpp
    material/base/state_machine.cpp
    material/base/ripple_helper.cpp
    material/base/elevation_controller.cpp
    material/base/focus_ring.cpp
    material/base/painter_layer.cpp
    material/widget/button/button.cpp
    material/widget/checkbox/checkbox.cpp
    material/widget/comboBox/combobox.cpp
    material/widget/groupbox/groupbox.cpp
    material/widget/label/label.cpp
    material/widget/progressbar/progressbar.cpp
    material/widget/radiobutton/radiobutton.cpp
    material/widget/slider/slider.cpp
    material/widget/switch/switch.cpp
    material/widget/textarea/textarea.cpp
    material/widget/textfield/textfield.cpp
)

# Define CFUI_EXPORTS so symbols are marked as dllexport when compiling for the DLL
target_compile_definitions(cf_ui_widget_material PUBLIC CFUI_EXPORTS)

# Add include directories
target_include_directories(cf_ui_widget_material PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link Qt libraries
target_link_libraries(cf_ui_widget_material PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    CFDesktop::base_headers
    cf_ui_base
    cf_ui_components
    cf_ui_components_material
    cf_ui_core
    cf_ui_application_support
)

# ============================================================
# Unified UI Widget Library - INTERFACE library
# ============================================================

add_library(cf_ui_widget INTERFACE)

target_include_directories(cf_ui_widget INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(cf_ui_widget INTERFACE
    cf_ui_widget_material
    cf_ui_application_support
)

log_info("UI_Widget" "the UI Widget Configuration Done")
