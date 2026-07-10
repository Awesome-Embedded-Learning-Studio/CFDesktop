# Notification service + UI (in-process desktop notification system).
add_library(cfdesktop_notification STATIC
    notification_service.cpp
    notification_card_widget.cpp
    notification_banner.cpp
    notification_center_panel.cpp
)

target_include_directories(cfdesktop_notification PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_notification
PRIVATE
    Qt6::Core              # QObject, QUuid, QDateTime
    Qt6::Widgets           # QWidget, QScrollArea, QVBoxLayout
    QuarkWidgets::quarkwidgets   # ThemeManager + MD3 fade/slide animations
    cfconfig               # ConfigStore (Do-Not-Disturb persistence)
    cflogger               # Diagnostic logging
)
