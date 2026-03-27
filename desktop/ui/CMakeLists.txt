
# Recursively Adds
add_subdirectory(base)
add_subdirectory(widget)
add_subdirectory(models)
add_subdirectory(platform)

add_library(CFDesktopUi STATIC)

# Export symbols for Windows DLL - define when building static library that will be linked into DLL
if(WIN32)
    target_compile_definitions(CFDesktopUi PRIVATE CFDESKTOP_EXPORTS)
endif()

target_sources(CFDesktopUi PRIVATE
    CFDesktop.cpp
    CFDesktopEntity.cpp
    CFDesktopProxy.cpp
    CFDesktopWindowProxy.cpp
)

target_include_directories(
    CFDesktopUi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CFDesktopUi
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    cf_desktop_ui_base
    cf_desktop_ui_widget  # Link widget submodule
    cf_desktop_ui_platform  # Link platform submodule
)

