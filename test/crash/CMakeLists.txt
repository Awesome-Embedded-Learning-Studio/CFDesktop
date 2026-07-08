# CrashHandler module unit tests using GoogleTest
include(add_gtest_executable)

log_info("crash_tests" "Configured CrashHandler unit tests:")

add_gtest_executable(
    TEST_NAME      crash_report_test
    SOURCE_FILE    crash_report_test.cpp
    LINK_LIBRARIES cfcrash;GTest::gtest;GTest::gtest_main
    LABELS         "crash;unit"
    LOG_MODULE     crash_tests
    INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}
)

add_gtest_executable(
    TEST_NAME      crash_retention_test
    SOURCE_FILE    crash_retention_test.cpp
    LINK_LIBRARIES cfcrash;GTest::gtest;GTest::gtest_main
    LABELS         "crash;unit"
    LOG_MODULE     crash_tests
    INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}
)

add_gtest_executable(
    TEST_NAME      crash_finalize_test
    SOURCE_FILE    crash_finalize_test.cpp
    LINK_LIBRARIES cfcrash;GTest::gtest;GTest::gtest_main
    LABELS         "crash;unit"
    LOG_MODULE     crash_tests
    INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}
)

add_gtest_executable(
    TEST_NAME      crash_signal_test
    SOURCE_FILE    crash_signal_test.cpp
    LINK_LIBRARIES cfcrash;GTest::gtest;GTest::gtest_main
    LABELS         "crash;unit;transport"
    LOG_MODULE     crash_tests
    INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}
)
