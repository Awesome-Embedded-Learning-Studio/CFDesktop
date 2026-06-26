# WindowPlacementPolicy unit tests.
# Pure geometry tests (clampToWorkArea + computeConstrain) — no IWindow, no
# backend, no display — so a plain gtest_main with no QGuiApplication suffices.
add_executable(window_placement_test
    window_placement_test.cpp
)

target_include_directories(window_placement_test PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/ui/components
)

target_link_libraries(window_placement_test PRIVATE
    cfdesktop_window_placement
    Qt6::Core
    GTest::gtest
    GTest::gtest_main
)

set_target_properties(window_placement_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

add_test(NAME window_placement_test COMMAND window_placement_test)
set_tests_properties(window_placement_test PROPERTIES
    LABELS "desktop;unit;components"
    TIMEOUT 60
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

log_info("window_placement_tests" "  - window_placement_test")
