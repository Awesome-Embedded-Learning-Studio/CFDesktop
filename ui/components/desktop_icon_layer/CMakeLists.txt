# Desktop icon layer (shortcut icon grid sitting on the wallpaper).
add_library(cfdesktop_desktop_icon_layer STATIC
    desktop_icon_layer.cpp
    desktop_shortcut_store.cpp
)

target_include_directories(cfdesktop_desktop_icon_layer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_desktop_icon_layer
PUBLIC
    cfdesktop_launcher       # LauncherTile + AppEntry (+ phase-2 app_icon_resolver)
    QuarkWidgets::quarkwidgets
PRIVATE
    Qt6::Widgets
    cfbase                   # Diagnostics utilities
    cflogger                 # Truncation warning logging
    cfconfig                 # DesktopShortcutStore persistence (ConfigStore)
)
