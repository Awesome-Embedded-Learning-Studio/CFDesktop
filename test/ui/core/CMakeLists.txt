# Include the custom helper function from parent directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")

log_info("UI_Core_Tests" "Configured ui/core tests")

# =============================================================================
# ui/core tests - token_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      token_test
    SOURCE_FILE    token_test.cpp
    LINK_LIBRARIES Qt6::Core;Qt6::Gui;GTest::gtest;GTest::gtest_main;cfui;CFDesktop::base_headers
    LABELS         "ui;core;token"
    LOG_MODULE     ui_core_tests
)
