# Unit tests for CFLogger using GoogleTest
include(add_gtest_executable)

log_info("logger_tests" "Configured CFLogger unit tests:")

# Common include directory for mock headers
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
    INCLUDE_DIRS   ${MOCK_INCLUDE_DIR}
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
    INCLUDE_DIRS   ${MOCK_INCLUDE_DIR}
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
    INCLUDE_DIRS   ${MOCK_INCLUDE_DIR}
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
    INCLUDE_DIRS   ${MOCK_INCLUDE_DIR}
)

# Tests link cflogger directly (no DLL), CFLOG_API must expand to nothing
if(WIN32)
    foreach(t logger_benchmark_test logger_concurrency_test
               logger_error_handling_test logger_formatter_test)
        target_compile_definitions(${t} PRIVATE CFLOG_STATIC_BUILD)
    endforeach()
endif()
