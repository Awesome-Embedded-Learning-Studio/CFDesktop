# Init session chain unit tests using GoogleTest
log_info("desktop_init_tests" "Configured desktop init unit tests:")

# Common include directories for headers
# Need to include: desktop, desktop/main, desktop/main/init for the init headers
# And base/include for base headers
# Also need logger for cflog.h
set(INIT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../desktop
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../desktop/main
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../desktop/main/init
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../desktop/base/logger/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../base/include
)

# =============================================================================
# Init session chain unit tests - init_session_chain_test
# =============================================================================
# Use standard CMake commands for better AUTOMOC support
add_executable(init_session_chain_test
    init_session_chain_test.cpp
)

# Add include directories
target_include_directories(init_session_chain_test PRIVATE ${INIT_INCLUDE_DIRS})

# Link libraries
target_link_libraries(init_session_chain_test PRIVATE
    CFDesktop::lib
    CFDesktopMain
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
