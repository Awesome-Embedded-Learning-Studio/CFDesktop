# App launcher service (QProcess-based external application launching).
add_library(cfdesktop_launcher STATIC
    app_launch_service.cpp
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
    cfbase      # aex::expected (propagated via cfbase -> aex::aex)
    cflogger    # Diagnostic logging for launch success/failure
)
