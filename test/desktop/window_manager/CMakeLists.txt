# WindowManager state-machine unit tests.
# Drives minimize/maximize/restore via an in-memory FakeWindow (no backend, no
# display). Pure logic + signal emission — Qt6::Widgets pulled in for the
# QObject/QSignalSpy machinery.
add_executable(window_manager_test
    window_manager_test.cpp
)

target_include_directories(window_manager_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ui/components
)

target_link_libraries(window_manager_test PRIVATE
    cf_desktop_components
    Qt6::Core
    Qt6::Widgets
    GTest::gtest
    GTest::gtest_main
)

# Qt6::Test enables QSignalSpy-based signal verification when available.
if(Qt6Test_FOUND)
    target_link_libraries(window_manager_test PRIVATE Qt6::Test)
    target_compile_definitions(window_manager_test PRIVATE QT_TEST_AVAILABLE)
    log_info("window_manager_tests" "  - Qt6::Test available, signal tests enabled")
else()
    log_info("window_manager_tests" "  - Qt6::Test not available, signal tests disabled")
endif()

set_target_properties(window_manager_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

add_test(NAME window_manager_test COMMAND window_manager_test)
set_tests_properties(window_manager_test PROPERTIES
    LABELS "desktop;unit;components"
    TIMEOUT 60
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

log_info("window_manager_tests" "  - window_manager_test")
