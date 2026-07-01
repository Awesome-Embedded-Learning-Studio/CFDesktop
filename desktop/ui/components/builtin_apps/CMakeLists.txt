# Builtin in-process apps (child widgets over the desktop, e.g. About).
# These are NOT launched via QProcess; CFDesktop shows them directly, which
# suits single-framebuffer targets (linuxfb) where an external GUI app would
# fight the desktop for /dev/fb0.
add_library(cfdesktop_builtin_apps STATIC
    about_panel.cpp
    builtin_panel_registry.cpp
    calculator_builtin_panel.cpp
    # Architecture exception: compile the standalone Calculator's panel source
    # into the builtin lib so the same panel runs in-process. The standalone
    # executable (apps/calculator/) compiles its own copy independently.
    ${CMAKE_SOURCE_DIR}/apps/calculator/calculator_panel.cpp
)

target_include_directories(cfdesktop_builtin_apps PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

# calculator_panel.h lives under apps/calculator/.
target_include_directories(cfdesktop_builtin_apps PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/calculator
)

target_link_libraries(cfdesktop_builtin_apps
PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets
    cfdesktop_calculator_parser
    QuarkWidgets::quarkwidgets
    cflogger    # Diagnostic logging for the registry
)
