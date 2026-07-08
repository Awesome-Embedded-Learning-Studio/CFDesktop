# Taskbar implementation (QWidget-based bottom-edge panel).
add_library(cfdesktop_taskbar STATIC
    centered_taskbar.cpp
    start_button.cpp
    taskbar_icon.cpp
    taskbar_scroll_arrow.cpp
    taskbar_icons.qrc
)

target_include_directories(cfdesktop_taskbar PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_taskbar
PUBLIC
    QuarkWidgets::quarkwidgets           # ThemeManager, color/typography tokens, ICFTheme
PRIVATE
    Qt6::Widgets
    cfbase         # WeakPtr / WeakPtrFactory
    cflogger       # Diagnostic logging for null-backdrop fallback
    cfdesktop_frosted_backdrop  # Frosted-glass backdrop renderer
    cfdesktop_launcher           # app_icon_resolver for full-color .desktop icons
)
