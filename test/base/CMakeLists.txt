# Unit tests for base library components using GoogleTest
include(add_gtest_executable)

log_info("base_tests" "Configured base library unit tests:")

# =============================================================================
# Base library unit tests - expected_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      expected_test
    SOURCE_FILE    expected/expected_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;expected"
    LOG_MODULE     base_tests
)

# =============================================================================
# Base library unit tests - scope_guard_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      scope_guard_test
    SOURCE_FILE    scope_guard/scope_guard_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;scope_guard"
    LOG_MODULE     base_tests
)

# =============================================================================
# Base library unit tests - constexpr_fnv1a_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      constexpr_fnv1a_test
    SOURCE_FILE    hash/constexpr_fnv1a_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;hash"
    LOG_MODULE     base_tests
)

# =============================================================================
# Base library unit tests - weak_ptr_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      weak_ptr_test
    SOURCE_FILE    weak_ptr/weak_ptr_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;weak_ptr"
    LOG_MODULE     base_tests
)

# =============================================================================
# Base library unit tests - mpsc_queue_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      mpsc_queue_test
    SOURCE_FILE    lockfree/mpsc_queue_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;lockfree"
    LOG_MODULE     base_tests
)

# =============================================================================
# Base library unit tests - policy_chain_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      policy_chain_test
    SOURCE_FILE    policy_chain/policy_chain_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;policy_chain"
    LOG_MODULE     base_tests
)

# =============================================================================
# Base library unit tests - indexed_vector_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      indexed_vector_test
    SOURCE_FILE    indexed_vector/indexed_vector_test.cpp
    LINK_LIBRARIES CFDesktop::base_headers;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "base;unit;indexed_vector"
    LOG_MODULE     base_tests
)
