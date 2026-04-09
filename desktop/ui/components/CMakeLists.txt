# Desktop UI Components
# Core component interfaces: IWindow, IWindowBackend, IDisplayServerBackend,
# PanelManager, ShellLayer, etc.
log_info("Dekstop UI" "Start UI Components Configurations")

add_subdirectory(wallpaper)
add_subdirectory(shell_layer_impl)
add_subdirectory(statusbar)

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
    Qt6::Core Qt6::Gui Qt6::Widgets
)
