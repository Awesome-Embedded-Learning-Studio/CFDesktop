# =============================================================================
# Base library unit tests - expected_test
# =============================================================================
add_executable(test_cpu_info_query
   test_cpu_info_query.cpp
)

target_link_libraries(test_cpu_info_query
    PRIVATE cfbase_cpu
            GTest::gtest
            GTest::gtest_main
)

# Register with CTest
add_test(NAME test_cpu_info_query COMMAND test_cpu_info_query)

# Set test properties
set_tests_properties(test_cpu_info_query PROPERTIES
    LABELS "base;system"
)

log_info("base_system" "Configured base unit tests:")
log_info("base_system" "  - test_cpu_info_query")
