# Wallpaper animation unit tests.
# Custom main() (no gtest_main) spins up an offscreen QGuiApplication so
# QPainter/QImage/QTimer work in headless / CI environments. cfdesktop_wallpaper
# is STATIC with PRIVATE dependencies, so this test re-links every transitive
# dependency it pulls in through the wallpaper headers and engine impl.
add_executable(wallpaper_animation_test
    wallpaper_animation_test.cpp
)

target_include_directories(wallpaper_animation_test PRIVATE
    ${CMAKE_SOURCE_DIR}/test/config_manager
)

target_link_libraries(wallpaper_animation_test PRIVATE
    cfdesktop_wallpaper
    cfconfig
    cflogger
    cfbase
    QuarkWidgets::quarkwidgets
    aex::aex
    Qt6::Gui
    Qt6::Core
    GTest::gtest
)

set_target_properties(wallpaper_animation_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

add_test(NAME wallpaper_animation_test COMMAND wallpaper_animation_test)
set_tests_properties(wallpaper_animation_test PROPERTIES
    LABELS "desktop;unit;wallpaper"
    TIMEOUT 60
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

log_info("wallpaper_animation_tests" "  - wallpaper_animation_test")
