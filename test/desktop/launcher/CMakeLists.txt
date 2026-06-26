# App launcher popup unit tests using GoogleTest.
log_info("desktop_launcher_tests" "Configured desktop launcher unit tests:")

add_executable(app_launcher_test
    app_launcher_test.cpp
)

target_link_libraries(app_launcher_test PRIVATE
    cfdesktop_launcher
    QuarkWidgets::quarkwidgets
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(app_launcher_test PRIVATE
    $<TARGET_PROPERTY:cfdesktop_launcher,INTERFACE_INCLUDE_DIRECTORIES>
)

set_target_properties(app_launcher_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

# Add Qt6::Test if available (enables signal/keyboard tests).
if(Qt6Test_FOUND)
    target_link_libraries(app_launcher_test PRIVATE Qt6::Test)
    target_compile_definitions(app_launcher_test PRIVATE QT_TEST_AVAILABLE)
    log_info("desktop_launcher_tests" "  - Qt6::Test available, signal tests enabled")
else()
    log_info("desktop_launcher_tests" "  - Qt6::Test not available, signal tests disabled")
endif()

# Register with CTest.
add_test(NAME app_launcher_test COMMAND app_launcher_test)
set_tests_properties(app_launcher_test PROPERTIES
    LABELS "desktop;launcher;functional"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

log_info("desktop_launcher_tests" "  - app_launcher_test")

# Windows: copy shared libraries to the test output directory so the test exe
# finds its DLL dependencies at runtime.
if(WIN32)
    add_custom_command(TARGET app_launcher_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:CFDesktop_shared>
            $<TARGET_FILE_DIR:app_launcher_test>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:cflogger>
            $<TARGET_FILE_DIR:app_launcher_test>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:cfbase>
            $<TARGET_FILE_DIR:app_launcher_test>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:quarkwidgets>
            $<TARGET_FILE_DIR:app_launcher_test>
        COMMENT "Copying shared libraries to test output directory"
    )
endif()
