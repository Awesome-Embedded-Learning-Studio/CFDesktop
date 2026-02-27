# Unit tests for base library components using GoogleTest
cmake_minimum_required(VERSION 3.16)

include(add_gtest_executable)

log_info("base_tests" "Configured base library unit tests:")

# Common include directory for base headers
set(BASE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../base/include")

# =============================================================================
# Base library unit tests - expected_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      expected_test
    SOURCE_FILE    expected/expected_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;expected"
    LOG_MODULE     base_tests
    INCLUDE_DIRS   ${BASE_INCLUDE_DIR}
)

# =============================================================================
# Base library unit tests - scope_guard_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      scope_guard_test
    SOURCE_FILE    scope_guard/scope_guard_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;scope_guard"
    LOG_MODULE     base_tests
    INCLUDE_DIRS   ${BASE_INCLUDE_DIR}
)

# =============================================================================
# Base library unit tests - constexpr_fnv1a_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      constexpr_fnv1a_test
    SOURCE_FILE    hash/constexpr_fnv1a_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;hash"
    LOG_MODULE     base_tests
    INCLUDE_DIRS   ${BASE_INCLUDE_DIR}
)

# =============================================================================
# Base library unit tests - weak_ptr_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      weak_ptr_test
    SOURCE_FILE    weak_ptr/weak_ptr_test.cpp
    LINK_LIBRARIES Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;weak_ptr"
    LOG_MODULE     base_tests
    INCLUDE_DIRS   ${BASE_INCLUDE_DIR}
)
