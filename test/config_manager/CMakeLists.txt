# Config manager module unit tests using GoogleTest
include(add_gtest_executable)

log_info("config_manager_tests" "Configured config manager unit tests:")

# =============================================================================
# Config manager unit tests - config_store_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      config_store_test
    SOURCE_FILE    config_store_test.cpp
    LINK_LIBRARIES CFDesktop::config;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "config_manager;unit;functional"
    LOG_MODULE     config_manager_tests
)

add_gtest_executable(
    TEST_NAME      config_json_backend_test
    SOURCE_FILE    config_json_backend_test.cpp
    LINK_LIBRARIES CFDesktop::config;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "config_manager;unit;functional"
    LOG_MODULE     config_manager_json_tests
    INCLUDE_DIRS   ${CMAKE_SOURCE_DIR}/base/config_manager/src
)
