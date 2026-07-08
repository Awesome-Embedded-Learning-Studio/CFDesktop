# Desktop home page: clock column + swipeable card stack (migrated from CCIMX).
find_package(Qt6 QUIET COMPONENTS Concurrent)

add_library(cfdesktop_home_page STATIC
    global_clock_sources.cpp
    analog_clock_widget.cpp
    digital_time_widget.cpp
    card_stack_widget.cpp
    home_card_manager.cpp
    date_card.cpp
    system_usage_card.cpp
    user_info_card.cpp
    net_status_card.cpp
    local_temp_card.cpp
    gauge_widget.cpp
    disk_gauge_card.cpp
    modern_calendar_widget.cpp
    home_page.cpp
    page_stack_widget.cpp
)

target_include_directories(cfdesktop_home_page PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(cfdesktop_home_page
PUBLIC
    QuarkWidgets::quarkwidgets           # ThemeManager, color/typography tokens
PRIVATE
    Qt6::Widgets
    Qt6::Concurrent                      # off-thread system probes (QtConcurrent::run)
    cfbase                               # memory/cpu probes
)
