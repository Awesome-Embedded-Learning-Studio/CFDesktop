# Desktop widget framework: draggable WidgetBase (future free-positionable widgets).
add_library(cfdesktop_desktop_widget STATIC
    widget_base.cpp
)

target_include_directories(cfdesktop_desktop_widget PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(cfdesktop_desktop_widget
PUBLIC
    QuarkWidgets::quarkwidgets           # ThemeManager, color/typography tokens
PRIVATE
    Qt6::Widgets
)
