# Settings window (Phase 12 minimal version: wallpaper/theme/about tabs).
add_library(cfdesktop_settings STATIC
    settings_window.cpp
)

target_include_directories(cfdesktop_settings PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_settings
PRIVATE
    Qt6::Core
    Qt6::Widgets
    QuarkWidgets::quarkwidgets   # ThemeManager + TabView + Slider/Switch/Button + animations
    cfconfig                     # ConfigStore (wallpaper keys)
    cflogger                     # Diagnostic logging
)
