# AppDiscoverer unit tests.
# cfdesktop_launcher is STATIC with PRIVATE deps, so re-link every transitive
# dependency pulled in through app_discoverer.cpp (cflogger for log macros,
# cfbase/aex/QuarkWidgets via the launcher target).
add_gtest_executable(
    TEST_NAME      app_discoverer_test
    SOURCE_FILE    app_discoverer_test.cpp
    LINK_LIBRARIES
        cfdesktop_launcher
        cflogger
        cfbase
        QuarkWidgets::quarkwidgets
        aex::aex
        Qt6::Core
        Qt6::Widgets
        GTest::gtest
        GTest::gtest_main
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/desktop/ui/components
    LABELS        "desktop;unit;launcher"
    LOG_MODULE    launcher_tests
)

add_gtest_executable(
    TEST_NAME      desktop_entry_index_test
    SOURCE_FILE    desktop_entry_index_test.cpp
    LINK_LIBRARIES
        cfdesktop_launcher
        cflogger
        cfbase
        Qt6::Core
        Qt6::Widgets
        GTest::gtest
        GTest::gtest_main
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/desktop/ui/components
    LABELS        "desktop;unit;launcher"
    LOG_MODULE    launcher_tests
)

# resolve_app_icon unit tests. Provides its own main() (constructs a
# QGuiApplication on the offscreen QPA platform for QIcon/QPixmap), so
# GTest::gtest_main is intentionally NOT linked here.
add_gtest_executable(
    TEST_NAME      app_icon_resolver_test
    SOURCE_FILE    app_icon_resolver_test.cpp
    LINK_LIBRARIES
        cfdesktop_launcher
        cflogger
        cfbase
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        GTest::gtest
    INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/desktop/ui/components
    LABELS        "desktop;unit;launcher"
    LOG_MODULE    launcher_tests
)
