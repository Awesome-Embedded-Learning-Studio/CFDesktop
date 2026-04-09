# Shell layer implementations (QWidget-based, Wayland, etc.)
add_library(cfdesktop_shell_layer_impl STATIC
    WidgetShellLayer.cpp
    DefaultShellLayerStrategy.cpp
    WallpaperShellLayerStrategy.cpp
)

target_include_directories(cfdesktop_shell_layer_impl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(
    cfdesktop_shell_layer_impl PRIVATE
    Qt6::Widgets
    cflogger_headers
    cfbase
    cf_ui_base
    cfdesktop_wallpaper
)
