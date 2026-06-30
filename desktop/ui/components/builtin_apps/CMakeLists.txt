# Builtin in-process apps (child widgets over the desktop, e.g. About).
# These are NOT launched via QProcess; CFDesktop shows them directly, which
# suits single-framebuffer targets (linuxfb) where an external GUI app would
# fight the desktop for /dev/fb0.
add_library(cfdesktop_builtin_apps STATIC
    about_panel.cpp
)

target_include_directories(cfdesktop_builtin_apps PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cfdesktop_builtin_apps
PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
)
