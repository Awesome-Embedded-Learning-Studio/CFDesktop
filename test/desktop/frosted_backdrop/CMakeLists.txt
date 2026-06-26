# FrostedBackdrop unit tests.
# Custom main() (no gtest_main) to spin up an offscreen QGuiApplication so
# QPixmap/QPainter work in headless / CI environments.
add_executable(frosted_backdrop_test
    frosted_backdrop_test.cpp
)

target_include_directories(frosted_backdrop_test PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/ui/components
)

target_link_libraries(frosted_backdrop_test PRIVATE
    cfdesktop_frosted_backdrop
    Qt6::Gui
    Qt6::Widgets
    GTest::gtest
)

set_target_properties(frosted_backdrop_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

add_test(NAME frosted_backdrop_test COMMAND frosted_backdrop_test)
set_tests_properties(frosted_backdrop_test PROPERTIES
    LABELS "desktop;unit;components"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

log_info("frosted_backdrop_tests" "  - frosted_backdrop_test")
