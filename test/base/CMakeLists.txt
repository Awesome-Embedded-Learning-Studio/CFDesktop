# Unit tests for base library components using GoogleTest
cmake_minimum_required(VERSION 3.16)

# =============================================================================
# Base library unit tests - expected_test
# =============================================================================
add_executable(expected_test
    expected/expected_test.cpp
)

target_include_directories(expected_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/base/include
)

target_link_libraries(expected_test
    PRIVATE
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
)

# Register with CTest
add_test(NAME expected_test COMMAND expected_test)

# Set test properties
set_tests_properties(expected_test PROPERTIES
    LABELS "base;unit;expected"
)

# =============================================================================
# Base library unit tests - scope_guard_test
# =============================================================================
add_executable(scope_guard_test
    scope_guard/scope_guard_test.cpp
)

target_include_directories(scope_guard_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/base/include
)

target_link_libraries(scope_guard_test
    PRIVATE
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
)

# Register with CTest
add_test(NAME scope_guard_test COMMAND scope_guard_test)

# Set test properties
set_tests_properties(scope_guard_test PROPERTIES
    LABELS "base;unit;scope_guard"
)

message(STATUS "Configured base unit tests:")
message(STATUS "  - expected_test")
message(STATUS "  - scope_guard_test")
