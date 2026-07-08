# Desktop home page: clock column + swipeable card stack (migrated from CCIMX).
add_library(cfdesktop_home_page STATIC
    global_clock_sources.cpp
    analog_clock_widget.cpp
    digital_time_widget.cpp
    card_stack_widget.cpp
    home_card_manager.cpp
    date_card.cpp
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
)
