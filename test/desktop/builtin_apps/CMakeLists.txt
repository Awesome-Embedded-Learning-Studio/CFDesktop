# BuiltinPanelRegistry unit tests.
add_gtest_executable(
    TEST_NAME      builtin_panel_registry_test
    SOURCE_FILE    builtin_panel_registry_test.cpp
    LINK_LIBRARIES
        cfdesktop_builtin_apps
        cflogger
        cfbase
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        GTest::gtest
        GTest::gtest_main
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/desktop/ui/components
    LABELS        "desktop;unit;builtin_apps"
    LOG_MODULE    builtin_apps_tests
)
