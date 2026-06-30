# App launcher service (QProcess-based external application launching).
add_library(cfdesktop_launcher STATIC
    app_discoverer.cpp
    app_launch_service.cpp
    app_launcher.cpp
    launcher_tile.cpp
)

target_include_directories(cfdesktop_launcher PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_launcher
PRIVATE
    Qt6::Core   # QProcess, QStringList
    Qt6::Widgets # QWidget, QGridLayout, QPainter
    QuarkWidgets::quarkwidgets # ThemeManager + Material tokens for the popup/tiles
    cfbase      # aex::expected / WeakPtrFactory (propagated via cfbase -> aex::aex)
    cflogger    # Diagnostic logging for launch success/failure
)
