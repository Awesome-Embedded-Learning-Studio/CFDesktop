# Desktop UI Widgets
# Convenient widget components for desktop-specific UI

add_subdirectory(init_session)

# Aggregate all widget sources
set(CFDESKTOP_UI_WIDGET_SOURCES)
set(CFDESKTOP_UI_WIDGET_HEADERS)

# Add init_session sources
list(APPEND CFDESKTOP_UI_WIDGET_SOURCES ${INIT_SESSION_WIDGET_SOURCES})
list(APPEND CFDESKTOP_UI_WIDGET_HEADERS ${INIT_SESSION_WIDGET_HEADERS})

# Create interface library for convenience
add_library(cf_desktop_ui_widget INTERFACE)
target_sources(cf_desktop_ui_widget INTERFACE
    ${CFDESKTOP_UI_WIDGET_SOURCES}
    ${CFDESKTOP_UI_WIDGET_HEADERS}
)
target_include_directories(cf_desktop_ui_widget INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(cf_desktop_ui_widget INTERFACE
    Qt6::Core Qt6::Gui Qt6::Widgets
)
