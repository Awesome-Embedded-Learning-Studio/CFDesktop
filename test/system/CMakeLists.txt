# =============================================================================
# Base library unit tests - system
# =============================================================================
include(add_gtest_executable)

log_info("base_system" "Configured base system unit tests:")

add_gtest_executable(
    TEST_NAME      test_cpu_info_query
    SOURCE_FILE    test_cpu_info_query.cpp
    LINK_LIBRARIES cfbase_cpu;GTest::gtest;GTest::gtest_main
    LABELS         "base;system"
    LOG_MODULE     base_system
)

add_gtest_executable(
    TEST_NAME      test_memory_info_query
    SOURCE_FILE    test_memory_info_query.cpp
    LINK_LIBRARIES cfbase_memory;GTest::gtest;GTest::gtest_main
    LABELS         "base;system"
    LOG_MODULE     base_system
)
