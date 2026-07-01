# Builtin in-process panels (child widgets over the desktop, e.g. About).
# These are NOT launched via QProcess; CFDesktop shows them directly, which
# suits single-framebuffer targets (linuxfb) where an external GUI app would
# fight the desktop for /dev/fb0.
#
# Note: user apps (calculator/noter/system_state/alarm_clock/calendar) are all
# standalone executables launched as detached processes — the
# desktop core holds no app source. This lib keeps only desktop-core builtin
# panels (About) + the generic registry. The builtin MECHANISM (IBuiltinPanel +
# BuiltinPanelRegistry + LaunchKind::BuiltinPanel + HardwareTier adjudication)
# stays available for any future panel that needs in-process rendering.
add_library(cfdesktop_builtin_apps STATIC
    about_panel.cpp
    builtin_panel_registry.cpp
)

target_include_directories(cfdesktop_builtin_apps PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cfdesktop_builtin_apps
PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
    QuarkWidgets::quarkwidgets
    cflogger    # Diagnostic logging for the registry
)
