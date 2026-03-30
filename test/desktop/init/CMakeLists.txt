# Init session chain unit tests using GoogleTest
log_info("desktop_init_tests" "Configured desktop init unit tests:")

# =============================================================================
# Init session chain unit tests - init_session_chain_test
# =============================================================================
# Use standard CMake commands for better AUTOMOC support
add_executable(init_session_chain_test
    init_session_chain_test.cpp
)

# Link libraries (transitive include dirs provided by linked targets)
target_link_libraries(init_session_chain_test PRIVATE
    CFDesktop::lib
    CFDesktopMain
    CFDesktop::logger
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Widgets
)

# Set output directory to test/bin
set_target_properties(init_session_chain_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

# Add Qt6::Test if available
if(Qt6Test_FOUND)
    target_link_libraries(init_session_chain_test PRIVATE Qt6::Test)
    target_compile_definitions(init_session_chain_test PRIVATE QT_TEST_AVAILABLE)
    log_info("desktop_init_tests" "  - Qt6::Test available, signal tests enabled")
else()
    log_info("desktop_init_tests" "  - Qt6::Test not available, signal tests disabled")
endif()

# Register with CTest
# Set working directory to where Qt DLLs are located
add_test(NAME init_session_chain_test COMMAND init_session_chain_test)
set_tests_properties(init_session_chain_test PROPERTIES
    LABELS "desktop;init;functional"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
)

log_info("desktop_init_tests" "  - init_session_chain_test")

# Windows: copy CFDesktop_shared.dll to test output directory
if(WIN32)
    add_custom_command(TARGET init_session_chain_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:CFDesktop_shared>
            $<TARGET_FILE_DIR:init_session_chain_test>
        COMMENT "Copying CFDesktop_shared.dll to test output directory"
    )
endif()

# Windows Qt auto-deploy support
qt_add_windows_deploy(init_session_chain_test AUTO_DEPLOY INSTALL_DEPLOY)
