# CrashReporter dialog + .seen marker (Phase 2 minimal loop).
add_library(cfdesktop_crash_reporter STATIC
    crash_reporter_dialog.cpp
    seen_marker.cpp
)

target_include_directories(cfdesktop_crash_reporter PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_crash_reporter
PRIVATE
    Qt6::Core              # QFile, QJsonDocument, QDateTime
    Qt6::Widgets           # QWidget, QLabel, QTextEdit
    QuarkWidgets::quarkwidgets   # ThemeManager + MD3 Button + animations
    cflogger               # Diagnostic logging
)
