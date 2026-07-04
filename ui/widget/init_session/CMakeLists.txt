# Init session widget library - Boot progress UI components
# These widgets are used during application initialization

set(INIT_SESSION_WIDGET_SOURCES
    boot_progress_widget.cpp
    boot_widget_factory.cpp
    simple_boot_widget.cpp
    step_dot_widget.cpp
)

set(INIT_SESSION_WIDGET_HEADERS
    boot_progress_widget.h
    simple_boot_widget.h
    step_dot_widget.h
)

# Create static library for init session widgets
add_library(cf_desktop_ui_widget_init_session STATIC
    ${INIT_SESSION_WIDGET_SOURCES}
    ${INIT_SESSION_WIDGET_HEADERS}
    simple_boot_widget.ui
    resources/resources.qrc
)

target_include_directories(cf_desktop_ui_widget_init_session PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    # For boot_stage_info.h, init_stage.h from main/init/
    # Note: Cannot link CFDesktopMain (circular dep: CFDesktopMain links this target)
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/main/init>
)

target_link_libraries(cf_desktop_ui_widget_init_session PUBLIC
    cfbase
    cflogger
    cf_desktop_ui_base
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
