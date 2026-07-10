# Notification service + model (in-process desktop notification store).
add_library(cfdesktop_notification STATIC
    notification_service.cpp
)

target_include_directories(cfdesktop_notification PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_notification
PRIVATE
    Qt6::Core        # QObject, QUuid, QDateTime
    cfconfig         # ConfigStore (Do-Not-Disturb persistence)
    cflogger         # Diagnostic logging
)
