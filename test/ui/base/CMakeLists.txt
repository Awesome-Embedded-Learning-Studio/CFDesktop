# Include the custom helper function from parent directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")

# =============================================================================
# ui/base tests - math_helper_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      math_helper_test
    SOURCE_FILE    math_helper_test.cpp
    LINK_LIBRARIES UI::base;GTest::gtest;GTest::gtest_main
    LABELS         "ui;base;math"
    LOG_MODULE     ui_base_tests
)

# =============================================================================
# ui/base tests - color_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      color_test
    SOURCE_FILE    color_test.cpp
    LINK_LIBRARIES UI::base;GTest::gtest;GTest::gtest_main
    LABELS         "ui;base;color"
    LOG_MODULE     ui_base_tests
)

# =============================================================================
# ui/base tests - color_helper_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      color_helper_test
    SOURCE_FILE    color_helper_test.cpp
    LINK_LIBRARIES UI::base;GTest::gtest;GTest::gtest_main
    LABELS         "ui;base;color"
    LOG_MODULE     ui_base_tests
)

# =============================================================================
# ui/base tests - device_pixel_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      device_pixel_test
    SOURCE_FILE    device_pixel_test.cpp
    LINK_LIBRARIES UI::base;GTest::gtest;GTest::gtest_main
    LABELS         "ui;base;device"
    LOG_MODULE     ui_base_tests
)

# =============================================================================
# ui/base tests - geometry_helper_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      geometry_helper_test
    SOURCE_FILE    geometry_helper_test.cpp
    LINK_LIBRARIES UI::base;GTest::gtest;GTest::gtest_main
    LABELS         "ui;base;geometry"
    LOG_MODULE     ui_base_tests
)

# =============================================================================
# ui/base tests - easing_test
# =============================================================================
add_gtest_executable(
    TEST_NAME      easing_test
    SOURCE_FILE    easing_test.cpp
    LINK_LIBRARIES UI::base;GTest::gtest;GTest::gtest_main
    LABELS         "ui;base;easing"
    LOG_MODULE     ui_base_tests
)

log_info("UI_Base_Tests" "Configured ui/base tests")
