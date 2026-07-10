# Notification module unit tests using GoogleTest
include(add_gtest_executable)

log_info("notification_tests" "Configured notification unit tests:")

# cfdesktop_notification is STATIC with PRIVATE deps (cfconfig, cflogger), so
# re-link every transitive dependency its .cpp pulls in. mock_path_provider.h
# (for ConfigStore initialization) lives under test/config_manager.
add_gtest_executable(
    TEST_NAME      notification_service_test
    SOURCE_FILE    notification_service_test.cpp
    LINK_LIBRARIES
        cfdesktop_notification
        cfconfig
        cflogger
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/test/config_manager
    LABELS        "notification;unit;service"
    LOG_MODULE    notification_tests
)
