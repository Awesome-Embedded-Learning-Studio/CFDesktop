# Render backend abstraction
# Provides RenderBackend interface, capabilities, and factory

add_library(cf_desktop_render STATIC)

target_sources(cf_desktop_render PUBLIC
    FILE_SET cf_desktop_render_headers TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        render_backend.h
        backend_capabilities.h
        render_backend_factory.h
)

target_include_directories(cf_desktop_render PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cf_desktop_render
PUBLIC
    Qt6::Core
    Qt6::Gui
PRIVATE
    cfbase
)
