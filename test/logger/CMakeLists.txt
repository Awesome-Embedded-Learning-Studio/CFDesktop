# Unit tests for CFLogger using GoogleTest
cmake_minimum_required(VERSION 3.16)

include(add_gtest_executable)

log_info("logger_tests" "Configured CFLogger unit tests:")

# Common include directory for logger headers
set(LOGGER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../desktop/base/logger/include")
set(MOCK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mock")

# =============================================================================
# CFLogger unit tests - logger_benchmark_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      logger_benchmark_test
    SOURCE_FILE    benchmark/logger_benchmark_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main;CFDesktop::logger
    LABELS         "logger;benchmark;performance"
    LOG_MODULE     logger_tests
    INCLUDE_DIRS   ${LOGGER_INCLUDE_DIR};${MOCK_INCLUDE_DIR}
)

# =============================================================================
# CFLogger unit tests - logger_concurrency_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      logger_concurrency_test
    SOURCE_FILE    logger_concurrency_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main;CFDesktop::logger
    LABELS         "logger;concurrency;threading"
    LOG_MODULE     logger_tests
    INCLUDE_DIRS   ${LOGGER_INCLUDE_DIR};${MOCK_INCLUDE_DIR}
)

# =============================================================================
# CFLogger unit tests - logger_error_handling_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      logger_error_handling_test
    SOURCE_FILE    error_handling/logger_error_handling_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main;CFDesktop::logger
    LABELS         "logger;error_handling;robustness"
    LOG_MODULE     logger_tests
    INCLUDE_DIRS   ${LOGGER_INCLUDE_DIR};${MOCK_INCLUDE_DIR}
)

# =============================================================================
# CFLogger unit tests - logger_formatter_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      logger_formatter_test
    SOURCE_FILE    logger_formatter_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main;CFDesktop::logger
    LABELS         "logger;formatter;unit"
    LOG_MODULE     logger_tests
    INCLUDE_DIRS   ${LOGGER_INCLUDE_DIR};${MOCK_INCLUDE_DIR}
)
