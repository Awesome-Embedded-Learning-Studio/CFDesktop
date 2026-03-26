project(ui_widget LANGUAGES CXX)

log_info("UI_Widget" "Start Configure the UI Widget")


add_library(cf_ui_application_support STATIC
    application_support/application.cpp
)

target_compile_definitions(cf_ui_application_support PUBLIC CFUI_STATIC_BUILD)

target_include_directories(cf_ui_application_support PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<INSTALL_INTERFACE:include>
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
    material/widget/spinbox/spinbox.cpp
    material/widget/doublespinbox/doublespinbox.cpp
    material/widget/listview/listview.cpp
    material/widget/scrollview/scrollview.cpp
    material/widget/separator/separator.cpp
    material/widget/tableview/tableview.cpp
    material/widget/tabview/tabview.cpp
    material/widget/tabview/private/materialtabbar.cpp
    material/widget/treeview/treeview.cpp
    material/widget/treeview/private/treeviewdelegate.cpp
)

# Define CFUI_STATIC_BUILD so symbols are NOT marked as dllexport when compiling for static library
target_compile_definitions(cf_ui_widget_material PUBLIC CFUI_STATIC_BUILD)

# Add include directories
target_include_directories(cf_ui_widget_material PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<INSTALL_INTERFACE:include>
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cf_ui_widget INTERFACE
    cf_ui_widget_material
    cf_ui_application_support
)

log_info("UI_Widget" "the UI Widget Configuration Done")
