# DesktopIconLayer grid math unit tests.
# Exercises the pure computeGridDimensions() function only (no QWidget),
# but links the full static lib for symbol resolution.
add_gtest_executable(
    TEST_NAME      desktop_icon_layer_test
    SOURCE_FILE    desktop_icon_layer_test.cpp
    LINK_LIBRARIES
        cfdesktop_desktop_icon_layer
        cflogger
        cfbase
        Qt6::Core
        Qt6::Widgets
        GTest::gtest
        GTest::gtest_main
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/desktop/ui/components
    LABELS        "desktop;unit;desktop_icon_layer"
    LOG_MODULE    desktop_icon_layer_tests
)

# DesktopShortcutStore JSON + seed unit tests (pure logic, no ConfigStore IO).
add_gtest_executable(
    TEST_NAME      desktop_shortcut_store_test
    SOURCE_FILE    desktop_shortcut_store_test.cpp
    LINK_LIBRARIES
        cfdesktop_desktop_icon_layer
        cflogger
        cfbase
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/desktop/ui/components
    LABELS        "desktop;unit;desktop_icon_layer"
    LOG_MODULE    desktop_icon_layer_tests
)
