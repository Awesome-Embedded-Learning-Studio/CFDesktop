# Control center quick-settings popup (MS6 minimal version).
add_library(cfdesktop_control_center STATIC
    control_center.cpp
)

target_include_directories(cfdesktop_control_center PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_control_center
PRIVATE
    Qt6::Core
    Qt6::Widgets
    QuarkWidgets::quarkwidgets   # ThemeManager + Slider / Switch / Button + animations
    cfdesktop_notification       # DND toggle via NotificationService
    cfconfig                     # ConfigStore (DND persistence)
    cflogger                     # Diagnostic logging
)
