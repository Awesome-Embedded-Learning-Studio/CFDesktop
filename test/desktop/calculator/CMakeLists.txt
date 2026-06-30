# Calculator parser unit tests (ported from CCIMXDesktop's test_parser).
add_gtest_executable(
    TEST_NAME      parser_test
    SOURCE_FILE    parser_test.cpp
    LINK_LIBRARIES cfdesktop_calculator_parser;Qt6::Core;GTest::gtest;GTest::gtest_main
    LABELS         "desktop;unit;calculator"
    LOG_MODULE     calculator_tests
)
