# Desktop UI Components
# Core component interfaces: IWindow, IWindowBackend, IDisplayServerBackend,
# PanelManager, ShellLayer, etc.
log_info("Dekstop UI" "Start UI Components Configurations")

add_subdirectory(wallpaper)
add_subdirectory(shell_layer_impl)
add_subdirectory(frosted_backdrop)
add_subdirectory(window_placement)
add_subdirectory(statusbar)
add_subdirectory(taskbar)
add_subdirectory(launcher)
add_subdirectory(desktop_icon_layer)
add_subdirectory(builtin_apps)
add_subdirectory(desktop_widget)
add_subdirectory(home_page)

# Create interface library for convenience
add_library(cf_desktop_components STATIC)

target_sources(cf_desktop_components PRIVATE
    WindowManager.cpp
    PanelManager.cpp
    IWindow.cpp
    IWindowBackend.cpp
    IDisplayServerBackend.cpp
)

target_include_directories(cf_desktop_components PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../render>
)

target_link_libraries(cf_desktop_components
PUBLIC
    cfbase
    cf_desktop_render
PRIVATE
    cfdesktop_shell_layer_impl
    cfdesktop_wallpaper
    cfdesktop_statusbar
    cfdesktop_taskbar
    cfdesktop_launcher
    cfdesktop_desktop_icon_layer
    cfdesktop_builtin_apps
    cfdesktop_window_placement
    cfdesktop_desktop_widget
    cfdesktop_home_page
    Qt6::Core Qt6::Gui Qt6::Widgets
)
